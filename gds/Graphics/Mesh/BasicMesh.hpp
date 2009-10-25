#ifndef __BasicMesh_HPP__
#define __BasicMesh_HPP__


#include <vector>
#include <boost/shared_ptr.hpp>
#include <d3dx9.h>
#include <gds/Graphics/fwd.hpp>
#include <gds/3DMath/AABB3.hpp>
#include <gds/base.hpp>
//#include "MeshImpl.hpp"

#include <gds/Graphics/TextureHandle.hpp>
#include <gds/Graphics/Shader/ShaderTechniqueHandle.hpp>
#include <gds/Graphics/MeshModel/MeshBone.hpp>
#include <gds/Graphics/MeshModel/3DMeshModelArchive.hpp>
using namespace MeshModel;


class CMaterial
{
public:
	float fEmmisive;
	float fSpecular;
	float fGlossiness;
	float fReflection;
};


class CMeshMaterial
{
public:

	CMaterial m_Mat;

	// array of textures for a material
	// - possible usages
	//   - surface color (most common)
	//   - normal map
	//   - specular map (often stored in alpha channel of normal map)
	// A texture handle will be empty if no texture is used for the stage
	std::vector<CTextureHandle> Texture;

	std::vector<CTextureResourceDesc> TextureDesc;

	float fMinVertexDiffuseAlpha;

public:

	/// Load the i-th texture asynchronously
	void LoadTextureAsync( int i );
};


class CMeshImpl
{
protected:

	std::string m_strFilename;

	/// bounding box of the mesh
	AABB3 m_AABB;

	std::vector<AABB3> m_vecAABB;	///< aabb for each triangle subset

	/// Number of materials
	int m_NumMaterials;

	std::vector<CMeshMaterial> m_vecMaterial;

protected:

	Result::Name LoadMaterials( C3DMeshModelArchive& rArchive, U32 option_flags );

public:

	CMeshImpl();

	/// returns true on success
	virtual bool LoadFromFile( const std::string& filename, U32 option_flags = 0 );

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags ) = 0;//{ return m_pImpl->LoadFromArchive( archive, filename, option_flasgs ); }

//	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes ) { return false; }

	virtual bool IsValid() { return true; }

	/// D3D mesh access

	virtual LPD3DXBASEMESH GetBaseMesh() { return NULL; }

	virtual LPD3DXMESH GetMesh() { return NULL; }

	/// rendering

	/// render object by using the fixed function pipeline
	virtual void Render() = 0;

	virtual void Render( CShaderManager& rShaderMgr ) = 0;

	virtual void Render( CShaderManager& rShaderMgr, std::vector<CShaderTechniqueHandle>& vecShaderTechnique );

	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex,
								std::vector<CShaderTechniqueHandle>& vecShaderTechnique ) {}

	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */) {}

	/// D3D-specific feature

	bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, std::vector<D3DVERTEXELEMENT9>& vecVertexElement ) { return false; }

	bool LoadNonAsyncResources( C3DMeshModelArchive& rArchive, U32 option_flags ) { return false; }

	int GetNumMaterials() const { return m_NumMaterials; }

	/// returns const reference to the i-th material
	const CMeshMaterial& GetMaterial( int material_index ) const { return m_vecMaterial[material_index]; }

	CMeshMaterial& Material( int material_index ) { return m_vecMaterial[material_index]; }

	/// the number of textures for the i-th material
	int GetNumTextures( int material_index ) const { return (int)m_vecMaterial[material_index].Texture.size(); }

	inline CTextureHandle& GetTexture( int material_index, int tex_index ) { return m_vecMaterial[material_index].Texture[tex_index]; }

	const AABB3& GetAABB( int material_index ) const { return m_vecAABB[material_index]; }

	// methods for skeletal mesh
	// - implementation class of skeletal mesh has to implement these functions

	virtual int GetNumBones() const { return 0; }

	virtual int GetBoneMatrixIndexByName( const std::string& bone_name ) const { return 0; }

//	inline const CMM_Bone& GetBone( int index ) const;

	virtual const CMM_Bone& GetBone( const std::string& bone_name ) const { return CMM_Bone::NullBone(); }

//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );
	/// calculate hierarchical transforms by handiing an array of matrices
	/// that represents local transformations at each bone
	virtual void SetLocalTransforms( Matrix34 *paLocalTransform ) {}
	/// set local transformation for a bone to a cache
	/// NOTE: do not updates transformation for the bone
	/// use is responsible for calling SetLocalTransformsFromCache() later to updates the transformations for bones
	virtual void SetLocalTransformToCache( int index, const Matrix34& local_transform ) {}
	virtual void SetLocalTransformsFromCache() {}
	virtual void ResetLocalTransformsCache() {}
	/// returns the pointer to the array of vertex blend matrices (world transforms)
