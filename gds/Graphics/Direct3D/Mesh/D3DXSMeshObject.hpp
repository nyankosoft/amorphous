#ifndef  __D3DXSMESHOBJECT_H__
#define  __D3DXSMESHOBJECT_H__


#include "D3DXPMeshObject.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/MeshModel/MeshBone.hpp"
using namespace MeshModel;


/**
 mesh with skeleton hierarchy
 - Cannot be loaded from .x mesh
*/
class CD3DXSMeshObject : public CD3DXPMeshObject
{
	/// holds vertex blend matrices (world transforms)
	Transform *m_paWorldTransforms;

	int m_iNumBones;

	/// hold pointers to each bone in a single array
	/// used to set matrices with indices
	std::vector<CMeshBone *> m_vecpBone;

	/// root node of the hierarchical structure
    CMeshBone *m_pRootBone;

	std::vector<Matrix34> m_vecLocalTransformCache;

//	std::vector<Transform> m_vecLocalTransformCache;

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

//	inline const CMeshBone& GetBone( int index ) const;

	inline const CMeshBone& GetBone( const std::string& bone_name ) const;

	inline const CMeshBone& GetRootBone() const { return m_pRootBone ? *m_pRootBone : CMeshBone::NullBone(); }

//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );

	/// calculate hierarchical transforms by handiing an array of matrices
	/// that represents local transformations at each bone
	inline void SetLocalTransforms( Matrix34 *paLocalTransform );

	/// Set local transformation for a bone to a cache.
	/// NOTE: this function does not update the vertex blend matrix of the bone.
	/// User is responsible for calling SetLocalTransformsFromCache() later to update the vertex blend matrix of the bone.
	inline void SetLocalTransformToCache( int index, const Matrix34& local_transform );

	inline void SetLocalTransformToCache( int index, const Transform& local_transform );

	inline void SetLocalTransformsFromCache();

	inline void ResetLocalTransformsCache();

	/// Returns the pointer to the array of vertex blend matrices (world transforms)
	inline Transform* GetBlendTransforms() { return m_paWorldTransforms; }

	inline void GetBlendTransforms( std::vector<Transform>& dest_transforms );

	void DumpSkeletonToTextFile( const std::string& output_filepath ) const;

	virtual CMeshType::Name GetMeshType() const { return CMeshType::SKELETAL; }
};


inline int CD3DXSMeshObject::GetBoneMatrixIndexByName( const std::string& bone_name ) const
{
	if( m_pRootBone )
		return m_pRootBone->GetBoneMatrixIndexByName_r( bone_name.c_str() );
	else
		return -1;
}

/*
inline const CMeshBone& CD3DXSMeshObject::GetBone( int index ) const
{
}
*/

inline const CMeshBone& CD3DXSMeshObject::GetBone( const std::string& bone_name ) const
{
	if( m_pRootBone )
		return m_pRootBone->GetBoneByName_r( bone_name.c_str() );
	else
		return CMeshBone::NullBone();
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


inline void CD3DXSMeshObject::SetLocalTransformToCache( int index, const Transform& local_transform )
{
//	if( 0 <= index && index < (int)m_vecLocalTransformCache.size() )
//		m_vecLocalTransformCache[index] = local_transform;
}


inline void CD3DXSMeshObject::SetLocalTransformsFromCache()
{
	if( 0 < m_vecLocalTransformCache.size() )
		SetLocalTransforms( &m_vecLocalTransformCache[0] );
}


inline void CD3DXSMeshObject::ResetLocalTransformsCache()
{
	m_vecLocalTransformCache.resize( m_vecLocalTransformCache.size(), Matrix34Identity() );
}


inline void CD3DXSMeshObject::GetBlendTransforms( std::vector<Transform>& dest_transforms )
{
	if( !m_pRootBone )
		return;

	if( m_vecLocalTransformCache.empty() )
		return;

	dest_transforms.resize( m_iNumBones );

	int index = 0;
	m_pRootBone->CalculateTransforms_r( NULL, &m_vecLocalTransformCache[0], index, &(dest_transforms[0]) );
}


/*
inline void CD3DXSMeshObject::SetLocalTransform( int matrix_index, const Matrix34 *local_transform )
{
	if( m_vecpBone.size() == 0 || m_vecpBone.size() <= matrix_index )
		return;

	m_vecpBone[matrix_index]->SetLocalTransform( local_transform );
}*/



#endif  /*  __D3DXSMESHOBJECT_H__  */
