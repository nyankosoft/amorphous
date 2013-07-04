#ifndef __GridMeshGenerator_HPP__
#define __GridMeshGenerator_HPP__


#include "MeshGenerator.hpp"
#include "amorphous/3DMath/GridPolygonModelMaker.hpp"


namespace amorphous
{


class GridMeshSideDesc
{
public:
	TEXCOORD2 tex_coord_min;
	TEXCOORD2 tex_coord_max;
	SFloatRGBAColor diffuse_color;

	GridMeshSideDesc()
		:
	tex_coord_min( TEXCOORD2(0,0) ),
	tex_coord_max( TEXCOORD2(1,1) ),
	diffuse_color( SFloatRGBAColor::White() )
	{}
};


class GridMeshGenerator : public MeshGenerator
{
//	Matrix34 m_Pose;
	float m_fWidth;
	float m_fHeight;
	unsigned int m_NumDivisions0;
	unsigned int m_NumDivisions1;
	unsigned int m_Axis;
	int m_Sign;
	bool m_DoubleSided;

	void SetPose( unsigned int axis, int sign );

public:

	/// \param axis [in] [0,2]
	/// \param sign [in] 1 or -1. The positive or negative direction the rectangle is facing along.
	GridMeshGenerator(
		float width,
		float height,
		unsigned int num_divisions_0,
		unsigned int num_divisions_1,
		unsigned int axis = 1,
		int sign = 1,
		bool double_sided = false
		)
		:
	m_fWidth(width),
	m_fHeight(height),
	m_NumDivisions0(num_divisions_0),
	m_NumDivisions1(num_divisions_1),
	m_Axis(axis),
	m_Sign(sign),
	m_DoubleSided(double_sided)
	{
		SetPose( axis, sign );
	}

	~GridMeshGenerator(){}

	Result::Name Generate()
	{
		using std::vector;

		vector<Vector3> positions, normals;
		vector<SFloatRGBAColor> diffuse_colors;
		vector<TEXCOORD2> tex_uvs;
		vector< std::vector<unsigned int> > polygons;

		Result::Name res = GenerateGridMesh( positions, normals, diffuse_colors, tex_uvs, polygons, 0 );
		if( res != Result::SUCCESS )
			return res;

		if( m_DoubleSided )
		{
			return GenerateGridMesh( positions, normals, diffuse_colors, tex_uvs, polygons, 1 );
		}

		CreateSingleSubsetMeshArchive(
			positions,
			normals,
			diffuse_colors,
			tex_uvs,
			polygons,
			m_MeshArchive
			);
		
		return Result::SUCCESS;
	}

	/// \param[in] side 0 (front) or 1 (back)
	Result::Name GenerateGridMesh(
		std::vector<Vector3>& positions,
		std::vector<Vector3>& normals,
		std::vector<SFloatRGBAColor>& diffuse_colors,
		std::vector<TEXCOORD2>& tex_uvs,
		std::vector< std::vector<unsigned int> > polygons,
		unsigned int side
		)
	{
		using std::vector;

		const unsigned int num_divisions_x = m_NumDivisions0;
		const unsigned int num_divisions_y = m_NumDivisions1;

		const unsigned int vertex_offset = positions.size();
		const unsigned int num_vertices = (num_divisions_x+1) * (num_divisions_y+1);
//		int d0 = (axis+1)%3;
//		int d1 = (axis+2)%3;

		CreateGridPoints( m_fWidth, m_fHeight, num_divisions_x, num_divisions_y, m_Axis, m_Sign, positions );

		// The rectangle faces along the positive direction of the y-axis, i.e. facing up.

		Matrix34 pose = CalcPoseForFlatMesh( m_Axis, m_Sign );

		Vector3 normal = pose.matOrient * Vector3(0,1,0);
		if( side == 1 )
			normal *= -1;

		normals.insert( normals.end(), num_vertices, normal );

		diffuse_colors.insert( diffuse_colors.end(), num_vertices, m_DiffuseColor );

		tex_uvs.insert( tex_uvs.end(), num_vertices, TEXCOORD2(0,0) );

		// Create a single 4-vertex polygon
		polygons.reserve( polygons.size() + num_divisions_x * num_divisions_y );
		int i0=0, i1=1, i2=2, i3=3;
		if( side == 1 ) { i0=3; i1=2; i2=1; i3=0; } // Invert the order of vertex indices to flip polygons.
		for( uint y=0; y<num_divisions_y; y++ )
		{
			for( uint x=0; x<num_divisions_x; x++ )
			{
				polygons.push_back( vector<unsigned int>() );
				polygons.back().resize( 4 );
				polygons.back()[i0] = vertex_offset + num_divisions_x * y + x;
				polygons.back()[i1] = vertex_offset + num_divisions_x * y + x + 1;
				polygons.back()[i2] = vertex_offset + num_divisions_x * (y+1) + x + 1;
				polygons.back()[i3] = vertex_offset + num_divisions_x * (y+1) + x;
			}
		}
	}
};


} // namespace amorphous



#endif /* __GridMeshGenerator_HPP__ */
