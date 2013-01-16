#ifndef __BaseEntity_HPP__
#define __BaseEntity_HPP__


#include "fwd.hpp"
#include "EntityGroupHandle.hpp"
#include "gds/Physics/fwd.hpp"
#include "gds/3DMath/AABB3.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/MeshObjectContainer.hpp"
#include "gds/Graphics/MeshContainerRenderMethod.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "gds/Support/TextFileScanner.hpp"
#include "gds/Support/Serialization/Serialization.hpp"


namespace amorphous
{
using namespace serialization;


class CBSPTree;
class CMeshBoneControllerBase;
class CEntityShaderLightParamsLoader;
class CBlendTransformsLoader;
class CRenderContext;
class CCoreBaseEntitiesLoader;


/**
 * holds mesh object for base entity
 * serialized with base entity
 */
class CBE_MeshObjectProperty : public CMeshObjectContainer
{
public:

	/// names of the materials that should be z-sorted
	std::vector<std::string> m_vecTransparentMaterialName;

	/// used by skeletal mesh (not serialized)
	std::vector<CMeshBoneControllerBase*> m_vecpMeshBoneController;

	boost::shared_ptr<CMeshContainerRenderMethod> m_pMeshRenderMethod;

	std::vector< boost::shared_ptr<CShaderParamsLoader> > m_vecpShaderParamsLoader;

//	boost::shared_ptr<CBlendMatricesLoader> m_pBlendMatricesLoader;
	boost::shared_ptr<CBlendTransformsLoader> m_pBlendTransformsLoader;

	boost::shared_ptr<CEntityShaderLightParamsLoader> m_pShaderLightParamsLoader;

	/// subsets of the mesh that should be rendered by the entity
	/// - Holds non-transparant materials(subsets) of the mesh
	/// - Used to separate material(s) that have transparant polygons
	/// - Default: 0 (render all the materials)
	std::vector<int> m_vecTargetMaterialIndex;

	enum ePropertyFlags
	{
		PF_USE_SINGLE_TECHNIQUE_FOR_ALL_MATERIALS = (1 << 0),
	};

	int m_PropertyFlags;

//	int m_NumProgressiveMeshes;

private:

	void ValidateShaderTechniqueTable();

public:

	CBE_MeshObjectProperty();

	CBE_MeshObjectProperty( const std::string& filename );

    ~CBE_MeshObjectProperty();

	bool LoadMeshObject();

	void Release();

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CMeshObjectContainer::Serialize( ar, version );

		ar & m_vecTargetMaterialIndex;

//		if( ar.GetMode() == IArchive::MODE_INPUT )
//		{
			// delete the current mesh controllers
//		}
	}
};


/**
 * base class of base entity
 */
class CBaseEntity : public IArchiveObjectBase
{
protected:

	std::string m_strName;

	/// stage that owns this base entity
	CStageWeakPtr m_pStageWeakPtr;

	/// raw pointer for the stage for quick access
	CStage *m_pStage;

	CBE_MeshObjectProperty m_MeshProperty;

	/// used as a temporary array to hold shader techniques for mesh materials
	/// - See CBaseEntity::DrawMeshObject()
	std::vector<CShaderTechniqueHandle> m_vecShaderTechniqueHolder;

	/// ENTITY_GROUP_MIN is set by default
	CEntityGroupHandle m_EntityGroup;

	/// when copy entities should be rendered in succession, turn 'm_bSweepRender' to true
	/// and implement SweepRender() function (e.g. bullet hole decals)
	bool m_bSweepRender;
	std::vector<CCopyEntity *> m_vecpSweepRenderTable;

	/// flag that defines various attributes of a base entity
	unsigned int m_EntityFlag;

	/// radius of the bounding sphere
	float m_fRadius;

	/// axis aligned bounding box for the base entity (local coord)
	AABB3 m_aabb;

	/// type of the bounding volume
	char m_BoundingVolumeType;

	CBSPTree *m_pBSPTree;	// for collision check against line segment

	/// true for non-collidable entities (ex. items)
	bool m_bNoClip;
	bool m_bNoClipAgainstMap;

	float m_fLife;

protected:

	//
	// Rendering
	//

