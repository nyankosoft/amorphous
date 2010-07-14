#include "ClothSystem.hpp"
#include "gds/Graphics/MeshGenerators.hpp"
#include "gds/Physics.hpp"
#include "gds/XML.hpp"
#include "gds/MotionSynthesis/MotionFSM.hpp"
#include "gds/Support/Log/DefaultLog.hpp"
#include "gds/Utilities/Physics/PhysicsMeshUtility.hpp"
//#include "gds/Support/lfs.hpp"

using namespace physics;
using namespace std;
using namespace boost;


static U16 sg_ClothGroup    = 0;
static U16 sg_CollObjGroup  = 1;
static U16 sg_AttachbjGroup = 2;


/**

Allowed formats

<any_name_for_pose_node>
	<t>0 1 2</t>
	<hpb>30 60 0</hpb>
</any_name_for_pose_node>

<any_name_for_pose_node>
	<pos>0 1 2</pos>
	<hpb>30 60 0</hpb>
</any_name_for_pose_node>

- angles are measured in degrees
*/
inline Matrix34 GetPose( CXMLNodeReader& parent_node, const std::string& element_name )
{
	Matrix34 dest( Matrix34Identity() );

	// translation
	Vector3& t = dest.vPosition;
	string text;
	parent_node.GetChildElementTextContent( element_name + "/t", text );
	if( 0 < text.length() )
	{
		sscanf( text.c_str(), "%f %f %f", &t.x, &t.y, &t );
	}
	else
	{
		parent_node.GetChildElementTextContent( element_name + "/pos", text );
		if( 0 < text.length() )
			sscanf( text.c_str(), "%f %f %f", &t.x, &t.y, &t.z );
	}

	Matrix33& r = dest.matOrient;
	parent_node.GetChildElementTextContent( element_name + "/hpb", text );
	if( 0 < text.length() )
	{
		float h=0,p=0,b=0;
		sscanf( text.c_str(), "%f %f %f", &h, &p, &b );
		r = Matrix33RotationY( deg_to_rad(h) )
		  * Matrix33RotationX( deg_to_rad(p) )
		  * Matrix33RotationZ( deg_to_rad(b) );
	}

	// Support <rotations heading="30" pitch="60" bank="0"></rotations> ?
//	else
//	{
//		CXMLNodeReader r_node = parent_node.GetChild( element_name + "/rotations" );
//		h = r_node.GetAttributeText( "heading" );
//		p = r_node.GetAttributeText( "pitch" );
//		b = r_node.GetAttributeText( "bank" );
//	}

	return dest;
}



void AttachClothToActor( CCloth *pCloth, CActor *pActor )
{
	if( !pCloth || !pActor )
		return;

	for( int i=0; i<pActor->GetNumShapes(); i++ )
		pCloth->AttachToShape( pActor->GetShape(i), 0 );
}

//=====================================================================
// CClothObject
//=====================================================================

void CClothObject::Init( physics::CScene *pScene )
{
	CSceneUtility util(pScene);
//	m_pCloth = util.CreateClothFromMesh( m_Mesh, Matrix34Identity(), true );

	// load mesh?

	if( m_pCloth )
	{
		m_pCloth->SetGroup( sg_ClothGroup );
	}
}


bool CClothObject::LoadMesh()
{
	bool res = m_Mesh.LoadFromFile( m_MeshFilepath );
	return res;
}


void CClothObject::Release( physics::CScene *pScene )
{
	if( m_pCloth )
		pScene->ReleaseCloth( m_pCloth );
}


void CClothObject::LoadFromXMLNode( CXMLNodeReader& node )
{
	CXMLNodeReader cloth_params = node.GetChild( "cloth_params" );
	cloth_params.GetAttributeValue("thickness",            m_Desc.Thickness );
	cloth_params.GetAttributeValue("density",              m_Desc.Density );
	cloth_params.GetAttributeValue("stretching_stiffness", m_Desc.StretchingStiffness );
	cloth_params.GetAttributeValue("bending_stiffness",    m_Desc.BendingStiffness );
	cloth_params.GetAttributeValue("friction",             m_Desc.Friction );

	node.GetChildElementTextContent( "mesh", m_MeshFilepath );

	m_AttachTargetNames.resize( 1 );
	node.GetChildElementTextContent( "attach_target", m_AttachTargetNames[0] );
}


