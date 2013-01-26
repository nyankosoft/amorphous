#ifndef __MeshObjectContainer_H__
#define __MeshObjectContainer_H__


#include "TextureHandle.hpp"
#include "MeshObjectHandle.hpp"
#include "ShaderHandle.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"
#include "Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/XML/fwd.hpp"
#include "gds/Support/2DArray.hpp"
#include "gds/Support/Serialization/Serialization_2DArray.hpp"


namespace amorphous
{
using namespace serialization;


/**
 See comments in MeshObjectContainer for details
*/
class CMeshObjectContainer : public IArchiveObjectBase
{
public:

	//
	// mesh
	//

	MeshResourceDesc m_MeshDesc;

	/// Used during runtime
	/// - Not serialized
	MeshHandle m_MeshObjectHandle;


	//
	// default shader
	//

	/// table of shader techniques
	/// - row:    corresponds to mesh materials
	/// - column: corresponds to resolution (intended to be shader version of LOD)
	C2DArray<CShaderTechniqueHandle> m_ShaderTechnique;

	ShaderResourceDesc m_ShaderDesc;

	ShaderHandle m_ShaderHandle;

	Matrix34 m_MeshTransform;

	std::vector<Matrix34> m_vecSubsetTransform;


	//
	// extra textures
	//

	std::vector<std::string> m_vecExtraTextureFilepath;

	/// holds textures used for any purposes other than
	/// usual color, alpha, normal map and specular map.
	/// For example,
	/// - look-up texture for specular highlight
	/// - look-up texture for membrane effect
	std::vector<TextureHandle> m_vecExtraTexture;

public:

	CMeshObjectContainer()
		:
	m_MeshTransform( Matrix34Identity() )
	{}

	virtual ~CMeshObjectContainer() {}

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual unsigned int GetVersion() const { return 1; }

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );
};


class CMeshContainerNode : public IArchiveObjectBase
{
	Matrix34 m_LocalPose; ///< local pose of the node

	Matrix34 m_LocalTransform;

	Matrix34 m_WorldTransform; ///< current world transform

	/// Mesh Containers
	/// - Don't do m_vecpMeshContainer.push_back(). See below.
	std::vector< boost::shared_ptr<CMeshObjectContainer> > m_vecpMeshContainer;

	std::vector<Matrix34> m_vecMeshLocalPose;

	std::vector< boost::shared_ptr<CMeshContainerNode> > m_vecpChild;

	std::vector<CShaderTechniqueHandle> m_vecShaderTechniqueBuffer;

private:

//	void Render_r( const Matrix34& parent_transform, CMeshContainerNodeRenderMethod& render_method, bool use_fallback_shaders );

public:

	CMeshContainerNode();

	~CMeshContainerNode() {}

	Matrix34 GetMeshContainerWorldTransform( int mesh_container_index );

	void AddMeshContainer( boost::shared_ptr<CMeshObjectContainer> pMeshContainer, const Matrix34& local_pose = Matrix34Identity() )
	{
		m_vecpMeshContainer.push_back( pMeshContainer );
		m_vecMeshLocalPose.push_back( local_pose );
	}


	// 
	// mesh containers of this node
	// 

	int GetNumMeshContainers() const { return (int)m_vecpMeshContainer.size(); }

	const boost::shared_ptr<CMeshObjectContainer> GetMeshContainer( int index ) const { return m_vecpMeshContainer[index]; }

	inline void SetMeshContainer( int index, boost::shared_ptr<CMeshObjectContainer>& pContainer, const Matrix34& local_pose = Matrix34Identity() );

	boost::shared_ptr<CMeshObjectContainer> MeshContainer( int index ) { return m_vecpMeshContainer[index]; }

	void ClearMeshContainers() { m_vecpMeshContainer.resize( 0 ); }

	void Render( /*const Matrix34& parent_transform*/ );

	void UpdateWorldTransforms( const Matrix34& parent_transform  );

	//
	// child nodes
	//

	int GetNumChildren() { return (int)m_vecpChild.size(); }

	const boost::shared_ptr<CMeshContainerNode> GetChild( int index ) const { return m_vecpChild[index]; }

	/// Returns non-const pointer
	boost::shared_ptr<CMeshContainerNode> Child( int index ) { return m_vecpChild[index]; }

	void AddChild( boost::shared_ptr<CMeshContainerNode> pChild ) { m_vecpChild.push_back( pChild ); }

	/// Recursively load all meshes on the nodes of the tree
	bool LoadMeshesFromDesc();

	bool LoadShadersFromDesc();

	void Serialize( IArchive& ar, const unsigned int version );

	void LoadFromXMLNode( CXMLNodeReader& reader );
};

//=============================== inline implementations ===============================

inline void CMeshContainerNode::SetMeshContainer( int index,
												  boost::shared_ptr<CMeshObjectContainer>& pContainer,
												  const Matrix34& local_pose )
{
	if( index < 0 )
		return;

	while( (int)m_vecpMeshContainer.size() <= index )
	{
		boost::shared_ptr<CMeshObjectContainer> pNewContainer( new CMeshObjectContainer );
		AddMeshContainer( pNewContainer );
	}

	m_vecpMeshContainer[index] = pContainer;
	m_vecMeshLocalPose[index] = local_pose;
}

} // namespace amorphous



#endif /* __MeshObjectContainer_H__ */
