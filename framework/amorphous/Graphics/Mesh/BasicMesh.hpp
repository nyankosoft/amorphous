#ifndef __BasicMesh_HPP__
#define __BasicMesh_HPP__


#include "../../3DMath/AABB3.hpp"
#include "../../3DMath/Transform.hpp"
#include "../../Graphics/TextureHandle.hpp"
#include "../../Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "BasicMaterialParams.hpp"
//#include "MeshImpl.hpp"


namespace amorphous
{


class MeshLoadOption
{
public:
	enum Flag
	{
		DO_NOT_LOAD_TEXTURES = ( 1 << 0 ), ///< specify this to load textures later. e.g., for asynchronous loading
		LOAD_ASYNC           = ( 1 << 1 ), ///< asynchronously load the mesh
		LOAD_TEXTURES_ASYNC  = ( 1 << 2 ), ///< asynchronously load the textures of the mesh
		CUSTOM_MESH          = ( 1 << 3 ), ///< Choose CustomMesh, a platform independent implementation, for mesh's impl class. This is a mesh creation option rather than a mesh loading option.
//		ANOTHER_OPTION       = ( 1 << 4 ),
//		YET_ANOTHER_OPTION   = ( 1 << 5 ),
	};
};


class MeshMaterial
{
public:

	CBasicMaterialParams m_Mat;

	// array of textures for a material
	// - possible usages
	//   - surface color (most common)
	//   - normal map
	//   - specular map (often stored in alpha channel of normal map)
	// A texture handle will be empty if no texture is used for the stage
	std::vector<TextureHandle> Texture;

	std::vector<TextureResourceDesc> TextureDesc;

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
class MeshImpl
{
protected:

	std::string m_strFilename;

	/// materials for each subset
	std::vector<MeshMaterial> m_vecMaterial;

	/// Holds material indices
	/// The values are always set to [0,m_vecMaterial.size()-1]
	/// Used to avoid dynamic memory allocation in RenderSubsets()
	std::vector<int> m_vecFullMaterialIndices;

	/// bounding box of the mesh
	AABB3 m_AABB;

	/// aabb for each subset
	std::vector<AABB3> m_vecAABB;

	/// enable / disable visibility clipping based on view frustum test.
	/// If true, user is responsible for calling UpdateVisibility( const Camera& camera )
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

	MeshImpl();

	/// returns true on success
	virtual bool LoadFromFile( const std::string& filename, U32 option_flags = 0 );

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags ) = 0;//{ return m_pImpl->LoadFromArchive( archive, filename, option_flasgs ); }

//	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes ) { return false; }

