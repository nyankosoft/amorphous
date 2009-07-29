#ifndef  __PostProcessManager_H__
#define  __PostProcessManager_H__


#include <string>
#include <vector>
#include <d3dx9.h>
#include "../base.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"


//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 


// PPCOUNT is the number of postprocess effects supported
//#define PPCOUNT (sizeof(g_aszFxFile) / sizeof(g_aszFxFile[0]))



// RT_COUNT is the number of simultaneous render targets used in the sample.
enum eRT_TYPE
{
	RT_COLOR = 0,
//	RT_NORMAL,
//	RT_POSITION,
	RT_COUNT
};

/*
// Name of the postprocess .fx files
LPCWSTR g_aszFxFile[] =
{
    L"PP_ColorMonochrome.fx",
    L"PP_ColorInverse.fx",
    L"PP_ColorGBlurH.fx",
    L"PP_ColorGBlurV.fx",
    L"PP_ColorBloomH.fx",
    L"PP_ColorBloomV.fx",
    L"PP_ColorBrightPass.fx",
    L"PP_ColorToneMap.fx",
    L"PP_ColorEdgeDetect.fx",
    L"PP_ColorDownFilter4.fx",
    L"PP_ColorUpFilter4.fx",
    L"PP_ColorCombine.fx",
    L"PP_ColorCombine4.fx",
    L"PP_NormalEdgeDetect.fx",
    L"PP_DofCombine.fx",
    L"PP_NormalMap.fx",
    L"PP_PositionMap.fx",
};


// Description of each postprocess supported
LPCWSTR g_aszPpDesc[] =
{
    L"[Color] Monochrome",
    L"[Color] Inversion",
    L"[Color] Gaussian Blur Horizontal",
    L"[Color] Gaussian Blur Vertical",
    L"[Color] Bloom Horizontal",
    L"[Color] Bloom Vertical",
    L"[Color] Bright Pass",
    L"[Color] Tone Mapping",
    L"[Color] Edge Detection",
    L"[Color] Down Filter 4x",
    L"[Color] Up Filter 4x",
    L"[Color] Combine",
    L"[Color] Combine 4x",
    L"[Normal] Edge Detection",
    L"DOF Combine",
    L"Normal Map",
    L"Position Map",
};*/


//--------------------------------------------------------------------------------------
// This is the vertex format used with the quad during post-process.
struct PPVERT
{
    float x, y, z, rhw;
    float tu, tv;       // Texcoord for post-process source
    float tu2, tv2;     // Texcoord for the original scene

    const static D3DVERTEXELEMENT9 Decl[4];
};


//--------------------------------------------------------------------------------------
// struct CPostProcess
// A struct that encapsulates aspects of a render target postprocess
// technique.
//--------------------------------------------------------------------------------------
struct CPostProcess
{
	enum eParam
	{
		// NUM_PARAMS is the maximum number of changeable parameters supported
		// in an effect.
		NUM_PARAMS = 2,
	};

	/// Effect object for this technique
    LPD3DXEFFECT m_pEffect;

	/// filepath of the HLSL effect file for the effect interface above
	std::string m_ShaderFilename;

    /// PostProcess technique handle
    D3DXHANDLE   m_hTPostProcess;

	/// Render target channel this PP outputs
    int          m_nRenderTarget;

    D3DXHANDLE   m_hTexSource[4];        ///< Handle to the post-process source textures

    D3DXHANDLE   m_hTexScene[4];         ///< Handle to the saved scene texture

	/// Indicates whether the post-process technique
	///   outputs data for this render target.
    bool         m_bWrite[4];            
                                         

    WCHAR        m_awszParamName[NUM_PARAMS][MAX_PATH]; ///< Names of changeable parameters

    WCHAR        m_awszParamDesc[NUM_PARAMS][MAX_PATH]; ///< Description of parameters

    D3DXHANDLE   m_ahParam[NUM_PARAMS];  ///< Handles to the changeable parameters

	/// Size of the parameter. Indicates how many
	/// components of float4 are used.
    int          m_anParamSize[NUM_PARAMS];

    D3DXVECTOR4  m_avParamDef[NUM_PARAMS]; ///< Parameter default

public:

    inline CPostProcess() :
        m_pEffect(NULL), m_hTPostProcess(NULL),
        m_nRenderTarget(0)
    {
        ZeroMemory( m_hTexSource, sizeof(m_hTexSource) );
        ZeroMemory( m_hTexScene, sizeof(m_hTexScene) );
        ZeroMemory( m_bWrite, sizeof(m_bWrite) );
        ZeroMemory( m_ahParam, sizeof(m_ahParam) );
        ZeroMemory( m_awszParamName, sizeof(m_awszParamName) );
        ZeroMemory( m_awszParamDesc, sizeof(m_awszParamDesc) );
        ZeroMemory( m_anParamSize, sizeof(m_anParamSize) );
        ZeroMemory( m_avParamDef, sizeof(m_avParamDef) );
    }

    inline ~CPostProcess() { Cleanup(); }

	HRESULT Init( LPDIRECT3DDEVICE9 pDev, const std::string& filename );

    inline void Cleanup() { SAFE_RELEASE( m_pEffect ); }

	LPD3DXEFFECT GetEffect() { return m_pEffect; }

	HRESULT OnLostDevice();

	HRESULT OnResetDevice( DWORD dwWidth, DWORD dwHeight );

	HRESULT SetScale( float scale_x, float scale_y );
};


//--------------------------------------------------------------------------------------
// struct CPProcInstance
// A class that represents an instance of a post-process to be applied
// to the scene.
//--------------------------------------------------------------------------------------
struct CPProcInstance
{
	D3DXVECTOR4 m_avParam[CPostProcess::NUM_PARAMS];

	float m_fScaleX, m_fScaleY;

    int m_nFxIndex;

public:

    inline CPProcInstance( int fx_index = -1 ) :
        m_nFxIndex(fx_index),
		m_fScaleX(1),
		m_fScaleY(1)
    {
        ZeroMemory( m_avParam, sizeof( m_avParam ) );
    }

	inline void SetScale( float fScaleX, float fScaleY ) { m_fScaleX = fScaleX; m_fScaleY = fScaleY; }
};


struct CRenderTargetChain
{
    int m_nNext;

    bool m_bFirstRender;

    LPDIRECT3DTEXTURE9 m_pRenderTarget[2];

public:

    inline CRenderTargetChain() :
        m_nNext( 0 ), m_bFirstRender( true )
    {
        ZeroMemory( m_pRenderTarget, sizeof(m_pRenderTarget) );
    }

    inline ~CRenderTargetChain()
    {
        Cleanup();
    }

    inline void Init( LPDIRECT3DTEXTURE9 *pRT )
    {
        for( int i = 0; i < 2; ++i )
        {
            m_pRenderTarget[i] = pRT[i];
            m_pRenderTarget[i]->AddRef();
        }
    }

    inline void Cleanup()
    {
        SAFE_RELEASE( m_pRenderTarget[0] );
        SAFE_RELEASE( m_pRenderTarget[1] );
    }

    inline void Flip() { m_nNext = 1 - m_nNext; }

    inline LPDIRECT3DTEXTURE9 GetPrevTarget() { return m_pRenderTarget[1 - m_nNext]; }
    inline LPDIRECT3DTEXTURE9 GetPrevSource() { return m_pRenderTarget[m_nNext]; }
    inline LPDIRECT3DTEXTURE9 GetNextTarget() { return m_pRenderTarget[m_nNext]; }
    inline LPDIRECT3DTEXTURE9 GetNextSource() { return m_pRenderTarget[1 - m_nNext]; }
};


// An CRenderTargetSet object dictates what render targets
// to use in a pass of scene rendering.
struct CRenderTargetSet
{
    IDirect3DSurface9* pRT[RT_COUNT];
};


//======================================================================================
// CPostProcessManager
//  A class that manages post process effects
//  Only the color channel is available
//======================================================================================

class CPostProcessManager : public CGraphicsComponent
{
	enum eParam
	{
		NUM_MAX_PPCOUNT = 16,
	};

	/// Indicates the scene # to render
	int                     m_nScene;

	LPD3DXEFFECT            m_pEffect;

	std::string             m_ShaderFilename;

	/// Effect object for postprocesses
	CPostProcess            m_aPostProcess[NUM_MAX_PPCOUNT];

	int                     m_iNumPostProcesses;

	D3DXHANDLE              m_hTRenderScene;         /// Handle to RenderScene technique

//	D3DXHANDLE              g_hTRenderEnvMapScene;   /// Handle to RenderEnvMapScene technique

	D3DXHANDLE              m_hTRenderNoLight;       /// Handle to RenderNoLight technique

	D3DFORMAT               m_TexFormat;             /// Render target texture format

	IDirect3DTexture9*      m_pSceneSave[RT_COUNT];  /// To save original scene image before postprocess

	CRenderTargetChain      m_RTChain[RT_COUNT];     /// Render target chain (4 used in sample)

//	bool                    m_bEnablePostProc = true;// Whether or not to enable post-processing

