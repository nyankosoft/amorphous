#ifndef __ENTITYNODE_H__
#define __ENTITYNODE_H__


#include "amorphous/base.hpp"
#include "amorphous/Graphics/fwd.hpp"

#include "fwd.hpp"
#include "CopyEntity.hpp"
#include "OverlapTestAABB.hpp"


namespace amorphous
{


/**
 * node for entity tree
 * - the entities are linked to the entity node
 * - used for both rendering and collision detection
 */
class EntityNode
{
	bool leaf;
	SPlane m_Plane;

//	short m_sNumEnts;	// how many entities are currently in this sub-space
	short sFrontChild;
	short sBackChild;
	short sParent;

	/// link list of entities
	/// - The first entity of in the list is stored in m_EntityLinkHead.pNext
	/// - m_EntityLinkHead.pOwner and m_EntityLinkHead.pPrev is always NULL
	CLinkNode<CCopyEntity> m_EntityLinkHead;

	CLinkNode<LightEntity> m_LightEntityLinkHead;

	short m_sCellIndex;	// index to the corresponding cell - only valid for complete leaf entity node

	/// bounding-box that represents sub-space of the node
	AABB3 m_AABB;

	EntityManager* m_pEntitySet;

	CStage *m_pStage;

//	inline bool CheckCollisionGroup( int group_index, const std::vector<int>& vecTargetGroup );
	inline bool CheckCollisionGroup( int group0, int group1 );

public:

	static int ms_NumRenderedEntities;

	EntityNode();

	void CheckPosition_r(STrace& tr, EntityNode* paEntTree);
	void CheckPosition_r(CTrace& tr, EntityNode* paEntTree);
	void GetVisibleEntities_r(CViewFrustumTest& vf_test, EntityNode* paEntTree);
	void GetOverlappingEntities( COverlapTestAABB& overlap_test, EntityNode* paEntTree );
	void ClipTrace_r(STrace& tr, EntityNode* paEntTree);

	/// Link 'pEntity' to this entity node.
	/// 'pEntity' is set to the head element of the entity list of this entity node
	void Link(CCopyEntity* pEntity) { m_EntityLinkHead.InsertNext( &pEntity->m_EntityLink ); }

	/// Render all the entites on this entity tree node.
	void RenderEntities( EntityRenderer& entity_renderer, Camera& rCam );

	/// Recursively render all the entites in this sub-space.
	void RenderEntitiesWithDownwardTraversal(
		EntityNode *pEntityTree,
		EntityRenderer& entity_renderer,
		Camera& rCam,
		bool do_camera_frustom_culling
	);

	/// link a light entity entity management
	void LinkLightEntity_r(LightEntity *pLightEntity, EntityNode* paEntTree);

	/// checks if the light is reaching the entity
	/// if it is true, registers the light index to the entity
	void CheckLight_r( CCopyEntity *pEntity, EntityNode* paEntTree );

	friend class EntityRenderManager;
	friend class EntityManager;
};

} // namespace amorphous



#endif  /*  __ENTITYNODE_H__  */
