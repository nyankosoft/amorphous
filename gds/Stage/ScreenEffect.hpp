#ifndef  __FADEEFFECT_H__
#define  __FADEEFFECT_H__

#include "../base.hpp"
#include "../Graphics/2DPrimitive/2DRect.hpp"
#include "../Graphics/GraphicsComponentCollector.hpp"

#define SCREENEFFECT_FADE_OUT 0
#define SCREENEFFECT_FADE_IN  1


struct CFadeEffect
{
	bool in_use;
	int m_iType;
	U32 m_dwColor; ///< ARGB color
	float m_fMaxAlpha;	// between 0 and 255
	float m_fCurrentTime;
	float m_fTotalTime;

	C2DRect m_ScreenRect;

public:

	CFadeEffect();
//	~CFadeEffect();

	void Draw();

	void SetFadeEffect( int iType, U32 dest_color, float fTotalTime, AlphaBlend::Mode blend_mode );

	bool IsInUse() { return in_use; }

	void SetScreenSize( int iScreenWidth, int iScreenHeight );

	inline void Update( float fFrameTime );
};


inline void CFadeEffect::Update( float fFrameTime )
{
	m_fCurrentTime += fFrameTime;

	if( m_fTotalTime < m_fCurrentTime )
	{	// effect has finished
		m_fCurrentTime = 0;
		m_fTotalTime = 0;
		in_use = false;
		return;
	}
}


#endif		/*  __FADEEFFECT_H__  */
