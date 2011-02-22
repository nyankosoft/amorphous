#include "BE_PhysicsBaseEntity.hpp"
#include "EntityCollisionGroups.hpp"
#include "CopyEntity.hpp"
#include "3DMath/PrimitivePolygonModelMaker.hpp"
#include "Physics/Enums.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Physics/Preprocessor.hpp"
#include "Support/SafeDeleteVector.hpp"
#include "Utilities/TextFileScannerExtensions.hpp"

using namespace std;
using namespace physics;


CBE_PhysicsBaseEntity::CBE_PhysicsBaseEntity()
:
m_pConvexMesh(NULL)
{
	this->m_EntityFlag |= ( BETYPE_RIGIDBODY | BETYPE_USE_PHYSSIM_RESULTS );
/*
	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;

//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;
*/

	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;

}


CBE_PhysicsBaseEntity::~CBE_PhysicsBaseEntity()
{
	if( m_pConvexMesh )
		physics::PhysicsEngine().ReleaseConvexMesh( m_pConvexMesh );

	SafeDeleteVector( m_ActorDesc.vecpShapeDesc );
}

/*
void CBE_PhysicsBaseEntity::ExtractShapeFromMesh( CCopyEntity *pCopyEnt )
{
	if( m_MeshProperty.m_MeshDesc.ResourcePath.length() == 0 )
		return;

	CShapeDetector detector;
}
*/

void CBE_PhysicsBaseEntity::Init()
{
	if( m_ConvexMeshDesc.IsValid() )
	{
		physics::CStream convex_mesh_stream;
		Result::Name res = physics::Preprocessor().CreateConvexMeshStream( m_ConvexMeshDesc, convex_mesh_stream );

		if( convex_mesh_stream.m_Buffer.buffer().empty() )
			return;

		convex_mesh_stream.m_Buffer.reset_pos();

		CConvexMesh *pConvexMesh = physics::PhysicsEngine().CreateConvexMesh( convex_mesh_stream );
		if( !pConvexMesh )
			return;

		CConvexShapeDesc *pConvexDesc = new CConvexShapeDesc;
		pConvexDesc->pConvexMesh = pConvexMesh;
//		convex_desc.pConvexMesh = pConvexMesh;
		m_ActorDesc.vecpShapeDesc.push_back( pConvexDesc );
	}
}


void CBE_PhysicsBaseEntity::InitCopyEntity( CCopyEntity *pCopyEnt )
{
//	if( 0 < m_ShapeName.length() )
//		ExtractShapeFromMesh();

}


static const int sg_cylinder_mesh_side_subdivisions = 16;


void CBE_PhysicsBaseEntity::CreatePseudoCylinderDescFromBoxes( float radius, float height )
{
	int num_subdivisions = sg_cylinder_mesh_side_subdivisions - sg_cylinder_mesh_side_subdivisions % 2;
	int num_boxes = num_subdivisions / 2;
	num_boxes -= num_boxes % 2;
	float box_radius_x = radius * cos( (float)PI / (float)num_subdivisions );
	float box_radius_y = radius * sin( (float)PI / (float)num_subdivisions );
//	num_boxes = 1;
	for( int i=0; i<num_boxes; i++ )
	{
		CBoxShapeDesc *pBoxDesc = new CBoxShapeDesc;
		pBoxDesc->vSideLength = Vector3( box_radius_x, height * 0.5f, box_radius_y );
		pBoxDesc->LocalPose.matOrient = Matrix33RotationY( 2.0f * (float)PI * (float)i / (float)num_boxes );
		pBoxDesc->MaterialIndex = 1;
		m_ActorDesc.vecpShapeDesc.push_back( pBoxDesc );
	}

}


