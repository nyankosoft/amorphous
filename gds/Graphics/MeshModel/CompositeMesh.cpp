
#include "CompositeMesh.hpp"

#include "Support/memory_helpers.hpp"


#include "Graphics/FVF_NormalVertex.h"
#include "Graphics/FVF_BumpVertex.h"
#include "Graphics/FVF_WeightVertex.h"
#include "Graphics/FVF_BumpWeightVertex.h"

#include "Graphics/FVF_ColorVertex.h"
#include "Graphics/Direct3D9.hpp"

#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"


//==========================================================-
// CMA_CompositeMeshArchive
//==========================================================-

void CMA_CompositeMeshArchive::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_OptionFlag;

	ar & m_Mesh;

	ar & m_ShadowVolumeMesh;

	ar & m_MassSpringModel;

	ar & m_vecMeshVertexIndex;

}



//==========================================================-
// CCompositeMesh
//==========================================================-

CCompositeMesh::CCompositeMesh()
{
	m_pMesh = NULL;
	m_pShadowVolumeMesh = NULL;

	m_iNumMassSpringPoints = 0;
	m_pMSpringToMeshVertexIndex = NULL;
}


CCompositeMesh::~CCompositeMesh()
{
	Release();
}


void CCompositeMesh::Release()
{
	SafeDelete( m_pMesh );

	SafeDelete( m_pShadowVolumeMesh );

	SafeDelete( m_pMSpringToMeshVertexIndex );
}


bool CCompositeMesh::LoadFromFile( const char *pcFilename )
{
	CMA_CompositeMeshArchive model_archive;

	CBinaryArchive_Input input_archive( pcFilename );

	if( !(input_archive >> model_archive) )
		return false;

	// delete any previous data
	Release();

    // load archive data for runtime use
	m_pMesh = new CD3DXMeshModel;
	if( !m_pMesh->LoadFromArchive( model_archive.m_Mesh, pcFilename ) )
		return false;


	if( model_archive.GetOptionFlag() & CMA_CompositeMeshArchive::CMA_SHADOWVOLUMEMESH )
	{
		// the archive has the mesh for shadow volume rendering
        m_pShadowVolumeMesh = new CD3DXMeshModel;
		if( !m_pShadowVolumeMesh->LoadFromArchive( model_archive.m_ShadowVolumeMesh ) )
			return false;
	}

	if( model_archive.GetOptionFlag() & CMA_CompositeMeshArchive::CMA_MASSSPRINGSIM )
	{
		m_MassSpringSim.LoadFromArchive( model_archive.m_MassSpringModel );

		// load indices necessary to update positions & normals of mesh vertices
		// after the physics integration of mass spring model
		m_iNumMassSpringPoints = (int)model_archive.m_vecMeshVertexIndex.size();

		m_pMSpringToMeshVertexIndex = new int [m_iNumMassSpringPoints * 2];
		for( int i=0; i<m_iNumMassSpringPoints; i++ )
		{
			m_pMSpringToMeshVertexIndex[i*2]   = model_archive.m_vecMeshVertexIndex[i].Index[0];
			m_pMSpringToMeshVertexIndex[i*2+1] = model_archive.m_vecMeshVertexIndex[i].Index[1];
		}

		SetVertexInfo( model_archive.m_Mesh );
	}

	// store the filename
	m_strFilename = pcFilename;

	return true;
}


void CCompositeMesh::SetVertexInfo( C3DMeshModelArchive& rModelArchive )
{
	CMMA_VertexSet& rVertexSet = rModelArchive.GetVertexSet();

	switch( rVertexSet.GetVertexFormat() )
	{

	case CMMA_VertexSet::VF_TEXTUREVERTEX:
		m_iVertexStride = sizeof(NORMALVERTEX);
		m_iPosOffset = offsetof(NORMALVERTEX,vPosition);
		m_iNormalOffset = offsetof(NORMALVERTEX,vNormal);
		break;

	case CMMA_VertexSet::VF_BUMPVERTEX:
		m_iVertexStride = sizeof(BUMPVERTEX);
		m_iPosOffset = offsetof(BUMPVERTEX,vPosition);
		m_iNormalOffset = offsetof(BUMPVERTEX,vNormal);
		break;

	case CMMA_VertexSet::VF_WEIGHTVERTEX:
		m_iVertexStride = sizeof(WEIGHTVERTEX);
		m_iPosOffset = offsetof(WEIGHTVERTEX,vPosition);
		m_iNormalOffset = offsetof(WEIGHTVERTEX,vNormal);
		break;

	case CMMA_VertexSet::VF_BUMPWEIGHTVERTEX:
		m_iVertexStride = sizeof(BUMPWEIGHTVERTEX);
		m_iPosOffset = offsetof(BUMPWEIGHTVERTEX,vPosition);
		m_iNormalOffset = offsetof(BUMPWEIGHTVERTEX,vNormal);
		break;
	}
}


