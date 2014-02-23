#include "D3DCgEffect.hpp"
#include "../Direct3D9.hpp"
#include "../../GraphicsComponentCollector.hpp"
#include <Cg/cgD3D9.h> /* Cg Direct3D9 API (part of Cg Toolkit) */


namespace amorphous
{


class CD3DCgManager : public GraphicsComponent
{
public:

	CD3DCgManager()
	{
		HRESULT hr = cgD3D9SetDevice( DIRECT3D9.GetDevice() );
	}

	~CD3DCgManager()
	{
		HRESULT hr = cgD3D9SetDevice( NULL );
	}

	void ReleaseGraphicsResources()
	{
		HRESULT hr = cgD3D9SetDevice( NULL );
	}

	void LoadGraphicsResources( const GraphicsParameters& rParam )
	{
		HRESULT hr = cgD3D9SetDevice( DIRECT3D9.GetDevice() );
	}
};



/*
inline Result::Name CD3DCgEffect::SetTexture( const int iStage, const TextureHandle& texture )
{
//	HRESULT hr = cgD3D9SetTexture( m_aTextureHandle[iStage], texture.GetTexture() );
	HRESULT hr = cgD3D9SetTexture( m_Textures[iStage], texture.GetTexture() );
	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


inline Result::Name CD3DCgEffect::SetCubeTexture( int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture )
{
	return E_FAIL;
//	if( m_aCubeTextureHandle[index] )
//		return m_pEffect->SetTexture( m_aCubeTextureHandle[index], pCubeTexture );
//	else
//		return E_FAIL;
}


inline Result::Name CD3DCgEffect::SetTechnique( const unsigned int id )
{
	HRESULT hr = m_pEffect->SetTechnique( m_aTechniqueHandle[id] );

	return ( SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR );
}


inline Result::Name CD3DCgEffect::SetTechnique( ShaderTechniqueHandle& tech_handle )
{
	const int tech_index = GetTechniqueIndex( tech_handle );

	HRESULT hr;
	if( 0 <= tech_index )
	{
		// valid index has already been set to handle
		hr = m_pEffect->SetTechnique( m_aTechniqueHandle[tech_index] );
	}
	else
		hr = SetNewTechnique( tech_handle );

	return ( SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR );
}*/


extern CGcontext g_myCgContext;
extern void InitCg();

uint CD3DCgEffect::ms_RefCount = 0;
boost::shared_ptr<CD3DCgManager> CD3DCgEffect::ms_pD3DCgManager;


CD3DCgEffect::CD3DCgEffect()
{
	ms_RefCount++;
	if( ms_RefCount == 1 )
		ms_pD3DCgManager.reset( new CD3DCgManager );

/*	m_CgContext = cgCreateContext();
	CheckForCgError("creating context");

	cgD3D9RegisterStates(m_CgContext);
	CheckForCgError("registering standard CgFX states");
	cgD3D9SetManageTextureParameters(m_CgContext, CG_TRUE);
	CheckForCgError("manage texture parameters");
*/
	ONCE( InitCg() );

	ONCE( InitCgContext() );
}


CD3DCgEffect::~CD3DCgEffect()
{
	ms_RefCount--;
	if( ms_RefCount == 0 )
		ms_pD3DCgManager.reset();
}


void CD3DCgEffect::SetCGTextureParameter( CGparameter& param, TextureHandle& texture )
{
	cgD3D9SetTextureParameter( param, texture.GetTexture() );
}


void CD3DCgEffect::InitCgContext()
{
	cgD3D9RegisterStates(g_myCgContext);
	CheckForCgError("registering standard CgFX states");
	cgD3D9SetManageTextureParameters(g_myCgContext, CG_TRUE);
	CheckForCgError("manage texture parameters");
}


} // namespace amorphous
