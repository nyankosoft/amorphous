#include "ScreenEffect.h"
#include "../3DCommon/Direct3D9.h"
#include "../3DCommon/Shader/ShaderManager.h"


CFadeEffect::CFadeEffect()
{
	in_use = false;
	m_iType = SCREENEFFECT_FADE_IN;
	m_dwColor = 0xFF000000;
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

	U32 dwCurrentAlpha = D3DCOLOR_ARGB( ((int)(fCurrentAlpha)), 0, 0, 0 );

	m_ScreenRect.SetColor( dwCurrentAlpha | m_dwColor );

	m_ScreenRect.Draw();
}


void CFadeEffect::SetFadeEffect(int iType, U32 dest_color, float fTotalTime, AlphaBlend::Mode blend_mode )
{
	m_iType = iType;
	m_fMaxAlpha = (float)( (dest_color & 0xFF000000) >> 24 );
	m_dwColor = dest_color & 0x00FFFFFF;	// 'm_dwColor' is transparent in itself - alpha value is determined during the drawing routine
	m_fTotalTime = fTotalTime;

	in_use = true;
	m_fCurrentTime = 0.0f;
	m_ScreenRect.SetDestAlphaBlendMode( blend_mode );
}


void CFadeEffect::SetScreenSize( int iScreenWidth, int iScreenHeight )
{
	m_ScreenRect.SetPosition( D3DXVECTOR2(0,0), D3DXVECTOR2((float)iScreenWidth,(float)iScreenHeight) );
}
