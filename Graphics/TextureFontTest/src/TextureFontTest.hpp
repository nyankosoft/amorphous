#ifndef  __TextureFontTest_HPP__
#define  __TextureFontTest_HPP__


#include <boost/shared_ptr.hpp>
#include "gds/base.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Support/indexed_vector.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CTextureFontTest : public CGraphicsTestBase
{
private:

//	boost::shared_ptr<CTrueTypeTextureFont> m_pFont;
	boost::shared_ptr<CFontBase> m_pFont;

	indexed_vector<std::string> m_FontFilePathnames;

	bool m_EnableRotation;

	float m_fRotationAngle;

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

	void LoadCurrentFont();

	void RenderText();
};


#endif /* __TextureFontTest_HPP__ */