	void DrawMeshObject( const Matrix34& world_pose,
						 BasicMesh *pMeshObject,
						 const std::vector<int>& vecTargetMaterialIndex,
						 C2DArray<CShaderTechniqueHandle>& rShaderTechHandleTable,
						 int ShaderLOD = 0 );

//	void DrawSkeletalMesh( CCopyEntity* pCopyEnt,
//		                   SkeletalMesh *pSkeletalMesh,
//		                   C2DArray<CShaderTechniqueHandle>& rShaderTechHandleTable,
//						   int ShaderLOD = 0 );

	/// \retval 0 shader for highest resolution mesh
	/// \retval higher_values shader for lower resolution model
	virtual int CalcShaderLOD( CCopyEntity* pCopyEnt ) { return 0; }

	void UpdateLightInfo( CCopyEntity& entity );

	void SetMeshRenderMethod( CCopyEntity& entity );

	void RenderEntity( CCopyEntity& entity );

public:

	CBaseEntity();

	virtual ~CBaseEntity();

	const char* GetName() const { return m_strName.c_str(); }

	const std::string& GetNameString() const { return m_strName; }

	void SetStagePtr( CStageWeakPtr pStage );


	//
	// Rendering
	//

	void Init3DModel();

	void CreateMeshGenerator( CTextFileScanner& scanner );

	// made public since alpha entity needs to call this
	void DrawMeshMaterial( const Matrix34& world_pose, int material_index, int ShaderLOD );

	// made public since alpha entity needs to call this
	void DrawMeshMaterial( const Matrix34& world_pose, int material_index, CShaderTechniqueHandle& shader_tech );

	/// draws a mesh object.
	/// For entities that have a single mesh object as their 3D model
	void Draw3DModel( CCopyEntity* pCopyEnt );

	void SetAsEnvMapTarget( CCopyEntity& entity );

	CBE_MeshObjectProperty& MeshProperty() { return m_MeshProperty; }

	/// loads a base entity on the memory from the disk
	/// base entities that use other base entities during runtime should
	/// pre-load them by calling this function
	bool LoadBaseEntity( CBaseEntityHandle& base_entity_handle );

	inline void RaiseEntityFlag( const unsigned int flag ) { m_EntityFlag |= flag; }
	inline unsigned int GetEntityFlag() const { return m_EntityFlag; }
	inline void ClearEntityFlag( const unsigned int flag ) { m_EntityFlag &= (~flag); }

	float GetRadius() const { return m_fRadius; }

	/// Enable / disable the lighting to the entities created from this base entity
	void SetLighting( bool lighting );

	/// returns an id for an arbitrary entity group
    int GetEntityGroupID( CEntityGroupHandle& entity_group_handle );

	/// returns entity group id of m_EntityGroup;
	/// - retrieves id from entity set if entity group handle is not initialized
	int GetEntityGroupID();

	void CategorizePosition(CCopyEntity* pCopyEnt);

	void NudgePosition(CCopyEntity* pCopyEnt);

	void FreeFall(CCopyEntity* pCopyEnt);

	char SlideMove(CCopyEntity* pCopyEnt);

	void GroundMove(CCopyEntity* pCopyEnt);

	void ApplyFriction(CCopyEntity* pCopyEnt, float fFriction);

	void ApplyFriction(float& rfSpeed, float fFriction);

	void Accelerate(CCopyEntity* pCopyEnt, Vector3& vWishdir, float& fWishspeed, float fAccel);

	void AirAccelerate(CCopyEntity* pCopyEnt, Vector3& vWishdir, float& fWishspeed, float fAccel);

	inline  void ClearSweepRenderTable() { m_vecpSweepRenderTable.resize(0); }

	virtual void Init() {}

	/// defines necessary initializations when a copy entity is created
	/// called after other properties (position/velocity, parent/child links, the entity flag, etc.)
	/// are initialized.
	virtual void InitCopyEntity( CCopyEntity* pCopyEnt ) {}

	/// difines the behavior of the entity during a frame
	virtual void Act(CCopyEntity* pCopyEnt) {}

	/// renders the copy entity
	virtual void Draw(CCopyEntity* pCopyEnt) {}

	virtual void RenderAsShadowCaster(CCopyEntity* pCopyEnt);

	virtual void RenderAsShadowReceiver(CCopyEntity* pCopyEnt);

	virtual void RenderAs( CCopyEntity& entity, CRenderContext& render_context );

