#include "CgEffectBase.hpp"
//#include "Graphics/Shader/ShaderManagerHub.hpp"
#include "Support/Log/DefaultLog.hpp"

using namespace std;
using namespace boost;


CGcontext g_myCgContext;

void InitCg()
{
	g_myCgContext = cgCreateContext();
}



CCgEffectBase::CCgEffectBase()
:
m_VacantTechniqueEntryIndex(0)
// Examples in nVidia do not these initializations. Why?
//m_CgEffect(NULL),
//m_CgContext(NULL)
{
	// Don't call cgCreateContext() here because cgD3D9SetDevice needs to be called first in Direct3D mode
//	m_CgContext = cgCreateContext();
//	CheckForCgError("creating context");
}


bool CCgEffectBase::LoadShaderFromFile( const std::string& filename )
{
	Release();

	m_EffectPath = filename;

//	m_CgEffect = cgCreateEffectFromFile( m_CgContext, filename.c_str(), NULL );
	m_CgEffect = cgCreateEffectFromFile( g_myCgContext, filename.c_str(), NULL );
	CheckForCgError("creating an effect from file");
//	assert(m_CgEffect);

	if( cgIsEffect( m_CgEffect ) )
	{
		// A valid effect has been created.
		return Init();
	}
	else
		return false;
}


void CCgEffectBase::Release()
{
	cgDestroyEffect( m_CgEffect );
	cgDestroyContext( m_CgContext );
}


void CCgEffectBase::CheckForCgError( const char *situation )
{
	CGerror error;
	const char *error_string = cgGetLastErrorString(&error);

	if( error != CG_NO_ERROR )
	{
		string error_log;
		if( error == CG_COMPILER_ERROR )
		{
			error_log = fmt_string(
			"Program: %s\n"
			"Situation: %s\n"
			"Error: %s\n\n"
			"Cg compiler output...\n%s",
			m_EffectPath.c_str(), situation, error_string,
			cgGetLastListing(m_CgContext) );
		}
		else
		{
			error_log = fmt_string(
			"Program: %s\n"
			"Situation: %s\n"
			"Error: %s",
			m_EffectPath.c_str(), situation, error_string );
		}

		LOG_PRINT_ERROR( error_log );
		// Active in nVidia's example code
//		exit(1);
	}
}


bool CCgEffectBase::Init()
{
	m_aMatrixHandle[MATRIX_WORLD]           = cgGetNamedEffectParameter( m_CgEffect, "World" );
	m_aMatrixHandle[MATRIX_VIEW]            = cgGetNamedEffectParameter( m_CgEffect, "View" );
	m_aMatrixHandle[MATRIX_PROJ]            = cgGetNamedEffectParameter( m_CgEffect, "Proj" );
	m_aMatrixHandle[MATRIX_WORLD_VIEW]      = cgGetNamedEffectParameter( m_CgEffect, "WorldView" );
	m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ] = cgGetNamedEffectParameter( m_CgEffect, "WorldViewProj" );
/*
	int i;
	char acStr[256];
	for( i=0; i<NUM_TEXTURE_STAGES; i++ )
	{
		sprintf( acStr, "Texture%d", i );
		m_aTextureHandle[i] = m_pEffect->GetParameterByName( NULL, acStr );
	}
*/
	m_aCubeTextureHandle[0] = cgGetNamedEffectParameter( m_CgEffect, "CubeMapTexture" );

	m_aHandle[HANDLE_VIEWER_POS]    = cgGetNamedEffectParameter( m_CgEffect, "g_vEyePos" );
	m_aHandle[HANDLE_AMBIENT_COLOR] = cgGetNamedEffectParameter( m_CgEffect, "g_vAmbientColor" );

	// create shader light manager
//	CCgShaderLightManager *pCgShaderLightMgr = new CCgShaderLightManager( m_CgEffect );

//	pCgShaderLightMgr->Init();

//	m_pCgShaderLightManager = shared_ptr<CHLSLShaderLightManager>( pD3DShaderLightMgr );
/*
	m_vecParamHandle.reserve( 8 );

	InitBlendTransformVariables();
*/
	return true;
}


