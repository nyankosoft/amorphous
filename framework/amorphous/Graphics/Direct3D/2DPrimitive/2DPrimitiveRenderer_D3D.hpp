#ifndef  __2DPrimitiveRenderer_D3D_HPP__
#define  __2DPrimitiveRenderer_D3D_HPP__


#include "../../2DPrimitive/2DPrimitiveRenderer.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/Direct3D/FVF_TLVertex.h"
#include "Graphics/Direct3D/Direct3D9.hpp"


namespace amorphous
{


class C2DPrimitiveRenderer_D3D : public C2DPrimitiveRenderer
{
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDecleration;

	LPDIRECT3DVERTEXDECLARATION9 m_pColoredVertexDecleration;

	std::vector<TLVERTEX> m_vecTLVertex;

private:

	/// copy general 2D vertices to the internal buffer which stores vertices
	/// with an FVF format to render them with DrawPrimitiveUP().
	inline void CopyVertices( const General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type );

public:

	C2DPrimitiveRenderer_D3D()
		:
	m_pVertexDecleration(NULL),
	m_pColoredVertexDecleration(NULL)
	{
	}

	void Init();

	void Release();

	void Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type );
	
	void Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type, const TextureHandle& texture );

	void Render( ShaderManager& rShaderManager, General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type );

	void RenderRect( ShaderManager& rShaderManager, C2DRect& rect );
};


inline C2DPrimitiveRenderer_D3D& PrimitiveRenderer_D3D()
{
	static C2DPrimitiveRenderer_D3D s_Renderer;
	return s_Renderer;
}


} // namespace amorphous



#endif /* __2DPrimitiveRenderer_D3D_HPP__ */
