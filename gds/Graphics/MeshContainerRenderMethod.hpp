#ifndef __MeshContainerRenderMethod_HPP__
#define __MeshContainerRenderMethod_HPP__


#include "TextureHandle.hpp"
#include "MeshObjectHandle.hpp"
#include "ShaderHandle.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"
#include "Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/XML/fwd.hpp"


namespace amorphous
{


class CShaderParamsLoader
{
public:

	virtual ~CShaderParamsLoader() {}

	virtual void UpdateShaderParams( CShaderManager& rShaderMgr ) = 0;

	virtual void ResetShaderParams( CShaderManager& rShaderMgr ) {};

	virtual bool LoadResource() { return true; }
};


/// Stores params and settings to render all the subsets of a mesh or a subset of a mesh
class CSubsetRenderMethod : public IArchiveObjectBase
{
public:

	ShaderResourceDesc m_ShaderDesc;

	ShaderHandle m_Shader;
	CShaderTechniqueHandle m_Technique;

	/// Need to be set at runtime
	std::vector< boost::shared_ptr<CShaderParamsLoader> > m_vecpShaderParamsLoader;

public:

	bool Load();

	void LoadFromXMLNode( CXMLNodeReader& reader );

	void Serialize( IArchive& ar, const unsigned int version );
};


/**
 See comments in MeshObjectContainerRenderMethod for details
*/
class CMeshContainerRenderMethod : public IArchiveObjectBase
{
	/// indices of mesh subsets (previously called materials) to render
	std::vector<int> m_vecIndicesOfSubsetsToRender;

//	std::vector<CSubsetRenderMethod> m_vecSubsetRenderMethod;

	std::vector<int> m_vecFullIndicesOfSubsets;


	/// 0-origin index
	uint m_LODIndex;

public:

	// name(s) of subsets to render
	/// Leave this empty to render all the subsets.
	/// - default: empty
//	std::vector<std::string> m_vecSubsetsToRender;
//	std::vector<int>& IndicesOfSubsetsToRender() { return m_vecIndicesOfSubsetsToRender; }

	//
	// shader
	//

	/// Used when m_vecSubsetNameToRenderMethod is empty
	/// - Applies the same shader and same techniques to all the subsets of the mesh
	/// - Uses array to support LOD of shader
//	std::vector<CSubsetRenderMethod> m_vecMeshRenderMethod;

	std::vector< std::pair< CSubsetRenderMethod, std::vector<int> > > m_RenderMethodsAndSubsetIndices;

	/// Different shader & technique for each mesh subset
	/// - Uses array to support LOD of shader
	/// map of string and subset render method
	/// - string(key) - subset name saved in CMeshMaterial::Name
	/// - CSubsetRenderMethod - method to render a subset of a mesh
	std::vector< std::map<std::string,CSubsetRenderMethod> > m_vecSubsetNameToRenderMethod;

	// shader params


	//
	// extra textures
	//
/*
	std::vector<std::string> m_vecExtraTextureFilepath;

	/// holds textures used for any purposes other than
	/// usual color, alpha, normal map and specular map.
	/// For example,
	/// - look-up texture for specular highlight
	/// - look-up texture for membrane effect
	std::vector<TextureHandle> m_vecExtraTexture;
*/
private:

	void RenderMeshOrMeshSubsets(
		BasicMesh &mesh,
		const std::vector<int>& subset_indices,
		CSubsetRenderMethod& render_method,
		const Matrix34& world_transform );

public:

	CMeshContainerRenderMethod()
		:
	m_LODIndex( 0 )
	{}

	virtual ~CMeshContainerRenderMethod() {}

	void RenderMesh( BasicMesh &mesh, const Matrix34& world_transform );

	inline void RenderMesh( MeshHandle& mesh, const Matrix34& world_transform );

	void RenderMeshContainer( CMeshObjectContainer& mesh_container,
		                      const Matrix34& world_transform );
							  //std::vector< boost::shared_ptr<CShaderParamsLoader> >& vecpShaderParamsWriter );

//	std::vector<CSubsetRenderMethod>& MeshRenderMethod() { return m_vecMeshRenderMethod; }

