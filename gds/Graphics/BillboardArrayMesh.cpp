#include "BillboardArrayMesh.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/Macro.h"
#include "Graphics/FVF_BillboardVertex.h"

using namespace std;


void CBillboardArrayMesh::CreateVertexElemenets( CMMA_VertexSet& rVertexSet )
{
	unsigned int billboard_vertex_flag =
		CMMA_VertexSet::VF_POSITION|
		CMMA_VertexSet::VF_DIFFUSE_COLOR|
		CMMA_VertexSet::VF_2D_TEXCOORD;

	if( billboard_vertex_flag & rVertexSet.GetVertexFormat() )
	{
		SafeDeleteArray( m_paVertexElements );
		m_paVertexElements = new D3DVERTEXELEMENT9 [numof(BILLBOARDVERTEX_DECLARATION)];
		memcpy( m_paVertexElements, BILLBOARDVERTEX_DECLARATION, sizeof(BILLBOARDVERTEX_DECLARATION) );
	}
	else
	{
		LOG_PRINT_ERROR( " - Invalid vertex format for billboard array mesh." );
	}
}


void CBillboardArrayMesh::LoadVertices( void*& pVBData,
								        C3DMeshModelArchive& archive )
{
	CMMA_VertexSet& rVertexSet = archive.GetVertexSet();

	CreateVertexElemenets( rVertexSet );

	int i, iNumVertices = rVertexSet.GetNumVertices();

	BILLBOARDVERTEX *pVert;

	unsigned int billboard_vertex_flag =
		CMMA_VertexSet::VF_POSITION|
		CMMA_VertexSet::VF_DIFFUSE_COLOR|
		CMMA_VertexSet::VF_2D_TEXCOORD;

	if( rVertexSet.GetVertexFormat() & billboard_vertex_flag )
	{
		m_dwFVF = BILLBOARDVERTEX::FVF;
		m_iVertexSize = sizeof(BILLBOARDVERTEX);
		pVBData = (void *)( new BILLBOARDVERTEX [iNumVertices] );
		pVert = (BILLBOARDVERTEX *)pVBData;
		for( i=0; i<iNumVertices; i++ )
		{
//			pVert[i].vPosition = rVertexSet.vecPosition[i];
//			pVert[i].vNormal   = rVertexSet.vecNormal[i];
			pVert[i].local_offset.x = rVertexSet.vecPosition[i].x;
			pVert[i].local_offset.y = rVertexSet.vecPosition[i].y;
			pVert[i].color     = rVertexSet.vecDiffuseColor[i].GetARGB32();
			pVert[i].tex       = rVertexSet.vecTex[0][i];	// use the first texture coord
		}

		DIRECT3D9.GetDevice()->CreateVertexDeclaration(BILLBOARDVERTEX_DECLARATION, &m_pVertexDecleration);
	}
}

bool CBillboardArrayMesh::LoadFromArchive( C3DMeshModelArchive& rArchive, const string& filename, U32 option_flags )
{
	bool loaded = CD3DXMeshObject::LoadFromArchive( rArchive, filename, option_flags );
	return loaded;

/*
	if( SUCCEEDED(hr) && m_pMesh )
	{
		LPD3DXMESH pNewMesh = NULL;
		DWORD * pAdjacencyIn = new DWORD [ m_pMesh->GetNumFaces() * 6 ];
		DWORD * pAdjacencyOut = new DWORD [ m_pMesh->GetNumFaces() * 6 ];
		DWORD * pFaceRemap = NULL;
		LPD3DXBUFFER pVertexRemap = NULL;

		hr = m_pMesh->GenerateAdjacency( 0.0f, pAdjacencyIn );

		hr = m_pMesh->Optimize( D3DXMESH_VB_DYNAMIC|D3DXMESH_VB_WRITEONLY,
		pAdjacencyIn,
		pAdjacencyOut,
		pFaceRemap,
		&pVertexRemap,
		&pNewMesh );

		if( SUCCEEDED(hr) )
		{
			SAFE_RELEASE( m_pMesh );
			m_pMesh = pNewMesh;

			SAFE_RELEASE( pVertexRemap );
		}

		SafeDeleteArray( pAdjacencyIn );
		SafeDeleteArray( pAdjacencyOut );

		return S_OK;
	}
	else
		return hr;
*/
}


