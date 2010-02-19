#include "NxPhysPreprocessorImpl.hpp"
#include "NxMathConv.hpp"
#include "NxPhysStream.hpp"
#include "../TriangleMeshDesc.hpp"
#include "../ClothMeshDesc.hpp"

// PhysX header
#include "NxCooking.h"

#include "Support/SafeDelete.hpp"

using namespace std;
using namespace physics;


//typedef NxU32 index_type;
typedef NxU16 index_type;


#define LOG_PRINT_OK_OR_FAILED( text, succeeded ) if(succeeded) LOG_PRINT(string(text) + "  [  OK  ]"); else LOG_PRINT_ERROR(string(text) + "  [FAILED]");


static void FillNxTriangleMessDesc( CTriangleMeshDesc& src, vector<NxVec3>& vecVertex, vector<index_type>& vecIndex, NxSimpleTriangleMesh& dest )
{
	// copy vertices
	size_t num_verts = src.m_vecVertex.size();
//	vector<NxVec3> vecVertex;
	vecVertex.resize( num_verts );
	for( size_t i=0; i<num_verts; i++ )
		vecVertex[i] = ToNxVec3( src.m_vecVertex[i] );

	// copy indices
	size_t num_indices = src.m_vecIndex.size();
	size_t num_triangles = num_indices / 3;
//	vector<index_type> vecIndex;
	vecIndex.resize( num_indices );
	for( size_t i=0; i<num_indices; i++ )
		vecIndex[i] = (index_type)src.m_vecIndex[i];

	dest.numVertices                = (NxU32)num_verts;
	dest.numTriangles               = (NxU32)num_triangles;

	dest.pointStrideBytes           = sizeof(NxVec3);
	dest.triangleStrideBytes        = 3*sizeof(index_type);

	dest.points                     = &(vecVertex[0]);
	dest.triangles                  = &(vecIndex[0]);                         
}


bool CNxPhysPreprocessorImpl::Init()
{
	LOG_FUNCTION_SCOPE();

	bool global_init = NxInitCooking( 0, &NxPhysOutputStream() );
	if( global_init )
		LOG_PRINT( " Initialized the PhysX Cooking module." );
	else
		LOG_PRINT_ERROR( " NxInitCooking() failed." );

	return global_init;

/*	m_pCooking = NxGetCookingLib(NX_SDK_VERSION_NUMBER);

	if( m_pCooking )
		LOG_PRINT( " Created an instance of NxCookingInterface: " );
	else
	{
		LOG_PRINT_ERROR( " NxGetCookingLib() returned NULL." );
		return false;
	}

	bool initialized = m_pCooking->NxInitCooking( 0, &NxPhysOutputStream() );

	return initialized;*/
}


CNxPhysPreprocessorImpl::~CNxPhysPreprocessorImpl()
{
//	SafeDelete( m_pCooking );
}


void CNxPhysPreprocessorImpl::CreateTriangleMeshStream( CTriangleMeshDesc& mesh_desc,
													    CStream& phys_stream )
{
	LOG_FUNCTION_SCOPE();

//	if( !m_pCooking )
//		return;

	size_t i;

	SetPhysicsEngineName( phys_stream );

	NxTriangleMeshDesc meshDesc;
	vector<NxVec3> vecVertex;
	vector<index_type> vecIndex;

	// vertices and points
	FillNxTriangleMessDesc( mesh_desc, vecVertex, vecIndex, meshDesc );

	// vecIndex now contains indices of triangles
	const size_t num_indices = vecIndex.size();

	// copy material indices
	size_t num_triangles = num_indices / 3;
	vector<NxU32> vecMatIndex;
	vecMatIndex.resize( num_triangles );
	for( i=0; i<num_triangles; i++ )
		vecMatIndex[i] = (NxU32)mesh_desc.m_vecMaterialIndex[i];

	meshDesc.materialIndexStride        = sizeof(NxU32);

	meshDesc.materialIndices            = &(vecMatIndex[0]);

	meshDesc.flags                      = 0;
	if( sizeof(index_type) == sizeof(NxU16) )
		meshDesc.flags                  = NX_MF_16_BIT_INDICES;

	bool valid_desc = meshDesc.isValid();
	if( !valid_desc )
		LOG_PRINT_ERROR( " An invalid triangle mesh desc" );

	{
		LOG_SCOPE( "NxCookingInterface::NxCookTriangleMesh()" );
//		m_pCooking->NxCookTriangleMesh( meshDesc, CNxPhysStream( &phys_stream, false ) );
		bool trimesh_cooked = NxCookTriangleMesh( meshDesc, CNxPhysStream( &(phys_stream.m_Buffer), false ) );

		LOG_PRINT_OK_OR_FAILED( "NxCookTriangleMesh()", trimesh_cooked );
	}
}


void CNxPhysPreprocessorImpl::CreateClothMeshStream( CClothMeshDesc& desc,
                                                     CStream& phys_stream )
{
	NxClothMeshDesc clothMeshDesc;
	vector<NxVec3> vecVertex;
	vector<index_type> vecIndex;

	FillNxTriangleMessDesc( desc, vecVertex, vecIndex, clothMeshDesc );

	clothMeshDesc.vertexMassStrideBytes = sizeof(NxReal);
	clothMeshDesc.vertexFlagStrideBytes = sizeof(NxReal);
	clothMeshDesc.vertexMasses = 0;
	clothMeshDesc.vertexFlags  = 0;
	clothMeshDesc.flags                 = 0;//NX_CLOTH_MESH_WELD_VERTICES;
//	clothMeshDesc.weldingDistance       = 0.0001f;

	bool valid_desc = clothMeshDesc.isValid();
	if( !valid_desc )
	{
		LOG_PRINT_ERROR( " An invalid cloth mesh desc" );
		return;
	}

	{
		LOG_SCOPE( "NxCookingInterface::NxCookClothMesh()" );
//		m_pCooking->NxCookTriangleMesh( meshDesc, CNxPhysStream( &phys_stream, false ) );
		bool clothmesh_cooked = NxCookClothMesh( clothMeshDesc, CNxPhysStream( &(phys_stream.m_Buffer), false ) );

		LOG_PRINT_OK_OR_FAILED( "NxCookClothMesh()", clothmesh_cooked );
	}
}
