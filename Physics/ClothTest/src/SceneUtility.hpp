#ifndef __SceneUtility_HPP__
#define __SceneUtility_HPP__


#include "gds/Physics.hpp"
using namespace physics;


class CSceneUtility
{
	CScene *m_pScene;

	CActor *CreateBoxActor( const Vector3& side_lengths, const Matrix34& world_pose, U32 body_flags );

public:

	CSceneUtility( CScene *pScene )
		:
	m_pScene(pScene)
	{}

	CActor *CreateBoxActor( const Vector3& side_lengths, const Matrix34& world_pose, float mass = 1.0f );

	CActor *CreateStaticBoxActor( const Vector3& side_lengths, const Matrix34& world_pose );

	CActor *CreateKinematicBoxActor( const Vector3& side_lengths, const Matrix34& world_pose );

	CActor *CreateStaticCapsuleActor( float radius, float length, const Matrix34& world_pose );

	CActor *CreateStaticSphereActor( Scalar radius, const Matrix34& world_pose );

	CActor *CreateStaticTriangleMeshFromMeshFile( const std::string& filepath );

	CCloth *CreateClothFromMeshFile( const std::string& filepath, const Matrix34& world_pose );

	CCloth *CreateClothFromMesh( CCustomMesh& mesh, const Matrix34& world_pose, bool set_mesh_data );
};


CActor *CSceneUtility::CreateBoxActor( const Vector3& side_lengths, const Matrix34& world_pose, float mass )
{
	if( !m_pScene )
		return NULL;

	CActorDesc actor_desc;
	actor_desc.WorldPose = world_pose;
	actor_desc.BodyDesc.fMass = mass;

	CBoxShapeDesc box;
	box.MaterialIndex = 1;
	box.vSideLength = side_lengths;
	actor_desc.vecpShapeDesc.push_back(&box);

	return m_pScene->CreateActor( actor_desc );
}


CActor *CSceneUtility::CreateBoxActor( const Vector3& side_lengths, const Matrix34& world_pose, U32 body_flags )
{
	if( !m_pScene )
		return NULL;

	CActorDesc actor_desc;
	actor_desc.WorldPose = world_pose;
	actor_desc.BodyDesc.Flags = body_flags;

	CBoxShapeDesc box;
	box.MaterialIndex = 0;
	box.vSideLength = side_lengths;
	actor_desc.vecpShapeDesc.push_back(&box);

	return m_pScene->CreateActor( actor_desc );
}


CActor *CSceneUtility::CreateStaticBoxActor( const Vector3& side_lengths, const Matrix34& world_pose )
{
	return CreateBoxActor( side_lengths, world_pose, (U32)PhysBodyFlag::Static );
}


CActor *CSceneUtility::CreateKinematicBoxActor( const Vector3& side_lengths, const Matrix34& world_pose )
{
	return CreateBoxActor( side_lengths, world_pose, (U32)PhysBodyFlag::Kinematic );
}


CActor *CSceneUtility::CreateStaticCapsuleActor( float radius, float length, const Matrix34& world_pose )
{
	if( !m_pScene )
		return NULL;

	CActorDesc actor_desc;
	actor_desc.WorldPose = world_pose;
	actor_desc.BodyDesc.Flags = PhysBodyFlag::Static;

	CCapsuleShapeDesc cap;
	cap.MaterialIndex = 0;
	cap.fRadius = radius;
	cap.fLength = length;
	actor_desc.vecpShapeDesc.push_back(&cap);

	return m_pScene->CreateActor( actor_desc );
}

U16 g_mesh_indices[0xFFFF];