void CClothObject::Serialize( IArchive& ar, const unsigned int version )
{
}



//=====================================================================
// CClothCollisionObject
//=====================================================================

CClothCollisionObject::~CClothCollisionObject()
{
	SafeDeleteVector( m_pShapeDescs );
}


void CClothCollisionObject::InitTransformNode( msynth::CTransformCacheTree& tree )
{
	m_pTransformNode = tree.GetNode( m_BoneName );
}


void CClothCollisionObject::InitPhysics( CScene *pScene )
{
	CActorDesc actor_desc;
	const size_t num_shapes = m_pShapeDescs.size();
	actor_desc.vecpShapeDesc.resize( num_shapes );
	for( size_t j=0; j<num_shapes; j++ )
		actor_desc.vecpShapeDesc[j] = m_pShapeDescs[j];

	actor_desc.WorldPose = Matrix34Identity();
	m_pActor = pScene->CreateActor( actor_desc );
}


void CClothCollisionObject::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_Name;

	CShapeDescFactory factory;
	ar.Polymorphic( m_pShapeDescs, factory );
	ar & m_BoneName;
}


void CClothCollisionObject::LoadFromXMLNode( CXMLNodeReader& node )
{
	node.GetChildElementTextContent( "name", m_Name );
	node.GetChildElementTextContent( "bone_name", m_BoneName );
	CXMLNodeReader shapes_node = node.GetChild( "shapes" );
	vector<CXMLNodeReader> shapes = shapes_node.GetImmediateChildren( "shape" );
	m_pShapeDescs.resize( shapes.size() );
	for( size_t i=0; i<shapes.size(); i++ )
	{
		CShapeDesc *pShape = NULL;
		const string type = shapes[i].GetAttributeText( "type" );
		if( type == "box" )
		{
			CBoxShapeDesc *pBox = new CBoxShapeDesc;
			shapes[i].GetChildElementTextContent( "radii", pBox->vSideLength );
			pShape = pBox;
		}
		else if( type == "capsule" )
		{
			CCapsuleShapeDesc *pCap = new CCapsuleShapeDesc;
			shapes[i].GetChildElementTextContent( "radius", pCap->fRadius );
			shapes[i].GetChildElementTextContent( "length", pCap->fLength );
			pShape = pCap;
		}
		else if( type == "sphere" )
		{
			CSphereShapeDesc *pBall = new CSphereShapeDesc;
			shapes[i].GetChildElementTextContent( "radius", pBall->Radius );
			pShape = pBall;
		}

		if( pShape )
		{
			pShape->LocalPose = GetPose( node, "local_pose" );
			m_pShapeDescs.push_back( pShape );
		}
	}
}


void CClothCollisionObject::UpdateWorldTransform()
{
	if( !m_pTransformNode || !m_pActor )
		return;
	
	Matrix34 world_transform;
	m_pTransformNode->GetWorldTransform( world_transform );

	m_pActor->SetWorldPose( world_transform );
}



//=====================================================================
// CClothSystem
//=====================================================================

CClothSystem::CClothSystem()
:
m_OwnsScene(true),
m_pScene(NULL)
{}


