#ifndef __MeshUtilities_HPP__
#define __MeshUtilities_HPP__


#include "Graphics/MeshGenerators.hpp"
#include "Graphics/MeshObjectHandle.hpp"


/// \param whd [in] width(x), height(y), and depth of the box
inline CMeshObjectHandle CreateBoxMesh( const Vector3& whd,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity() )
{
	boost::shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	CMeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;
	mesh_desc.pMeshGenerator->SetDiffuseColor( diffuse_color );

	CMeshObjectHandle mesh;
	bool loaded = mesh.Load( mesh_desc );

	if( loaded )
		return mesh;
	else
		return CMeshObjectHandle();
}


inline CMeshObjectHandle CreateSphereMesh( float radius,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity() )
{
	CSphereDesc sphere_desc;
	sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = radius;
	sphere_desc.axis = 1;
	CMeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new CSphereMeshGenerator(sphere_desc) );
	mesh_desc.pMeshGenerator->SetDiffuseColor( diffuse_color );

	CMeshObjectHandle mesh;
	bool loaded = mesh.Load( mesh_desc );

	if( loaded )
		return mesh;
	else
		return CMeshObjectHandle();
}


inline CMeshObjectHandle CreateConeMesh( const CConeDesc& desc,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity() )
{
	CMeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new CConeMeshGenerator(desc) );
	mesh_desc.pMeshGenerator->SetDiffuseColor( diffuse_color );

	CMeshObjectHandle mesh;
	bool loaded = mesh.Load( mesh_desc );

	if( loaded )
		return mesh;
	else
		return CMeshObjectHandle();
}


inline CMeshObjectHandle CreateConeMesh( float radius = 0.5f, float height = 1.0f,
								const SFloatRGBAColor& diffuse_color = SFloatRGBAColor::White(),
								const Matrix34& model_pose = Matrix34Identity() )
{
	CConeDesc desc;
	desc.radius = radius;
	desc.cone_height = height;

	return CreateConeMesh( desc, diffuse_color, model_pose );
}



#endif /* __MeshUtilities_HPP__ */