#ifndef  __2DPrimitivesTest_HPP__
#define  __2DPrimitivesTest_HPP__


#include "amorphous/Graphics/2DPrimitive/2DFrameRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRoundRect.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class C2DPrimitivesTest : public CGraphicsTestBase
{
//	boost::shared_ptr<FontBase> m_pFont;

	enum Params
	{
		NUM_2D_PRIMITIVE_TYPES_TO_RENDER = 2,
	};

	int m_Type;

	std::vector<TextureHandle> m_vecTexture;

	std::vector<C2DFrameRect> m_FrameRects;

	std::vector<C2DRoundFrameRect> m_RoundFrameRects;

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
