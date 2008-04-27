#ifndef  __RenderTask_H__
#define  __RenderTask_H__


#include "Direct3D9.h"
#include "Support/Log/DefaultLog.h"


class CRenderTask
{
protected:

	int m_TypeFlags;

protected:

	enum eTypeFlags
	{
		DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE = ( 1 << 0 ),
		RENDER_TO_BACKBUFFER                = ( 1 << 1 ), //<<< Present() is called at the end of Render(). Mutually exclusive with 'RENDER_TO_TEXTURE'
		RENDER_TO_TEXTURE                   = ( 1 << 2 ), //<<< Mutually exclusive with 'RENDER_TO_BACKBUFFER'
	};

public:

	CRenderTask() : m_TypeFlags(0) {}

	virtual ~CRenderTask() {}

	inline void RenderBase();

	virtual void Render() = 0;

};


inline void CRenderTask::RenderBase()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;

	if( m_TypeFlags & DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE )
	{
		Render();
	}
	else
	{
		// call BeginScene() and EndScene() pair before and after the rendering

		hr = pd3dDev->BeginScene();

//		if( FAILED(hr) )
//			LOG_PRINT_ERROR( "IDirect3DDevice9::BeginScene() failed." );

		Render();

		pd3dDev->EndScene();
	}

	if( m_TypeFlags & RENDER_TO_BACKBUFFER )
	{
		// present the backbuffer contents to the display
		pd3dDev->Present( NULL, NULL, NULL, NULL );
	}
}


#endif		/*  __RenderTask_H__  */


//#include <vector>
//using namespace std;


/* rendering stage


setting camera
setting parameters for post-process effects
  from item update routines (e.g. binocular, NVG)
*/


