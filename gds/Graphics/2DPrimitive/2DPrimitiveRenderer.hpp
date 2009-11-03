#ifndef  __2DPrimitiveRenderer_HPP__
#define  __2DPrimitiveRenderer_HPP__


#include "Graphics/General2DVertex.hpp"
#include "Graphics/GraphicsDevice.hpp"


class C2DPrimitiveRenderer
{
public:

	virtual ~C2DPrimitiveRenderer() {}

	virtual void Init() {}

	virtual void Release() {}

	virtual void Render( CGeneral2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type ) = 0;

	virtual void Render( CShaderManager& rShaderManager, CGeneral2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type ) = 0;

	virtual void RenderRect( CShaderManager& rShaderManager, const C2DRect& rect ) = 0;
};


/*
inline C2DPrimitiveRenderer& PrimitiveRenderer()
{
	static C2DPrimitiveRenderer_D3D s_Renderer;
	return s_Renderer;
}
*/

inline C2DPrimitiveRenderer*& PrimitiveRendererPtr()
{
	static C2DPrimitiveRenderer *s_pRenderer = NULL;
	return s_pRenderer;
}


/// 2D primitive renderer must be initialized before calling this.
inline C2DPrimitiveRenderer& PrimitiveRenderer() { return *PrimitiveRendererPtr(); }



#endif /* __2DPrimitiveRenderer_HPP__ */