Result::Name CClothSystem::InitMotionSystem( boost::shared_ptr<msynth::CSkeleton> pSkeleton )
{
	if( !pSkeleton )
		return Result::INVALID_ARGS;

	msynth::CSkeleton& skeleton = *pSkeleton;

	m_TransformCacheTree.Create( skeleton );

	set<string> target_bone_names;
	for( size_t i=0; i<m_ClothAttachObjects.size(); i++ )
		target_bone_names.insert( m_ClothAttachObjects[i].m_BoneName );
	for( size_t i=0; i<m_ClothCollisionObjects.size(); i++ )
		target_bone_names.insert( m_ClothCollisionObjects[i].m_BoneName );

	m_TransformCacheTree.UpdateActiveNodes( target_bone_names );

	for( size_t i=0; i<m_ClothAttachObjects.size(); i++ )
		m_ClothAttachObjects[i].InitTransformNode( m_TransformCacheTree );
	for( size_t i=0; i<m_ClothCollisionObjects.size(); i++ )
		m_ClothCollisionObjects[i].InitTransformNode( m_TransformCacheTree );

	return Result::SUCCESS;
}


void CClothSystem::InitPhysics( CScene *pScene )
{
	if( pScene )
	{
		m_OwnsScene = false;
		m_pScene = pScene;
	}
	else
	{
		m_OwnsScene = true;
		CSceneDesc scene_desc;
		m_pScene = PhysicsEngine().CreateScene( scene_desc );
		pScene = m_pScene;
	}

	size_t num_attach_objects = m_ClothAttachObjects.size();
	for( size_t i=0; i<num_attach_objects; i++ )
	{
		CClothCollisionObject &obj = m_ClothAttachObjects[i];
		obj.InitPhysics( pScene );
		if( obj.m_pActor )
			obj.m_pActor->SetCollisionGroup( sg_AttachbjGroup );
	}

	size_t num_coll_objects = m_ClothCollisionObjects.size();
	for( size_t i=0; i<num_coll_objects; i++ )
	{
		CClothCollisionObject &obj = m_ClothCollisionObjects[i];
		obj.InitPhysics( pScene );
		if( obj.m_pActor )
			obj.m_pActor->SetCollisionGroup( sg_CollObjGroup );
	}

	size_t num_cloths = m_Cloths.size();
	for( size_t i=0; i<num_cloths; i++ )
	{
		m_Cloths[i].Init( pScene );
		for( size_t j=0; j<m_Cloths[i].GetAttachTargetNames().size(); j++ )
		{
			int index = FindAttachTarget( m_Cloths[i].GetAttachTargetNames()[j] );
			if( 0 <= index )
				AttachClothToActor( m_Cloths[i].GetCloth(), m_ClothAttachObjects[index].m_pActor );
		}
	}

/*	{
		CClothCollisionObject& coll_obj = m_ClothCollisionObjects[i];

		CActorDesc actor_desc;
		const size_t num_shapes = coll_obj.m_pShapeDescs.size();
		actor_desc.vecpShapeDesc.resize( num_shapes );
		for( size_t j=0; j<num_shapes; j++ )
			actor_desc.vecpShapeDesc[j] = coll_obj.m_pShapeDescs[j];

		actor_desc.WorldPose = Matrix34Identity();
		coll_obj.m_pActor = pScene->CreateActor( actor_desc );
	}*/

	// set collision groups
	if( m_pScene )
	{
		if( m_OwnsScene )
		{
			U16 cloth      = sg_ClothGroup;
			U16 coll_obj   = sg_CollObjGroup;
			U16 attach_obj = sg_AttachbjGroup;
			m_pScene->SetGroupCollisionFlag( cloth,      cloth,      false );
			m_pScene->SetGroupCollisionFlag( cloth,      coll_obj,   true );
			m_pScene->SetGroupCollisionFlag( cloth,      attach_obj, false );
			m_pScene->SetGroupCollisionFlag( coll_obj,   coll_obj,   false );
			m_pScene->SetGroupCollisionFlag( coll_obj,   attach_obj, false );
			m_pScene->SetGroupCollisionFlag( attach_obj, attach_obj, false );
		}
		else
		{
			// ???
		}
	}
}

/*
void CClothSystem::SetBlendNodes( msynth::CMotionFSM& motion_fsm )
{

}
*/

