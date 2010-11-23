#ifndef __BasicMesh_HPP__
#define __BasicMesh_HPP__


#include <gds/base.hpp>
#include <gds/3DMath/AABB3.hpp>
#include <gds/3DMath/Transform.hpp>
//#include "MeshImpl.hpp"
#include <gds/Graphics/fwd.hpp>
#include <gds/Graphics/TextureHandle.hpp>
#include <gds/Graphics/Shader/ShaderTechniqueHandle.hpp>
using namespace MeshModel;


class MeshLoadOption
{
public:
	enum Flag
	{
		DO_NOT_LOAD_TEXTURES = ( 1 << 0 ), ///< specify this to load textures later. e.g., for asynchronous loading
		LOAD_ASYNC           = ( 1 << 1 ), ///< asynchronously load the mesh
		LOAD_TEXTURES_ASYNC  = ( 1 << 2 ), ///< asynchronously load the textures of the mesh
		CUSTOM_MESH          = ( 1 << 3 ), ///< Choose CCustomMesh, a platform independent implementation, for mesh's impl class. This is a mesh creation option rather than a mesh loading option.
//		ANOTHER_OPTION       = ( 1 << 4 ),
//		YET_ANOTHER_OPTION   = ( 1 << 5 ),
	};
};


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

	std::string Name;

	float fMinVertexDiffuseAlpha;

public:

	/// Load the i-th texture asynchronously
	void LoadTextureAsync( int i );
};


/**
base class of mesh implementation
- Derived by implementation classes of Direct3D and OpenGL
- Platform independent attributes are stored here.
*/
class CMeshImpl
{
protected:

	std::string m_strFilename;

	/// Number of materials
	int m_NumMaterials;

	/// materials for each subset
	std::vector<CMeshMaterial> m_vecMaterial;

	/// Holds material indices
	/// The values are always set to [0,m_NumMaterials-1]
	/// Used to avoid dynamic memory allocation in RenderSubsets()
	std::vector<int> m_vecFullMaterialIndices;

	/// bounding box of the mesh
	AABB3 m_AABB;

	/// aabb for each subset
	std::vector<AABB3> m_vecAABB;

	/// enable / disable visibility clipping based on view frustum test.
	/// If true, user is responsible for calling UpdateVisibility( const CCamera& camera )
	/// every time the mesh is rendered.
	bool m_bViewFrustumTest;

	/// visibility flag for the mesh and subsets
	/// 1 visible / 0: not visible
	/// All elements are set to 1(visible) by default
	std::vector<int> m_IsVisible;

protected:

	/// Loads materials and aabbs. Also initializes m_IsVisible.
	Result::Name LoadMaterialsFromArchive( C3DMeshModelArchive& rArchive, U32 option_flags );

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

	virtual void RenderSubset( CShaderManager& rShaderMgr, int material_index ) = 0;

	/// Calls the raw array version by default
	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex,
								std::vector<CShaderTechniqueHandle>& vecShaderTechnique )
	{
		if( !vecMaterialIndex.empty() && vecMaterialIndex.size() == vecShaderTechnique.size() )
			RenderSubsets( rShaderMgr, &vecMaterialIndex[0], &vecShaderTechnique[0], (int)vecMaterialIndex.size() );
	}

	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const int *paMaterialIndex,
								CShaderTechniqueHandle *paShaderTechnique,
								int num_indices ) {}

	/// Calls the raw array version by default
	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */)
	{
		if( !vecMaterialIndex.empty() )
			RenderSubsets( rShaderMgr, &vecMaterialIndex[0], (int)vecMaterialIndex.size() );
	}

	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const int* paMaterialIndex,
	                            int num_indices /* some option to specify handles for texture */) {}

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

	// returns the axis-aligned bounding box of the mesh
	const AABB3& GetAABB() const { return m_AABB; }

	virtual unsigned int GetNumVertices() const { return 0; }

	virtual unsigned int GetNumTriangles() const { return 0; }

	// methods for skeletal mesh
	// - implementation class of skeletal mesh has to implement these functions

	virtual int GetNumBones() const { return 0; }

	virtual int GetBoneMatrixIndexByName( const std::string& bone_name ) const { return 0; }

//	inline const CMeshBone& GetBone( int index ) const;

	virtual const CMeshBone& GetBone( const std::string& bone_name ) const;

	virtual const CMeshBone& GetRootBone() const;

