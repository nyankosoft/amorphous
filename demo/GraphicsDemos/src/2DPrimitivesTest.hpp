#ifndef  __2DPrimitivesTest_HPP__
#define  __2DPrimitivesTest_HPP__


#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DFrameRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRoundRect.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class C2DPrimitivesTest : public CGraphicsTestBase
{
	enum Params
	{
		NUM_2D_PRIMITIVE_TYPES_TO_RENDER = 4,
	};

	int m_Type;

	std::vector<TextureHandle> m_RectTextures;

	std::vector<TextureHandle> m_FrameTextures;

	std::vector<C2DRect> m_Rects;

	std::vector<C2DRoundRect> m_RoundRects;

	std::vector<C2DFrameRect> m_FrameRects;

	std::vector<C2DRoundFrameRect> m_RoundFrameRects;

	ShaderHandle m_Shader;

	bool m_UseTexture;

private:

	int InitRects();

	int InitFrameRects();

	void RenderRects();

public:

	C2DPrimitivesTest();

	~C2DPrimitivesTest();

	const char *GetAppTitle() const { return "2DPrimitivesTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );
};


#endif /* __2DPrimitivesTest_HPP__ */