void CCompositeMesh::Render()
{
	if( !m_pMesh )
		return;

	m_pMesh->Render();

	if( m_MassSpringSim.GetNumControlPoints() == 0 )
		return;	// no mass spring sim

	// copy mass spring states to mesh (positions & normals)
	m_MassSpringSim.ComputeNormals();

	int i, iNumPoints = m_MassSpringSim.GetNumControlPoints();
	CMS_PointState *paPoint = m_MassSpringSim.GetControlPoints();

	int iVertStride = m_iVertexStride, iPosOffset = m_iPosOffset, iNormalOffset = m_iNormalOffset;
	void *p = NULL;
	m_pMesh->LockVertexBuffer( p );

	if( !p )
		return;
    
	char *pVertex = (char *)p;

	// update the mesh vertices whose positions should be calculated by the mass spring simulation
	int iNumMSpringPoints = m_iNumMassSpringPoints;
	int *pIndex = m_pMSpringToMeshVertexIndex;
	Vector3 vInvNormal;
	const int vec3_size = sizeof(float) * 3;
	for( i=0; i<iNumMSpringPoints; i++ )
	{
		if( pIndex[i*2] < 0 )
			continue;

		memcpy( pVertex + pIndex[i*2] * iVertStride + iPosOffset,      &(paPoint[i].vPosition), vec3_size );
		memcpy( pVertex + pIndex[i*2] * iVertStride + iNormalOffset,   &(paPoint[i].vNormal),   vec3_size );

		if( pIndex[i*2+1] < 0 )
			continue;

		vInvNormal = paPoint[i].vNormal * (-1.0f);

		memcpy( pVertex + pIndex[i*2+1] * iVertStride + iPosOffset,    &(paPoint[i].vPosition), vec3_size );
		memcpy( pVertex + pIndex[i*2+1] * iVertStride + iNormalOffset, &vInvNormal,             vec3_size );
	}

	// draw points
/*	NORMALVERTEX v;
	v.vNormal = D3DXVECTOR3(0,0,1);
	v.color = 0xFFFFFFFF;
	v.tex.u = v.tex.v = 0;
	for( i=0; i<iNumMSpringPoints; i++ )
	{
		v.vPosition = paPoint[i].vPosition;
		DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_POINTLIST, 1, &v, sizeof(COLORVERTEX) );
	}*/

	m_pMesh->UnlockVertexBuffer();
}

/*
void CCompositeMesh::ApplyHierarchicalTransform( float *pafFrameData, unsigned int data_type )
{
	// apply transform
	// m_pMesh->
}
*/


void CCompositeMesh::Update( float dt )
{
	// progress animation

	m_MassSpringSim.Update( dt );
}


void CCompositeMesh::Transform( Matrix34 *paWorldTransform )
{

	// apply transform
	m_pMesh->Transform( paWorldTransform );

	// update the positions of fixed control points in the mass spring model
	m_MassSpringSim.UpdateFixedMSpringPointPositions( paWorldTransform );

	// update the positions of fixed points in mass spring grid
/*	int i, iNumFixedPoints = m_iNumFixedPoints;
	int *paMSpringFixedPointIndex = m_pMSpringFixedPointIndex;
	int *paMeshFixedPointIndex = m_pMeshFixedPointIndex;

	int iVertexStride = m_iVertexStride, iPosOffset = m_iPosOffset;
	void *p = NULL;
	m_pMesh->LockVertexBuffer( p );
	char *pVertex = (char *)p;

	CMS_PointState *paPoint = m_MassSpringSim.GetControlPoints();

	for( i=0; i<iNumFixedPoints; i++ )
	{
		memcpy( &(paPoint[paMSpringFixedPointIndex[i]].vPosition),
			    pVertex + paMeshFixedPointIndex[i] * iVertexStride + iPosOffset,
				sizeof(float) * 3 );
	}*/
}

//#define CMESH_NUM_MAX_BLEND_MATS	64
//static D3DXMATRIX s_aWorldMatrix[CMESH_NUM_MAX_BLEND_MATS];

void CCompositeMesh::Transform_Local( Matrix34 *paLocalTransform )
{
	// update transforms for mesh
	m_pMesh->SetTransforms_Local( paLocalTransform );

	if( m_pShadowVolumeMesh )
		m_pShadowVolumeMesh->SetTransforms_Local( paLocalTransform );

	// set the last blend matrix in the shader constants to identity
	// so that control points in the physics simulation be not affected
	// by the transformation matrices
	CShaderManager *pShaderMgr = CShader::Get()->GetCurrentShaderManager();
	if( pShaderMgr )
	{
		LPD3DXEFFECT pEffect = pShaderMgr->GetEffect();
		if( pEffect )
		{
			char acParam[32];
			const int num_bones = m_pMesh->GetNumBones();
			sprintf( acParam, "g_aBlendMatrix[%d]", num_bones );
			D3DXMATRIX matWorld;
            D3DXMatrixIdentity( &matWorld );
			pEffect->SetMatrix( acParam, &matWorld );
		}
	}
	D3DXMATRIX* paWorldTransform = m_pMesh->GetWorldTransforms();

//	assert( num_bones < CMESH_NUM_MAX_BLEND_MATS );
//	memcpy( (void *)s_aWorldMatrix, (void *)paWorldTransform, sizeof(D3DXMATRIX) * num_bones );
//	D3DXMatrixIdentity( &s_aWorldMatrix[num_bones] );

	// update world positions of pinned control points
	// and collision shapes in the mass spring simulation
	m_MassSpringSim.UpdateWorldProperties( paWorldTransform );
//	m_MassSpringSim.UpdateWorldProperties( s_aWorldMatrix );


}


void CCompositeMesh::RenderShadowVolume()
{
	if( m_pShadowVolumeMesh )
		m_pShadowVolumeMesh->Render();
}
