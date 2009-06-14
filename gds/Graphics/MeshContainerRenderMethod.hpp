#ifndef __MeshContainerRenderMethod_HPP__
#define __MeshContainerRenderMethod_HPP__


#include "TextureHandle.hpp"
#include "MeshObjectHandle.hpp"
#include "ShaderHandle.hpp"
#include "MeshObjectContainer.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"
#include "Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "3DMath/Matrix34.hpp"
#include "XML/fwd.hpp"
#include "Support/2DArray.hpp"
#include "Support/Serialization/Serialization_2DArray.hpp"
using namespace GameLib1::Serialization;


class CShaderParamsLoader
{
public:

	virtual ~CShaderParamsLoader() {}

	virtual void UpdateShaderParams( CShaderManager& rShaderMgr ) = 0;

	virtual void ResetShaderParams( CShaderManager& rShaderMgr ) {};
};


/// Stores params and settings to render all the subsets of a mesh or a subset of a mesh
class CSubsetRenderMethod : public IArchiveObjectBase
{
public:

	std::string m_ShaderFilepath;

	CShaderHandle m_Shader;
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
	std::vector<int>& IndicesOfSubsetsToRender() { return m_vecIndicesOfSubsetsToRender; }

	//
	// shader
	//

	/// Used when m_vecSubsetNameToRenderMethod is empty
	/// - Applies the same shader and same techniques to all the subsets of the mesh
	std::vector<CSubsetRenderMethod> m_vecMeshRenderMethod;

	/// Different shader & technique for each mesh subset
	std::vector< std::map< std::string, CSubsetRenderMethod > > m_vecSubsetNameToRenderMethod;

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
	std::vector<CTextureHandle> m_vecExtraTexture;
*/
public:

	CMeshContainerRenderMethod()
		:
	m_LODIndex( 0 )
	{}

	virtual ~CMeshContainerRenderMethod() {}

	void RenderMesh( CMeshObjectHandle& mesh, const Matrix34& world_transform );


	void RenderMeshContainer( CMeshObjectContainer& mesh_container,
		                      const Matrix34& world_transform );
							  //std::vector< boost::shared_ptr<CShaderParamsLoader> >& vecpShaderParamsWriter );

	std::vector<CSubsetRenderMethod>& MeshRenderMethod() { return m_vecMeshRenderMethod; }

	void SetShaderParamsLoaderToAllMeshRenderMethods( boost::shared_ptr<CShaderParamsLoader> pShaderParamsLoader );

	void RemoveShaderParamsLoaderToAllMeshRenderMethods( boost::shared_ptr<CShaderParamsLoader> pShaderParamsLoader );

	bool LoadRenderMethodResources();

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



#endif /* __MeshContainerRenderMethod_HPP__ */