inline void CBillboardArrayMeshGenerator::SetTexCoord( int rect_index, int vert_index, const TEXCOORD2& tex )
{
	m_MeshArchive.GetVertexSet().vecTex[0][ rect_index * 4 + vert_index ] = tex;
}


void CBillboardArrayMeshGenerator::SetTextureCoordinates( int num_particles, int num_tex_edge_divisions )
{
	int i;
	int num_tex_segs = num_tex_edge_divisions;
	int num_tex_patterns = num_tex_segs * num_tex_segs;
	float fTex = 1.0f / (float)num_tex_edge_divisions;	// TODO: random texture coord for particles
	int pattern;

	for(i=0; i<num_particles; i++)
	{
		// set random texture patterns for each particle in advance
		pattern = RangedRand( 0, num_tex_patterns - 1 );
		float u,v;
		u = (float)(pattern % num_tex_segs) * fTex;
		v = (float)(pattern / num_tex_segs) * fTex;

		SetTexCoord( i, 0, TEXCOORD2(u,        v       ) );
		SetTexCoord( i, 1, TEXCOORD2(u + fTex, v       ) );
		SetTexCoord( i, 2, TEXCOORD2(u + fTex, v + fTex) );
		SetTexCoord( i, 3, TEXCOORD2(u,        v + fTex) );
	}
}


void CBillboardArrayMeshGenerator::SetVertices( int num_particles )
{
	CMMA_VertexSet& rVertSet = m_MeshArchive.GetVertexSet();

	int i;
	const int num_vertices = num_particles * 4;
	rVertSet.vecPosition.resize( num_vertices );
	rVertSet.vecTex.resize( 1 );
	rVertSet.vecTex[0].resize( num_vertices );
	rVertSet.vecDiffuseColor.resize( num_vertices, SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f) );
	rVertSet.SetVertexFormat( 
		CMMA_VertexSet::VF_POSITION|
		CMMA_VertexSet::VF_DIFFUSE_COLOR|
		CMMA_VertexSet::VF_2D_TEXCOORD
		);

	// vertices
	int vert_index = 0;
	for( i=0; i<num_particles; i++ )
	{
		rVertSet.vecPosition[vert_index++] = Vector3( -GetRandRadius(),  GetRandRadius(), 0 );
		rVertSet.vecPosition[vert_index++] = Vector3(  GetRandRadius(),  GetRandRadius(), 0 );
		rVertSet.vecPosition[vert_index++] = Vector3(  GetRandRadius(), -GetRandRadius(), 0 );
		rVertSet.vecPosition[vert_index++] = Vector3( -GetRandRadius(), -GetRandRadius(), 0 );
//		rVertSet.vecPosition[vert_index++].local_offset = D3DXVECTOR2(-GetRandRadius(), GetRandRadius());
//		rVertSet.vecPosition[vert_index++].local_offset = D3DXVECTOR2( GetRandRadius(), GetRandRadius());
//		rVertSet.vecPosition[vert_index++].local_offset = D3DXVECTOR2( GetRandRadius(),-GetRandRadius());
//		rVertSet.vecPosition[vert_index++].local_offset = D3DXVECTOR2(-GetRandRadius(),-GetRandRadius());
	}
}


