#ifndef __MeshGenerator_H__
#define __MeshGenerator_H__


#include "Graphics/MeshModel/PrimitiveShapeMeshes.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
using namespace MeshModel;


class CMeshGenerator
{
protected:

	C3DMeshModelArchive m_MeshArchive;

	SFloatRGBAColor m_DiffuseColor;

	U32 m_RequestedVertexFormatFlags;

	std::string m_TexturePath;

protected:

	/// Generate() of subclasses need to call this after creating mesh
	/// and store it to m_MeshArchive
	void SetMiscMeshAttributes();

public:

	CMeshGenerator()
		:
	m_DiffuseColor( SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f) )
	{}

	virtual ~CMeshGenerator() {}

	const C3DMeshModelArchive& GetMeshArchive() const { return m_MeshArchive; }

	C3DMeshModelArchive& MeshArchive() { return m_MeshArchive; }

	void SetVertexFormatFlags( U32 vert_flags ) { m_RequestedVertexFormatFlags = vert_flags; }

	void SetDiffuseColor( const SFloatRGBAColor &diffuse_color ) { m_DiffuseColor = diffuse_color; }

	void SetTexturePath( const std::string& texture_path ) { m_TexturePath = texture_path; }

	virtual Result::Name Generate() = 0;
};


class CBoxMeshGenerator : public CMeshGenerator
{
	Vector3 m_vEdgeLengths;

public:

	enum Params
	{
		DEFAULT_VERTEX_FLAGS
		= CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0
	};

	CBoxMeshGenerator() {}
	~CBoxMeshGenerator() {}

	void SetEdgeLengths( Vector3 vEdgeLengths ) { m_vEdgeLengths = vEdgeLengths; }

	Result::Name Generate();

	/// \param vLengths full edge lengths
	Result::Name Generate( Vector3 vLengths,
		U32 vertex_flags = DEFAULT_VERTEX_FLAGS,
		const SFloatRGBAColor& diffuse_color = SFloatRGBAColor(1.0f, 1.0f, 1.0f, 1.0f) );
};


class CConeMeshGenerator : public CMeshGenerator
{
	CConeDesc m_Desc;

public:

	CConeMeshGenerator( CConeDesc& desc )
		:
	m_Desc(desc)
	{}

	Result::Name Generate();
};


#endif  /* __MeshGenerator_H__ */
