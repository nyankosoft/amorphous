#ifndef __MeshContainerRenderMethod_HPP__
#define __MeshContainerRenderMethod_HPP__


#include "TextureHandle.hpp"
#include "MeshObjectHandle.hpp"
#include "ShaderHandle.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"
#include "Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/XML/fwd.hpp"


namespace amorphous
{


class ShaderParamsLoader
{
public:

	virtual ~ShaderParamsLoader() {}

	virtual void UpdateShaderParams( ShaderManager& rShaderMgr ) = 0;

	virtual void ResetShaderParams( ShaderManager& rShaderMgr ) {};

	virtual bool LoadResource() { return true; }
};


/// Stores params and settings to render all the subsets of a mesh or a subset of a mesh
class SubsetRenderMethod : public IArchiveObjectBase
{
public:

	ShaderResourceDesc m_ShaderDesc;

	ShaderHandle m_Shader;
	ShaderTechniqueHandle m_Technique;

	/// Need to be set at runtime
	std::vector< boost::shared_ptr<ShaderParamsLoader> > m_vecpShaderParamsLoader;

public:

	bool Load();

	void LoadFromXMLNode( XMLNode& reader );

	void Serialize( IArchive& ar, const unsigned int version );
};


/**
 See comments in MeshObjectContainerRenderMethod for details
*/
class MeshContainerRenderMethod : public IArchiveObjectBase
{
	/// indices of mesh subsets (previously called materials) to render
	std::vector<int> m_vecIndicesOfSubsetsToRender;

//	std::vector<SubsetRenderMethod> m_vecSubsetRenderMethod;

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
//	std::vector<SubsetRenderMethod> m_vecMeshRenderMethod;

	std::vector< std::pair< SubsetRenderMethod, std::vector<int> > > m_RenderMethodsAndSubsetIndices;

	/// Different shader & technique for each mesh subset
	/// - Uses array to support LOD of shader
	/// map of string and subset render method
	/// - string(key) - subset name saved in MeshMaterial::Name
	/// - SubsetRenderMethod - method to render a subset of a mesh
	std::vector< std::map<std::string,SubsetRenderMethod> > m_vecSubsetNameToRenderMethod;

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
		SubsetRenderMethod& render_method,
		const Matrix34& world_transform );

public:

	MeshContainerRenderMethod()
		:
	m_LODIndex( 0 )
	{}

	virtual ~MeshContainerRenderMethod() {}

	void RenderMesh( BasicMesh &mesh, const Matrix34& world_transform );

	inline void RenderMesh( MeshHandle& mesh, const Matrix34& world_transform );

	void RenderMeshContainer( MeshObjectContainer& mesh_container,
		                      const Matrix34& world_transform );
							  //std::vector< boost::shared_ptr<ShaderParamsLoader> >& vecpShaderParamsWriter );

//	std::vector<SubsetRenderMethod>& MeshRenderMethod() { return m_vecMeshRenderMethod; }

	SubsetRenderMethod& PrimaryMeshRenderMethod()
	{
		if( m_RenderMethodsAndSubsetIndices.empty() )
			m_RenderMethodsAndSubsetIndices.resize( 1 );

		return m_RenderMethodsAndSubsetIndices[0].first;
	}

	std::vector< std::pair< SubsetRenderMethod, std::vector<int> > >& RenderMethodsAndSubsetIndices() { return m_RenderMethodsAndSubsetIndices; }

	std::vector< std::map<std::string,SubsetRenderMethod> >& SubsetRenderMethodMaps() { return m_vecSubsetNameToRenderMethod; }

	void SetShaderParamsLoaderToAllMeshRenderMethods( boost::shared_ptr<ShaderParamsLoader> pShaderParamsLoader );

	void RemoveShaderParamsLoaderFromAllMeshRenderMethods( boost::shared_ptr<ShaderParamsLoader> pShaderParamsLoader );

	/// Creates a copy or copies of render methods at each LoD for subsets whose names are specified by the argument.
	void BreakMeshRenderMethodsToSubsetRenderMethods( const std::vector<std::string>& vecName );

	bool LoadRenderMethodResources();

	boost::shared_ptr<MeshContainerRenderMethod> CreateCopy();

	virtual void LoadFromXMLNode( XMLNode& reader );

	virtual void Serialize( IArchive& ar, const unsigned int version );
};


class MeshContainerNodeRenderMethod : public IArchiveObjectBase
{
	std::vector< boost::shared_ptr<MeshContainerRenderMethod> > m_vecpContainerRenderMethod;

	std::vector< boost::shared_ptr<MeshContainerNodeRenderMethod> > m_vecpChild;

public:

	MeshContainerNodeRenderMethod();

	~MeshContainerNodeRenderMethod() {}

	void AddMeshContainer( boost::shared_ptr<MeshContainerRenderMethod> pMeshContainerRenderMethod )
	{
		m_vecpContainerRenderMethod.push_back( pMeshContainerRenderMethod );
	}

//	void RenderMeshContainer( MeshObjectContainer& mesh_container, int index );

	void RenderMeshContainerNode( MeshContainerNode& node );//, std::vector< boost::shared_ptr<ShaderParamsLoader> >& vecpShaderParamsWriter );

	//
	// child nodes
	//

	int GetNumChildren() { return (int)m_vecpChild.size(); }

	const boost::shared_ptr<MeshContainerNodeRenderMethod> GetChild( int index ) const { return m_vecpChild[index]; }

	/// Returns non-const pointer
	boost::shared_ptr<MeshContainerNodeRenderMethod> Child( int index ) { return index < (int)m_vecpChild.size() ? m_vecpChild[index] : boost::shared_ptr<MeshContainerNodeRenderMethod>(); }

	void AddChild( boost::shared_ptr<MeshContainerNodeRenderMethod> pChild ) { m_vecpChild.push_back( pChild ); }

	/// Recursively load all meshes on the nodes of the tree
	bool LoadRenderMethodResources();

	void Serialize( IArchive& ar, const unsigned int version );

	void LoadFromXMLNode( XMLNode& reader );


	// 
	// mesh containers of this node
	// 

//	int GetNumMeshContainers() const { return (int)m_vecpMeshContainer.size(); }
//	const boost::shared_ptr<MeshContainerRenderMethod> GetMeshContainer( int index ) const { return m_vecpMeshContainer[index]; }
//	boost::shared_ptr<MeshContainerRenderMethod> MeshContainer( int index ) { return m_vecpMeshContainer[index]; }
};

//================================ inline implementations ================================

inline void MeshContainerRenderMethod::RenderMesh( MeshHandle& mesh, const Matrix34& world_transform )
{
	boost::shared_ptr<BasicMesh> pMesh = mesh.GetMesh();

	if( !pMesh )
		return;

	RenderMesh( *pMesh, world_transform );
}

} // namespace amorphous



#endif /* __MeshContainerRenderMethod_HPP__ */
