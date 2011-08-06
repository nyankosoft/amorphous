#ifndef __PrimitiveShapeRenderer_HPP__
#define __PrimitiveShapeRenderer_HPP__


#include "gds/3DMath/Sphere.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/FloatRGBAColor.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"


class CPrimitiveShapeRenderer
{
	CShaderHandle m_Shader; ///< default: fixed function pipeline manager

	static CCustomMesh ms_BoxMesh;

public:

	CPrimitiveShapeRenderer();

	void RenderSphere( const Sphere& sphere, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void RenderBox( const Vector3& vEdgeLengths, const Matrix34& world_pose, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void RenderCapsule( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void RenderCylinder( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	/// \param axis [in] [0,2] for x, y, and z axis in the positive direction respectively. [3,5] in the negative direction.
	void RenderAxisAlignedPlane( uint axis = 1, const Vector3& vCenter = Vector3(0,0,0), float span_0 = 100.0f, float span_1 = 100.0f, const SFloatRGBAColor& color = SFloatRGBAColor::White() );


	void RenderWireframeBox( const Vector3& vEdgeLengths, const Matrix34& world_pose, const SFloatRGBAColor& wireframe_color = SFloatRGBAColor::White() );


	void SetShader( CShaderHandle& shader );
};



#endif /* __PrimitiveShapeRenderer_HPP__ */