void CClothSystem::UpdateCollisionObjectPoses( const msynth::CKeyframe& keyframe, const Matrix34& world_pose )
{
	if( !m_pSkeleton )
		return;

	// Update world transforms stored in the cache nodes.
	m_TransformCacheTree.CalculateWorldTransforms( *m_pSkeleton, keyframe, world_pose );

	// Update the poses of the attach objects
	const size_t num_attach_objects = m_ClothAttachObjects.size();
	for( size_t i=0; i<num_attach_objects; i++ )
	{
		CClothCollisionObject& attach_obj = m_ClothAttachObjects[i];

//		attach_obj.UpdateWorldTransform();
		Matrix34 world_pose( Matrix34Identity() );
		attach_obj.m_pActor->SetWorldPose( world_pose );
	}

	// Update the poses of the collison objects
	const size_t num_coll_objects = m_ClothCollisionObjects.size();
	for( size_t i=0; i<num_coll_objects; i++ )
	{
		CClothCollisionObject& coll_obj = m_ClothCollisionObjects[i];

//		coll_obj.UpdateWorldTransform();
		Matrix34 world_pose( Matrix34Identity() );
		coll_obj.m_pActor->SetWorldPose( world_pose );
	}
}


void CClothSystem::Release()
{
	CScene *pScene = m_pScene;
	if( !pScene )
		return; // already released

	size_t num_attach_objects = m_ClothAttachObjects.size();
	for( size_t i=0; i<num_attach_objects; i++ )
		pScene->ReleaseActor( m_ClothAttachObjects[i].m_pActor );

	size_t num_coll_objects = m_ClothCollisionObjects.size();
	for( size_t i=0; i<num_coll_objects; i++ )
		pScene->ReleaseActor( m_ClothCollisionObjects[i].m_pActor );

	size_t num_cloths = m_Cloths.size();
	for( size_t i=0; i<num_cloths; i++ )
	{
		m_Cloths[i].Release( pScene );
	}

	if( m_OwnsScene )
		PhysicsEngine().ReleaseScene( m_pScene );

	m_pScene = NULL;
}


void CClothSystem::LoadMeshes()
{
	size_t num_cloths = m_Cloths.size();
	for( size_t i=0; i<num_cloths; i++ )
	{
		bool res = m_Cloths[i].LoadMesh();
	}
}


void CClothSystem::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_Cloths;
	ar & m_ClothAttachObjects;
	ar & m_ClothCollisionObjects;

//	CTriangleMeshDesc::Serialize( ar, version );
}


Result::Name CClothSystem::LoadFromXMLFile( const string& xml_filepath )
{
	shared_ptr<CXMLDocument> pDoc = CreateXMLDocument( xml_filepath );
	if( !pDoc )
		return Result::UNKNOWN_ERROR;

	CXMLNodeReader root_node = pDoc->GetRootNodeReader();
	LoadFromXMLNode( root_node );

	return Result::SUCCESS;
}


