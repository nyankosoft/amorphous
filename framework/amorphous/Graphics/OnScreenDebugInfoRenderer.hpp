#ifndef __OnScreenDebugInfoRenderer_HPP__
#define __OnScreenDebugInfoRenderer_HPP__


#include "amorphous/3DMath/Vector2.hpp"
#include "amorphous/Support/DebugOutput.hpp"
#include "2DPrimitive/2DRect.hpp"


namespace amorphous
{


class OnScreenDebugInfoRenderer
{
	boost::shared_ptr<FontBase> m_pFont;

	C2DRect m_BackgroundRect;

	Vector2 m_vTopLeftPos;

private:

	void RenderBGRect( int text_width );

public:

	OnScreenDebugInfoRenderer();

	~OnScreenDebugInfoRenderer(){}

	void RenderDebugInfo( const std::string& debug_info_text );

	void RenderDebugInfo(
		const std::vector<std::string>& debug_info_texts,
		const std::vector<SFloatRGBAColor> text_colors
		);

	void SetBackgroundRect( const C2DRect& bg_rect ) { m_BackgroundRect = bg_rect; }

	void SetTopLeftPos( Vector2& pos ) { m_vTopLeftPos = pos; }

	void RenderFPS();
};


} // namespace amorphous


#endif /* __OnScreenDebugInfoRenderer_HPP__ */
