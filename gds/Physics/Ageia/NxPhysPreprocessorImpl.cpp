#include "NxPhysPreprocessorImpl.h"
#include "NxMathConv.h"
#include "NxPhysStream.h"
#include "../TriangleMeshDesc.h"

// PhysX header
#include "NxCooking.h"

#include "Support/SafeDelete.h"

using namespace physics;


#define LOG_PRINT_OK_OR_FAILED( text, succeeded ) if(succeeded) LOG_PRINT(string(text) + "  [  OK  ]"); else LOG_PRINT_ERROR(string(text) + "  [FAILED]");


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

	// copy vertices
	size_t num_verts = mesh_desc.m_vecVertex.size();
	vector<NxVec3> vecVertex;
	vecVertex.resize( num_verts );
	for( i=0; i<num_verts; i++ )
		vecVertex[i] = ToNxVec3( mesh_desc.m_vecVertex[i] );

	// copy indices
	size_t num_indices = mesh_desc.m_vecIndex.size();
	vector<NxU32> vecIndex;
	vecIndex.resize( num_indices );
	for( i=0; i<num_indices; i++ )
		vecIndex[i] = (NxU32)mesh_desc.m_vecIndex[i];

	// copy material indices
	size_t num_triangles = num_indices / 3;
	vector<NxU32> vecMatIndex;
	vecMatIndex.resize( num_triangles );
	for( i=0; i<num_triangles; i++ )
		vecMatIndex[i] = (NxU32)mesh_desc.m_vecMaterialIndex[i];


	NxTriangleMeshDesc meshDesc;

	meshDesc.numVertices                = (NxU32)num_verts;
	meshDesc.numTriangles               = (NxU32)num_triangles;

	meshDesc.pointStrideBytes           = sizeof(NxVec3);
	meshDesc.triangleStrideBytes        = 3*sizeof(NxU32);
	meshDesc.materialIndexStride        = sizeof(NxU32);

	meshDesc.points                     = &(vecVertex[0]);
	meshDesc.triangles                  = &(vecIndex[0]);                         
	meshDesc.materialIndices            = &(vecMatIndex[0]);

	meshDesc.flags                      = 0;

	{
		LOG_SCOPE( "NxCookingInterface::NxCookTriangleMesh()" );
//		m_pCooking->NxCookTriangleMesh( meshDesc, CNxPhysStream( &phys_stream, false ) );
		bool trimesh_cooked = NxCookTriangleMesh( meshDesc, CNxPhysStream( &(phys_stream.m_Buffer), false ) );

		LOG_PRINT_OK_OR_FAILED( "NxCookTriangleMesh()", trimesh_cooked );
	}
}
