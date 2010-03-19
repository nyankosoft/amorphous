#include "D3DShaderManager.hpp"
#include "Graphics/Shader/ShaderManagerHub.hpp"
#include "Graphics/Direct3D/Shader/HLSLShaderLightManager.hpp"
#include "Graphics/Direct3D9.hpp"

#include "Support/Log/DefaultLog.hpp"

using namespace std;
using namespace boost;

/*
inline static void SetRGBAColorAs4Floats( const SFloatRGBAColor& c, D3DXHANDLE handle, LPD3DXEFFECT pEffect )
{
	const float rgba[4] = { c.fRed, c.fGreen, c.fBlue, c.fAlpha };
	HRESULT hr = pEffect->SetFloatArray( handle, rgba, 4 );
}*/


class CD3DXInclude : public ID3DXInclude
{
public:

	HRESULT CALLBACK Open(
		D3DXINCLUDE_TYPE IncludeType,
		LPCSTR pFileName,
		LPCVOID pParentData,
		LPCVOID * ppData,
		UINT * pBytes
		);

	HRESULT CALLBACK Close( LPCVOID pData );
};


HRESULT CD3DXInclude::Open(
							D3DXINCLUDE_TYPE IncludeType,
							LPCSTR pFileName,
							LPCVOID pParentData,
							LPCVOID * ppData,
							UINT * pBytes
							)
{
	return S_OK;
}


HRESULT CD3DXInclude::Close( LPCVOID pData )
{
	return S_OK;
}



CHLSLShaderManager::CHLSLShaderManager()
:
m_pEffect(NULL),
m_NumBlendTransforms(0)
{
	SetHandlesToNULL();
}


CHLSLShaderManager::~CHLSLShaderManager()
{
	Release();
}


void CHLSLShaderManager::Release()
{
	SetHandlesToNULL();

	SAFE_RELEASE(m_pEffect);
}


void CHLSLShaderManager::Reload()
{
	LoadShaderFromFile( m_strFilename );

	// reload techniques
	for( int i=0; i<NUM_MAX_TECHNIQUES; i++ )
	{
		if( 0 < m_astrTechniqueName[i].length() )
			m_aTechniqueHandle[i] = m_pEffect->GetTechniqueByName( m_astrTechniqueName[i].c_str() );
	}

	for( size_t i=0; i<m_vecParamHandle.size(); i++ )
	{
		m_vecParamHandle[i].Handle
			= m_pEffect->GetParameterByName( NULL, m_vecParamHandle[i].ParameterName.c_str() );
	}
}


void CHLSLShaderManager::SetHandlesToNULL()
{
	int i;
	for( i=0; i<NUM_HANDLES; i++ )
		m_aHandle[i] = NULL;

	for( i=0; i<NUM_MAX_TECHNIQUES; i++ )
		m_aTechniqueHandle[i] = NULL;

	for( i=0; i<NUM_MATRIX_HANDLES; i++ )
		m_aMatrixHandle[i] = NULL;

	for( i=0; i<NUM_TEXTURE_STAGES; i++ )
		m_aTextureHandle[i] = NULL;

	for( i=0; i<NUM_MAX_CUBETEXTURES; i++ )
		m_aCubeTextureHandle[i] = NULL;

	m_VacantTechniqueEntryIndex = 0;
}


void CHLSLShaderManager::PrintCompilerErrors( LPD3DXBUFFER pCompileErrors )
{
	if( pCompileErrors )
	{
		char *pBuffer = (char *)pCompileErrors->GetBufferPointer();
		g_Log.Print( WL_ERROR, "CHLSLShaderManager::LoadShaderFromFile() - %s", pBuffer );
		pCompileErrors->Release();
	}
/*
	if(hr == D3DERR_INVALIDCALL)			return;
	else if(hr == D3DXERR_INVALIDDATA)		return;
	else if(hr == E_OUTOFMEMORY)			return;
	else									return;
*/
	return;
}


