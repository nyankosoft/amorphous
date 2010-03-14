#ifndef  __GLCustomMeshRenderer_HPP__
#define  __GLCustomMeshRenderer_HPP__


#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"


class CGLCustomMeshRenderer : public CCustomMeshRenderer
{
public:

	void RenderMesh( CCustomMesh& mesh );

	static CGLCustomMeshRenderer ms_Instance;
};


#endif /* __GLCustomMeshRenderer_HPP__ */
