#ifndef  __2DPrimitiveRenderer_GL_HPP__
#define  __2DPrimitiveRenderer_GL_HPP__


#include "../../2DPrimitive/2DPrimitiveRenderer.hpp"
#include <gl/gl.h>


namespace amorphous
{


class C2DPrimitiveRenderer_GL : public C2DPrimitiveRenderer
{

public:

	C2DPrimitiveRenderer_GL() {}

	void Init();

	void Release();

	/// Use this to render with a primitive type only available in OpenGL
	/// - Should be used only by the system
	/// - See C2DRectSetImpl_GL::draw()
	void RenderGL( General2DVertex *paVertex, int num_vertices, GLenum primitive_type );

	void Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type );

	void Render( ShaderManager& rShaderManager, General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type );

	void RenderRect( ShaderManager& rShaderManager, C2DRect& rect );
};


inline C2DPrimitiveRenderer_GL& PrimitiveRenderer_GL()
{
	static C2DPrimitiveRenderer_GL s_Renderer;
	return s_Renderer;
}


} // namespace amorphous



#endif /* __2DPrimitiveRenderer_GL_HPP__ */
