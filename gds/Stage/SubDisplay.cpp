#include "SubDisplay.hpp"
#include "Stage.hpp"
#include "ScreenEffectManager.hpp"

#include "Graphics/Direct3D/Direct3D9.hpp"
#include "3DMath/MathMisc.hpp"
#include "Graphics/3DGameMath.hpp"
#include "Graphics/TextureRenderTarget.hpp"
#include "Graphics/RenderTask.hpp"
#include "Graphics/RenderTaskProcessor.hpp"
#include "Support/SafeDeleteVector.hpp"


class CSubMonitorRenderTask : public CRenderTask
{
//	SubMonitor *m_pSubMonitor;
	CSubDisplay *m_pSubDisplay;

public:

	CSubMonitorRenderTask( CSubDisplay *pSubDisplay )
		:
	m_pSubDisplay(pSubDisplay)
	{
	}

	virtual ~CSubMonitorRenderTask() {}

	virtual void Render()
	{
		if( m_pSubDisplay->GetCurrentMonitor() )
		{
			// render the stage to texture render target
			m_pSubDisplay->GetCurrentMonitor()->Render();

			m_pSubDisplay->m_pTextureRenderTarget->ResetRenderTarget();
		}
	}
};


SubMonitor::SubMonitor()
{
	m_OwnerWorldPose = Matrix34Identity();
	m_Camera.SetNearClip( 35.0f );
	m_Camera.SetFarClip( 20000.0f );
}


void SubMonitor::Render()
{
	CStageSharedPtr pStage = m_pStage.lock();
	if( pStage )
	{
		unsigned int orig_flag = pStage->GetScreenEffectManager()->GetEffectFlag();

		pStage->GetScreenEffectManager()->SetEffectFlag( 0 );

		pStage->Render( GetCamera() );

		pStage->GetScreenEffectManager()->SetEffectFlag( orig_flag );
	}
}


void SubMonitor::CreateRenderTasks()
{
	CStageSharedPtr pStage = m_pStage.lock();
	if( pStage )
	{
		// register render tasks necessary to render the stage
		// - does not include the rendering of stage
		pStage->CreateStageRenderTasks( &GetCamera() );

		// register render task to render the stage on texture render target
		// and reset the render target
//		RenderTaskProcessor.AddRenderTask( new CSubMonitorRenderTask( pSubDisplay ) );
	}
}



SubMonitor_EntityTracker::SubMonitor_EntityTracker()
{
	m_Camera.SetNearClip( 35.0f );
	m_Camera.SetFarClip( 30000.0f );
	m_Camera.SetFOV( 3.141592f * 0.3f );

	m_vTargetPosition = Vector3(0,0,0);
	m_fTargetRadius = 45.0f;

	m_fOverlapTime = 0;

	m_CamOrient.current = Quaternion(0,0,0,0);
	m_CamOrient.target = Quaternion(0,0,0,0);
	m_CamOrient.vel = Quaternion(0,0,0,0);
	m_CamOrient.smooth_time = 0.05f;

	m_FOV.SetZeroState();
	m_FOV.smooth_time = 0.15f;

	m_fFocusDelay = 0.0f;

}

void SubMonitor_EntityTracker::Update( float dt )
{
	Vector3 vDist, vDir;
	vDist = m_vTargetPosition - m_OwnerWorldPose.vPosition;
	float dist = Vec3Length( vDist );
	vDir = vDist / dist;

	// calc fov of the camera
	float r = m_fTargetRadius;//45.0f;
	float tangent = r / dist;
	Limit( tangent, 0.0001f, 2.5f );
	float fov = atan( tangent );
//	m_Camera.SetFOV( fov * 2.0f );
	m_FOV.target = fov * 2.0f;

	// calc the dest orientation of the camera
	m_CamOrient.target.FromRotationMatrix( CreateOrientFromFwdDir( vDir ) );

	Matrix34 pose;

	// position - copy the owner's current position
	pose.vPosition = m_OwnerWorldPose.vPosition;

	// orientation - Make the submonitor camera look like it turns at a limited speed
	// using critical damping

	// udpate critical damping
	float total_time = dt + m_fOverlapTime;
	float timestep = 0.005f;
	int num_loops = (int)(total_time / timestep);
	m_fOverlapTime = total_time - num_loops * timestep;
	for(int i=0;i<num_loops;i++)
	{
		m_FOV.Update(dt);
		m_CamOrient.Update(dt);
	}

	m_CamOrient.current.ToRotationMatrix( pose.matOrient );

	// update camera fov & pose
	m_Camera.SetFOV( m_FOV.current );
	m_Camera.SetPose( pose );
	m_Camera.UpdateVFTreeForWorldSpace();

	// blur effect for focus delay
	if( 0.25f < m_FOV.vel )
	{
		m_fFocusDelay += ( m_FOV.vel - 0.25f ) * dt * 1.5f;
	}
	else
	{
		m_fFocusDelay -= 4.0f * dt;
	}

	Limit( m_fFocusDelay, 0.0f, 2.0f );
}