bool CHLSLShaderManager::Init()
{
	m_aMatrixHandle[MATRIX_WORLD]           = m_pEffect->GetParameterBySemantic( NULL, "WORLD" );
	m_aMatrixHandle[MATRIX_VIEW]            = m_pEffect->GetParameterBySemantic( NULL, "VIEW" );
	m_aMatrixHandle[MATRIX_PROJ]            = m_pEffect->GetParameterBySemantic( NULL, "PROJ" );
	m_aMatrixHandle[MATRIX_WORLD_VIEW]      = m_pEffect->GetParameterBySemantic( NULL, "WORLDVIEW" );
	m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ] = m_pEffect->GetParameterBySemantic( NULL, "WORLDVIEWPROJ" );

	int i;
	char acStr[256];
	for( i=0; i<NUM_TEXTURE_STAGES; i++ )
	{
		sprintf( acStr, "Texture%d", i );
		m_aTextureHandle[i] = m_pEffect->GetParameterByName( NULL, acStr );
	}

	m_aCubeTextureHandle[0] = m_pEffect->GetParameterByName( NULL, "g_txCubeMap" );

	m_aHandle[HANDLE_VIEWER_POS]    = m_pEffect->GetParameterByName( NULL, "g_vEyePos" );
	m_aHandle[HANDLE_AMBIENT_COLOR] = m_pEffect->GetParameterByName( NULL, "g_vAmbientColor" );

	// create shader light manager
	CHLSLShaderLightManager *pD3DShaderLightMgr = new CHLSLShaderLightManager( m_pEffect );

	pD3DShaderLightMgr->Init();

	m_pHLSLShaderLightManager = shared_ptr<CHLSLShaderLightManager>( pD3DShaderLightMgr );

	m_vecParamHandle.reserve( 8 );

	InitBlendTransformVariables();

	return true;
}


void CHLSLShaderManager::LoadNullShader()
{
	Release();

	static const char *s_pNullShader = "technique NoShader{ pass P0 {VertexShader = NULL;PixelShader = NULL;} }";

	HRESULT hr = D3DXCreateEffect(
		DIRECT3D9.GetDevice(),
		s_pNullShader,
		(UINT)strlen(s_pNullShader),
		NULL, NULL, 0,
		NULL, &m_pEffect, NULL );
}


bool CHLSLShaderManager::LoadShaderFromFile( const string& filename )
{
	LOG_FUNCTION_SCOPE();

	Release();

	m_strFilename = filename;

	CD3DXInclude d3dx_include;
	ID3DXInclude *pD3DXInclude = NULL; // = &d3dx_include;

	HRESULT hr;	
	LPD3DXBUFFER pCompileErrors;
	hr = D3DXCreateEffectFromFile( DIRECT3D9.GetDevice(), filename.c_str(),
		                           NULL, pD3DXInclude, 0, 
                                   NULL, &m_pEffect, &pCompileErrors );

	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( " - Failed create an effect object from the HLSL effect file from: " + filename );
		PrintCompilerErrors( pCompileErrors );
		LoadNullShader();
		return false;
	}

	return Init();
}


bool CHLSLShaderManager::LoadShaderFromText( const stream_buffer& buffer )
{
	LOG_FUNCTION_SCOPE();

	Release();

	m_strFilename = "";

	if( buffer.get_buffer().size() == 0 )
		return false;

	CD3DXInclude d3dx_include;
	ID3DXInclude *pD3DXInclude = &d3dx_include;

	HRESULT hr;	
	LPD3DXBUFFER pCompileErrors;
	hr = D3DXCreateEffect( DIRECT3D9.GetDevice(), &(buffer.get_buffer()[0]),
		                               (UINT)buffer.get_buffer().size(),
		                               NULL, pD3DXInclude, 0, 
                                       NULL, &m_pEffect, &pCompileErrors );

	if( FAILED(hr) )
	{
		PrintCompilerErrors( pCompileErrors );
		LoadNullShader();
		return false;
	}

	return Init();
}


