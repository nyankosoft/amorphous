#ifndef __STAGE_H__
#define __STAGE_H__


#include "gds/Graphics/fwd.hpp"
#include "gds/Physics/fwd.hpp"
#include "gds/Sound/fwd.hpp"
#include "gds/Input/fwd.hpp"

#include "gds/Graphics/Camera.hpp"
#include "gds/Support/Timer.hpp"

#include "fwd.hpp"
#include "CopyEntity.hpp"
#include "EntitySet.hpp"
#include "EntityCollisionGroups.hpp"


namespace amorphous
{


class CStage
{
	std::string m_ScriptArchiveFilename;

	/// set by stage loader
	CStageWeakPtr m_pSelf;

	/// borrowed reference to the camera which is used to render the stage
	Camera *m_pCamera;

    /// holds static geometry
	CStaticGeometryBase *m_pStaticGeometry;

	/// manages entities
	EntityManager *m_pEntitySet;

	/// handles physics simulation
	physics::CScene *m_pPhysicsScene;

	boost::shared_ptr<CSurfaceMaterialManager> m_pMaterialManager;

	std::vector<physics::CMaterial *> m_vecpMaterial;

	/// manages scripted events
	boost::shared_ptr<CScriptManager> m_pScriptManager;

	boost::shared_ptr<ScreenEffectManager> m_pScreenEffectManager;

	/// keeps time elapsed in stage
	/// CGameTask_Stage() pauses the timer when the stage task is left
	/// and resumes it when the task gets active again
	Timer *m_pTimer;	
//	PrecisionTimer *m_pTimer;

	/// owned reference
	CInputHandler *m_pStageDebugInputHandler;

private:

	/// instantiated by StageLoader
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

	bool LoadBaseEntity( BaseEntityHandle& base_entity_handle );

	/// returns true on success
	bool Initialize( const std::string& script_archive_filename );

	const std::string& GetScriptName() const { return m_ScriptArchiveFilename; }


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
	/// the same caution applies to the above CStage::Render() as well
	void Render( Camera& rCam );

	void CreateRenderTasks();

	void CreateStageRenderTasks( Camera *pCamera );

//	inline Camera* GetCurrentCamera() { return m_pEntitySet->GetCurrentCamera(); }
	inline Camera* GetCurrentCamera() { return m_pCamera; }
	inline void GetBillboardRotationMatrix( Matrix33& matBillboard ) const;
	inline boost::shared_ptr<ScreenEffectManager> GetScreenEffectManager() { return m_pScreenEffectManager; }


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

	inline EntityManager* GetEntitySet() { return m_pEntitySet; }

	inline BaseEntity* FindBaseEntity( char* pcBaseEntityName );

	/// create an entity in the stage and return its pointer
	/// \return pointer to the created entity
	inline CCopyEntity *CreateEntity( CCopyEntityDesc& rCopyEntityDesc );

	inline CCopyEntity *CreateEntity( BaseEntityHandle& rBaseEntityHandle, const Vector3& rvPosition,
	                      	          const Vector3& rvVelocity, const Vector3& rvDirection = Vector3(0,0,0));

	template<class T>
	inline EntityHandle<T> CreateEntity( boost::shared_ptr<T> pEntity, BaseEntityHandle& rBaseEntityHandle, physics::CActorDesc *pPhysActorDesc = NULL );

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

	void NotifyEntityTerminationToEventManager( CCopyEntity* pEntity );


	//
	// Others
	//

	Vector3 GetGravityAccel() const;

	CSurfaceMaterial& GetMaterial( int index );

	EntityHandle<> LoadStaticGeometryFromFile( const std::string filename );

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

	boost::shared_ptr<CScriptManager> GetScriptManager() { return m_pScriptManager; }

	friend class StageLoader;
	friend class EntityManager;


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


inline BaseEntity* CStage::FindBaseEntity( char* pcBaseEntityName )
{
	return m_pEntitySet->FindBaseEntity( pcBaseEntityName );
}


inline CCopyEntity *CStage::CreateEntity( CCopyEntityDesc& rCopyEntityDesc )
{
	return m_pEntitySet->CreateEntity( rCopyEntityDesc );
}

/*
template<class T>
inline EntityHandle<T> CStage::CreateEntity( shared_ptr<CCopyEntity> pEntity )
{
	return m_pEntitySet->CreateEntity( pEntity );
}
*/

inline CCopyEntity *CStage::CreateEntity( BaseEntityHandle& rBaseEntityHandle,
								          const Vector3& rvPosition,
		                                  const Vector3& rvVelocity,
								          const Vector3& rvDirection )
{
	return m_pEntitySet->CreateEntity( rBaseEntityHandle, rvPosition, rvVelocity, rvDirection );
}


template<class T>
inline EntityHandle<T> CStage::CreateEntity( boost::shared_ptr<T> pEntity, BaseEntityHandle& rBaseEntityHandle, physics::CActorDesc *pPhysActorDesc )
{
	EntityHandle<T> entity_handle = m_pEntitySet->CreateEntity( pEntity, rBaseEntityHandle, pPhysActorDesc );
	return entity_handle;
}


inline void CStage::TerminateEntity( CCopyEntity*& pEntity )
{
	LOG_PRINT_VERBOSE( " - Terminating an entity: " + pEntity->GetName() );

	pEntity->pBaseEntity->OnEntityDestroyed( pEntity );

/*	if( pEntity->pPhysicsActor )
	{
		ReleasePhysicsActor( pEntity->pPhysicsActor );
		pEntity->pPhysicsActor = NULL;
	}*/

	// Release physics actors from the physics scene
	for( size_t i=0; i<pEntity->m_vecpPhysicsActor.size(); i++ )
	{
		ReleasePhysicsActor( pEntity->m_vecpPhysicsActor[i] );
		pEntity->m_vecpPhysicsActor[i] = NULL;
	}
	pEntity->m_vecpPhysicsActor.resize( 0 );

	if( pEntity->GetName().length() != 0 )
		NotifyEntityTerminationToEventManager( pEntity );

	m_pEntitySet->HandleEntityTerminated( pEntity );

	pEntity->Terminate();
	pEntity = NULL;
}

} // namespace amorphous



#endif		/*  __STAGE_H__  */
