#ifndef  __2DPrimitiveRenderer_GL_HPP__
#define  __2DPrimitiveRenderer_GL_HPP__


#include "../../2DPrimitive/2DPrimitiveRenderer.hpp"
#include "../../TextureHandle.hpp"
#include "amorphous/Graphics/OpenGL/GLHeaders.h"


namespace amorphous
{


class C2DPrimitiveRenderer_GL : public C2DPrimitiveRenderer
{
	void RenderViaVertexAttribArray( const General2DVertex *pVertices, uint num_vertices, ushort *indices, uint num_indices, GLenum primitive_type );

public:

	C2DPrimitiveRenderer_GL() {}

	void Init();

	void Release();

	void Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type );

	void Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type, const TextureHandle& texture );

	void Render( const General2DVertex *pVertices, uint num_vertices, U16 *indices, uint num_indices, PrimitiveType::Name primitive_type );

	void Render( const General2DVertex *pVertices, uint num_vertices, U16 *indices, uint num_indices, PrimitiveType::Name primitive_type, TextureHandle texture );

	void Render( ShaderManager& rShaderManager, General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type );

	void Render( ShaderManager& rShaderManager, General2DVertex *paVertex, int num_vertices, U16 *indices, uint num_indices, PrimitiveType::Name primitive_type );

	void RenderRect( ShaderManager& rShaderManager, C2DRect& rect );
};


inline C2DPrimitiveRenderer_GL& PrimitiveRenderer_GL()
{
	static C2DPrimitiveRenderer_GL s_Renderer;
	return s_Renderer;
}


} // namespace amorphous



#endif /* __2DPrimitiveRenderer_GL_HPP__ */