	int                     m_nPasses;           /// Number of passes required to render scene

	int                     m_nRtUsed;           /// Number of simultaneous RT used to render scene

	CRenderTargetSet        m_aRtTable[RT_COUNT];    // Table of which RT to use for all passes

	/// used to temporarily hold the original render target
	IDirect3DSurface9*      m_pOrigRenderTarget;

	/// an array of post process instances
	/// each instance holds an index to a post-process shader
	std::vector<CPProcInstance> m_vecPPInstance;

	IDirect3DVertexDeclaration9* m_pVertDeclPP; // Vertex decl for post-processing


	HRESULT PerformSinglePostProcess( IDirect3DDevice9 *pd3dDevice,
		                              CPostProcess &PP,
		                              CPProcInstance &Inst,
									  LPDIRECT3DVERTEXBUFFER9 pVB,
									  PPVERT *aQuad,
									  float &fExtentX,
									  float &fExtentY );


//	inline int GetNumPostProcesses() const { m_aPostProcess.size(); }
	inline int GetNumPostProcesses() const { return m_iNumPostProcesses; }

public:

    CPostProcessManager();

	/// Returns the index of the added shader.
	/// Returns -1 on failure
	int AddPostProcessShader( const std::string& shader_filename );

	std::vector<CPProcInstance>& GetPostProcessInstance() { return m_vecPPInstance; }

	CPostProcess& GetPostProcess( int index ) { return m_aPostProcess[index]; }

	void Release();

	/// set a texture render target on which the scene is to be rendered.
	/// Thus, this method must be called before the entire scene is rendered.
	/// Before calling this method, user is responsible for saving the original render target
	/// by calling GetRenderTarget()
/*	inline HRESULT SetTextureRenderTarget( IDirect3DDevice9 *pd3dDevice, UINT pass )
	{
		HRESULT hr;
		// if pass == 0 && m_nRtUsed == 0, this is equal to
		// pd3dDevice->SetRenderTarget( 0, m_pSceneSave[0] )
		for( int rt = 0; rt < m_nRtUsed; ++rt )
		{
			hr = pd3dDevice->SetRenderTarget( rt, m_aRtTable[pass].pRT[rt] );
			if( FAILED(hr) )
				return hr;
		}

		return S_OK;
	}*/

	inline HRESULT SetTextureRenderTarget( UINT pass )
	{
		HRESULT hr;

		// save the original render target
		// NOTE: this method will increase the internal reference count
		//       on the IDirect3DSurface9 interface
		hr = DIRECT3D9.GetDevice()->GetRenderTarget( 0, &m_pOrigRenderTarget );

		// if pass == 0 && m_nRtUsed == 0, this is equal to
		// pd3dDevice->SetRenderTarget( 0, m_pSceneSave[0] )
		for( int rt = 0; rt < m_nRtUsed; ++rt )
		{
			hr = DIRECT3D9.GetDevice()->SetRenderTarget( rt, m_aRtTable[pass].pRT[rt] );
			if( FAILED(hr) )
				return hr;
		}

		return S_OK;
	}

	inline HRESULT ResetTextureRenderTarget()
	{
		// restore the original render target 0 (back buffer)
		HRESULT hr = DIRECT3D9.GetDevice()->SetRenderTarget( 0, m_pOrigRenderTarget );
		SAFE_RELEASE( m_pOrigRenderTarget );	// need to decrement the reference count on the interface

		return hr;
	}


	/// after rendering the scene, call this method to apply post process effects
	HRESULT PerformPostProcess();

	/// output the final result image onto the backbuffer.
	/// after calling PerformPostProcess(), the user is suppoed to restore the original
	/// render target and call this method
	/// 18:37 2008-02-11 changed: restores the original render target
	/// 22:25 2008-02-11 commented out. replaced by DrawSceneWithPostProcessEffects() below
	///                  also commented out the BeginScene() & EndScene() pair
//	HRESULT RenderPostProcess( IDirect3DDevice9 *pd3dDevice );

	HRESULT DrawSceneWithPostProcessEffects();

//	HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice,
//							const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
//							const std::string& shader_filename,
//							void* pUserContext );

	/// Calls this for initialization
	/// - Returns true on success
    Result::Name OnCreateDevice( const std::string& shader_filename );

	/// Used to release the resources to change the resolution of the back buffer
    void OnDestroyDevice();

	void OnLostDevice( void* pUserContext );

//	HRESULT Init( const std::string& filename );

    HRESULT OnResetDevice();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif  /* __PostProcessManager_H__ */