void CBillboardArrayMeshGenerator::SetIndices( int num_particles )
{
	vector<unsigned int>& rvecVertIndex = m_MeshArchive.GetVertexIndex();

	// indices
	const int num_indices = num_particles * 6;
	int index_offset = 0;
	rvecVertIndex.resize( num_indices );
	for( int i=0; i<num_particles; i++ )
	{
		// indices for triangle lists
		index_offset = i * 4;
		rvecVertIndex[i*6]   = index_offset + 0;
		rvecVertIndex[i*6+1] = index_offset + 1;
		rvecVertIndex[i*6+2] = index_offset + 2;
		rvecVertIndex[i*6+3] = index_offset + 0;
		rvecVertIndex[i*6+4] = index_offset + 2;
		rvecVertIndex[i*6+5] = index_offset + 3;

	}
}


bool CBillboardArrayMeshGenerator::Generate( int num_particles, float particle_radius, int num_tex_edge_divisions )
{
	m_fParticleRadius = particle_radius;

	SetVertices( num_particles );

	const int num_vertices = num_particles * 4;

	SetTextureCoordinates( num_particles, num_tex_edge_divisions );

/*	// texture coordinates
	vector<TEXCOORD2>& vecTex = rVertSet.vecTex[0];
	vert_index = 0;
	for( i=0; i<num_particles; i++ )
	{
		vecTex[vert_index++] = TEXCOORD2( 0, 0 );
		vecTex[vert_index++] = TEXCOORD2( 1, 0 );
		vecTex[vert_index++] = TEXCOORD2( 1, 1 );
		vecTex[vert_index++] = TEXCOORD2( 0, 1 );
	}
*/

	SetIndices( num_particles );

	// triangle set
	m_MeshArchive.GetTriangleSet().resize( 1 );
	CMMA_TriangleSet& rTriangleSet = m_MeshArchive.GetTriangleSet()[0];

	rTriangleSet.m_iStartIndex = 0;
	rTriangleSet.m_iMinIndex = 0;
	rTriangleSet.m_iNumVertexBlocksToCover = num_vertices;
	rTriangleSet.m_iNumTriangles = num_particles * 2;

	rTriangleSet.m_AABB = AABB3( Vector3(-1,-1,-1), Vector3(1,1,1) );

	// materials
	m_MeshArchive.GetMaterial().resize( 1 );
	CMMA_Material& rMaterial = m_MeshArchive.GetMaterial()[0];


//	m_MeshArchive.WriteToTextFile( mesh_filename + ".txt" );

//	m_MeshArchive.SaveToFile( mesh_filename );

	return true;
}

bool CBillboardArrayMeshGenerator::GenerateForMultipleParticleGroups( int num_particles_per_group,
		                                                              int num_particle_groups,
		                                                              float particle_radius,
											                          int num_tex_edge_divisions )
{
	const int num_total_particles = num_particles_per_group * num_particle_groups;

	m_fParticleRadius = particle_radius;

	SetVertices( num_total_particles );

	const int num_vertices_per_group = num_particles_per_group * 4;
	const int num_total_vertices     = num_total_particles * 4;

	SetTextureCoordinates( num_total_particles, num_tex_edge_divisions );

	SetIndices( num_total_particles );

	// triangle set
	m_MeshArchive.GetTriangleSet().resize( num_particle_groups );
	m_MeshArchive.GetMaterial().resize( num_particle_groups );
	int i;
	for( i=0; i<num_particle_groups; i++ )
	{
		CMMA_TriangleSet& rTriangleSet = m_MeshArchive.GetTriangleSet()[i];

		rTriangleSet.m_iStartIndex = i * num_particles_per_group * 6;
		rTriangleSet.m_iMinIndex = i * num_vertices_per_group;
		rTriangleSet.m_iNumVertexBlocksToCover = num_vertices_per_group;
		rTriangleSet.m_iNumTriangles = num_particles_per_group * 2;

		rTriangleSet.m_AABB = AABB3( Vector3(-1,-1,-1), Vector3(1,1,1) );

		// materials
		CMMA_Material& rMaterial = m_MeshArchive.GetMaterial()[i];

		// what to do with the materials?
	}



	return true;
}

