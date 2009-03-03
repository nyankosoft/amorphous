#ifndef __MeshObjectContainer_H__
#define __MeshObjectContainer_H__


#include "TextureHandle.hpp"
#include "MeshObjectHandle.hpp"
#include "ShaderHandle.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"
#include "Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "XML.hpp"
#include "Support/2DArray.hpp"
#include "Support/Serialization/Serialization_2DArray.hpp"
using namespace GameLib1::Serialization;


class CMeshObjectContainer : public IArchiveObjectBase
{
public:

	CMeshResourceDesc m_MeshDesc;

	/// Used during runtime
	/// - Not serialized
	CMeshObjectHandle m_MeshObjectHandle;

	/// table of shader techniques
	/// - row:    corresponds to mesh materials
	/// - column: corresponds to resolution (intended to be shader version of LOD)
	C2DArray<CShaderTechniqueHandle> m_ShaderTechnique;

	std::string m_ShaderFilepath;

	CShaderHandle m_ShaderHandle;

	std::vector<std::string> m_vecExtraTextureFilepath;

	/// holds textures used for any purposes other than
	/// usual color, alpha, normal map and specular map.
	/// For example,
	/// - look-up texture for specular highlight
	/// - look-up texture for membrane effect
	std::vector<CTextureHandle> m_vecExtraTexture;

public:

	CMeshObjectContainer() {}

	virtual ~CMeshObjectContainer() {}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_MeshDesc;
		ar & m_ShaderTechnique;
		ar & m_vecExtraTextureFilepath;
		ar & m_ShaderFilepath;
	}

	virtual void LoadFromXMLNode( CXMLNodeReader& reader )
	{
		m_MeshDesc.LoadFromXMLNode( reader.GetChild( "MeshDesc" ) );

		reader.GetChildElementTextContent( "ShaderFilepath", m_ShaderFilepath );
		
		string shader_technique;
		if( reader.GetChildElementTextContent( "SingleShaderTechnique", shader_technique ) )
		{
			m_ShaderTechnique.resize( 1, 1 );
			m_ShaderTechnique(0,0).SetTechniqueName( shader_technique.c_str() );
		}
	}
};


/*
--------------------------------------------------
 XML format for shader technique
--------------------------------------------------

no LOD, single technique

<SingleShaderTechnique>HSLighting</SingleShaderTechnique>


no LOD, multiple materials (Not implemented yet)

<ShaderTechniques>
	<Material index="0">EnvMap</Material>
	<Material index="1">HSLighting</Material>
	<Material index="2">HSLighting</Material>
</ShaderTechniques>


LOD, multiple materials (Not implemented yet)

<ShaderTechniques>
	<Material index="0">
		<High>EnvMap</High>
		<Mid>EnvMap</Mid>
		<Low>Default</Low>
	</Material>
	<Material index="1">
		<High>HSLighting</High>
		<Mid>HSLighting</Mid>
		<Low>Default</Low>
	</Material>
	<Material index="2">
		<High>HSLighting</High>
		<Mid>HSLighting</Mid>
		<Low>Default</Low>
	</Material>
</ShaderTechniques>

*/




//  old text desc file (deprecated. Use the XML format instead)
/*

How to load from text file?

text file
MeshContainer.MeshObjectFilename	plane.msh

begin: MeshContainer
	filename	models/plane.msh
	extra_tex	0 textures/specular.msh
	shader_tech 0 VertexBlend_1HSDL_Specular_CTS  SingleHSDL_Specular_CTS     VertLights
	shader_tech 1 SpecularMembrane_1HSDL_CTS      SpecularMembrane_1HSDL_CTS  VertLights
end: MeshContainer
*/


#endif /* __MeshObjectContainer_H__ */
