#ifndef  __ClothSystem_HPP__
#define  __ClothSystem_HPP__


#include <vector>
#include <boost/shared_ptr.hpp>
#include "gds/Physics/fwd.hpp"
#include "gds/Physics/ShapeDesc.hpp"
#include "gds/Physics/Cloth.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/MotionSynthesis/fwd.hpp"
#include "gds/MotionSynthesis/TransformCacheTree.hpp"
#include "gds/XML/fwd.hpp"


#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"
using namespace GameLib1::Serialization;


/*
collision groups (separate scene for cloth sim)

            cloth   coll obj   attach obj
------------------------------------------------------------
cloth       No      Yes        No
coll obj    ---     No         No
attach obj  ---     ---        No

*/

/*
class CBasicMesh
{
public:
	/// Used, for example, by a module such as cloth simulation engine
	/// which needs to update the vertex positions and normals based on the simulation results
	/// Lock the vertex buffer, copy the vertices, and unlock the vertex buffer
	Result::Name SetVertices( const std::vector<CGeneral3DVertex>& src, U32 flags );
};
*/

/*
class CFixedPoint
{
public:
	Vector3 pos;
	Vector3 normal;
	uint index;

	CFixedPoint()
	:
	pos(Vector3(0,0,0)),
	normal(Vector3(0,0,0)),
	index(0)
	{}
};
*/


/**
 Holds a cloth object of physics engine and mesh that represents the graphics of the cloth.
 - The positions and normals of the mesh are automatically updated by physics engine.
*/
class CClothObject : public IArchiveObjectBase
{
public:
	physics::CCloth *m_pCloth;
	physics::CClothDesc m_Desc;

//	CMeshObjectHandle m_Mesh;
	CCustomMesh m_Mesh;
	std::string m_MeshFilepath;

	// The names of target objects to attach the cloth to.
	// Target objects are selected from CClothSystem::m_ClothAttachObjects.
	// For now, only one target per cloth is supported.
	std::vector<std::string> m_AttachTargetNames;

public:

	CClothObject()
		:
	m_pCloth(NULL)
	{}

	void Init( physics::CScene *pScene );

	bool LoadMesh();

	void Release( physics::CScene *pScene );

	physics::CCloth *GetCloth() { return m_pCloth; }

	const std::vector<std::string>& GetAttachTargetNames() const { return m_AttachTargetNames; }

	void LoadFromXMLNode( CXMLNodeReader& node );

	void Serialize( IArchive& ar, const unsigned int version );
};


/**
Used as either,
- object to attach the cloth to
- collision objects for cloths

*/
class CClothCollisionObject : public IArchiveObjectBase
{
public:

	std::string m_Name;

	physics::CActor *m_pActor; ///< kinematic actor
	std::vector<physics::CShapeDesc *> m_pShapeDescs;

	std::string m_BoneName;

//	boost::shared_ptr<msynth::CBlendNode> m_pBlendNode;
	const msynth::CTransformCacheNode *m_pTransformNode;

public:

	CClothCollisionObject()
	:
	m_pActor(NULL),
	m_pTransformNode(NULL)
	{}

	~CClothCollisionObject();

	const std::string& GetName() const { return m_Name; }

	void InitTransformNode( msynth::CTransformCacheTree& tree );

	void InitPhysics( physics::CScene *pScene );

	void UpdateWorldTransform();

	void Serialize( IArchive& ar, const unsigned int version );

	void LoadFromXMLNode( CXMLNodeReader& node );
};



class CClothSystem : public IArchiveObjectBase
{
public:

	/// set to true if the cloth system has its own scene
	/// set to false if hte cloth system uses a scene borrowed from client
	bool m_OwnsScene;

	/// owned or borrowed reference to a scene
	physics::CScene *m_pScene;

	std::vector<CClothObject> m_Cloths;

	std::vector<CClothCollisionObject> m_ClothAttachObjects;

	std::vector<CClothCollisionObject> m_ClothCollisionObjects;

	msynth::CTransformCacheTree m_TransformCacheTree;

	boost::shared_ptr<msynth::CSkeleton> m_pSkeleton;

public:

	CClothSystem();

	virtual ~CClothSystem() { Release(); }

	Result::Name InitMotionSystem( boost::shared_ptr<msynth::CSkeleton> pSkeleton );

	/**
	When called without arguments, the cloth system creates a new scene for physics objects.
	Creates 3 types of physics objects: cloths, collision objects, and attachment objects.
	*/
	void InitPhysics( physics::CScene *pScene = NULL );

	void Release();

	void UpdateCollisionObjectPoses( const msynth::CKeyframe& keyframe, const Matrix34& world_pose = Matrix34Identity() );

	int FindAttachTarget( const std::string& target_name )
	{
		for( size_t i=0; i<m_ClothAttachObjects.size(); i++ )
		{
			if( m_ClothAttachObjects[i].GetName() == target_name )
				return (int)i;
		}

		return -1;
	}

	uint GetNumCloths() const { return (uint)m_Cloths.size(); }

	CCustomMesh& GetClothMesh( int i ) { return m_Cloths[i].m_Mesh; }

	void LoadMeshes();

	void Serialize( IArchive& ar, const unsigned int version );

	void LoadFromXMLNode( CXMLNodeReader& node );

	Result::Name LoadFromXMLFile( const std::string& xml_filepath );

//	unsigned int GetArchiveObjectID() const { return ; }

	void RenderObjectsForDebugging();
};



#endif  /*  __ClothSystem_HPP__  */
