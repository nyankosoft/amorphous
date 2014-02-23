#ifndef __HelixMeshGenerator_HPP__
#define __HelixMeshGenerator_HPP__


#include "MeshGenerator.hpp"
#include "amorphous/3DMath/HelixPolygonModelMaker.hpp"


namespace amorphous
{


class HelixMeshDesc
{
public:
	float helix_length;
	float helix_radius;
	unsigned int num_coils;
	float cord_radius;
	unsigned int num_helix_sides;
	unsigned int num_cord_sides;
	bool create_top_face;
	PolygonModelStyle::Name top_face_style;
	bool create_bottom_face;
	PolygonModelStyle::Name bottom_face_style;

	HelixMeshDesc()
		:
	helix_length(1.0f),
	helix_radius(0.25f),
	num_coils(10),
	cord_radius(0.03f),
	num_helix_sides(16),
	num_cord_sides(16),
	create_top_face(true),
	top_face_style(PolygonModelStyle::EDGE_VERTICES_UNWELDED),
	create_bottom_face(true),
	bottom_face_style(PolygonModelStyle::EDGE_VERTICES_UNWELDED)
	{}
};


class HelixMeshGenerator : public MeshGenerator
{
public:

	HelixMeshDesc m_Desc;

public:

	HelixMeshGenerator() {}

	HelixMeshGenerator( const HelixMeshDesc& desc ) : m_Desc(desc) {}

	HelixMeshGenerator(
		float helix_length,
		float helix_radius,
		unsigned int num_coils,
		float cord_radius,
		unsigned int num_helix_sides,
		unsigned int num_cord_sides,
		bool create_top_face,
		PolygonModelStyle::Name top_face_style,
		bool create_bottom_face,
		PolygonModelStyle::Name bottom_face_style
		)
	{
		m_Desc.helix_length       = helix_length;
		m_Desc.helix_radius       = helix_radius;
		m_Desc.num_coils          = num_coils;
		m_Desc.cord_radius        = cord_radius;
		m_Desc.num_helix_sides    = num_helix_sides;
		m_Desc.num_cord_sides     = num_cord_sides;
		m_Desc.create_top_face    = create_top_face;
		m_Desc.top_face_style     = top_face_style;
		m_Desc.create_bottom_face = create_bottom_face;
		m_Desc.bottom_face_style  = bottom_face_style;
	}

	~HelixMeshGenerator() {}

	Result::Name Generate()
	{
		using namespace std;

		vector<Vector3> positions, normals;
		vector<SFloatRGBAColor> diffuse_colors;
		vector<TEXCOORD2> tex_uvs;
		vector< std::vector<unsigned int> > polygons;

		CreateUniformCylindricalHelix(
			m_Desc.helix_length,
			m_Desc.helix_radius,
			m_Desc.num_coils,
			m_Desc.cord_radius,
			m_Desc.num_helix_sides,
			m_Desc.num_cord_sides,
			m_Desc.create_top_face,
			m_Desc.top_face_style,
			m_Desc.create_bottom_face,
			m_Desc.bottom_face_style,
			positions,
			normals,
			polygons
			);

		const size_t num_vertices = positions.size();
		diffuse_colors.resize( num_vertices, SFloatRGBAColor::White() );
		tex_uvs.resize( num_vertices, TEXCOORD2(0,0) );

		Result::Name res = CreateSingleSubsetMeshArchive(
			positions,
			normals,
			diffuse_colors,
			tex_uvs,
			polygons,
			m_MeshArchive
			);

		return res;
	}

	Result::Name Generate(
		float helix_length,
		float helix_radius,
		unsigned int num_coils,
		float cord_radius,
		U32 vertex_flags = DEFAULT_VERTEX_FLAGS,
		const SFloatRGBAColor& diffuse_color = SFloatRGBAColor(1.0f, 1.0f, 1.0f, 1.0f),
		MeshPolygonDirection::Type polygon_direction = MeshPolygonDirection::OUTWARD );
};


} // amorphous



#endif /* __HelixMeshGenerator_HPP__ */