void CCgEffectBase::LoadNullShader()
{
	Release();

	static const char *s_pNullShader = "technique NoShader{ pass P0 {VertexShader = NULL;PixelShader = NULL;} }";

	m_CgEffect = cgCreateEffect( m_CgContext, s_pNullShader, NULL );
}


void CCgEffectBase::SetParam( CShaderParameter<int>& int_param )
{
	int index = GetParameterIndex( int_param );
	if( index < 0 )
		index = RegisterCgParam( int_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
		cgSetParameter1i( m_vecParamHandle[index].Handle, int_param.GetParameter() );
}


void CCgEffectBase::SetParam( CShaderParameter<float>& float_param )
{
	int index = GetParameterIndex( float_param );
	if( index < 0 )
		index = RegisterCgParam( float_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
		cgSetParameter1f( m_vecParamHandle[index].Handle, float_param.GetParameter() );
}


void CCgEffectBase::SetParam( CShaderParameter<Vector3>& vec3_param )
{
	LOG_PRINT_ERROR( " - Not implemented." );
}


void CCgEffectBase::SetParam( CShaderParameter<SFloatRGBAColor>& color_param )
{
	int index = GetParameterIndex( color_param );
	if( index < 0 )
		index = RegisterCgParam( color_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
	{
//		SetRGBAColorAs4Floats( color_param.GetParameter(), m_vecParamHandle[index].Handle, m_pEffect );
		const SFloatRGBAColor& c = color_param.GetParameter();
		const float rgba[4] = { c.fRed, c.fGreen, c.fBlue, c.fAlpha };
		cgSetParameter4fv( m_vecParamHandle[index].Handle, rgba );
	}
}


void CCgEffectBase::SetParam( CShaderParameter< std::vector<float> >& float_param )
{
	int index = GetParameterIndex( float_param );
	if( index < 0 )
		index = RegisterCgParam( float_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
	{
		CGparameter cg_param = m_vecParamHandle[index].Handle;
		switch( m_vecParamHandle.size() )
		{
		case 1:
			cgSetParameter1fv( cg_param, &(float_param.GetParameter()[0]) );
			break;
		case 2:
			cgSetParameter2fv( cg_param, &(float_param.GetParameter()[0]) );
			break;
		case 3:
			cgSetParameter3fv( cg_param, &(float_param.GetParameter()[0]) );
			break;
		case 4:
			cgSetParameter4fv( cg_param, &(float_param.GetParameter()[0]) );
			break;
		default:
			LOG_PRINT_WARNING( fmt_string(" Cg has no API to set %d floats at once.",m_vecParamHandle.size() ) );
			break;
		}

	}
}


void CCgEffectBase::SetParam( CShaderParameter<CTextureParam>& tex_param )
{
	int index = GetParameterIndex( tex_param );
	if( index < 0 )
	{
		// Check if the tex param is intended to be set to an indexed stage.
		// If the parameter name starts with "stage:", the parameter is supposed to be set
		// to a texture stage with an index specified by the digit that comes after the colon,
		// and the stage index of the parameter is updated.
		if( tex_param.GetParameterName().find( "stage:" ) == 0 )
		{
			string tex_stage = tex_param.GetParameterName().substr( 6 );
			tex_param.Parameter().m_Stage = to_int(tex_stage);
			tex_param.SetParameterName( "Texture" + tex_stage ); // change the parameter name from "stage:?" to "Texture?"
		}

		index = RegisterCgParam( tex_param );
	}

//	if( 0 <= index && index < (int)m_vecParamHandle.size() )
//		hr = m_pEffect->SetTexture( m_vecParamHandle[index].Handle, tex_param.Parameter().m_Handle.GetTexture() );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
	{
		if( 0 <= tex_param.Parameter().m_Stage )
			SetTexture( tex_param.Parameter().m_Stage, tex_param.Parameter().m_Handle );
		else
			SetCGTextureParameter( m_vecParamHandle[index].Handle, tex_param.Parameter().m_Handle );
	}
}


void CCgEffectBase::SetParam( CShaderParameter<Matrix44>& mat44_param )
{
	int index = GetParameterIndex( mat44_param );
	if( index < 0 )
		index = RegisterCgParam( mat44_param );

	if( 0 <= index && index < (int)m_vecParamHandle.size() )
	{
		cgSetMatrixParameterfc( m_vecParamHandle[index].Handle, mat44_param.Parameter().GetData() );
/*		float row_major_matrix_44[16];
		mat44_param.Parameter().GetRowMajorMatrix44( row_major_matrix_44 );
		cgSetMatrixParameter44fr( m_vecParamHandle[index].Handle, row_major_matrix_44 );*/
//		hr = m_pEffect->SetMatrixTranspose( m_vecParamHandle[index].Handle, (D3DXMATRIX *)&mat44_param );
	}
}


void CCgEffectBase::SetParam( const char *parameter_name, int int_param )
{
	CGparameter cg_param = cgGetNamedEffectParameter( m_CgEffect, parameter_name );
	cgSetParameter1i( cg_param, int_param );
}


void CCgEffectBase::SetParam( const char *parameter_name, float float_param )
{
	CGparameter cg_param = cgGetNamedEffectParameter( m_CgEffect, parameter_name );
	cgSetParameter1f( cg_param, float_param );
}


void CCgEffectBase::SetParam( const char *parameter_name, const Vector3& vec3_param )
{
	const float values[3] = { vec3_param.x, vec3_param.y, vec3_param.z };
	CGparameter cg_param = cgGetNamedEffectParameter( m_CgEffect, parameter_name );
	cgSetParameter3fv( cg_param, values );
}


void CCgEffectBase::SetParam( const char *parameter_name, const SFloatRGBAColor& color_param )
{
//	SetRGBAColorAs4Floats( color_param, parameter_name, m_pEffect );
	const SFloatRGBAColor& c = color_param;
	const float rgba[4] = { c.fRed, c.fGreen, c.fBlue, c.fAlpha };
	CGparameter cg_param = cgGetNamedEffectParameter( m_CgEffect, parameter_name );
	cgSetParameter4fv( cg_param, rgba );
}


static void SetFloats( CGparameter cg_param, const float *float_param, uint num_float_values )
{
	switch( num_float_values )
	{
	case 1:
		cgSetParameter1fv( cg_param, float_param );
		break;
	case 2:
		cgSetParameter2fv( cg_param, float_param );
		break;
	case 3:
		cgSetParameter3fv( cg_param, float_param );
		break;
	case 4:
		cgSetParameter4fv( cg_param, float_param );
		break;
	default:
		LOG_PRINT_WARNING( fmt_string(" Cg has no API to set %d floats at once.",num_float_values ) );
		break;
	}
}

void CCgEffectBase::SetParam( const char *parameter_name, const float *float_param, uint num_float_values )
{
	if( 4 < num_float_values )
		return;

	CGparameter cg_param = cgGetNamedEffectParameter( m_CgEffect, parameter_name );
	SetFloats( cg_param, float_param, num_float_values );
}


void CCgEffectBase::SetParam( const char *parameter_name, const Matrix44& mat44_param )
{
	CGparameter cg_param = cgGetNamedEffectParameter( m_CgEffect, parameter_name );
	cgSetMatrixParameterfc( cg_param, mat44_param.GetData() );

//	D3DXMATRIX mat;
//	mat44_param.GetRowMajorMatrix44( (float *)&mat );
//	m_pEffect->SetMatrix( parameter_name, &mat );

	// test
//	D3DXMATRIX transposed;
//	D3DXMatrixTranspose( &transposed, &mat );
//	Matrix44 test;
//	test.SetRowMajorMatrix44( (Scalar *)&transposed );

	// wrong
//	m_pEffect->SetMatrixTranspose( parameter_name, (D3DXMATRIX *)&mat44_param );
}


boost::shared_ptr<CShaderLightManager> CCgEffectBase::GetShaderLightManager()
{
	LOG_PRINT_ERROR( " Not implemented." );
	return boost::shared_ptr<CShaderLightManager>();
}


void CCgEffectBase::UpdateVacantTechniqueIndex()
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


bool CCgEffectBase::RegisterTechnique( const unsigned int id, const char *pcTechnique )
{
	if( id < 0 || NUM_MAX_TECHNIQUES <= id )
		return false;

//	D3DXHANDLE handle =  m_pEffect->GetTechniqueByName( pcTechnique );
	CGtechnique handle =  cgGetNamedTechnique( m_CgEffect, pcTechnique );

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
Result::Name CCgEffectBase::SetNewTechnique( CShaderTechniqueHandle& tech_handle )
{
//	const int tech_index = tech_handle.GetTechniqueIndex();

	if( IsUninitializedTechnique( tech_handle ) )
	{
		if( tech_handle.GetTechniqueName() == NULL )
		{
			// The handle has no valid name - return error
			SetInvalidTechnique( tech_handle );
			return Result::UNKNOWN_ERROR;
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

				m_FirstPass = cgGetFirstPass( m_aTechniqueHandle[i] );
				return cgIsPass( m_FirstPass ) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
//				return m_pEffect->SetTechnique( m_aTechniqueHandle[i] );
			}
		}

		// the requested technique has not been registered to the table yet
		// - Try setting it as a current techqniue.
		// - If the technique has been set successfully (i.e. the technique is valid), register it to the table
		CGtechnique technique = cgGetNamedTechnique( m_CgEffect, tech_handle.GetTechniqueName() );
		if( technique )
		{
			if( m_VacantTechniqueEntryIndex < NUM_MAX_TECHNIQUES )
			{
				LOG_PRINT( " - Registering a new technique: " + string(tech_handle.GetTechniqueName()) );
				m_aTechniqueHandle[m_VacantTechniqueEntryIndex] = technique;
				m_astrTechniqueName[m_VacantTechniqueEntryIndex] = tech_handle.GetTechniqueName();
				UpdateVacantTechniqueIndex();
				return Result::SUCCESS;
			}
			else
			{
				LOG_PRINT_ERROR( " Cannot register any more technique. - technique name: " + string(tech_handle.GetTechniqueName()) );
				SetInvalidTechnique( tech_handle );
				return Result::UNKNOWN_ERROR;
			}
		}
		else
		{
			LOG_PRINT_ERROR( " An unexpected error: failed to obtain the technique from the shader - technique name: " + string(tech_handle.GetTechniqueName()) );
			SetInvalidTechnique( tech_handle );
			return Result::UNKNOWN_ERROR;
		}
	}
	else //( tech_index == CShaderTechniqueHandle::INVALID_INDEX )
	{
		return Result::UNKNOWN_ERROR;
	}

	return Result::UNKNOWN_ERROR;
}

/*
void CCgEffectBase::InitBlendTransformVariables( const std::string& variable_name )
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


void CCgEffectBase::InitBlendTransformVariables()
{
	InitBlendTransformVariables( "g_aBlendTransform" );
}
*/

void CCgEffectBase::SetVertexBlendTransforms( const std::vector<Transform>& src_transforms )
{
	int num_transforms = take_min( (int)src_transforms.size(), m_NumBlendTransforms );

	if( 0 == num_transforms )
		return;

	HRESULT hr = S_OK;
	char name_t[64], name_r[64];
	memset( name_t, 0, sizeof(name_t) );
	memset( name_r, 0, sizeof(name_r) );
	for( int i=0; i<num_transforms; i++ )
	{
		// See the help of cgGetNamedEffectParameter
//		int index = i;
		int index = i + 1;

		sprintf( name_r, "g_aBlendTransform[%d].rotation",    index );
		sprintf( name_t, "g_aBlendTransform[%d].translation", index );

		CGparameter hRotationQuaternion = cgGetNamedEffectParameter( m_CgEffect, name_r );
		cgSetParameter4fv( hRotationQuaternion, (float *)&(src_transforms[i].qRotation) );

		float t[4] = {0,0,0,0};
		t[0] = src_transforms[i].vTranslation.x;
		t[1] = src_transforms[i].vTranslation.y;
		t[2] = src_transforms[i].vTranslation.z;
		t[3] = 1.0f;

		CGparameter hTranslation = cgGetNamedEffectParameter( m_CgEffect, name_t );
		cgSetParameter4fv( hTranslation, t );
	}

/*	HRESULT hr = S_OK;
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
	}*/
}
