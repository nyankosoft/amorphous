#ifndef  __TextureFontTest_HPP__
#define  __TextureFontTest_HPP__


#include <boost/shared_ptr.hpp>
#include "gds/base.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Support/indexed_vector.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CTextureFontTest : public CGraphicsTestBase
{
private:

//	boost::shared_ptr<TrueTypeTextureFont> m_pFont;
	boost::shared_ptr<FontBase> m_pFont;

	indexed_vector<std::string> m_FontFilePathnames;

	bool m_EnableRotation;

	float m_fRotationAngle;

	U32 m_FontFlags;
	int m_FontWidth;
	int m_FontHeight;

	TextureHandle m_BGTexture;

public:

	CTextureFontTest();

	~CTextureFontTest();

	const char *GetAppTitle() const { return "TextureFontTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );

	void LoadCurrentFont();

	void RenderText();
};


#endif /* __TextureFontTest_HPP__ */
