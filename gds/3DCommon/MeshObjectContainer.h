#ifndef __MeshObjectContainer_H__
#define __MeshObjectContainer_H__

#include "TextureHandle.h"
#include "MeshObjectHandle.h"
#include "Shader/ShaderTechniqueHandle.h"
#include "Shader/Serialization_ShaderTechniqueHandle.h"
#include "Support/2DArray.h"

#include <string>


#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


class CMeshObjectContainer : public IArchiveObjectBase
{
public:

	CMeshObjectHandle m_MeshObjectHandle;

	/// table of shader techniques
	/// - row:    corresponds to mesh materials
	/// - column: corresponds to resolution (intended to be shader version of LOD)
	C2DArray<CShaderTechniqueHandle> m_ShaderTechnique;

	/// holds textures used for any purposes other than
	/// usual color, alpha, normal map and specular map.
	/// For example,
	/// - look-up texture for specular highlight
	/// - look-up texture for membrane effect
	vector<CTextureHandle> m_vecExtraTexture;

public:

	CMeshObjectContainer() {}
	virtual ~CMeshObjectContainer() {}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_MeshObjectHandle;
		ar & m_ShaderTechnique;
		ar & m_vecExtraTexture;
	}
};



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