/*
void CGameTask::CreateRenderTasks()
{
	RenderTaskProcessor.AddRenderTask( new CRenderTask_GameTask( this ) );
}


void CGameTask_Stage::CreateRenderTasks()
{
	// create a render task to render the current scene
	if( g_pStage )
		g_pStage->CreateRenderTasks();

	// some extra things to render
	// - text messages
	// - animated 2D graphics elements
	// - debug info
	CGameTask::CreateRenderTasks();
}


void CBE_ScritedCamera::RenderStage_Blended()
{
	CRenderTask *pTask0 = new CRenderTask_RenderToTexture( new CRenderTask_Stage( ms_pSstage, GetCamera0() ) );
	CRenderTask *pTask1 = new CRenderTask_RenderToTexture( new CRenderTask_Stage( ms_pSstage, GetCamera1() ) );
	CRenderTask *pTask2 = new CRenderTask_RenderToTexture( new CRenderTask_Stage( ms_pSstage, GetCamera1() ) );
	RenderTaskProcessor.AddRenderTask( pTask0 );
	RenderTaskProcessor.AddRenderTask( pTask1 );
	RenderTaskProcessor.AddRenderTask( pTask2 );
}


void CBE_ScritedCamera::RenderStage()
{
	RenderTaskProcessor.AddRenderTask( new CRenderTask_Stage( ms_pSstage, GetCamera() ) );
}


void CStage::RenderScene()
{
	if( !m_pCameraEntity )
		return;

	m_pCameraEntity->pBaseEntity->RenderStage( m_pCameraEntity );
}


class PPEffectParam
{
public:

	int EffectFlag;
	float fBlurX;
	float fBlurY;
	float fMotionBlurStrength;
	SFloatRGBColor MonochromeColorOffset;
	float fGlareThreashold;
	float fNoiseLevel;
//	stripe;

	PPEffectParam()
		:
	EffectFlag(0),
	fBlurX(0),
	fBlurY(0),
	fMotionBlurStrength(0),
	MonochromeColorOffset(SFloatRGBColor(0,0,0)),
	fGlareThreashold(0),
	fNoiseLevel(0)
	{}

};


class CRenderTask_ScriptedScene : public CRenderTask
{
	PPEffectParam m_Effect;

	/// stage to render (borrowed reference)
	CStage* m_pStage;

	/// borrowed reference
	CCamera* m_pCamera;

public:

	CRenderTask_ScriptedScene( CStage *pStage, CCamera *pCamera )
		: m_pStage(pStage), m_pCamera(pCamera) {}

	~CRenderTask_ScriptedScene() {}

	void Init();
	void Release();

	void UpdatePostProcessEffectStates()
	{
	}

	virtual void Render()
	{
		LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		// set the camera and draw the scene
		// BeginScene() & EndScene() pairs are called inside the function
		ProfileBegin( "Graphics Draw Routine" );
		m_pStage->Render();
		ProfileEnd( "Graphics Draw Routine" );

		// render text messages
		pd3dDevice->BeginScene();
		GetTextMessageManager().Render();
		pd3dDevice->EndScene();

		// need to finish profile before calling DrawDebugInfo()
		ProfileEnd( "Main Loop" );
		ProfileDumpOutputToBuffer();

		// display fps and other performance information
		pd3dDevice->BeginScene();
		DrawDebugInfo();
		pd3dDevice->EndScene();

		// Present the backbuffer contents to the display
		pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	void MemberFunction02();

};



class CRenderTask_AlphaBlendedRects
{
//	TEXTURE m_Texture[2];
	CTextureRenderTarget *m_apTexture[2];

	float m_fBlendWeight;

public:

	CRenderTask_AlphaBlendedRects( CTextureRenderTarget* pTexRenderTarget0,
		                           CTextureRenderTarget* pTexRenderTarget1,
								   float fBlendWeight )
		: m_fBlendWeight(fBlendWeight)
	{
		m_apTexture[0] = pTexRenderTarget0;
		m_apTexture[1] = pTexRenderTarget1;
	}

	~CRenderTask_AlphaBlendedRects() {}

	void Init();
	void Release();

	void UpdatePostProcessEffectStates()
	{
	}

	virtual void Render()
	{
//		C2DTexRect rect( 0, 0, w, h );
//		rect.SetTextureUV(  );

//		rect.Draw( m_Texture[0], m_Texture[1] );

		C2DRect rect( 0, 0, w, h );

		// turn off alpha blend

//		rect.Draw( m_Texture[0] );
		rect.Draw( m_apTexture[0]->GetRenderTargetTexture() );

		int a = 255.0f * m_fBlendWeight;
		Limit( a, 0, 255 );
		rect.SetColor( a << 24 | 0x00FFFFFF );

		pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TEXTURE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );

		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

		// use alpha of vertex diffuse only

//		rect.draw( m_Texture[1] );
		rect.draw( m_apTexture[1]->GetRenderTargetTexture() );
	}
};


class CRenderTask_Stage
{
	PPEffectParam m_Effect;

	/// stage to render (borrowed reference)
	CStage* m_pStage;

public:

	CRenderTask_Stage( CStage* pStage, camera ) : m_pStage(pStage) {}
	~CRenderTask_Stage() {}

	void Init();
	void Release();

	void UpdatePostProcessEffectStates()
	{
	}

//	virtual void Render()
//	{
//		UpdatePostProcessEffectStates();
//		m_pStage->Render( m_pCamera );
//	}

	virtual void Render()
	{
		LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

//		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
///		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,255,0), 1.0f, 0 );
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,255,0), 1.0f, 0 );

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		// set the camera and draw the scene
		// BeginScene() & EndScene() pairs are called inside the function
		ProfileBegin( "Graphics Draw Routine" );
//		g_pStage->Render(cam);
		m_pStage->Render();
		ProfileEnd( "Graphics Draw Routine" );

		// render player status info
//		pd3dDevice->BeginScene();
//		PLAYERINFO.RenderHUD();
//		pd3dDevice->EndScene();

		// render text messages
//		pd3dDevice->BeginScene();
//		GetTextMessageManager().Render();
//		pd3dDevice->EndScene();

		// need to finish profile before calling DrawDebugInfo()
		ProfileEnd( "Main Loop" );
		ProfileDumpOutputToBuffer();

		// display fps and other performance information
//		pd3dDevice->BeginScene();
//		DrawDebugInfo();
//		pd3dDevice->EndScene();

		// Present the backbuffer contents to the display
		pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	void MemberFunction02();

};


class CRenderTask_RenderToTexture
{
	/// borrowed ref?
	CTextureRenderTarget *m_pTexRenderTarget;

	/// render task which is rendered to the texture render target (borrowed reference)
	CRenderTask *m_pTask;

public:

	CRenderTask_RenderToTexture( CTextureRenderTarget *pTexRenderTarget, CRenderTask *pTask )
		: m_pTexRenderTarget(pTexRenderTarget), m_pTask(pTask) {}

	virtual ~CRenderTask_RenderToTexture() { delete m_pTask; }

	void Init();
	void Release();

	virtual void Render()
	{
		m_pTexRenderTarget->SetRenderTarget();

		m_pTask->Render();

		// restore the original render target
		m_pTexRenderTarget->ResetRenderTarget();
	}
};


class CRenderTask_PlayerView : public CRenderTask_Stage
{
	/// stage to render (borrowed reference)
	CStage *m_pStage;

	/// borrowed reference
	CCamera* m_pCamera;

public:

	CRenderTask_PlayerView( CStage* pStage, CCamera* pCamera )
		: m_pStage(pStage), m_pCamera(pCamera) {}

	~CRenderTask_PlayerView() {}

	void Init();
	void Release();

//	virtual void Render()
//	{
//		m_pStage->Render( m_pCamera );
//
//		PLAYERINFO.RenderHUD();
//	}


	virtual void Render()
	{
		LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	//	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
	///	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,255,0), 1.0f, 0 );
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,255,0), 1.0f, 0 );

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		// set the camera and draw the scene
		// BeginScene() & EndScene() pairs are called inside the function
		ProfileBegin( "Graphics Draw Routine" );
		m_pStage->Render( *m_pCamera );
		ProfileEnd( "Graphics Draw Routine" );

		// render player status info
		pd3dDevice->BeginScene();
	//	PlayerStatusDisplay.Render();
		PLAYERINFO.RenderHUD();
		pd3dDevice->EndScene();

		// render text messages
		pd3dDevice->BeginScene();
		GetTextMessageManager().Render();
		pd3dDevice->EndScene();

		// need to finish profile before calling DrawDebugInfo()
		ProfileEnd( "Main Loop" );
		ProfileDumpOutputToBuffer();

		// display fps and other performance information
		pd3dDevice->BeginScene();
		DrawDebugInfo();
		pd3dDevice->EndScene();

		// Present the backbuffer contents to the display
		pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	void MemberFunction02();

};


class CRenderTask_SubMonitor : public CRenderTask_Stage
{
//	CTextureRenderTarget *m_p;
	CRefClass2 *m_pRefPtr2;

public:

	CRenderTask_SubMonitor( camera ) {}
	~CRenderTask_SubMonitor() {}

	void Init();
	void Release();

	virtual void Render()
	{
		stage->Render( m_pCamera );

		hud->Render();
	}

	void MemberFunction02();

};


class CRenderTask_SceneBlended// : public CRenderTask_Stage
{
//	CRenderTask *m_pTask0;
//	CRenderTask *m_pTask1;
	CRenderTask_RenderToTexture *m_pTask0;
	CRenderTask_RenderToTexture *m_pTask1;

	float m_fBlendWeight;

public:

	CRenderTask_SceneBlended( float blend_weight,
		CRenderTask* pTask0,
		CRenderTask* pTask1 ) {}

	~CRenderTask_SceneBlended() {}

	void Init();
	void Release();

	virtual void Render()
	{
		stage->Render( m_pCamera );

		hud->Render();
	}

	void MemberFunction02();

};
*/