	virtual void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}

	virtual void ClipTrace( STrace& rTrace, CCopyEntity* pMyself );

	/// handles the message sent to the entity
	virtual void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) {}

	virtual void OnEntityDestroyed(CCopyEntity* pCopyEnt) {}

	/// called when the entity is set as camera entity for the stage
	virtual void RenderStage(CCopyEntity* pCopyEnt) {}

	virtual void CreateRenderTasks(CCopyEntity* pCopyEnt) {}

	/// load parameters from text file
	void LoadFromFile( CTextFileScanner& scanner );

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner ) { return false; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	/// called when a copy entity of this base entity is being used as a camera entity
	virtual void UpdateCamera( CCopyEntity *pCopyEnt ) {}
	virtual CCamera *GetCamera() { return NULL; }

	// Updates for properties that need to be calculated with fixed size timestep.
	// Mainly used for physics properties.
	virtual void UpdatePhysics( CCopyEntity *pCopyEnt, float dt ) {}

	virtual void UpdateScriptedMotionPath( CCopyEntity* pCopyEnt, CBEC_MotionPath& path );

	virtual void SweepRender() {}

//	void DrawMesh( CCopyEntity *pCopyEnt, int shader_tech_id = SHADER_TECH_INVALID );

	void CreateAlphaEntities( CCopyEntity *pCopyEnt );

	void InitEntityGraphics( CCopyEntity &entity,
                             CShaderHandle& shader = CShaderHandle(),
                             CShaderTechniqueHandle& tech = CShaderTechniqueHandle() );

	virtual void UpdateBaseEntity( float frametime )
	{
		int pass = 1;
	}

	/// implemented by physics base entity
	virtual const physics::CActorDesc& GetPhysicsActorDesc();

	virtual void AdaptToNewScreenSize() {}
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );
	virtual void ReleaseGraphicsResources();

/*
	void RequestTaskTransition( const std::string& next_task_title,
	                            float delay_in_sec = 0,
	                            float fade_out_time_in_sec = -1.0f,
								float fade_in_time_in_sec = -1.0f );

	void RequestTaskTransitionMS( const std::string& next_task_title,
								  int delay_in_ms = 0,
	                              int fade_out_time_in_ms = -1,
								  int fade_in_time_in_ms = -1 );
*/

	//
	// friend class
	//
	friend class CEntitySet;
	friend class CCoreBaseEntitiesLoader;

	enum BaseEntityID
	{
		BE_AREASENSOR,
		BE_BLAST,
		BE_BULLET,
		BE_CLOUD,
		BE_DECAL,
		BE_DOOR,
		BE_DOORCONTROLLER,
		BE_ENEMY,
		BE_EVENTTRIGGER,
		BE_EXPLOSIONSMOKE,
		BE_EXPLOSIVE,
		BE_FIXEDTURRETPOD,
		BE_FLOATER,
		BE_GENERALENTITY,
		BE_HOMINGMISSILE,
		BE_LASERDOT,
		BE_MUZZLEFLASH,
		BE_PARTICLESET,
		BE_PHYSICSBASEENTITY,
		BE_PLATFORM,
		BE_PLAYER,
		BE_PLAYERPSEUDOAIRCRAFT,
		BE_PLAYERPSEUDOLEGGEDVEHICLE,
		BE_PLAYERSHIP,
		BE_POINTLIGHT,
		BE_SMOKETRACE,
		BE_SUPPLYITEM,
		BE_TEXTUREANIMATION,
		BE_TURRET,
		BE_ENEMYAIRCRAFT,
		BE_ENEMYSHIP,
		BE_DIRECTIONALLIGHT,
		BE_SCRIPTEDCAMERA,
		BE_STATICPARTICLESET,
		BE_NOZZLEEXHAUST,       // 00:19 2007/04/18
		BE_STATICGEOMETRY,      // 17:27 2007/08/17
		BE_SKYBOX,              // 17:27 2007/08/17
		BE_CAMERACONTROLLER,    // 02:48 2007/09/09
		BE_INDIVIDUALENTITY,    // 01:23 2008/02/25
		BE_STATICLIQUID,        // 22:21 2011/04/19
		NUM_BASE_ENTITIES
	};

	enum eParams
	{
		USER_BASE_ENTITY_ID_OFFSET = NUM_BASE_ENTITIES
	};

};

} // namespace amorphous



#endif  /*  __BaseEntity_HPP__  */
