#ifndef  __HUD_SubDisplay_H__
#define  __HUD_SubDisplay_H__


#include <vector>
#include <string>

#include "3DMath/Matrix34.h"
#include "3DMath/Quaternion.h"
#include "3DCommon/Camera.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DMisc.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/TextureRenderTarget.h"
#include "GameCommon/CriticalDamping.h"
//#include "3DCommon/GraphicsComponentCollector.h"
#include "Support/SafeDelete.h"

#include "fwd.h"
#include "CopyEntity.h"


class SubMonitor
{
protected:

	CCamera m_Camera;


public:

	enum type
	{
		FIXED_VIEW,
		MISSILE_VIEW,
		ENTITY_TRACKER,
		NUM_SUBMONITOR_TYPES
	};

	/// stage to monitor
	CStageWeakPtr m_pStage;

	void SetStage( CStageWeakPtr pStage ) { m_pStage = pStage; }

public:

	SubMonitor();

	virtual ~SubMonitor() {}

	virtual void Render();

//	virtual CCamera& GetCamera() = 0;
	virtual CCamera& GetCamera() { return m_Camera; }

	virtual void Update( float dt ) {}

	virtual int GetType() const = 0;

	void CreateRenderTasks();
};


class SubMonitor_EntityTracker : public SubMonitor
{
	CCopyEntity *m_pOwner;

//	CCopyEntity *m_pTarget;

	Vector3 m_vTargetPosition;
	float m_fTargetRadius;

	cdv<float> m_FOV;
//	cdv<Matrix33> m_CamOrient;
	cdv<Quaternion> m_CamOrient;

	float m_fOverlapTime;

	float m_fFocusDelay;

public:

	SubMonitor_EntityTracker( CCopyEntity *pOwner );

//	virtual Camera& GetCamera() { return m_Camera; }

	virtual void Render();

	virtual void Update( float dt );

//	void UpdateTarget( CCopyEntity *pTarget );

	void UpdateTargetPosition( const Vector3& pos ) { m_vTargetPosition = pos; }

	void UpdateTargetRadius( float radius ) { m_fTargetRadius = radius; }

	virtual int GetType() const { return ENTITY_TRACKER; }
};


class SubMonitor_FixedView : public SubMonitor
{
	CCopyEntity *m_pParent;

	Matrix34 m_LocalCameraPose;

public:

	SubMonitor_FixedView( CCopyEntity *pParent, Matrix34& local_camera_pose )
		:
	m_pParent(pParent),
	m_LocalCameraPose(local_camera_pose)
	{}

//	virtual Camera& GetCamera() { return m_Camera; }

	virtual void Update( float dt )
	{
		if( !IsValidEntity( m_pParent ) )
		{
			m_pParent = NULL;
			return;
		}

		m_Camera.SetPose( m_pParent->GetWorldPose() * m_LocalCameraPose );
		m_Camera.UpdateVFTreeForWorldSpace();
	}

	virtual int GetType() const { return FIXED_VIEW; }
};


class CSubMonitorRenderTask;


class HUD_SubDisplay// : public CGraphicsComponent
{
	C2DRect m_DisplayRect;

	CTextureRenderTarget m_TextureRenderTarget;

//	SubMonitor *m_pMonitor;

	// owned reference
	std::vector<SubMonitor *> m_vecpMonitor;

	int m_CurrentMonitor;

public:

	HUD_SubDisplay();

	~HUD_SubDisplay();

//	SubMonitor *GetMomitor() { return m_pMonitor; }

	std::vector<SubMonitor *>& Monitor() { return m_vecpMonitor; }

	inline SubMonitor *GetCurrentMonitor();

	void SetMonitorIndex( int index ) { m_CurrentMonitor = index; }

	void Render();

	void Update( float dt );

	void SetStage( CStageWeakPtr pStage )
	{
		for( size_t i=0; i<m_vecpMonitor.size(); i++ )
			m_vecpMonitor[i]->SetStage( pStage );
	}

	void CreateRenderTasks();

/* tex render target releases / loads its own resources */
//	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );
//	virtual void ReleaseGraphicsResources();

	friend class CSubMonitorRenderTask;

};


inline SubMonitor *HUD_SubDisplay::GetCurrentMonitor()
{
	if(  0 < m_vecpMonitor.size() && 0 <= m_CurrentMonitor )
        return m_vecpMonitor[m_CurrentMonitor];
	else
		return NULL;
}


#endif		/*  __HUD_SubDisplay_H__  */