void SubMonitor_EntityTracker::Render()
{
	CStageSharedPtr pStage = m_pStage.lock();
	if( pStage )
	{
		unsigned int orig_flag = pStage->GetScreenEffectManager()->GetEffectFlag();

		unsigned int effect_flag = 0;
	//	unsigned int effect_flag =
	//		ScreenEffect::PostProcessEffects|
	//		ScreenEffect::PseudoMotionBlur|
	//		ScreenEffect::PseudoBlur;

		pStage->GetScreenEffectManager()->SetEffectFlag( effect_flag );

		pStage->GetScreenEffectManager()->SetBlurEffect( m_fFocusDelay );

		pStage->Render( GetCamera() );

		pStage->GetScreenEffectManager()->SetEffectFlag( orig_flag );
	}
}


void SubMonitor_FixedView::Update( float dt )
{
	m_Camera.SetPose( m_OwnerWorldPose * m_LocalCameraPose );
	m_Camera.UpdateVFTreeForWorldSpace();
}


CSubDisplay::CSubDisplay()
:
m_vTargetPosition(Vector3(0,0,0)),
m_fTargetRadius(0),
m_pTextureRenderTarget( CTextureRenderTarget::Create() )
{
	uint uw=0,uh=0;
	GraphicsDevice().GetViewportSize(uw,uh);
	const int w = (int)uw, h = (int)uh;

	// witdh & height of the subdisplay
	int dw = (int)(w * 0.2f);
	int dh = (int)(h * 0.2f);

	m_pTextureRenderTarget->Init( dw, dh );
	m_pTextureRenderTarget->SetBackgroundColor( SFloatRGBAColor::Black() );

	m_DisplayRect.SetPositionLTWH( (int)((w-dw)*0.5f), (int)(h-dh*1.05f), dw, dh );
	m_DisplayRect.SetTextureUV( TEXCOORD2(0.0f,0.0f), TEXCOORD2(1.0f,1.0f) );
//	m_DisplayRect.SetColor( 0xFF8080FF );
	m_DisplayRect.SetColor( 0xFFFFFFFF );

//	Matrix34 local_pose = Matrix34( Vector3( 0.0f, 0.0f, -20.0f ), Matrix33RotationY( 3.141592f ) );
//	m_pMonitor = new SubMonitor_FixedView( pEntity, local_pose );

	m_CurrentMonitor = CSubDisplayType::NONE;
}


CSubDisplay::~CSubDisplay()
{
}


void CSubDisplay::Render()
{
//	if( !m_pMonitor )
	if( !GetCurrentMonitor()
		|| GetCurrentMonitor()->GetType() == SubMonitor::TYPE_NULL )
		return;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();


	m_pTextureRenderTarget->SetRenderTarget();

	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255,0,255,255), 1.0f, 0 );
	pd3dDev->SetRenderState( D3DRS_ZENABLE,  D3DZB_TRUE );
	pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render the stage to texture
	GetCurrentMonitor()->Render();

	m_pTextureRenderTarget->ResetRenderTarget();

//	pd3dDevice->SetVertexShader( NULL );
//	pd3dDevice->SetPixelShader( NULL );

	// commented out 23:21 2007-12-01
	// alpha value of render target texture is set below 0xFF when transparent
	// particles are rendered.
	// need to use the alpha of rect vertices only
	// TODO: add option for C2DRect::Draw() to do alpha blending only with rect vertices alpha
///	m_DisplayRect.Draw( m_pTextureRenderTarget->GetRenderTargetTexture() );

	// set render states for submonitor rect

	// enable alpha blending
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDev->SetTexture( 0, m_pTextureRenderTarget->GetD3DRenderTargetTexture() );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	m_DisplayRect.draw();

//	m_pMonitor->DrawOverlayEffect();
}


void CSubDisplay::CreateRenderTasks()
{
	m_pTextureRenderTarget->SetRenderTarget();

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255,0,255,255), 1.0f, 0 );
	pd3dDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render task to render the scene of the stage to texture render target
	GetCurrentMonitor()->CreateRenderTasks();

//	m_pTextureRenderTarget->ResetRenderTarget();
}


void CSubDisplay::Update( float dt )
{

//	std::for_each(
//		m_vecpMonitor.begin(),
//		m_vecpMonitor.end(),
//		std::mem_fun(

	const size_t num_monitors = m_vecpMonitor.size();
	for( size_t i=0; i<num_monitors; i++ )	
	{
		if( m_vecpMonitor[i] )
		{
			m_vecpMonitor[i]->UpdateTargetPosition( m_vTargetPosition );
			m_vecpMonitor[i]->UpdateTargetRadius( m_fTargetRadius );
			m_vecpMonitor[i]->Update( dt );
		}
	}

//	if( 0 < m_vecpMonitor.size() )
//		m_vecpMonitor[m_CurrentMonitor]->Update( dt );
}

/* tex render target releases / loads its own resources */
/*
void CSubDisplay::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}
void CSubDisplay::ReleaseGraphicsResources()
{
}
*/