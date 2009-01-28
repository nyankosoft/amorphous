#ifndef  __D3DXSMESHOBJECT_H__
#define  __D3DXSMESHOBJECT_H__


#include "Graphics/fwd.h"
#include "Graphics/D3DXPMeshObject.h"
#include "Graphics/MeshModel/MeshBone.h"
using namespace MeshModel;


/**
 mesh with skeleton hierarchy
 - Cannot be loaded from .x mesh
*/
class CD3DXSMeshObject : public CD3DXPMeshObject
{
	/// holds vertex blend matrices (world transforms)
	D3DXMATRIX *m_paBoneMatrix;

	int m_iNumBones;

	/// hold pointers to each bone in a single array
	/// used to set matrices with indices
	std::vector<CMM_Bone *> m_vecpBone;

	/// root node of the hierarchical structure
    CMM_Bone *m_pRootBone;

	std::vector<Matrix34> m_vecLocalTransformCache;

private:

	bool LoadSkeletonFromArchive( C3DMeshModelArchive& archive );

public:

	CD3DXSMeshObject();

	CD3DXSMeshObject( const std::string& filename );

	~CD3DXSMeshObject();

	virtual void Release();

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes );

	inline int GetNumBones() const { return m_iNumBones; }

	inline int GetBoneMatrixIndexByName( const std::string& bone_name ) const;

//	inline const CMM_Bone& GetBone( int index ) const;

	inline const CMM_Bone& GetBone( const std::string& bone_name ) const;

//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );

	/// calculate hierarchical transforms by handiing an array of matrices
	/// that represents local transformations at each bone
	inline void SetLocalTransforms( Matrix34 *paLocalTransform );

	/// set local transformation for a bone to a cache
	/// NOTE: do not updates transformation for the bone
	/// use is responsible for calling SetLocalTransformsFromCache() later to updates the transformations for bones
	inline void SetLocalTransformToCache( int index, const Matrix34& local_transform );

	inline void SetLocalTransformsFromCache();

	inline void ResetLocalTransformsCache();

	/// returns the pointer to the array of vertex blend matrices (world transforms)
//	inline void GetBlendMatrices( D3DXMATRIX* paDestMatrix ) { paDestMatrix = m_paBoneMatrix; }
	inline D3DXMATRIX* GetBlendMatrices() { return m_paBoneMatrix; }

	LPD3DXMESH m_pMeshForTest;

	virtual CMeshType::Name GetMeshType() const { return CMeshType::SKELETAL; }

//	virtual LPD3DXBASEMESH GetBaseMesh() { return m_pMeshForTest; }
};


inline int CD3DXSMeshObject::GetBoneMatrixIndexByName( const std::string& bone_name ) const
{
	if( m_pRootBone )
		return m_pRootBone->GetBoneMatrixIndexByName_r( bone_name.c_str() );
	else
		return -1;
}

/*
inline const CMM_Bone& CD3DXSMeshObject::GetBone( int index ) const
{
}
*/

inline const CMM_Bone& CD3DXSMeshObject::GetBone( const std::string& bone_name ) const
{
	if( m_pRootBone )
		return m_pRootBone->GetBoneByName_r( bone_name.c_str() );
	else
		return CMM_Bone::NullBone();
}


inline void CD3DXSMeshObject::SetLocalTransforms( Matrix34 *paLocalTransform )
{
	if( !m_pRootBone )
		return;

	int index = 0;
	m_pRootBone->Transform_r( NULL, paLocalTransform, index );
}


inline void CD3DXSMeshObject::SetLocalTransformToCache( int index, const Matrix34& local_transform )
{
	if( 0 <= index && index < (int)m_vecLocalTransformCache.size() )
		m_vecLocalTransformCache[index] = local_transform;
}


inline void CD3DXSMeshObject::SetLocalTransformsFromCache()
{
	SetLocalTransforms( &m_vecLocalTransformCache[0] );
}


inline void CD3DXSMeshObject::ResetLocalTransformsCache()
{
	m_vecLocalTransformCache.resize( m_vecLocalTransformCache.size(), Matrix34Identity() );
}



/*
inline void CD3DXSMeshObject::SetLocalTransform( int matrix_index, const Matrix34 *local_transform )
{
	if( m_vecpBone.size() == 0 || m_vecpBone.size() <= matrix_index )
		return;

	m_vecpBone[matrix_index]->SetLocalTransform( local_transform );
}*/



#endif  /*  __D3DXSMESHOBJECT_H__  */
