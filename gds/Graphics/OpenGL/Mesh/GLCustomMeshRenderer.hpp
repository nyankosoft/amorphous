#ifndef  __GLCustomMeshRenderer_HPP__
#define  __GLCustomMeshRenderer_HPP__


#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


class CGLCustomMeshRenderer : public CCustomMeshRenderer
{
public:

	void RenderMesh( CCustomMesh& mesh );

	void RenderZSortedMesh( CCustomMesh& mesh ) { RenderMesh( mesh ); }

	void RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr );

	void RenderZSortedMesh( CCustomMesh& mesh, CShaderManager& shader_mgr ) { RenderMesh( mesh, shader_mgr ); }

	static CGLCustomMeshRenderer ms_Instance;
};


#endif /* __GLCustomMeshRenderer_HPP__ */
