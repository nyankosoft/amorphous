#ifndef  __MultibyteFontTest_H__
#define  __MultibyteFontTest_H__


#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/Input.hpp"
#include "gds/GUI/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CMultibyteFontTest : public CGraphicsTestBase
{
	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<FontBase> m_pFont;

	boost::shared_ptr<UTFFont> m_pUTFFont;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	TextureHandle m_TextBufferTexture;

	std::string m_UTFText;

	std::string m_TextBuffer;

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