	CSubsetRenderMethod& PrimaryMeshRenderMethod()
	{
		if( m_RenderMethodsAndSubsetIndices.empty() )
			m_RenderMethodsAndSubsetIndices.resize( 1 );

		return m_RenderMethodsAndSubsetIndices[0].first;
	}

	std::vector< std::pair< CSubsetRenderMethod, std::vector<int> > >& RenderMethodsAndSubsetIndices() { return m_RenderMethodsAndSubsetIndices; }

	std::vector< std::map<std::string,CSubsetRenderMethod> >& SubsetRenderMethodMaps() { return m_vecSubsetNameToRenderMethod; }

	void SetShaderParamsLoaderToAllMeshRenderMethods( boost::shared_ptr<CShaderParamsLoader> pShaderParamsLoader );

	void RemoveShaderParamsLoaderFromAllMeshRenderMethods( boost::shared_ptr<CShaderParamsLoader> pShaderParamsLoader );

	/// Creates a copy or copies of render methods at each LoD for subsets whose names are specified by the argument.
	void BreakMeshRenderMethodsToSubsetRenderMethods( const std::vector<std::string>& vecName );

	bool LoadRenderMethodResources();

	boost::shared_ptr<CMeshContainerRenderMethod> CreateCopy();

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	virtual void Serialize( IArchive& ar, const unsigned int version );
};


class CMeshContainerNodeRenderMethod : public IArchiveObjectBase
{
	std::vector< boost::shared_ptr<CMeshContainerRenderMethod> > m_vecpContainerRenderMethod;

	std::vector< boost::shared_ptr<CMeshContainerNodeRenderMethod> > m_vecpChild;

public:

	CMeshContainerNodeRenderMethod();

	~CMeshContainerNodeRenderMethod() {}

	void AddMeshContainer( boost::shared_ptr<CMeshContainerRenderMethod> pMeshContainerRenderMethod )
	{
		m_vecpContainerRenderMethod.push_back( pMeshContainerRenderMethod );
	}

//	void RenderMeshContainer( CMeshObjectContainer& mesh_container, int index );

	void RenderMeshContainerNode( CMeshContainerNode& node );//, std::vector< boost::shared_ptr<CShaderParamsLoader> >& vecpShaderParamsWriter );

	//
	// child nodes
	//

	int GetNumChildren() { return (int)m_vecpChild.size(); }

	const boost::shared_ptr<CMeshContainerNodeRenderMethod> GetChild( int index ) const { return m_vecpChild[index]; }

	/// Returns non-const pointer
	boost::shared_ptr<CMeshContainerNodeRenderMethod> Child( int index ) { return index < (int)m_vecpChild.size() ? m_vecpChild[index] : boost::shared_ptr<CMeshContainerNodeRenderMethod>(); }

	void AddChild( boost::shared_ptr<CMeshContainerNodeRenderMethod> pChild ) { m_vecpChild.push_back( pChild ); }

	/// Recursively load all meshes on the nodes of the tree
	bool LoadRenderMethodResources();

	void Serialize( IArchive& ar, const unsigned int version );

	void LoadFromXMLNode( CXMLNodeReader& reader );


	// 
	// mesh containers of this node
	// 

//	int GetNumMeshContainers() const { return (int)m_vecpMeshContainer.size(); }
//	const boost::shared_ptr<CMeshContainerRenderMethod> GetMeshContainer( int index ) const { return m_vecpMeshContainer[index]; }
//	boost::shared_ptr<CMeshContainerRenderMethod> MeshContainer( int index ) { return m_vecpMeshContainer[index]; }
};

//================================ inline implementations ================================

inline void CMeshContainerRenderMethod::RenderMesh( MeshHandle& mesh, const Matrix34& world_transform )
{
	boost::shared_ptr<BasicMesh> pMesh = mesh.GetMesh();

	if( !pMesh )
		return;

	RenderMesh( *pMesh, world_transform );
}

} // namespace amorphous



#endif /* __MeshContainerRenderMethod_HPP__ */
