#ifndef __MeshUtilities_HPP__
#define __MeshUtilities_HPP__


#include "MeshGenerators/MeshGenerators.hpp"
#include "MeshObjectHandle.hpp"
#include "Mesh/CustomMesh.hpp"


namespace amorphous
{


/// \param whd [in] width(x), height(y), and depth of the box
inline MeshHandle CreateBoxMesh( const Vector3& whd,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity(),
								const std::string& texture_pathname = "" )
{
	boost::shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new BoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( whd );
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;
	mesh_desc.pMeshGenerator->SetDiffuseColor( diffuse_color );
	mesh_desc.pMeshGenerator->SetTexturePath( texture_pathname );

	MeshHandle mesh;
	bool loaded = mesh.Load( mesh_desc );

	if( loaded )
		return mesh;
	else
		return MeshHandle();
}


/// \param num_horizontal_segments # of divisions in the direction of longitude
/// \param num_vertical_segments # of divisions in the direction of latitude
inline MeshHandle CreateSphereMesh(
								float radius,
								const unsigned int num_horizontal_segments = 24,
								const unsigned int num_vertical_segments = 12,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity()
								)
{
	SphereDesc sphere_desc;
	sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = radius;
	sphere_desc.num_sides    = (int)num_horizontal_segments;
	sphere_desc.num_segments = (int)num_vertical_segments;
	sphere_desc.axis = 1;
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new SphereMeshGenerator(sphere_desc) );
	mesh_desc.pMeshGenerator->SetDiffuseColor( diffuse_color );

	MeshHandle mesh;
	bool loaded = mesh.Load( mesh_desc );

	if( loaded )
		return mesh;
	else
		return MeshHandle();
}


inline MeshHandle CreateConeMesh( const ConeDesc& desc,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity() )
{
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new ConeMeshGenerator(desc) );
	mesh_desc.pMeshGenerator->SetDiffuseColor( diffuse_color );

	MeshHandle mesh;
	bool loaded = mesh.Load( mesh_desc );

	if( loaded )
		return mesh;
	else
		return MeshHandle();
}


inline MeshHandle CreateConeMesh( float radius = 0.5f, float height = 1.0f,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity() )
{
	ConeDesc desc;
	desc.radius = radius;
	desc.cone_height = height;

	return CreateConeMesh( desc, diffuse_color, model_pose );
}


inline MeshHandle CreateCylinderMesh( float radius = 0.5f, float height = 1.0f,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity() )
{
	CylinderDesc cylinder_desc;
	cylinder_desc.radii[0] = cylinder_desc.radii[1] = radius;
	cylinder_desc.height = height;

	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new CylinderMeshGenerator(cylinder_desc) );
	mesh_desc.pMeshGenerator->SetDiffuseColor( diffuse_color );

	MeshHandle mesh;
	bool loaded = mesh.Load( mesh_desc );

	if( loaded )
		return mesh;
	else
		return MeshHandle();
}


inline Result::Name Set6FaceColors(
	CustomMesh& box_mesh,
	const SFloatRGBAColor& px = SFloatRGBAColor::Red(),
	const SFloatRGBAColor& py = SFloatRGBAColor::Green(),
	const SFloatRGBAColor& pz = SFloatRGBAColor::Blue(),
	const SFloatRGBAColor& nx = SFloatRGBAColor::Aqua(),
	const SFloatRGBAColor& ny = SFloatRGBAColor::Magenta(),
	const SFloatRGBAColor& nz = SFloatRGBAColor::Yellow()
	)
{
	SFloatRGBAColor face_colors[6] = { py, ny, nz, pz, px, nx };

	if( box_mesh.GetNumVertices() != 24 )
		return Result::INVALID_ARGS;

	std::vector<SFloatRGBAColor> diffuse_colors;
	diffuse_colors.resize( 24 );
	for( int i=0; i<6; i++ )
	{
		for( int j=0; j<4; j++ )
		{
			diffuse_colors[ i * 4 + j ] = face_colors[i];
		}
	}

	box_mesh.SetDiffuseColors( diffuse_colors );

	return Result::SUCCESS;
}


} // amorphous



#endif /* __MeshUtilities_HPP__ */