CCloth *CSceneUtility::CreateClothFromMesh( CCustomMesh& mesh, const Matrix34& world_pose, bool set_mesh_data )
{
	CClothMeshDesc cloth_mesh_desc;
	const uint num_vertices = mesh.GetNumVertices();
	cloth_mesh_desc.m_vecVertex.resize( num_vertices, Vector3(0,0,0) );
	for( uint i=0; i<num_vertices; i++ )
		cloth_mesh_desc.m_vecVertex[i] = mesh.GetPosition(i);

	const uint num_indices = mesh.GetNumIndices();
	cloth_mesh_desc.m_vecIndex.resize( num_indices, 0 );
	for( uint i=0; i<num_indices; i++ )
		cloth_mesh_desc.m_vecIndex[i] = mesh.GetIndex(i);

	CStream cloth_mesh_stream;
	Result::Name res = Preprocessor().CreateClothMeshStream( cloth_mesh_desc, cloth_mesh_stream );

	if( res != Result::SUCCESS )
		return NULL;

	cloth_mesh_stream.m_Buffer.reset_pos();

	CTriangleMesh *pClothMesh = PhysicsEngine().CreateClothMesh( cloth_mesh_stream );

	CClothDesc cloth_desc;
	cloth_desc.pClothMesh = pClothMesh;
	cloth_desc.WorldPose = world_pose;

	if( set_mesh_data )
	{
		uchar *pV = mesh.GetVertexBufferPtr();
		uchar *pI = mesh.GetIndexBufferPtr();

		CMeshData mesh_data;
		mesh_data.pVerticesPosBegin        = pV + mesh.GetVertexElementOffset(VEE::POSITION);
		mesh_data.VerticesPosByteStride    = mesh.GetVertexSize();
		mesh_data.pNumVerticesPtr          = &mesh.m_NumUpdatedVertices;
		mesh_data.pVerticesNormalBegin     = pV + mesh.GetVertexElementOffset(VEE::NORMAL);
		mesh_data.VerticesNormalByteStride = mesh.GetVertexSize();
		mesh_data.NumMaxVertices           = num_vertices;
		mesh_data.pIndicesBegin            = pI;
//		mesh_data.NumMaxIndices            = num_indices / 2;
		mesh_data.NumMaxIndices            = num_indices; // Causes HEAP CORRUPTION. Why?
		mesh_data.IndicesByteStride        = sizeof(U16);
		mesh_data.pNumIndicesPtr           = &mesh.m_NumUpdatedIndices;
		memset( g_mesh_indices, 0, sizeof(g_mesh_indices) );
		mesh_data.pIndicesBegin = &g_mesh_indices;
		cloth_desc.MeshData = mesh_data;
	}

	return m_pScene->CreateCloth( cloth_desc );
}


CCloth *CSceneUtility::CreateClothFromMeshFile( const std::string& filepath, const Matrix34& world_pose )
{/*
	C3DMeshModelArchive archive;
	bool loaded = archive.LoadFromFile( filepath );
	if( !loaded )
		return NULL;

	const CMMA_VertexSet& vs = archive.GetVertexSet();
	const int num_vertices = vs.GetNumVertices();
	CClothMeshDesc cloth_mesh_desc;
	cloth_mesh_desc.m_vecVertex.resize( num_vertices, Vector3(0,0,0) );
	for( int i=0; i<num_vertices; i++ )
		cloth_mesh_desc.m_vecVertex[i] = vs.vecPosition[i];

	const int num_indices = (int)archive.GetVertexIndex().size();
	archive.GetVertexIndex().resize( num_indices, 0 );
	for( int i=0; i<num_indices; i++ )
		cloth_mesh_desc.m_vecIndex[i] = archive.GetVertexIndex()[i];

	CStream cloth_mesh_stream;
	Preprocessor().CreateClothMeshStream( cloth_mesh_desc, cloth_mesh_stream );

	CTriangleMesh *pClothMesh = PhysicsEngine().CreateClothMesh( cloth_mesh_stream );

	CClothDesc cloth_desc;
	cloth_desc.pClothMesh = pClothMesh;
	cloth_desc.WorldPose = world_pose;

	return m_pScene->CreateCloth( cloth_desc );*/
	return NULL;
}



#endif /* __SceneUtility_HPP__ */