//	inline void GetBlendMatrices( D3DXMATRIX* paDestMatrix ) { paDestMatrix = m_paBoneMatrix; }
//	inline D3DXMATRIX* GetBlendMatrices() { return m_paBoneMatrix; }
	virtual D3DXMATRIX* GetBlendMatrices() { return NULL; }

	virtual void SetVertexDeclaration() {}

	virtual void UpdateVisibility( const CCamera& cam ) {}
};


class CBasicMesh
{
protected:

	boost::shared_ptr<CMeshImpl> m_pImpl;

public:

	CBasicMesh();
/*
	CBasicMesh( boost::shared_ptr<CMeshImpl pImpl )
		:
	m_pImpl(pImpl)
	{}
*/
	virtual ~CBasicMesh() {}

	virtual CMeshType::Name GetMeshType() const { return CMeshType::BASIC; }

	bool CBasicMesh::IsValid() { return m_pImpl->IsValid(); }

	/// returns true on success
	bool LoadFromFile( const std::string& filename, U32 option_flags = 0 ) { return m_pImpl->LoadFromFile( filename, option_flags ); }

	bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags ) { return m_pImpl->LoadFromArchive( archive, filename, option_flags ); }

//	bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes ) { return m_pImpl->LoadFromArchive( archive, filename, option_flags, num_pmeshes ); }

	/// D3D mesh access

	LPD3DXBASEMESH GetBaseMesh() { return m_pImpl->GetBaseMesh(); }

	LPD3DXMESH GetMesh() { return m_pImpl->GetMesh(); }

	/// rendering

	/// render object by using the fixed function pipeline
	void Render() { m_pImpl->Render(); }

	void Render( CShaderManager& rShaderMgr ) { m_pImpl->Render( rShaderMgr ); }

	void Render( CShaderManager& rShaderMgr, std::vector<CShaderTechniqueHandle>& vecShaderTechnique ) { m_pImpl->Render( rShaderMgr, vecShaderTechnique ); }

	void RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex,
								std::vector<CShaderTechniqueHandle>& vecShaderTechnique ) { m_pImpl->RenderSubsets( rShaderMgr, vecMaterialIndex, vecShaderTechnique ); }

	void RenderSubsets( CShaderManager& rShaderMgr,
		const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */) { m_pImpl->RenderSubsets( rShaderMgr, vecMaterialIndex ); }

	virtual bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, std::vector<D3DVERTEXELEMENT9>& vecVertexElement ) { return m_pImpl->CreateMesh( num_vertices, num_indices, option_flags, vecVertexElement ); }

	bool LoadNonAsyncResources( C3DMeshModelArchive& rArchive, U32 option_flags ) { return m_pImpl->LoadNonAsyncResources( rArchive, option_flags ); }

	int GetNumMaterials() const { return m_pImpl->GetNumMaterials(); }

	/// returns const reference to the i-th material
	const CMeshMaterial& GetMaterial( int material_index ) const { return m_pImpl->GetMaterial( material_index ); }

	CMeshMaterial& Material( int material_index ) { return m_pImpl->Material( material_index ); }

	/// the number of textures for the i-th material
	int GetNumTextures( int material_index ) const { return m_pImpl->GetNumTextures( material_index ); }

	inline CTextureHandle& GetTexture( int material_index, int tex_index ) { return m_pImpl->GetTexture( material_index, tex_index ); }

	const AABB3& GetAABB( int material_index ) const { return m_pImpl->GetAABB( material_index ); }

	// methods for skeletal mesh
	// - skeletal mesh class has to implement these functions
/*
	int GetNumBones() const { return m_pImpl->GetNumBones(); }
	int GetBoneMatrixIndexByName( const std::string& bone_name ) const { return m_pImpl->GetBoneMatrixIndexByName( bone_name ); }
//	inline const CMM_Bone& GetBone( int index ) const;
	const CMM_Bone& GetBone( const std::string& bone_name ) const { return m_pImpl->GetBone( bone_name ); }
//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );
	/// calculate hierarchical transforms by handiing an array of matrices
	/// that represents local transformations at each bone
	void SetLocalTransforms( Matrix34 *paLocalTransform ) { m_pImpl->SetLocalTransforms( paLocalTransform ); }

	/// set local transformation for a bone to a cache
	/// NOTE: do not updates transformation for the bone
	/// use is responsible for calling SetLocalTransformsFromCache() later to updates the transformations for bones
	void SetLocalTransformToCache( int index, const Matrix34& local_transform ) { m_pImpl->SetLocalTransformToCache( index, local_transform ); }
	void SetLocalTransformsFromCache() { m_pImpl->SetLocalTransformsFromCache(); }
	void ResetLocalTransformsCache() { m_pImpl->ResetLocalTransformsCache(); }
	/// returns the pointer to the array of vertex blend matrices (world transforms)
//	inline void GetBlendMatrices( D3DXMATRIX* paDestMatrix ) { paDestMatrix = m_paBoneMatrix; }
//	inline D3DXMATRIX* GetBlendMatrices() { return m_paBoneMatrix; }
	D3DXMATRIX* GetBlendMatrices() { return m_pImpl->GetBlendMatrices(); }
*/
	// Call this before rendering when you render subsets separately by RenderSubsets()
	void SetVertexDeclaration() { m_pImpl->SetVertexDeclaration(); }

	void UpdateVisibility( const CCamera& cam ) { m_pImpl->UpdateVisibility( cam ); }

	// Access GetImpl() and dynamic_cast the returned pointer to lock/unlock vertex/index buffer, and to do other operations on mesh
	friend class CD3DXMeshVerticesLoader;
	friend class CD3DXMeshIndicesLoader;
};


