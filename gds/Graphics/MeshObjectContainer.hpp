#ifndef __MeshObjectContainer_H__
#define __MeshObjectContainer_H__


#include "TextureHandle.hpp"
#include "MeshObjectHandle.hpp"
#include "ShaderHandle.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"
#include "Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "3DMath/Matrix34.hpp"
#include "XML/fwd.hpp"
#include "Support/2DArray.hpp"
#include "Support/Serialization/Serialization_2DArray.hpp"
using namespace GameLib1::Serialization;


/**
 See comments in MeshObjectContainer for details
*/
class CMeshObjectContainer : public IArchiveObjectBase
{
public:

	//
	// mesh
	//

	CMeshResourceDesc m_MeshDesc;

	/// Used during runtime
	/// - Not serialized
	CMeshObjectHandle m_MeshObjectHandle;


	//
	// shader
	//

	/// table of shader techniques
	/// - row:    corresponds to mesh materials
	/// - column: corresponds to resolution (intended to be shader version of LOD)
	C2DArray<CShaderTechniqueHandle> m_ShaderTechnique;

	std::string m_ShaderFilepath;

	CShaderHandle m_ShaderHandle;

	Matrix34 m_MeshTransform;


	//
	// extra textures
	//

	std::vector<std::string> m_vecExtraTextureFilepath;

	/// holds textures used for any purposes other than
	/// usual color, alpha, normal map and specular map.
	/// For example,
	/// - look-up texture for specular highlight
	/// - look-up texture for membrane effect
	std::vector<CTextureHandle> m_vecExtraTexture;

public:

	CMeshObjectContainer()
		:
	m_MeshTransform( Matrix34Identity() )
	{}

	virtual ~CMeshObjectContainer() {}

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );
};


class CMeshContainerNode : public IArchiveObjectBase
{
	Matrix34 m_LocalPose; /// local pose of the node

	Matrix34 m_LocalTransform;

	std::vector< boost::shared_ptr<CMeshObjectContainer> > m_vecpMeshContainer;

	std::vector<Matrix34> m_vecMeshLocalPose;

	std::vector< boost::shared_ptr<CMeshContainerNode> > m_vecpChild;

	std::vector<CShaderTechniqueHandle> m_vecShaderTechniqueBuffer;

public:

	CMeshContainerNode();

	~CMeshContainerNode() {}

	void AddMeshContainer( boost::shared_ptr<CMeshObjectContainer> pMeshContainer, Matrix34& local_pose )
	{
		m_vecpMeshContainer.push_back( pMeshContainer );
		m_vecMeshLocalPose.push_back( local_pose );
	}


	// 
	// mesh containers of this node
	// 

	int GetNumMeshContainers() const { return (int)m_vecpMeshContainer.size(); }

	const boost::shared_ptr<CMeshObjectContainer> GetMeshContainer( int index ) const { return m_vecpMeshContainer[index]; }

	boost::shared_ptr<CMeshObjectContainer> MeshContainer( int index ) { return m_vecpMeshContainer[index]; }

	void Render( const Matrix34& parent_transform );

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



#endif /* __MeshObjectContainer_H__ */