	virtual bool IsValid() const
	{
		if( 0 < GetNumVertices()
		 && 0 < GetNumIndices() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/// rendering

	/// render object by using the fixed function pipeline
	virtual void Render() = 0;

	virtual void Render( ShaderManager& rShaderMgr ) = 0;

	virtual void Render( ShaderManager& rShaderMgr, std::vector<ShaderTechniqueHandle>& vecShaderTechnique );

	virtual void RenderSubset( ShaderManager& rShaderMgr, int material_index ) = 0;

	/// Calls the raw array version by default
	virtual void RenderSubsets( ShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex,
								std::vector<ShaderTechniqueHandle>& vecShaderTechnique )
	{
		if( !vecMaterialIndex.empty() && vecMaterialIndex.size() == vecShaderTechnique.size() )
			RenderSubsets( rShaderMgr, &vecMaterialIndex[0], &vecShaderTechnique[0], (int)vecMaterialIndex.size() );
	}

	virtual void RenderSubsets( ShaderManager& rShaderMgr,
		                        const int *paMaterialIndex,
								ShaderTechniqueHandle *paShaderTechnique,
								int num_indices ) {}

	/// Calls the raw array version by default
	virtual void RenderSubsets( ShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */)
	{
		if( !vecMaterialIndex.empty() )
			RenderSubsets( rShaderMgr, &vecMaterialIndex[0], (int)vecMaterialIndex.size() );
	}

	virtual void RenderSubsets( ShaderManager& rShaderMgr,
		                        const int* paMaterialIndex,
	                            int num_indices /* some option to specify handles for texture */) {}

	/// Currently implemented only by CustomMesh
	virtual void RenderZSorted( ShaderManager& rShaderMgr ) {};

	bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, U32 vertex_format_flags ) { return false; }

	bool LoadNonAsyncResources( C3DMeshModelArchive& rArchive, U32 option_flags ) { return false; }

	int GetNumMaterials() const { return (int)m_vecMaterial.size(); }

	/// returns const reference to the i-th material
	const MeshMaterial& GetMaterial( int material_index ) const { return m_vecMaterial[material_index]; }

	const std::vector<MeshMaterial>& GetMaterials() const { return m_vecMaterial; }

	MeshMaterial& Material( int material_index ) { return m_vecMaterial[material_index]; }

	std::vector<MeshMaterial>& Materials() { return m_vecMaterial; }

	/// the number of textures for the i-th material
	int GetNumTextures( int material_index ) const { return (int)m_vecMaterial[material_index].Texture.size(); }

	inline TextureHandle& GetTexture( int material_index, int tex_index ) { return m_vecMaterial[material_index].Texture[tex_index]; }

	const AABB3& GetAABB( int material_index ) const { return m_vecAABB[material_index]; }

	// returns the axis-aligned bounding box of the mesh
	const AABB3& GetAABB() const { return m_AABB; }

	virtual unsigned int GetNumVertices() const { return 0; }

	virtual void GetVertexPositions( std::vector<Vector3>& dest_vertices ) const {}

	virtual void GetVertexNormals( std::vector<Vector3>& dest_normals ) const {}

	virtual unsigned int GetNumTriangles() const { return 0; }

	virtual unsigned int GetNumIndices() const { return 0; }

	virtual void GetVertexIndices( std::vector<unsigned int>& dest ) {}

	// methods for skeletal mesh
	// - implementation class of skeletal mesh has to implement these functions

	virtual int GetNumBones() const { return 0; }

	virtual int GetBoneMatrixIndexByName( const std::string& bone_name ) const { return 0; }

//	inline const MeshBone& GetBone( int index ) const;

	virtual const MeshBone& GetBone( const std::string& bone_name ) const;

	virtual const MeshBone& GetRootBone() const;

//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );

	/// set local transformation for a bone to a cache
	/// NOTE: do not updates transformation for the bone
	/// use is responsible for calling SetLocalTransformsFromCache() later to updates the transformations for bones
	virtual void SetLocalTransformToCache( int index, const Matrix34& local_transform ) {}
	virtual void CalculateBlendTransformsFromCachedLocalTransforms() {}
	virtual void ResetLocalTransformsCache() {}
	virtual void CalculateBlendTransforms( const std::vector<Transform>& src_local_transforms, std::vector<Transform>& dest_blend_transforms ) {}

	/// returns the pointer to the array of vertex blend matrices (world transforms)
	virtual Transform* GetBlendTransforms() { return nullptr; }

	virtual void GetBlendTransforms( std::vector<Transform>& dest_transforms ) {}

	virtual void DumpSkeletonToTextFile( const std::string& output_filepath ) const {}

	virtual void SetVertexDeclaration() {}

	virtual void UpdateVisibility( const Camera& cam ) {}

	virtual bool IsCustomMesh() const { return false; }
};


class BasicMesh
{
protected:

	std::shared_ptr<MeshImpl> m_pImpl;

public:

	BasicMesh();
/*
	BasicMesh( std::shared_ptr<MeshImpl pImpl )
		:
	m_pImpl(pImpl)
	{}
*/
	virtual ~BasicMesh() {}

	virtual MeshTypeName GetMeshType() const { return MeshTypeName::BASIC; }

	bool IsValid() const { return m_pImpl->IsValid(); }

	/// returns true on success
	bool LoadFromFile( const std::string& filename, U32 option_flags = 0 ) { return m_pImpl->LoadFromFile( filename, option_flags ); }

	bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags ) { return m_pImpl->LoadFromArchive( archive, filename, option_flags ); }

//	bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes ) { return m_pImpl->LoadFromArchive( archive, filename, option_flags, num_pmeshes ); }

	/// rendering

	/// render object by using the fixed function pipeline
	void Render() { m_pImpl->Render(); }

	void Render( ShaderManager& rShaderMgr ) { m_pImpl->Render( rShaderMgr ); }

	void Render( ShaderManager& rShaderMgr, std::vector<ShaderTechniqueHandle>& vecShaderTechnique ) { m_pImpl->Render( rShaderMgr, vecShaderTechnique ); }

	void RenderSubset( ShaderManager& rShaderMgr, int material_index ) { m_pImpl->RenderSubset( rShaderMgr, material_index ); }

	void RenderSubsets( ShaderManager& rShaderMgr,
		                const std::vector<int>& vecMaterialIndex,
						std::vector<ShaderTechniqueHandle>& vecShaderTechnique ) { m_pImpl->RenderSubsets( rShaderMgr, vecMaterialIndex, vecShaderTechnique ); }

