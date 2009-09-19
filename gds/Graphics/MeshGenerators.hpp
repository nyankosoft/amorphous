#ifndef __MeshGenerator_H__
#define __MeshGenerator_H__


#include "../base.hpp"
#include "Graphics/MeshModel/PrimitiveShapeMeshes.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
using namespace MeshModel;


class MeshPolygonDirection
{
public:
	enum Type
	{
		INWARD,
		OUTWARD,
		NUM_TYPES
	};
};

class TexCoordStyle
{
public:
	enum Flags
	{
//		LINEAR_SHIFT_X     = (1 << 0),
//		LINEAR_SHIFT_INV_X = (1 << 1),
		LINEAR_SHIFT_Y     = (1 << 2),
		LINEAR_SHIFT_INV_Y = (1 << 3),
//		LINEAR_SHIFT_Z     = (1 << 4),
//		LINEAR_SHIFT_INV_Z = (1 << 5),
	};
};


class CMeshGenerator
{
protected:

	C3DMeshModelArchive m_MeshArchive;

	SFloatRGBAColor m_DiffuseColor;

	U32 m_RequestedVertexFormatFlags;

	std::string m_TexturePath;

	std::string m_ResourceIDString;

	U32 m_TexCoordStyleFlags;

protected:

	/// Generate() of subclasses need to call this after creating mesh
	/// and store it to m_MeshArchive
	void SetMiscMeshAttributes();

	/// Derived classes are responsible for calling this function if it wants to
	/// generate texture coords based on m_TexCoordStyleFlags.
	void GenerateTextureCoords( CGeneral3DMesh& mesh );

public:

	enum Params
	{
		DEFAULT_VERTEX_FLAGS
		= CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0
	};

	CMeshGenerator()
		:
	m_DiffuseColor( SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f) ),
	m_RequestedVertexFormatFlags( DEFAULT_VERTEX_FLAGS ),
	m_TexCoordStyleFlags(0)
	{}

	virtual ~CMeshGenerator() {}

	const C3DMeshModelArchive& GetMeshArchive() const { return m_MeshArchive; }

	C3DMeshModelArchive& MeshArchive() { return m_MeshArchive; }

	void SetVertexFormatFlags( U32 vert_flags ) { m_RequestedVertexFormatFlags = vert_flags; }

	void SetDiffuseColor( const SFloatRGBAColor &diffuse_color ) { m_DiffuseColor = diffuse_color; }

	void SetTexturePath( const std::string& texture_path ) { m_TexturePath = texture_path; }

	void SetTexCoordStyleFlags( U32 flags ) { m_TexCoordStyleFlags = flags; }

	virtual Result::Name Generate() = 0;
};


class CBoxMeshGenerator : public CMeshGenerator
{
	Vector3 m_vEdgeLengths;

	MeshPolygonDirection::Type m_PolygonDirection;

public:

	CBoxMeshGenerator();

	~CBoxMeshGenerator() {}

	void SetEdgeLengths( Vector3 vEdgeLengths ) { m_vEdgeLengths = vEdgeLengths; }

	void SetPolygonDirection( MeshPolygonDirection::Type polygon_direction ) { m_PolygonDirection = polygon_direction; }

	Result::Name Generate();

	/// \param vLengths full edge lengths
	Result::Name Generate( Vector3 vLengths,
		U32 vertex_flags = DEFAULT_VERTEX_FLAGS,
		const SFloatRGBAColor& diffuse_color = SFloatRGBAColor(1.0f, 1.0f, 1.0f, 1.0f),
		MeshPolygonDirection::Type polygon_direction = MeshPolygonDirection::OUTWARD );
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