void CBE_PhysicsBaseEntity::CreatePseudoCylinderMeshDesc( float radius, float height )
{
	// The "vertices only" version.
	// Since triangle indices are not needed to create a convex mesh,
	// use this if skipping index calculations brings certain benefits.
	// NOTE: the code is not tested and may take some debugging to make it work.
/*
	for( int i=0; i<2; i++ )
	{
		const float y = (i==0) ? h * 0.5f : -h * 0.5f;
		for( int j=0; j<num_segments; j++ )
		{
			const float angle = (float)j / (float)num_segments * 2.0f * (float)PI;
			Vector3 pos = Vector3( cos( angle ) * radius, y, sin( angle ) * radius );
			vertices.push_back( pos );

//			LOG_PRINT( "cylinder vertex: " + to_string( pos ) );

		}
	}
*/
	// This calculate the triangle mesh of a cylinder, i.e. both vertices and 
	vector<Vector3> normals;
	vector< vector<int> > polygons;
	float radii[2] = { radius, radius };
	CreateCylinder( height, radii, sg_cylinder_mesh_side_subdivisions, PrimitiveModelStyle::EDGE_VERTICES_WELDED,
		m_ConvexMeshDesc.m_vecVertex, normals, polygons );

	const int num_polygons = (int)polygons.size();
	m_ConvexMeshDesc.m_vecIndex.reserve( num_polygons );
	for( int i=0; i<num_polygons; i++ )
	{
		for( size_t j=0; j<polygons[i].size()-2; j++ )
		{
			m_ConvexMeshDesc.m_vecIndex.push_back( 0 );
			m_ConvexMeshDesc.m_vecIndex.push_back( j+1 );
			m_ConvexMeshDesc.m_vecIndex.push_back( j+2 );
		}
	}

	m_ConvexMeshDesc.m_vecMaterialIndex.resize( m_ConvexMeshDesc.m_vecIndex.size() / 3, 0 );
}



/**
 * base entites derived from CPhysicsBaseEntity must call this functiont explicitly
 * in their own LoadSpecificPropertiesFromFile()
 * i.e. put CPhysicsBaseEntity::LoadSpecificPropertiesFromFile(pcLine) at the start of
 * the function
*/
bool CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	Vector3 vBoxSideLength = Vector3(0,0,0), vLocalPos = Vector3(0,0,0);
	float fAngle=0;
	float fRadius=0, fLength=0;
	bool enable_physics_sim = true;
	string coll_group;

	if( scanner.TryScanBool( "PHYSICS", "TRUE/FALSE", enable_physics_sim ) )
	{
		if( enable_physics_sim )	RaiseEntityFlag( BETYPE_RIGIDBODY );
		if( !enable_physics_sim )	ClearEntityFlag( BETYPE_RIGIDBODY );
	}

	if( scanner.TryScanLine( "PHYS_COLL_GROUP", coll_group ) )
	{
		if( coll_group == "STATICGEOMETRY" )	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_STATICGEOMETRY;
		else if( coll_group == "PLAYER" )		m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_PLAYER;
		else if( coll_group == "DOOR" )			m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_DOOR;
		else if( coll_group == "OTHERS" )		m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
		else if( coll_group == "NOCLIP" )		m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_NOCLIP;
		else if( coll_group == "NO_COLLISION" )	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_NOCLIP;
	}

	if( scanner.TryScanLine( "MASS", m_ActorDesc.BodyDesc.fMass ) ) return true;

	if( scanner.TryScanLine( "SHAPE_BOX", vBoxSideLength ) )
	{
		// parse the full lengths of the 3 edges of the box
		CBoxShapeDesc *pBoxDesc = new CBoxShapeDesc;
		pBoxDesc->vSideLength = vBoxSideLength * 0.5f;
		m_ActorDesc.vecpShapeDesc.push_back( pBoxDesc );
		return true;
	}

	if( scanner.TryScanLine( "SHAPE_CAPSULE", fRadius, fLength ) )
	{
		CCapsuleShapeDesc *pCapsuleDesc = new CCapsuleShapeDesc;
		pCapsuleDesc->fLength = fLength;
		pCapsuleDesc->fRadius = fRadius;
		m_ActorDesc.vecpShapeDesc.push_back( pCapsuleDesc );
		return true;
	}

	if( scanner.TryScanLine( "SHAPE_CYLINDER", fRadius, fLength ) )
	{
		CreatePseudoCylinderMeshDesc( fRadius, fLength );
//		CreatePseudoCylinderDescFromBoxes( fRadius, fLength );
		return true;
	}

