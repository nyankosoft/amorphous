#ifndef  __TextureFontTest_HPP__
#define  __TextureFontTest_HPP__


#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include <gds/base.hpp>
#include "Graphics/fwd.hpp"
#include "Graphics/TextureHandle.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CTextureFontTest : public CGraphicsTestBase
{
//	CInputHandlerSharedPtr m_pInputHandler;

private:

//	boost::shared_ptr<CTrueTypeTextureFont> m_pFont;
	boost::shared_ptr<CFontBase> m_pFont;

	U32 m_FontFlags;
	int m_FontWidth;
	int m_FontHeight;

	CTextureHandle m_BGTexture;

public:

	CTextureFontTest();

	~CTextureFontTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );
};


#endif /* __TextureFontTest_HPP__ */
