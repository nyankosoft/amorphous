#ifndef  __TextureFontDemo_HPP__
#define  __TextureFontDemo_HPP__


#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Support/indexed_vector.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class TextureFontDemo : public CGraphicsTestBase
{
private:

//	std::shared_ptr<TrueTypeTextureFont> m_pFont;
	std::shared_ptr<FontBase> m_pDemoFont;

	indexed_vector<std::string> m_FontFilePathnames;

	bool m_EnableRotation;

	float m_fRotationAngle;

	U32 m_FontFlags;
	int m_FontWidth;
	int m_FontHeight;

	TextureHandle m_BGTexture;

public:

	TextureFontDemo();

	~TextureFontDemo();

	const char *GetAppTitle() const { return "TextureFontDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );

	void LoadCurrentFont();

	void RenderText();
};


#endif /* __TextureFontDemo_HPP__ */
