#ifndef  __2DPrimitiveRenderer_D3D_HPP__
#define  __2DPrimitiveRenderer_D3D_HPP__


#include "../../2DPrimitive/2DPrimitiveRenderer.hpp"
#include "Graphics/FVF_TLVertex.h"
#include "Graphics/Direct3D9.hpp"


class C2DPrimitiveRenderer_D3D : public C2DPrimitiveRenderer
{
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDecleration;

	LPDIRECT3DVERTEXDECLARATION9 m_pColoredVertexDecleration;

	std::vector<TLVERTEX> m_vecTLVertex;

public:

	C2DPrimitiveRenderer_D3D()
		:
	m_pVertexDecleration(NULL),
	m_pColoredVertexDecleration(NULL)
	{
	}

	void Init();

	void Release();

	void Render( CGeneral2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type );
};


inline C2DPrimitiveRenderer_D3D& PrimitiveRenderer_D3D()
{
	static C2DPrimitiveRenderer_D3D s_Renderer;
	return s_Renderer;
}



#endif /* __2DPrimitiveRenderer_D3D_HPP__ */