void CHLSLShaderManager::SetParam( CShaderParameter<int>& int_param )
{
	HRESULT hr;
	int index = GetParameterIndex( int_param );
	if( index < 0 )
		index = RegisterHLSLParam( int_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
		hr = m_pEffect->SetInt( m_vecParamHandle[index].Handle, int_param.GetParameter() );
}


void CHLSLShaderManager::SetParam( CShaderParameter<float>& float_param )
{
	HRESULT hr;
	int index = GetParameterIndex( float_param );
	if( index < 0 )
		index = RegisterHLSLParam( float_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
		hr = m_pEffect->SetFloat( m_vecParamHandle[index].Handle, float_param.GetParameter() );
}


void CHLSLShaderManager::SetParam( CShaderParameter<Vector3>& vec3_param )
{
	LOG_PRINT_ERROR( " - Not implemented." );
}


void CHLSLShaderManager::SetParam( CShaderParameter<SFloatRGBAColor>& color_param )
{
	int index = GetParameterIndex( color_param );
	if( index < 0 )
		index = RegisterHLSLParam( color_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
	{
//		SetRGBAColorAs4Floats( color_param.GetParameter(), m_vecParamHandle[index].Handle, m_pEffect );
		const SFloatRGBAColor& c = color_param.GetParameter();
		const float rgba[4] = { c.fRed, c.fGreen, c.fBlue, c.fAlpha };
		HRESULT hr = m_pEffect->SetFloatArray( m_vecParamHandle[index].Handle, rgba, 4 );
	}
}


void CHLSLShaderManager::SetParam( CShaderParameter< std::vector<float> >& float_param )
{
	HRESULT hr;
	int index = GetParameterIndex( float_param );
	if( index < 0 )
		index = RegisterHLSLParam( float_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
		hr = m_pEffect->SetFloatArray( m_vecParamHandle[index].Handle, &(float_param.GetParameter()[0]), (UINT)float_param.GetParameter().size() );
}


void CHLSLShaderManager::SetParam( CShaderParameter<CTextureParam>& tex_param )
{
	HRESULT hr;
	int index = GetParameterIndex( tex_param );
	if( index < 0 )
	{
		index = RegisterHLSLParam( tex_param );

		// Also, check if the tex param is intended to be set to an indexed stage.
		// If that is the case, update the stage index
		if( tex_param.GetParameterName().find( "stage:" ) == 0 )
			tex_param.Parameter().m_Stage = to_int( tex_param.GetParameterName().substr( 6 ) );
	}

//	if( 0 <= index && index < (int)m_vecParamHandle.size() )
//		hr = m_pEffect->SetTexture( m_vecParamHandle[index].Handle, tex_param.Parameter().m_Handle.GetTexture() );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
	{
		if( 0 <= tex_param.Parameter().m_Stage )
			SetTexture( tex_param.Parameter().m_Stage, tex_param.Parameter().m_Handle );
		else
			hr = m_pEffect->SetTexture( m_vecParamHandle[index].Handle, tex_param.Parameter().m_Handle.GetTexture() );
	}
}


void CHLSLShaderManager::SetParam( const char *parameter_name, int int_param )
{
	m_pEffect->SetInt( parameter_name, int_param );
}


void CHLSLShaderManager::SetParam( const char *parameter_name, float float_param )
{
	m_pEffect->SetFloat( parameter_name, float_param );
}


void CHLSLShaderManager::SetParam( const char *parameter_name, const Vector3& vec3_param )
{
	LOG_PRINT_ERROR( " - Not implemented." );
//	ID3DXEffect::SetVector() only takes D3DXVECTOR4 types.
//	D3DXVECTOR4 v = ???;
//	m_pEffect->SetVector( parameter_name, &v );
}


void CHLSLShaderManager::SetParam( const char *parameter_name, const SFloatRGBAColor& color_param )
{
//	SetRGBAColorAs4Floats( color_param, parameter_name, m_pEffect );
	const SFloatRGBAColor& c = color_param;
	const float rgba[4] = { c.fRed, c.fGreen, c.fBlue, c.fAlpha };
	HRESULT hr = m_pEffect->SetFloatArray( parameter_name, rgba, 4 );
}


void CHLSLShaderManager::SetParam( const char *parameter_name, const float *float_param, uint num_float_values )
{
	m_pEffect->SetFloatArray( parameter_name, float_param, (UINT)num_float_values );
}


boost::shared_ptr<CShaderLightManager> CHLSLShaderManager::GetShaderLightManager()
{
	return m_pHLSLShaderLightManager;
}


void CHLSLShaderManager::UpdateVacantTechniqueIndex()
{
	int i, start = m_VacantTechniqueEntryIndex;
	for( i=start; i<NUM_MAX_TECHNIQUES; i++ )
	{
		if( !m_aTechniqueHandle[i] )
		{
			m_VacantTechniqueEntryIndex = i;
			return;	// found an empty slot
		}
	}

	m_VacantTechniqueEntryIndex = NUM_MAX_TECHNIQUES;
}


bool CHLSLShaderManager::RegisterTechnique( const unsigned int id, const char *pcTechnique )
{
	if( id < 0 || NUM_MAX_TECHNIQUES <= id )
		return false;

	D3DXHANDLE handle =  m_pEffect->GetTechniqueByName( pcTechnique );

	if( !handle )
		return false;

	m_aTechniqueHandle[id] = handle;
	m_astrTechniqueName[id] = pcTechnique;

	UpdateVacantTechniqueIndex();

	return true;
}


/// called if SetTechnique() is called with an unregistered technique handle
/// Through the checks in SetTechnique(), the value of tech_index is known to be
/// either CShaderTechniqueHandle::UNINITIALIZED
/// or CShaderTechniqueHandle::INVALID_INDEX
HRESULT CHLSLShaderManager::SetNewTechnique( CShaderTechniqueHandle& tech_handle )
{
//	const int tech_index = tech_handle.GetTechniqueIndex();

	if( IsUninitializedTechnique( tech_handle ) )
	{
		if( tech_handle.GetTechniqueName() == NULL )
		{
			// The handle has no valid name - return error
			SetInvalidTechnique( tech_handle );
			return E_FAIL;
		}

		int num_techniques = NUM_MAX_TECHNIQUES;
		for( int i=0; i<num_techniques; i++ )
		{
			if( m_astrTechniqueName[i] == tech_handle.GetTechniqueName() )
			{
				// The technique names matched.
				// - the technique has already been registered.

				// cache index to avoid search in subsequent calls
				SetTechniqueIndex( tech_handle, i );

				return m_pEffect->SetTechnique( m_aTechniqueHandle[i] );
			}
		}

		// the requested technique has not been registered to the table yet
		// - Try setting it as a current techqniue.
		// - If the technique has been set successfully (i.e. the technique is valid), register it to the table
		LOG_PRINT( " - Registering a new technique: " + string(tech_handle.GetTechniqueName()) );
		HRESULT hr = m_pEffect->SetTechnique( tech_handle.GetTechniqueName() );
		if( FAILED(hr) )
		{
			// mark the handle as invalid
			LOG_PRINT_ERROR( " - An invalid technique: " + string(tech_handle.GetTechniqueName()) );
			SetInvalidTechnique( tech_handle );
			return E_FAIL;
		}
		else if( m_VacantTechniqueEntryIndex < NUM_MAX_TECHNIQUES )
		{
			// register the technique to the table
			D3DXHANDLE handle =  m_pEffect->GetTechniqueByName( tech_handle.GetTechniqueName() );
			if( handle )
			{
				m_aTechniqueHandle[m_VacantTechniqueEntryIndex] = handle;
				m_astrTechniqueName[m_VacantTechniqueEntryIndex] = tech_handle.GetTechniqueName();
				UpdateVacantTechniqueIndex();
				return S_OK;
			}
			else
			{
				LOG_PRINT_ERROR( " An unexpected error: succeeded in setting the technique but failed to obtain the handle of the technique. technique name: " + std::string(tech_handle.GetTechniqueName()) );
				SetInvalidTechnique( tech_handle );
				return E_FAIL;
			}
		}
	}
	else //( tech_index == CShaderTechniqueHandle::INVALID_INDEX )
	{
		return E_FAIL;
	}

	return E_FAIL;
}


void CHLSLShaderManager::InitBlendTransformVariables( const std::string& variable_name )
{
	LPD3DXEFFECT pEffect = m_pEffect;//= rShaderMgr.GetEffect();
//	D3DXMATRIX *paBlendMatrix = m_pSkeletalMesh->GetBlendMatrices();
	if( !pEffect )//&& paBlendMatrix )
		return;

//	HRESULT hr;
	char acParam[64];
	memset( acParam, 0, sizeof(acParam) );


	memset( m_aVertexBlendTransform, 0, sizeof(m_aVertexBlendTransform) );
	m_NumBlendTransforms = 0;

	for( int i=0; i<NUM_MAX_VERTEX_BLEND_TRANSFORMS; i++ )
	{
		// by default, global array variables of type Transfom, named "g_aBlendTransform" is taken from the effect file
		// i.e., g_aBlendTransform[0], g_aBlendTransform[1], g_aBlendTransform[2]...
		sprintf( acParam, "%s[%d]", variable_name.c_str(), i );
		m_aVertexBlendTransform[i] = pEffect->GetParameterByName( NULL, acParam );

		if( !m_aVertexBlendTransform[i] )
		{
			m_NumBlendTransforms = i;
			break;
		}

//		if( FAILED(hr) ) return;
	}
}


void CHLSLShaderManager::InitBlendTransformVariables()
{
	InitBlendTransformVariables( "g_aBlendTransform" );
}


void CHLSLShaderManager::SetVertexBlendTransforms( const std::vector<Transform>& src_transforms )
{
	int num_transforms = take_min( (int)src_transforms.size(), m_NumBlendTransforms );

	if( 0 == num_transforms )
		return;
/*
	int size_of_transform = sizeof(Transform);

	for( int i=0; i<num_transforms; i++ )
	{
		m_pEffect->SetFloatArray( m_aVertexBlendTransform[i], (float *)(&src_transforms[i]), sizeof(Transform) );
	}

	return;

	// Is it possible to set all the transforms at once?
	m_pEffect->SetFloatArray( m_aVertexBlendTransform[0], (float *)(&src_transforms[0]), num_transforms * sizeof(Transform) );
*/

	HRESULT hr = S_OK;
	char buffer[64];
	memset( buffer, 0, sizeof(buffer) );
	for( int i=0; i<num_transforms; i++ )
	{
		sprintf( buffer, "g_aBlendTransform[%d]", i );
		D3DXHANDLE hTransform = m_pEffect->GetParameterByName( NULL, buffer );

		D3DXHANDLE hRotationQuaternion = m_pEffect->GetParameterByName( hTransform, "rotation" );
		hr = m_pEffect->SetFloatArray( hRotationQuaternion, (float *)&(src_transforms[i].qRotation), 4 );

		float t[4] = {0,0,0,0};
		t[0] = src_transforms[i].vTranslation.x;
		t[1] = src_transforms[i].vTranslation.y;
		t[2] = src_transforms[i].vTranslation.z;
		t[3] = 1.0f;

		D3DXHANDLE hTranslation = m_pEffect->GetParameterByName( hTransform, "translation" );
		hr = m_pEffect->SetFloatArray( hTranslation, t, 4 );
	}
}
