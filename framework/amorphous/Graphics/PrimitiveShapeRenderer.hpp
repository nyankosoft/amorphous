#ifndef __PrimitiveShapeRenderer_HPP__
#define __PrimitiveShapeRenderer_HPP__


#include "amorphous/3DMath/Sphere.hpp"
#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"


namespace amorphous
{


class PrimitiveShapeRenderer
{
	ShaderHandle m_Shader; ///< default: fixed function pipeline manager

	static CustomMesh ms_BoxMesh;

	static CustomMesh ms_RectMesh;
	
private:

	ShaderManager *GetShaderManagerForPrimitiveShape();

	Result::Name RenderPlane(
		const Vector3 *positions,
		const Vector3& normal,
		const SFloatRGBAColor& color,
		TextureHandle& texture,
		const TEXCOORD2& top_left,
		const TEXCOORD2& bottom_right,
		ShaderManager& shader_mgr
		);

public:

	PrimitiveShapeRenderer();

	void RenderSphere( const Sphere& sphere, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void RenderBox( const Vector3& vEdgeLengths, const Matrix34& world_pose, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void RenderCapsule( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void RenderCylinder( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void RenderPlane(
		const Matrix34& pose = Matrix34Identity(),
		float width  = 100.0f,
		float height = 100.0f,
		const SFloatRGBAColor& color = SFloatRGBAColor::White(),
		TextureHandle& tex = TextureHandle(),
		const TEXCOORD2& top_left     = TEXCOORD2(0,0),
		const TEXCOORD2& bottom_right = TEXCOORD2(1,1)
		);

	/// \param axis [in] [0,2] for x, y, and z axis in the positive direction respectively. [3,5] in the negative direction.
	void RenderAxisAlignedPlane(
		uint axis = 1,
		const Vector3& vCenter = Vector3(0,0,0),
		float span_0 = 100.0f,
		float span_1 = 100.0f,
		const SFloatRGBAColor& color = SFloatRGBAColor::White(),
		TextureHandle& texture = TextureHandle(),
		const TEXCOORD2& top_left     = TEXCOORD2(0,0),
		const TEXCOORD2& bottom_right = TEXCOORD2(1,1)
		);

	void RenderFloorPlane( const Vector3& vCenter, float width, float depth, const SFloatRGBAColor& color, TextureHandle& texture = TextureHandle(), const TEXCOORD2& top_left = TEXCOORD2(0,0), const TEXCOORD2& bottom_right = TEXCOORD2(1,1) );

	void RenderWireframeBox( const Vector3& vEdgeLengths, const Matrix34& world_pose, const SFloatRGBAColor& wireframe_color = SFloatRGBAColor::White() );


	void SetShader( ShaderHandle& shader ) { m_Shader = shader; }
};


inline void RenderFloorPlane( ShaderHandle& shader, const Vector3& vCenter = Vector3(0,0,0), float width = 100.0f, float depth = 100.0f, const SFloatRGBAColor& color = SFloatRGBAColor::White() )
{
	PrimitiveShapeRenderer renderer;
	renderer.SetShader( shader );
	renderer.RenderFloorPlane( vCenter, width, depth, color );
}


} // namespace amorphous



#endif /* __PrimitiveShapeRenderer_HPP__ */
