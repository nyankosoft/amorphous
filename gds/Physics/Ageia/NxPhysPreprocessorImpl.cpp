
#include "NxPhysPreprocessorImpl.h"
#include "NxMathConv.h"


bool CNxPhysPreprocessorImpl::Init()
{
	m_pCooking = NxGetCookingLib(NX_SDK_VERSION_NUMBER);
	m_pCooking->NxInitCooking();
}


CNxPhysPreprocessorImpl::~CNxPhysPreprocessorImpl()
{
	SafeDelete( m_pCooking );
}


void CNxPhysPreprocessorImpl::CreateTriangleMeshStream( CTriangleMeshDesc& mesh_desc,
													    CStream& phys_stream )
{
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

	meshDesc.numVertices                = num_verts;
	meshDesc.numTriangles               = num_triangles;

	meshDesc.pointStrideBytes           = sizeof(NxVec3);
	meshDesc.triangleStrideBytes        = 3*sizeof(NxU32);
	meshDesc.materialIndexStride        = sizeof(NxU32);

	meshDesc.points                     = &(vecVertex[0]);
	meshDesc.triangles                  = &(vecIndex[0]);                         
	meshDesc.materialIndices            = &(vecMatIndex[0]);

	meshDesc.flags                      = 0;

	m_pCooking->NxCookTriangleMesh( meshDesc, CNxPhysStream( &phys_stream, false ) );
}