//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );
	/// calculate hierarchical transforms by handiing an array of matrices
	/// that represents local transformations at each bone
	virtual void SetLocalTransforms( Matrix34 *paLocalTransform ) {}
	/// set local transformation for a bone to a cache
	/// NOTE: do not updates transformation for the bone
	/// use is responsible for calling SetLocalTransformsFromCache() later to updates the transformations for bones
	virtual void SetLocalTransformToCache( int index, const Matrix34& local_transform ) {}
	virtual void CalculateBlendTransformsFromCachedLocalTransforms() {}
	virtual void ResetLocalTransformsCache() {}
	virtual void CalculateBlendTransforms( const std::vector<Transform>& src_local_transforms, std::vector<Transform>& dest_blend_transforms ) {}

	/// returns the pointer to the array of vertex blend matrices (world transforms)
	virtual Transform* GetBlendTransforms() { return NULL; }

	virtual void GetBlendTransforms( std::vector<Transform>& dest_transforms ) {}

	virtual void DumpSkeletonToTextFile( const std::string& output_filepath ) const {}

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

	void RenderSubset( CShaderManager& rShaderMgr, int material_index ) { m_pImpl->RenderSubset( rShaderMgr, material_index ); }

	void RenderSubsets( CShaderManager& rShaderMgr,
		                const std::vector<int>& vecMaterialIndex,
						std::vector<CShaderTechniqueHandle>& vecShaderTechnique ) { m_pImpl->RenderSubsets( rShaderMgr, vecMaterialIndex, vecShaderTechnique ); }

	/// paMaterialIndex [in] pointer to the array of subset indices
	/// paShaderTechnique [in] pointer to the array of shader techniques. Array size must be the same as paMaterialIndex.
	/// num_indices [in] the number of indices
	void RenderSubsets( CShaderManager& rShaderMgr,
		                const int *paMaterialIndex,
						CShaderTechniqueHandle *paShaderTechnique,
						int num_indices ) { m_pImpl->RenderSubsets( rShaderMgr, paMaterialIndex, paShaderTechnique, num_indices ); }

	/// renders subsets of the mesh with the current shader technique
	/// - the same shader technique is used to render all the materials
	void RenderSubsets( CShaderManager& rShaderMgr,
		const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */) { m_pImpl->RenderSubsets( rShaderMgr, vecMaterialIndex ); }

	/// paMaterialIndex [in] pointer to the array of subset indices
	/// num_indices [in] the number of indices
	void RenderSubsets( CShaderManager& rShaderMgr,
		const int* paMaterialIndex,
		int num_indices /* some option to specify handles for texture */) { m_pImpl->RenderSubsets( rShaderMgr, paMaterialIndex, num_indices ); }

	virtual bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, std::vector<D3DVERTEXELEMENT9>& vecVertexElement ) { return m_pImpl->CreateMesh( num_vertices, num_indices, option_flags, vecVertexElement ); }

	bool LoadNonAsyncResources( C3DMeshModelArchive& rArchive, U32 option_flags ) { return m_pImpl->LoadNonAsyncResources( rArchive, option_flags ); }

	int GetNumMaterials() const { return m_pImpl->GetNumMaterials(); }

	/// returns const reference to the i-th material
	const CMeshMaterial& GetMaterial( int material_index ) const { return m_pImpl->GetMaterial( material_index ); }

	CMeshMaterial& Material( int material_index ) { return m_pImpl->Material( material_index ); }

	/// the number of textures for the i-th material
	int GetNumTextures( int material_index ) const { return m_pImpl->GetNumTextures( material_index ); }

	inline CTextureHandle& GetTexture( int material_index, int tex_index ) { return m_pImpl->GetTexture( material_index, tex_index ); }

	const AABB3& GetAABB() const { return m_pImpl->GetAABB(); }

	const AABB3& GetAABB( int material_index ) const { return m_pImpl->GetAABB( material_index ); }

	unsigned int GetNumVertices() const { return m_pImpl->GetNumVertices(); }

	unsigned int GetNumTriangles() const { return m_pImpl->GetNumTriangles(); }

	// methods for skeletal mesh
	// - skeletal mesh class has to implement these functions
/*
	int GetNumBones() const { return m_pImpl->GetNumBones(); }
	int GetBoneMatrixIndexByName( const std::string& bone_name ) const { return m_pImpl->GetBoneMatrixIndexByName( bone_name ); }
//	inline const CMeshBone& GetBone( int index ) const;
	const CMeshBone& GetBone( const std::string& bone_name ) const { return m_pImpl->GetBone( bone_name ); }
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
*/
	// Call this before rendering when you render subsets separately by RenderSubsets()
	void SetVertexDeclaration() { m_pImpl->SetVertexDeclaration(); }

	void UpdateVisibility( const CCamera& cam ) { m_pImpl->UpdateVisibility( cam ); }

	friend boost::shared_ptr<CCustomMesh> GetCustomMesh( CBasicMesh& src_mesh );

	/// Overwrites the m_pImpl with a CCustomMesh instance
	friend void SetCustomMesh( CBasicMesh& src_mesh );

	// Access GetImpl() and dynamic_cast the returned pointer to lock/unlock vertex/index buffer, and to do other operations on mesh
	friend class CD3DXMeshVerticesLoader;
	friend class CD3DXMeshIndicesLoader;
};



#endif /* __BasicMesh_HPP__ */
