#ifndef  __PrimitiveRendererDemo_H__
#define  __PrimitiveRendererDemo_H__


#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class PrimitiveRendererDemo : public CGraphicsTestBase
{
	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

private:

//	bool InitShader();

public:

	PrimitiveRendererDemo();

	~PrimitiveRendererDemo();

	const char *GetAppTitle() const { return "PrimitiveRendererDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __PrimitiveRendererDemo_H__ */
