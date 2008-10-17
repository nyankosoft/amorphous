#ifndef  __D3DXMESHOBJECTBASE_H__
#define  __D3DXMESHOBJECTBASE_H__


#include <vector>
#include <string>

#include "3DMath/aabb3.h"
#include "3DMath/Sphere.h"
#include "3DCommon/fwd.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/TextureHandle.h"
#include "3DCommon/Shader/ShaderTechniquehandle.h"

#include "3DCommon/MeshModel/3DMeshModelArchive.h"
using namespace MeshModel;


class MeshLoadOption
{
public:
	enum Flag
	{
		DO_NOT_LOAD_TEXTURES = ( 1 << 0 ), ///< specify this to load textures later. e.g., for asynchronous loading
		LOAD_ASYNC           = ( 1 << 1 ), ///< asynchronously load the mesh
//		ANOTHER_OPTION       = ( 1 << 2 ),
//		YET_ANOTHER_OPTION   = ( 1 << 3 ),
	};
};


/**
 Base class of the D3D implementation of the mesh class
*/
class CD3DXMeshObjectBase
{
public:
	
	class CMeshMaterial
	{
	public:

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

private:

	std::vector<int> m_vecFullMaterialIndices;

protected:

	// platform independent attributes

	std::string m_strFilename;

	std::vector<CMeshMaterial> m_vecMaterial;

	/// bounding sphere in local space of the model
	/// - Each implementation must properly initialize the sphere
	Sphere m_LocalShpere;

	/// bounding box of the mesh
	AABB3 m_AABB;

	std::vector<AABB3> m_vecAABB;	///< aabb for each triangle subset

	/// visibility flag for each triangle set
	/// 1 visible / 0: not visible
	/// all elements set to 1 by default
	std::vector<int> m_IsVisible;

	/// enable / disable visibility clipping based on view frustum test.
	/// If true, user is responsible for calling UpdateVisibility( const CCamera& camera )
	/// every time the mesh is rendered.
	bool m_bViewFrustumTest;


	// D3D attributes

	/// Number of materials
	int m_NumMaterials;

	D3DMATERIAL9 *m_pMeshMaterials;

	/// flexible vertex format flag
	DWORD m_dwFVF;

	/// size of vertex (in bytes)
	int m_iVertexSize;

	/// vertex elements
	/// - updated when LoadVertices() is called.
	D3DVERTEXELEMENT9 *m_paVertexElements;

	/// vertex decleration
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDecleration;

private:

	/// allocate material buffers, etc.
	void InitMaterials( int num_materials );

protected:

	virtual void LoadVertices( void*& pVBData, C3DMeshModelArchive& archive );

	bool LoadIndices( unsigned short*& pIBData, C3DMeshModelArchive& archive );

	HRESULT LoadMaterials( D3DXMATERIAL* d3dxMaterials, int num_materials );

	HRESULT LoadMaterialsFromArchive( C3DMeshModelArchive& rArchive, U32 option_flags );

//	virtual const D3DVERTEXELEMENT9 *GetVertexElemenets( CMMA_VertexSet& rVertexSet );

	bool FillIndexBuffer( LPD3DXMESH pMesh, C3DMeshModelArchive& archive );

	/// materials must be loaded before calling this method
	HRESULT SetAttributeTable( LPD3DXMESH pMesh, const vector<CMMA_TriangleSet>& vecTriangleSet );

	/// Synchronously load D3DXMesh
	LPD3DXMESH LoadD3DXMeshFromArchive( C3DMeshModelArchive& archive );

	/// \param xfilename [in] .x file
	/// \param rpMesh [out] reference to a pointer that receives the mesh loaded by this function
	/// \param rpAdjacencyBuffer [out] reference to a pointer that receives adjacency buffer
	/// materials are also loaded from the input .x file and stored in CD3DXMeshObjectBase::m_pMeshMaterials
	HRESULT LoadD3DXMeshAndMaterialsFromXFile( const std::string& xfilename,
		                                       LPD3DXMESH& rpMesh,
		                                       LPD3DXBUFFER& rpAdjacencyBuffer );

	/// check the attribute tables (for debugging)
	void PeekAttribTables( LPD3DXBASEMESH pMesh );

	/// procedures to create bounding sphere from D3DXMesh
	/// - Deprecated, and nobody's using this. See this as a sample code
	/// - The bounding shpere should be create from mesh archives
	HRESULT CreateLocalBoundingSphereFromD3DXMesh( LPD3DXMESH pMesh );

	/// load mesh from .x file
	/// - Declared protected since this is a platform dependent
	///   and sould not be directly called by outside module
	///   unlike LoadFromArchive()
	virtual HRESULT LoadFromXFile( const std::string& filename ) = 0;

public:

	inline CD3DXMeshObjectBase();

	virtual ~CD3DXMeshObjectBase() { Release(); }

	/// returns true on success
	bool LoadFromFile( const std::string& filename, U32 opiton_flags = 0 );

	/// load basic mesh properties from a mesh archive
	/// NOTE: filename is required to get the path for textures files
	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags = 0 ) = 0;

	virtual void Release();

	virtual LPD3DXBASEMESH GetBaseMesh() { return NULL; }

