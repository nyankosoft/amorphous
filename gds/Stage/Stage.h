#ifndef __STAGE_H__
#define __STAGE_H__


#include "fwd.h"
#include "3DCommon/fwd.h"
#include "Physics/fwd.h"
#include "Sound/fwd.h"

#include "3DCommon/Camera.h"
#include "Support/Timer.h"

#include "CopyEntity.h"
#include "EntitySet.h"
#include "EntityCollisionGroups.h"


class CStage
{
	std::string m_ScriptArchiveFilename;

	/// set by stage loader
	CStageWeakPtr m_pSelf;

	/// borrowed reference to the camera which is used to render the stage
	CCamera *m_pCamera;

    /// holds static geometry
	CStaticGeometryBase *m_pStaticGeometry;

	/// manges entities
	CEntitySet *m_pEntitySet;

	/// handles physics simulation
	physics::CScene *m_pPhysicsScene;

	CSurfaceMaterialManager *m_pMaterialManager;

	std::vector<physics::CMaterial *> m_vecpMaterial;

	/// manages scripted events
	CScriptManager *m_pScriptManager;

	CScreenEffectManager* m_pScreenEffectManager;

	/// keeps time elapsed in stage
	/// CGameTask_Stage() pauses the timer when the stage task is left
	/// and resumes it when the task gets active again
	CTimer *m_pTimer;	
//	PrecisionTimer *m_pTimer;

private:

	/// instantiated by CStageLoader
	CStage();

	bool InitPhysicsManager();

	/// load scripts
	/// - runs scripts that are written for initialization
	bool InitEventScriptManager( const std::string& script_archive_filename );

	/// load the binary material file for the stage. The file must be in the same directory
	/// as the stage file and have the extension ".mat".
	/// This function is called internally after the stage is loaded.
	bool LoadMaterial();

public:

	~CStage();

	CStageWeakPtr GetWeakPtr() { return m_pSelf; }

	bool LoadBaseEntity( CBaseEntityHandle& base_entity_handle );

	/// returns true on success
	bool Initialize( const std::string& script_archive_filename );


	//
	// Rendering
	//

	/// Renders the stage.
	/// Renders the current scene to the current render target. Does not call BeginScene()
	/// and EndScene() pairs since external modules may want to draw something on top of
	/// the rendered scene (overlay icons, etc.)
	void Render();

	/// renders the stage
	/// NOTE: the camera must not be released / destroyed until the Render( camera ) call ends
	/// if you use separate threads for rendering and entity updates, you need to make sure
	/// the camera is not destroyed during entity updates while CStage::Render() is running
	/// TODO: make this thread safe
	/// the same caution applies to the above CStage::Render()
	void Render( CCamera& rCam );

	void CreateRenderTasks();

	void CreateStageRenderTasks( CCamera *pCamera );

//	inline CCamera* GetCurrentCamera() { return m_pEntitySet->GetCurrentCamera(); }
	inline CCamera* GetCurrentCamera() { return m_pCamera; }
	inline void GetBillboardRotationMatrix( Matrix33& matBillboard ) const;
	inline CScreenEffectManager *GetScreenEffectManager() { return m_pScreenEffectManager; }


	//
	// Collision Detection
	//

	void ClipTrace( STrace& tr );
//	void ClipTrace( CJL_LineSegment& segment );
	void CheckPosition( STrace& tr );
	void CheckCollision( CTrace& tr );
	void GetVisibleEntities( CViewFrustumTest& vf_test );


	//
	// Entities
	//

	inline CEntitySet* GetEntitySet() { return m_pEntitySet; }

	inline CBaseEntity* FindBaseEntity( char* pcBaseEntityName );

	/// create an entity in the stage and return its pointer
	/// \return pointer to the created entity
	inline CCopyEntity *CreateEntity( CCopyEntityDesc& rCopyEntityDesc );

	inline CCopyEntity *CreateEntity( CBaseEntityHandle& rBaseEntityHandle, Vector3& rvPosition,
	                      	          Vector3& rvVelocity, Vector3& rvDirection = Vector3(0,0,0));

	/// removes an entity from the stage
	inline void TerminateEntity( CCopyEntity*& pEntity );

	void ReleasePhysicsActor( physics::CActor*& pPhysicsActor );


	//
	// Sound
	//

//	void PlaySound3D( char* pcSoundName, Vector3& rvPosition );
//	void PlaySound3D( int iIndex, Vector3& rvPosition );
//	void PlaySound3D( CSoundHandle &rSoundHandle, Vector3& rvPosition );

	void UpdateListener();


	//
	// Game Events (deprecated)
	//

	void LoadEvents();
	void NotifyEntityTerminationToEventManager( CCopyEntity* pEntity );


	//
	// Others
	//

	Vector3 GetGravityAccel() const;

	CSurfaceMaterial& GetMaterial( int index );

	bool LoadStaticGeometryFromFile( const std::string filename );

	CStaticGeometryBase *GetStaticGeometry() { return m_pStaticGeometry; }

//	char IsCurrentlyVisibleCell(short sCellIndex);

	/// update all the entities in the stage. 
	/// - must be called once per frame.
	/// - also updates scripted event routines
	void Update( float dt );

	float GetFrameTime() const { return m_pTimer->GetFrameTime(); }

	/// \return the elapsed time in [sec] since the stage has been started
	double GetElapsedTime();

	/// \return the elapsed time in [ms] since the stage has been started
	unsigned long GetElapsedTimeMS();

	void PauseTimer();
	void ResumeTimer();

	physics::CScene *GetPhysicsScene() { return m_pPhysicsScene; }

	CScriptManager *GetScriptManager() { return m_pScriptManager; }

	friend class CStageLoader;
	friend class CEntitySet;


//	void SaveCurrentState();
//	void LoadSavedData();
};



// ================================ inline implementations ================================ 

inline void CStage::GetBillboardRotationMatrix( Matrix33& matBillboard ) const
{
	if( m_pCamera )
		m_pCamera->GetOrientation( matBillboard );

//	m_pEntitySet->GetBillboardRotationMatrix( matBillboard );
}


inline CBaseEntity* CStage::FindBaseEntity( char* pcBaseEntityName )
{
	return m_pEntitySet->FindBaseEntity( pcBaseEntityName );
}


inline CCopyEntity *CStage::CreateEntity( CCopyEntityDesc& rCopyEntityDesc )
{
	return m_pEntitySet->CreateEntity( rCopyEntityDesc );
}


inline CCopyEntity *CStage::CreateEntity( CBaseEntityHandle& rBaseEntityHandle,
								          Vector3& rvPosition,
		                                  Vector3& rvVelocity,
								          Vector3& rvDirection )
{
	return m_pEntitySet->CreateEntity( rBaseEntityHandle, rvPosition, rvVelocity, rvDirection );
}


inline void CStage::TerminateEntity( CCopyEntity*& pEntity )
{
	LOG_PRINT( " - Terminating an entity: " + pEntity->GetName() );

	pEntity->pBaseEntity->OnEntityDestroyed( pEntity );

	if( pEntity->pPhysicsActor )
	{
		ReleasePhysicsActor( pEntity->pPhysicsActor );
		pEntity->pPhysicsActor = NULL;
	}

	if( pEntity->GetName().length() != 0 )
		NotifyEntityTerminationToEventManager( pEntity );

	m_pEntitySet->HandleEntityTerminated( pEntity );

	pEntity->Terminate();
	pEntity = NULL;
}


#endif		/*  __STAGE_H__  */