/*	float f0=0, f1=0, f2=0;
	string shape_name;
	if( scanner.TryScanLine( "SHAPE", shape_name, f0, f1, f2 ) )
	{
//		if( shape_name == "BOX" )                m_ShapeName = "box";
//		else if( shape_name == "SPHERE" )        m_ShapeName = "sphere";
//		else if( shape_name == "CAPSULE" )       m_ShapeName = "capsule";
//		else if( shape_name == "CYLINDER" )      m_ShapeName = "cylinder";
//		else if( shape_name == "CONVEX" )        m_ShapeName = "convex";
//		else if( shape_name == "TRIANGLE_MESH" ) m_ShapeName = "triangle_mesh";
		if( shape_name == "BOX" )
		{
		}
		else if( shape_name == "CAPSULE" )
		{
		}
		else if( shape_name == "CYLINDER" )
		{
			fRadius = f0;
			fLength = f1;
		}
	}*/

	if( scanner.TryScanLine( "SHAPE_LOCAL_POSITION", vLocalPos ) )
	{
		if( m_ActorDesc.vecpShapeDesc.size() == 0 )
			return false;

		m_ActorDesc.vecpShapeDesc.back()->LocalPose.vPosition = vLocalPos;
		return true;
	}

	float heading=0, pitch=0, bank=0;
	if( scanner.TryScanLine( "SHAPE_LOCAL_ROTATIONS", heading, pitch, bank ) )
	{
		if( m_ActorDesc.vecpShapeDesc.size() == 0 )
			return false;

		m_ActorDesc.vecpShapeDesc.back()->LocalPose.matOrient
			= Matrix33RotationHPR_deg( heading, pitch, bank );
	}

	if( scanner.GetTagString().find( "SHAPE_LOCAL_ROT" ) == 0 )
	{
		// the tag starts with "SHAPE_LOCAL_ROT"
		string str;
		scanner.ScanLine( str, fAngle );

		// convert from degree to radisan
		fAngle = fAngle / 360.0f * 2.0f * 3.141592f;

		if( m_ActorDesc.vecpShapeDesc.size() == 0 )
			return false;

		const string& tag = scanner.GetTagString();
		Matrix33 matRot;
		if( tag == "SHAPE_LOCAL_ROT_X" )		matRot.SetRotationX(fAngle);
		else if( tag == "SHAPE_LOCAL_ROT_Y" )	matRot.SetRotationY(fAngle);
		else if( tag == "SHAPE_LOCAL_ROT_Z" )	matRot.SetRotationZ(fAngle);
		else return false;

		Matrix33& rmatLocalOrient = m_ActorDesc.vecpShapeDesc.back()->LocalPose.matOrient;
		rmatLocalOrient = matRot * rmatLocalOrient;	// the rotation is applied following ealier rotations

		return true;
	}

	if( scanner.GetTagString() == "DISABLE_FREEZING" )
	{
//		m_ActorDesc.bAllowFreezing = false;
		m_ActorDesc.ActorFlags |= ActorFlag::DISABLE_FREEZING;
		return true;
	}

	if( scanner.GetTagString() == "BODY_FLAGS" )
	{
		vector<string> flags;
		SeparateStrings( flags, scanner.GetCurrentLine().c_str(), "| \t" );
		for( size_t i=0; i<flags.size(); i++ )
		{
			if( flags[i] == "DISABLE_GRAVITY" )
				m_ActorDesc.BodyDesc.Flags |= BodyFlag::DisableGravity;
		}
	}

//	if( scanner.TryScanLine( "S_FRICTION", m_fStaticFriction ) ) return true;
//	if( scanner.TryScanLine( "D_FRICTION", m_fDynamicFriction ) ) return true;

	return false;
}


void CBE_PhysicsBaseEntity::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_ActorDesc;

	ar & m_ConvexMeshDesc;

//	ar & m_fStaticFriction & m_fDynamicFriction;
}
