#ifndef __MeshGenerator_HPP__
#define __MeshGenerator_HPP__


#include "../MeshModel/PrimitiveShapeMeshes.hpp"
#include "../MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{


class MeshGenerator
{
protected:

	C3DMeshModelArchive m_MeshArchive;

	SFloatRGBAColor m_DiffuseColor;

	U32 m_RequestedVertexFormatFlags;

	std::string m_TexturePath;

	std::string m_ResourceIDString;

	U32 m_TexCoordStyleFlags;

	MeshPolygonDirection::Type m_PolygonDirection;

protected:

	/// Generate() of subclasses need to call this after creating mesh
	/// and store it to m_MeshArchive
	void SetMiscMeshAttributes();

	/// Derived classes are responsible for calling this function if it wants to
	/// generate texture coords based on m_TexCoordStyleFlags.
	void GenerateTextureCoords( CGeneral3DMesh& mesh );

	Result::Name CreateArchiveFromGeneral3DMesh( boost::shared_ptr<CGeneral3DMesh>& pSrcMesh );

public:

	enum Params
	{
		DEFAULT_VERTEX_FLAGS
		= CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0
	};

	MeshGenerator()
		:
	m_DiffuseColor( SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f) ),
	m_RequestedVertexFormatFlags( DEFAULT_VERTEX_FLAGS ),
	m_TexCoordStyleFlags(0),
	m_PolygonDirection(MeshPolygonDirection::OUTWARD)
	{}

	virtual ~MeshGenerator() {}

	const C3DMeshModelArchive& GetMeshArchive() const { return m_MeshArchive; }

	C3DMeshModelArchive& MeshArchive() { return m_MeshArchive; }

	void SetVertexFormatFlags( U32 vert_flags ) { m_RequestedVertexFormatFlags = vert_flags; }

	void SetDiffuseColor( const SFloatRGBAColor &diffuse_color ) { m_DiffuseColor = diffuse_color; }

	void SetTexturePath( const std::string& texture_path ) { m_TexturePath = texture_path; }

	void SetTexCoordStyleFlags( U32 flags ) { m_TexCoordStyleFlags = flags; }

	void SetPolygonDirection( MeshPolygonDirection::Type polygon_direction ) { m_PolygonDirection = polygon_direction; }

	virtual Result::Name Generate() = 0;
};


} // namespace amorphous



#endif /* __MeshGenerator_HPP__ */
