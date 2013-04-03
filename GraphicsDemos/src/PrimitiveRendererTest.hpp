#ifndef  __PrimitiveRendererTest_H__
#define  __PrimitiveRendererTest_H__


#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CPrimitiveRendererTest : public CGraphicsTestBase
{
	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

	std::string m_TextBuffer;

private:

//	bool InitShader();

public:

	CPrimitiveRendererTest();

	~CPrimitiveRendererTest();

	const char *GetAppTitle() const { return "PrimitiveRendererTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __PrimitiveRendererTest_H__ */
