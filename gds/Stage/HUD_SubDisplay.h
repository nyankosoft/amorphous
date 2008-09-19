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

	Matrix34 m_OwnerWorldPose;

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

	virtual CCamera& GetCamera() { return m_Camera; }

	void SetOwnerWorldPose( const Matrix34& world_pose ) { m_OwnerWorldPose = world_pose; }

	virtual void Update( float dt ) {}

	virtual int GetType() const = 0;

	virtual void UpdateTargetPosition( const Vector3& pos ) {}

	virtual void UpdateTargetRadius( float radius ) {}

	void CreateRenderTasks();
};


class SubMonitor_EntityTracker : public SubMonitor
{
	Vector3 m_vTargetPosition;
	float m_fTargetRadius;

	cdv<float> m_FOV;

	cdv<Quaternion> m_CamOrient;

	float m_fOverlapTime;

	float m_fFocusDelay;

public:

	SubMonitor_EntityTracker();

//	virtual Camera& GetCamera() { return m_Camera; }

	virtual void Render();

	virtual void Update( float dt );

	void UpdateTargetPosition( const Vector3& pos ) { m_vTargetPosition = pos; }

	void UpdateTargetRadius( float radius ) { m_fTargetRadius = radius; }

	virtual int GetType() const { return ENTITY_TRACKER; }
};


class SubMonitor_FixedView : public SubMonitor
{
	Matrix34 m_LocalCameraPose;

public:

	SubMonitor_FixedView( Matrix34& local_camera_pose )
		:
	m_LocalCameraPose(local_camera_pose)
	{}

	virtual void Update( float dt );

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

	void SetOwnerWorldPose( const Matrix34& world_pose )
	{
		for( size_t i=0; i<m_vecpMonitor.size(); i++ )
			m_vecpMonitor[i]->SetOwnerWorldPose( world_pose );
	}

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