class CMeshImplFactory
{
public:

	CMeshImplFactory() {}
	virtual ~CMeshImplFactory() {}

//	virtual CMeshImpl* CreateMeshImpl( CMeshType::Name mesh_type ) = 0;

//	boost::shared_ptr<CMeshImpl> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );// = 0; //{ return boost::shared_ptr<CMeshImpl>(); }

	virtual CMeshImpl* CreateBasicMeshImpl() { return NULL; }
	virtual CMeshImpl* CreateProgressiveMeshImpl() { return NULL; }
	virtual CMeshImpl* CreateSkeletalMeshImpl() { return NULL; }

/*
	CMeshImpl* LoadMeshObjectFromFile( const std::string& filepath,
		                               U32 load_option_flags = 0,
		                               CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	CMeshImpl*  LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                 const std::string& filepath,
										 U32 load_option_flags = 0,
										 CMeshType::Name mesh_type = CMeshType::SKELETAL );
*/
};


inline boost::shared_ptr<CMeshImplFactory>& MeshImplFactory()
{
	static boost::shared_ptr<CMeshImplFactory> s_pMeshFactory;
	return s_pMeshFactory;
}


class CMeshFactory
{
public:

//	virtual boost::shared_ptr<CBasicMesh> CreateBasicMeshImpl() = 0;
//	virtual boost::shared_ptr<CProgressiveMesh> CreateProgressiveMeshImpl() = 0;
//	virtual boost::shared_ptr<CSkeletalMesh> CreateSkeletalMeshImpl() = 0;
/*	virtual boost::shared_ptr<CMeshImpl> CreateBasicMeshImpl() = 0;
	virtual boost::shared_ptr<CMeshImpl> CreateProgressiveMeshImpl() = 0;
	virtual boost::shared_ptr<CMeshImpl> CreateSkeletalMeshImpl() = 0;
*/
	CBasicMesh *CreateMeshInstance( CMeshType::Name mesh_type = CMeshType::SKELETAL );

	boost::shared_ptr<CBasicMesh> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );

	CBasicMesh *CreateBasicMeshInstance();
	CProgressiveMesh *CreateProgressiveMeshInstance();
	CSkeletalMesh *CreateSkeletalMeshInstance();

	boost::shared_ptr<CBasicMesh> CreateBasicMesh();
	boost::shared_ptr<CProgressiveMesh> CreateProgressiveMesh();
	boost::shared_ptr<CSkeletalMesh> CreateSkeletalMesh();

	CBasicMesh* LoadMeshObjectFromFile( const std::string& filepath,
		                                U32 load_option_flags = 0,
		                                CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	CBasicMesh* LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                    const std::string& filepath,
											U32 load_option_flags = 0,
											CMeshType::Name mesh_type = CMeshType::SKELETAL );
};

/*
inline CMeshFactory& MeshFactory()
{
	static boost::shared_ptr<CMeshFactory> s_pMeshFactory;
	if( s_pMeshFactory == boost::shared_ptr<CMeshFactory>() )
	{
		InitMeshFactory( s_pMeshFactory );
	}

	return *(MeshFactoryPtr().get());
}*/



inline boost::shared_ptr<CMeshFactory>& MeshFactoryPtr()
{
	static boost::shared_ptr<CMeshFactory> s_pMeshFactory;
	return s_pMeshFactory;
}


// MeshFactoryPtr() must be initialized before calling this
inline CMeshFactory& MeshFactory()
{
	return *(MeshFactoryPtr().get());
}


/*
class CMeshObjectFactory
{
public:

	CMeshObjectFactory() {}
	virtual ~CMeshObjectFactory() {}

	boost::shared_ptr<CBasicMesh> CreateMesh( CMeshType::Name mesh_type = CMeshType::SKELETAL );

	CD3DXMeshObjectBase* LoadMeshObjectFromFile( const std::string& filepath,
		                                         U32 load_option_flags = 0,
		                                         CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	CD3DXMeshObjectBase*  LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                             const std::string& filepath,
													 U32 load_option_flags = 0,
													 CMeshType::Name mesh_type = CMeshType::SKELETAL );
};
*/


#endif /* __BasicMesh_HPP__ */
