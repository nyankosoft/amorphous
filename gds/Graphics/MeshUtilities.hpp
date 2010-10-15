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

	CMeshObjectHandle mesh;
	bool loaded = mesh.Load( mesh_desc );

	if( loaded )
		return mesh;
	else
		return CMeshObjectHandle();
}


#endif /* __MeshUtilities_HPP__ */
