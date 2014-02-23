#ifndef  __2DPrimitiveRenderer_HPP__
#define  __2DPrimitiveRenderer_HPP__


#include "../General2DVertex.hpp"
#include "../GraphicsDevice.hpp"


namespace amorphous
{


class C2DPrimitiveRenderer
{
public:

	C2DPrimitiveRenderer() : m_Profile(false) {}

	virtual ~C2DPrimitiveRenderer() {}

	virtual void Init() {}

	virtual void Release() {}

	virtual void Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type ) = 0;

	virtual void Render( ShaderManager& rShaderManager, General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type ) = 0;

	virtual void RenderRect( ShaderManager& rShaderManager, C2DRect& rect ) = 0;

	/// \brief Set this to true to profile the function calls.
	/// This is not declared static because the class is used as a singleton.
	bool m_Profile;
};


/*
inline C2DPrimitiveRenderer& PrimitiveRenderer()
{
	static C2DPrimitiveRenderer_D3D s_Renderer;
	return s_Renderer;
}
*/

inline C2DPrimitiveRenderer*& Ref2DPrimitiveRendererPtr()
{
	static C2DPrimitiveRenderer *s_pRenderer = NULL;
	return s_pRenderer;
}


/// 2D primitive renderer must be initialized before calling this.
inline C2DPrimitiveRenderer& Get2DPrimitiveRenderer() { return *Ref2DPrimitiveRendererPtr(); }


} // namespace amorphous



#endif /* __2DPrimitiveRenderer_HPP__ */
