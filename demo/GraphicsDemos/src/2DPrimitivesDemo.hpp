#ifndef  __2DPrimitivesDemo_HPP__
#define  __2DPrimitivesDemo_HPP__


#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DFrameRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRoundRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRectSet.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class C2DPrimitivesDemo : public CGraphicsTestBase
{
	enum Params
	{
		NUM_2D_PRIMITIVE_TYPES_TO_RENDER = 5,
	};

	int m_Type;

	static const int m_NumFrameTextureVariations = 5;

	std::vector<TextureHandle> m_RectTextures;

	std::vector<TextureHandle> m_FrameTextures;

	std::vector<C2DRect> m_Rects;

	std::vector<C2DRoundRect> m_RoundRects;

	std::vector<C2DFrameRect> m_FrameRects;

	std::vector<C2DRoundFrameRect> m_RoundFrameRects;

	C2DRectSet m_RectSet;

	ShaderHandle m_Shaders[2];

	bool m_UseTexture;

	bool m_RenderWithUserDefinedShader;

	TextureHandle m_BackgroundTexture;

private:

	int InitShaders();

	int InitRects();

	int InitFrameRects();

	void RenderRects();

	void RenderFrameRects();

	void RenderRoundRects();

	void RenderRoundFrameRects();

	void RenderRectSet();

public:

	C2DPrimitivesDemo();

	~C2DPrimitivesDemo();

	const char *GetAppTitle() const { return "2DPrimitivesDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );
};


#endif /* __2DPrimitivesDemo_HPP__ */
