#ifndef  __MultibyteFontDemo_H__
#define  __MultibyteFontDemo_H__


#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class MultibyteFontDemo : public CGraphicsTestBase
{
	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	std::shared_ptr<UTFFont> m_pUTFFont;

//	CGM_DialogManagerSharedPtr m_pSampleUI;

	TextureHandle m_TextBufferTexture;

	std::string m_UTFText;

	std::vector<std::string> m_FontFilePaths;

	unsigned int m_FontIndex;

	unsigned int m_FontSize;

private:

	void CreateSampleUI();

	bool InitShader();

	int InitFont();

//	bool CreateFontTextureFromTrueTypeFont( array2d<U8>& dest_bitmap_buffer );
	bool RenderUTF8TextToBufferToImageFile( const std::string& text,
											 const std::string& font_file,
											 const int base_char_height
											  );

public:

	MultibyteFontDemo();

	~MultibyteFontDemo();

	const char *GetAppTitle() const { return "MultibyteFontDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __MultibyteFontDemo_H__ */
