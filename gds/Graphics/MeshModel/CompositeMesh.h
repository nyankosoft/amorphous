#ifndef  __COMPOSITEMESH_H__
#define  __COMPOSITEMESH_H__

#include <vector>
using namespace std;

#include "Graphics/MeshModel/3DMeshModelArchive.h"
#include "Graphics/MeshModel/D3DXMeshModel.h"
//#include "Graphics/MeshModel/MeshBone.h"
using namespace MeshModel;


#include "PhysicsSim/MassSpringSim.h"



class CMSPointToMeshVertexMap : public IArchiveObjectBase
{
public:
	/// indices from control points in mass spring to mesh vertices
	int Index[2];

	void Serialize( IArchive& ar, const unsigned int version ) { ar & Index[0] & Index[1]; }
};

/*
class CFixedPointIndexMap : public IArchiveObjectBase
{
public:

	/// indices to mesh vertices that are marked as fixed control points
	/// in the mesh mass spring grid.
	/// used to update pinned points by animation transforms
	int iMeshIndex[2];

	/// indices to the fixed control points in the mass spring sim
	int iControlPointIndex;

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & iMeshIndex[0] & iMeshIndex[1] & iControlPointIndex;
	}
};*/


class CMA_CompositeMeshArchive : public IArchiveObjectBase
{
public:

	unsigned int m_OptionFlag;

	C3DMeshModelArchive m_Mesh;

	C3DMeshModelArchive m_ShadowVolumeMesh;

	CMS_MassSpringArchive m_MassSpringModel;

	/// indices from control points in the mass spring to mesh vertices
	/// which have to be updated after physics is done in the mass spring simulation
	vector<CMSPointToMeshVertexMap> m_vecMeshVertexIndex;


	CMA_CompositeMeshArchive() { m_OptionFlag = CMA_MESH; }

	void Serialize( IArchive& ar, const unsigned int version );

	unsigned int GetOptionFlag() const { return m_OptionFlag; }

	enum eOptionFlag
	{
		CMA_MESH				= (1 << 0),
		CMA_SHADOWVOLUMEMESH	= (1 << 1),
		CMA_MASSSPRINGSIM		= (1 << 2),
	};
};


class CCompositeMesh
{
	string m_strFilename;

	CD3DXMeshModel *m_pMesh;

	CD3DXMeshModel *m_pShadowVolumeMesh;

	CMS_MassSpringSim m_MassSpringSim;

//	int *m_pMSpringFixedPointIndex;
//	int *m_pMeshFixedPointIndex;
//	int m_iNumFixedPoints;

//	int *m_pMassSpringPointIndex;
	int *m_pMSpringToMeshVertexIndex;
	int m_iNumMassSpringPoints;

	// used to acces position & normal of model vertices
	int m_iVertexStride, m_iPosOffset, m_iNormalOffset;

private:

	void SetVertexInfo( C3DMeshModelArchive& rModelArchive );


public:

	CCompositeMesh();

	~CCompositeMesh();

	void Release();

	bool LoadFromFile( const char *pcFilename );

	string& GetFilename() { return m_strFilename; }

	void Render();

	void RenderShadowVolume();

//	void ApplyHierarchicalTransform( float *pafFrameData, unsigned int data_type );

	void Transform( Matrix34 *paWorldTransform );

	/// update world property given the local transforms for bones
	void Transform_Local( Matrix34 *paLocalTransform );

	void Update( float dt );

	void ResetPhysics() { m_MassSpringSim.SetBasePositions(); }

	CMS_MassSpringSim *GetMassSpringSim() { return &m_MassSpringSim; }

	CD3DXMeshModel *GetMeshModel() { return m_pMesh; }
};



#endif		/*  __COMPOSITEMESH_H__  */