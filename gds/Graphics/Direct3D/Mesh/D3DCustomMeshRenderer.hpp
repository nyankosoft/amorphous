#ifndef __D3DCustomMeshRenderer_HPP__
#define __D3DCustomMeshRenderer_HPP__


#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


class CD3DCustomMeshRenderer : public CCustomMeshRenderer
{
public:

	void RenderMesh( CCustomMesh& mesh );

	static CD3DCustomMeshRenderer ms_Instance;
};


#endif /* __D3DCustomMeshRenderer_HPP__ */
