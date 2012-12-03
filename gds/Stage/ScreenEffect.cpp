#include "ScreenEffect.hpp"
#include "Graphics/Shader/ShaderManager.hpp"


CFadeEffect::CFadeEffect()
{
	in_use = false;
	m_iType = SCREENEFFECT_FADE_IN;
	m_Color = SFloatRGBAColor::Black();
	m_fCurrentTime = 0.0f;
	m_fTotalTime = 0.0f;
	m_fMaxAlpha = 0.0f;
}


void CFadeEffect::Draw()
{
	if( !in_use )
		return;

	float fCurrentAlpha = m_fMaxAlpha * m_fCurrentTime / m_fTotalTime;
	if( m_iType == SCREENEFFECT_FADE_IN )
		fCurrentAlpha = m_fMaxAlpha - fCurrentAlpha;

	clamp( fCurrentAlpha, 0.0f, 1.0f );

	m_Color.alpha = fCurrentAlpha;

	m_ScreenRect.SetColor( m_Color );

	m_ScreenRect.Draw();
}


void CFadeEffect::SetFadeEffect(int iType, const SFloatRGBAColor& dest_color, float fTotalTime, AlphaBlend::Mode blend_mode )
{
	m_iType = iType;
	m_fMaxAlpha = dest_color.alpha;//(float)( (dest_color & 0xFF000000) >> 24 );
	m_Color = dest_color;
	m_Color.alpha = 0.0f; // Set the alpha of the current color to 0 and change it over time until it reaches the value specified in the dest_color.
	m_fTotalTime = fTotalTime;

	in_use = true;
	m_fCurrentTime = 0.0f;
	m_ScreenRect.SetDestAlphaBlendMode( blend_mode );
}


void CFadeEffect::SetScreenSize( int iScreenWidth, int iScreenHeight )
{
	m_ScreenRect.SetPosition( Vector2(0,0), Vector2((float)iScreenWidth,(float)iScreenHeight) );
}