	/// paMaterialIndex [in] pointer to the array of subset indices
	/// paShaderTechnique [in] pointer to the array of shader techniques. Array size must be the same as paMaterialIndex.
	/// num_indices [in] the number of indices
	void RenderSubsets( ShaderManager& rShaderMgr,
		                const int *paMaterialIndex,
						ShaderTechniqueHandle *paShaderTechnique,
						int num_indices ) { m_pImpl->RenderSubsets( rShaderMgr, paMaterialIndex, paShaderTechnique, num_indices ); }

	/// renders subsets of the mesh with the current shader technique
	/// - the same shader technique is used to render all the materials
	void RenderSubsets( ShaderManager& rShaderMgr,
		const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */) { m_pImpl->RenderSubsets( rShaderMgr, vecMaterialIndex ); }

	/// paMaterialIndex [in] pointer to the array of subset indices
	/// num_indices [in] the number of indices
	void RenderSubsets( ShaderManager& rShaderMgr,
		const int* paMaterialIndex,
		int num_indices /* some option to specify handles for texture */) { m_pImpl->RenderSubsets( rShaderMgr, paMaterialIndex, num_indices ); }

	void RenderZSorted( ShaderManager& rShaderMgr ) { m_pImpl->RenderZSorted( rShaderMgr ); }

//	virtual bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, std::vector<D3DVERTEXELEMENT9>& vecVertexElement ) { return m_pImpl->CreateMesh( num_vertices, num_indices, option_flags, vecVertexElement ); }

	virtual bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, U32 vertex_format_flags ) { return m_pImpl->CreateMesh( num_vertices, num_indices, option_flags, vertex_format_flags ); }

	bool LoadNonAsyncResources( C3DMeshModelArchive& rArchive, U32 option_flags ) { return m_pImpl->LoadNonAsyncResources( rArchive, option_flags ); }

	int GetNumMaterials() const { return m_pImpl->GetNumMaterials(); }

	/// returns const reference to the i-th material
	const MeshMaterial& GetMaterial( int material_index ) const { return m_pImpl->GetMaterial( material_index ); }

	const std::vector<MeshMaterial>& GetMaterials() const { return m_pImpl->GetMaterials(); }

	MeshMaterial& Material( int material_index ) { return m_pImpl->Material( material_index ); }

	/// the number of textures for the i-th material
	int GetNumTextures( int material_index ) const { return m_pImpl->GetNumTextures( material_index ); }

	inline TextureHandle& GetTexture( int material_index, int tex_index ) { return m_pImpl->GetTexture( material_index, tex_index ); }

	const AABB3& GetAABB() const { return m_pImpl->GetAABB(); }

	const AABB3& GetAABB( int material_index ) const { return m_pImpl->GetAABB( material_index ); }

	unsigned int GetNumVertices() const { return m_pImpl->GetNumVertices(); }

	void GetVertexPositions( std::vector<Vector3>& dest_vertices ) const { return m_pImpl->GetVertexPositions( dest_vertices ); }

	void GetVertexNormals( std::vector<Vector3>& dest_normals ) const { return m_pImpl->GetVertexNormals( dest_normals ); }

	unsigned int GetNumTriangles() const { return m_pImpl->GetNumTriangles(); }

	unsigned int GetNumIndices() const { return m_pImpl->GetNumIndices(); }

	void GetVertexIndices( std::vector<unsigned int>& dest ) const { m_pImpl->GetVertexIndices( dest ); }

	// Call this before rendering when you render subsets separately by RenderSubsets()
	void SetVertexDeclaration() { m_pImpl->SetVertexDeclaration(); }

	void UpdateVisibility( const Camera& cam ) { m_pImpl->UpdateVisibility( cam ); }

	bool IsCustomMesh() const { return m_pImpl->IsCustomMesh(); }

	friend std::shared_ptr<CustomMesh> GetCustomMesh( BasicMesh& src_mesh );

	/// Overwrites the m_pImpl with a CustomMesh instance
	friend void SetCustomMesh( BasicMesh& src_mesh );

	// Access GetImpl() and dynamic_cast the returned pointer to lock/unlock vertex/index buffer, and to do other operations on mesh
	friend class CD3DXMeshVerticesLoader;
	friend class CD3DXMeshIndicesLoader;
	friend class CD3DXMeshAttributeTableLoader;
};


} // namespace amorphous



#endif /* __BasicMesh_HPP__ */