	/// added to call LockAttributeBuffer() during asynchrnous loading
	/// - LockAttributeBuffer() is not a member of LPD3DXBASEMESH
	virtual LPD3DXMESH GetMesh() { return NULL; }

	inline int GetNumMaterials() const { return m_NumMaterials; }

	inline const D3DMATERIAL9& GetD3DMaterial( int i ) const { return m_pMeshMaterials[i]; }

	/// returns const reference to the i-th material
	inline const CMeshMaterial& GetMaterial( int material_index ) const { return m_vecMaterial[material_index]; }

	/// returns non-const reference to the i-th material
	inline CMeshMaterial& Material( int material_index ) { return m_vecMaterial[material_index]; }

	bool CreateVertexDeclaration();

	inline LPDIRECT3DVERTEXDECLARATION9 GetVertexDeclaration() { return m_pVertexDecleration; }

	int GetVertexSize() const { return m_iVertexSize; }

	/// user is responsible for updating the visibility by calling UpdateVisibility( const CCamera& cam )
	void ViewFrustumTest( bool do_test ) { m_bViewFrustumTest = do_test; }

	void UpdateVisibility( const CCamera& cam );

	inline bool IsMeshVisible( int triset_index ) const { return m_IsVisible[triset_index]==1 ? true : false; }


	inline bool IsMeshVisible() const { return m_IsVisible[m_NumMaterials]==1 ? true : false; }

	inline CTextureHandle& GetTexture( int material_index, int tex_index );

	/// the number of textures for the i-th material
	int GetNumTextures( int material_index ) const { return (int)m_vecMaterial[material_index].Texture.size(); }

	const AABB3& GetAABB( int material_index ) const { return m_vecAABB[material_index]; }

	//
	// render functions
	//

	/// renders a single subset of the mesh with the current shader technique
	inline void RenderSubset( CShaderManager& rShaderMgr, int material_index );

	/// renders subsets of the mesh
	/// - use different shader techniques for each material
	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex,
								std::vector<CShaderTechniqueHandle>& vecShaderTechnique );

	/// renders subsets of the mesh with the current shader technique
	/// - the same shader technique is used to render all the materials
	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */);

	/// renders the mesh with the current shader technique
	/// - Assumes that you have already set a valid technique that can be obtained from 'rShaderMgr'
	/// - the same shader technique is used to render all the materials
	inline void Render( CShaderManager& rShaderMgr );

	/// Use this when you wanna use different shader techniques for each material
	/// \param vecShaderTechnique shader techniques for each material
	/// - What to do if the single shader technique is applied for all materials
	///   - shader_mgr.SetShaderTechnique() and call CD3DXMeshObjectBase::Render( shader_mgr )
	inline void Render( CShaderManager& rShaderMgr, std::vector<CShaderTechniqueHandle>& vecShaderTechnique );

	virtual bool LockVertexBuffer( void*& pLockedVertexBuffer );

	virtual bool LockIndexBuffer( void*& pLockedIndexBuffer );

	virtual bool LockAttributeBuffer( DWORD*& pLockedAttributeBuffer );

	virtual bool UnlockVertexBuffer();

	virtual bool UnlockIndexBuffer();

	virtual bool UnlockAttributeBuffer();

	virtual CMeshType::Name GetMeshType() const = 0;

	friend class CD3DXMeshVerticesLoader;
	friend class CD3DXMeshIndicesLoader;

};


class CMeshObjectFactory
{
public:

	CMeshObjectFactory() {}
	virtual ~CMeshObjectFactory() {}

	CD3DXMeshObjectBase* LoadMeshObjectFromFile( const std::string& filepath,
		                                         U32 load_option_flags = 0,
		                                         CMeshType::Name mesh_type = CMeshType::SKELETAL );

	/// TODO: support PMesh and SMesh
	CD3DXMeshObjectBase*  LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
		                                             const std::string& filepath,
													 U32 load_option_flags = 0,
													 CMeshType::Name mesh_type = CMeshType::SKELETAL );

};



// ================================= inline implementations =================================


inline CD3DXMeshObjectBase::CD3DXMeshObjectBase()
:
m_NumMaterials(0L),
m_pMeshMaterials(NULL),
m_iVertexSize(0),
m_paVertexElements(NULL),
m_pVertexDecleration(NULL),
m_bViewFrustumTest(false)
{
}


inline CTextureHandle& CD3DXMeshObjectBase::GetTexture( int material_index, int tex_index )
{
	return m_vecMaterial[material_index].Texture[tex_index];
}


/// \param vecMaterialIndex indices of materials(subsets) to render 
inline void CD3DXMeshObjectBase::Render( CShaderManager& rShaderMgr )
{
	RenderSubsets( rShaderMgr, m_vecFullMaterialIndices );
}


inline void CD3DXMeshObjectBase::Render( CShaderManager& rShaderMgr,
										 std::vector<CShaderTechniqueHandle>& vecShaderTechnique )
{
	RenderSubsets( rShaderMgr, m_vecFullMaterialIndices, vecShaderTechnique );
}


inline void CD3DXMeshObjectBase::RenderSubset( CShaderManager& rShaderMgr, int material_index )
{
	vector<int> single_index;
	single_index.push_back( material_index );

	RenderSubsets( rShaderMgr, single_index );
}


#endif		/*  __D3DXMESHOBJECTBASE_H__  */
