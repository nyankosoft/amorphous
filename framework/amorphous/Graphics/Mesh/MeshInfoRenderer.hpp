#ifndef __amorphous_MeshInfoRenderer_HPP__
#define __amorphous_MeshInfoRenderer_HPP__


#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/3DMath/Matrix44.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"


namespace amorphous
{


class MeshInfoRenderer
{
public:

	MeshInfoRenderer() {}

	~MeshInfoRenderer() {}

	Result::Name RenderNormals( const BasicMesh& mesh, const Matrix44& world_transform, float normal_length = 1.0f, const SFloatRGBAColor& color = SFloatRGBAColor(1.00f,0.27f,0.00f,1.00f) );

	Result::Name RenderNormals( const BasicMesh& mesh, const Matrix34& mesh_pose = Matrix34Identity(), float normal_length = 1.0f, const SFloatRGBAColor& color = SFloatRGBAColor(1.00f,0.27f,0.00f,1.00f) );

//	Result::Name RenderWireframe( const BasicMesh& mesh );
};


} // namespace amorphous


#endif  /* __amorphous_MeshInfoRenderer_HPP__ */
