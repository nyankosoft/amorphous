#include "PhysicsMeshUtility.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"
#include "gds/Physics/ActorDesc.hpp"
#include "gds/Physics/ConvexShapeDesc.hpp"
#include "gds/Physics/Stream.hpp"
#include "gds/Physics/PhysicsEngine.hpp"
#include "gds/Physics/PreProcessor.hpp"
#include "gds/Physics/Scene.hpp"
#include "gds/Physics/ClothMeshDesc.hpp"
#include "gds/Physics/Cloth.hpp"
#include "gds/3DMath/PrimitivePolygonModelMaker.hpp"


namespace amorphous
{

using namespace std;
using namespace physics;


static U16 g_mesh_indices[0xFFFF];


CActor *CPhysicsMeshUtility::CreateConvexActorFromMesh( const CCustomMesh& src_mesh, const Matrix34& world_pose, Vector3 linear_velocity, float mass, int material_index )
{
	CConvexShapeDesc convex_shape_desc;
	CTriangleMeshDesc trimeshdesc;

	if( !src_mesh.IsValid() )
		return NULL;

	// copy indices
	uint num_indices = src_mesh.GetNumIndices();
	trimeshdesc.m_vecIndex.resize( num_indices );
	for( uint i=0; i<num_indices; i++ )
		trimeshdesc.m_vecIndex[i] = (int)src_mesh.GetIndex(i);

	// copy vertices
	src_mesh.GetPositions( trimeshdesc.m_vecVertex );

	trimeshdesc.m_vecMaterialIndex.resize( num_indices / 3, material_index );

	physics::CStream convex_mesh_stream;
	Result::Name res = physics::Preprocessor().CreateConvexMeshStream( trimeshdesc, convex_mesh_stream );
	convex_mesh_stream.m_Buffer.reset_pos();

	CConvexMesh *pConvexMesh = PhysicsEngine().CreateConvexMesh( convex_mesh_stream );
	if( !pConvexMesh )
		return NULL;

	convex_shape_desc.pConvexMesh = pConvexMesh;

	CActorDesc actor_desc;
	actor_desc.vecpShapeDesc.push_back( &convex_shape_desc );
	actor_desc.BodyDesc.fMass = mass;
	actor_desc.WorldPose = world_pose;
	actor_desc.BodyDesc.LinearVelocity = linear_velocity;

	return GetScene()->CreateActor( actor_desc );
}


CCloth *CPhysicsMeshUtility::CreateClothFromMesh( CCustomMesh& mesh, const Matrix34& world_pose, bool set_mesh_data )
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

	return GetScene()->CreateCloth( cloth_desc );
}


CCloth *CPhysicsMeshUtility::CreateClothFromMeshFile( const std::string& filepath, const Matrix34& world_pose )
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



} // namespace amorphous
