#ifndef __MeshConvenienceFunctions_HPP__
#define __MeshConvenienceFunctions_HPP__


#include "TriangleMeshDesc.hpp"
#include "ConvexShapeDesc.hpp"
#include "PhysicsEngine.hpp"
#include "Preprocessor.hpp"


namespace amorphous
{

namespace physics
{


inline bool SetConvexShapeDesc( CTriangleMeshDesc& src_convex_mesh_desc, CConvexShapeDesc& dest_convex_shape_desc )
{
	if( !src_convex_mesh_desc.IsValid() )
		return false;

	CStream convex_mesh_stream;
	Result::Name res = Preprocessor().CreateConvexMeshStream( src_convex_mesh_desc, convex_mesh_stream );
	if( res != Result::SUCCESS )
		return false;

	if( convex_mesh_stream.m_Buffer.buffer().empty() )
		return false;

	convex_mesh_stream.m_Buffer.reset_pos();

	CConvexMesh *pConvexMesh = PhysicsEngine().CreateConvexMesh( convex_mesh_stream );
	if( !pConvexMesh )
		return false;

	dest_convex_shape_desc.pConvexMesh = pConvexMesh;

	return true;
}


} // namespace physics

} // namespace amorphous


#endif /* __MeshConvenienceFunctions_HPP__ */
