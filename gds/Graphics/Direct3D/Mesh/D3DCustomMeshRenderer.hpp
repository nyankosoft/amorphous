#ifndef __D3DCustomMeshRenderer_HPP__
#define __D3DCustomMeshRenderer_HPP__


#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


class CD3DCustomMeshRenderer : public CCustomMeshRenderer
{
	void DrawPrimitives( CCustomMesh& mesh );

public:

	void RenderMesh( CCustomMesh& mesh );

	void RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr );

	static CD3DCustomMeshRenderer ms_Instance;
};


#endif /* __D3DCustomMeshRenderer_HPP__ */
