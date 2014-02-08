#ifndef  __MultibyteFontTest_H__
#define  __MultibyteFontTest_H__


#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CMultibyteFontTest : public CGraphicsTestBase
{
	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<UTFFont> m_pUTFFont;

//	CGM_DialogManagerSharedPtr m_pSampleUI;

	TextureHandle m_TextBufferTexture;

	std::string m_UTFText;

private:

	void CreateSampleUI();

	bool InitShader();

//	bool CreateFontTextureFromTrueTypeFont( array2d<U8>& dest_bitmap_buffer );
	bool RenderUTF8TextToBufferToImageFile( const std::string& text,
											 const std::string& font_file,
											 const int base_char_height
											  );

public:

	CMultibyteFontTest();

	~CMultibyteFontTest();

	const char *GetAppTitle() const { return "MultibyteFontTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __MultibyteFontTest_H__ */
