#ifndef __RectMeshGenerator_HPP__
#define __RectMeshGenerator_HPP__


#include "MeshGenerator.hpp"


namespace amorphous
{


class RectMeshGenerator : public MeshGenerator
{
	Matrix34 m_Pose;
	float m_fWidth;
	float m_fHeight;

	void SetPose( unsigned int axis, int sign )
	{
		const float pi      = (float)PI;
		const float half_pi = (float)PI * 0.5f;
		float angle = 0;
		switch( axis )
		{
		case 0: m_Pose.matOrient = Matrix33RotationY(-half_pi * (float)sign) * Matrix33RotationX(-half_pi);
			break;

		case 1:
			if( sign == -1 )
				m_Pose.matOrient = Matrix33RotationZ(pi);
			break;

		case 2:
			m_Pose.matOrient = Matrix33RotationX(-half_pi);
			if( sign == 1 )
				m_Pose.matOrient = Matrix33RotationX(pi) * m_Pose.matOrient;
			break;

		default:
			break;
		}
	}

public:

	/// \param axis [in] [0,2]
	/// \param sign [in] 1 or -1. The positive or negative direction the rectangle is facing along.
	RectMeshGenerator( float width, float height, unsigned int axis = 1, int sign = 1 )
		:
	m_fWidth(width),
	m_fHeight(height)
	{
		SetPose( axis, sign );
	}

	~RectMeshGenerator(){}

	Result::Name Generate()
	{
		using std::vector;
		using boost::shared_ptr;

		vector<Vector3> positions, normals;
		vector<SFloatRGBAColor> diffuse_colors;
		vector<TEXCOORD2> tex_uvs;
		vector< vector<unsigned int> > polygons;

		positions.resize( 4 );

		// Create the rectangle facing along the positive direction of the y-axis, i.e. facing up.
		float hw = m_fWidth  * 0.5f;
		float hh = m_fHeight * 0.5f;
		positions[0] = m_Pose * Vector3( -hw, 0,  hh );
		positions[1] = m_Pose * Vector3(  hw, 0,  hh );
		positions[2] = m_Pose * Vector3(  hw, 0, -hh );
		positions[3] = m_Pose * Vector3( -hw, 0, -hh );

		Vector3 normal = m_Pose.matOrient * Vector3(0,1,0);

		normals.resize( 4, normal );

		diffuse_colors.resize( 4, m_DiffuseColor );

		tex_uvs.resize( 4 );
		tex_uvs[0] = TEXCOORD2( 0, 0 );
		tex_uvs[1] = TEXCOORD2( 1, 0 );
		tex_uvs[2] = TEXCOORD2( 1, 1 );
		tex_uvs[3] = TEXCOORD2( 0, 1 );

		// Create a single 4-vertex polygon
		polygons.resize( 1 );
		polygons[0].resize( 4 );
		for( int i=0; i<4; i++ )
			polygons[0][i] = i;

//		SetVertices( positions, normals, tex_uvs );

		CreateSingleSubsetMeshArchive(
			positions,
			normals,
			diffuse_colors,
			tex_uvs,
			polygons,
			m_MeshArchive
			);
	}
};


} // namespace amorphous



#endif /* __RectMeshGenerator_HPP__ */