void CClothSystem::LoadFromXMLNode( CXMLNodeReader& node )
{
	CXMLNodeReader cloths_node = node.GetChild( "cloths" );
	vector<CXMLNodeReader> cloths = cloths_node.GetImmediateChildren( "cloth" );
	m_Cloths.resize( cloths.size() );
	for( size_t i=0; i<cloths.size(); i++ )
		m_Cloths[i].LoadFromXMLNode( cloths[i] );

	// cloth attach objects

	CXMLNodeReader attach_objs_node = node.GetChild( "attach_objects" );
	vector<CXMLNodeReader> attach_objs = attach_objs_node.GetImmediateChildren( "object" );
	m_ClothAttachObjects.resize( attach_objs.size() );
	for( size_t i=0; i<attach_objs.size(); i++ )
		m_ClothAttachObjects[i].LoadFromXMLNode( attach_objs[i] );

/*	string mesh_filepath = attach_objs_node.GetChild("mesh").GetTextContent();
	if( 0 < mesh_filepath.length() )
	{
//		mesh_filepath = path( path(m_XMLFilepath).parent_path() / mesh_filepath ).string();
		mesh_filepath = connect_path( lfs::get_parent_path(m_XMLFilepath), mesh_filepath );
		C3DMeshModelArchive mesh_archive;
		bool mesh_loaded = mesh_archive.LoadFromFile( mesh_filepath );
		if( mesh_loaded )
		{
			shared_ptr<CGeneral3DMesh> pMesh = ToGeneral3DMesh( mesh_archive );
			if( pMesh )
			{
				connected_sets = GetConnectedSets( pMesh, surface_name );
				for( int i=0; i<connected_sets.size(); i++ )
				{
					if( IsCapsule( connected_sets[i] ) )
					{
						// add a capsule shape
					}
					else if( IsSphere( connected_sets[i] ) )
					{
						// add a sphere shape
					}
				}
			}
		}
	}*/

	// cloth collision objects

	CXMLNodeReader coll_objs_node = node.GetChild( "collision_objects" );
	vector<CXMLNodeReader> coll_objs = coll_objs_node.GetImmediateChildren( "object" );
	m_ClothCollisionObjects.resize( coll_objs.size() );
	for( size_t i=0; i<coll_objs.size(); i++ )
		m_ClothCollisionObjects[i].LoadFromXMLNode( coll_objs[i] );

//	node.GetChildElementTextContent( "name",       m_Name );
}


void CClothSystem::RenderObjectsForDebugging()
{
	vector<CClothCollisionObject> *pObjs[] = { &m_ClothAttachObjects, &m_ClothCollisionObjects };
	SFloatRGBAColor colors[] = { SFloatRGBAColor::Blue(), SFloatRGBAColor::Red() };

	static CMeshObjectHandle m_SphereMesh;
	static CMeshObjectHandle m_CapsuleMesh;
	if( !m_SphereMesh.IsLoaded() )
	{
		CMeshResourceDesc mesh_desc;
		mesh_desc.pMeshGenerator.reset( new CSphereMeshGenerator(CSphereDesc()) );
		m_SphereMesh.Load( mesh_desc );
	}

	for( int i=0; i<2; i++ )
	{
		vector<CClothCollisionObject>& objs = *pObjs[i];
		for( int j=0; j<(int)objs.size(); j++ )
		{
			CClothCollisionObject& obj = objs[j];
			if( !obj.m_pActor )
				continue;

			for( int k=0; k<obj.m_pActor->GetNumShapes(); k++ )
			{
				CShape *pShape = obj.m_pActor->GetShape(k);
				switch( pShape->GetType() )
				{
				case PhysShape::Sphere:
					{
						CSphereShape *pSphere = dynamic_cast<CSphereShape *>(pShape);
						if( !pSphere )
							break;

//						Get3DPrimitiveRenderer().RenderSphere( pose, pSphere->GetRadius(), colors[i] );
					}
					break;

				case PhysShape::Capsule:
					{
						CCapsuleShape *pCap = dynamic_cast<CCapsuleShape *>(pShape);
						if( !pCap )
							break;

//						Get3DPrimitiveRenderer().RenderCapsule( pose, pCap->GetRadius(), pCap->GetLength(), colors[i] );
					}
					break;

				default:
					break;
				}
			}
		}


	}

	const size_t num_attach_objects = m_ClothAttachObjects.size();
	for( size_t i=0; i<num_attach_objects; i++ )
	{
		CClothCollisionObject& attach_obj = m_ClothAttachObjects[i];

//		attach_obj.UpdateWorldTransform();
		Matrix34 world_pose( Matrix34Identity() );
		attach_obj.m_pActor->SetWorldPose( world_pose );
	}

	// Update the poses of the collison objects
	const size_t num_coll_objects = m_ClothCollisionObjects.size();
	for( size_t i=0; i<num_coll_objects; i++ )
	{
		CClothCollisionObject& coll_obj = m_ClothCollisionObjects[i];

//		coll_obj.UpdateWorldTransform();
		Matrix34 world_pose( Matrix34Identity() );
		coll_obj.m_pActor->SetWorldPose( world_pose );
	}
}
