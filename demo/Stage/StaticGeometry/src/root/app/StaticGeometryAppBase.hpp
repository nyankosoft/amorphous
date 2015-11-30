#ifndef  __StaticGeometryAppBase_HPP__
#define  __StaticGeometryAppBase_HPP__


#include "App/GameApplicationBase.hpp"
#include "Task/StageViewerGameTask.hpp"
#include "Task/GameTaskFactoryBase.hpp"


enum ShadowAppTaskID
{
//	GAMETASK_ID_SHADOWS_TEST_STAGE,
	GAMETASK_ID_STATIC_GEOMETRY_STAGE_VIEWER,
	NUM_SHADOW_APP_GAMETASK_IDS
};


class CStaticGeometryAppTask : public CStageViewerGameTask
{
public:

	CStaticGeometryAppTask();

	~CStaticGeometryAppTask() {}

};

class CStaticGeometryAppGUITask : public CGUIGameTask
{
	enum GUI_ID
	{
		GUI_ID_DLG_ROOT_STAGE_SELECT = 1000,
		GUI_ID_LBX_STAGE_SELECT,
	};

	std::string m_StageScriptToLoad;

public:

	CStaticGeometryAppGUITask();

	~CStaticGeometryAppGUITask() {}

	int FrameMove( float dt );

	void LoadStage( const std::string& stage_script_name );
};


class CStaticGeometryAppTaskFactory : public CGameTaskFactoryBase
{
public:

	CGameTask *CreateTask( int iTaskID )
	{
		switch( iTaskID )
		{
		case GAMETASK_ID_STATIC_GEOMETRY_STAGE_VIEWER:
			return new CStaticGeometryAppTask();
//		case GAMETASK_ID_SHADOWS_STAGE_SELECT:
//			return new CStaticGeometryAppGUITask();
		default:
			return CGameTaskFactoryBase::CreateTask( iTaskID );
		}
	}
};


class CStaticGeometryAppBase : public CGameApplicationBase
{
public:

	CStaticGeometryAppBase();

	virtual ~CStaticGeometryAppBase();

	virtual bool Init();

	// virtual function implementations

	const std::string GetApplicationTitle() { return "StaticGeometry"; }

	const std::string GetStartTaskName() const;

	int GetStartTaskID() const;

	CGameTaskFactoryBase *CreateGameTaskFactory() const { return new CStaticGeometryAppTaskFactory(); }

	void Release();
};


#endif		/*  __StaticGeometryAppBase_HPP__  */
