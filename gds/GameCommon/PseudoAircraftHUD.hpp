#ifndef  __PSEUDOAIRCRAFT_HUD_H__
#define  __PSEUDOAIRCRAFT_HUD_H__


#include "gds/Graphics/2DPrimitive/2DRectSet.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/3DMath/Vector2.hpp"
#include "gds/3DMath/Vector3.hpp"

#include <vector>
#include <string>


namespace amorphous
{

class CPseudoAircraftSimulator;


/**
 * renderes basic HUD components for aircraft
 * - speed
 * - altitude
 * - direction
 * - pitch ladder
 * TODO: flight path indicator
 */
class CPseudoAircraftHUD : public CGraphicsComponent
{
	TextureFont *m_pFont;

	C2DRectSet m_RectSet;

	Vector2 m_vScreenCenter;

	enum { NUM_MAX_RECTS = 64, NUM_MAX_TGT_INFO = 32 };

	U32 m_HUDColor;

	TextureHandle m_Texture;

//	fixed_vector<HUD_TargetInfo, NUM_MAX_TGT_INFO> m_vecTargetInfo;

private:

	void RenderCenterComponents();

public:

	CPseudoAircraftHUD();
	virtual ~CPseudoAircraftHUD();

	void Init();

	void Release();

	void Render( const CPseudoAircraftSimulator& craft );

	inline void SetScreenCenter( float x, float y ) { m_vScreenCenter.x = (Scalar)x, m_vScreenCenter.y = (Scalar)y; }
	inline void SetScreenCenter( int x, int y ) { m_vScreenCenter.x = (Scalar)x, m_vScreenCenter.y = (Scalar)y; }

	Vector2 GetScreenCenter() const { return m_vScreenCenter; }

	void SetColor( U32 color ) { m_HUDColor = color; }

	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );
	virtual void ReleaseGraphicsResources();

//	inline void AddTargetInfo( const HUD_TargetInfo& info );
};


//--------------------- inline implementations ---------------------
/*
inline void CPseudoAircraftHUD::AddTargetInfo( const HUD_TargetInfo& info )
{
	if( m_vecTargetInfo.size() < NUM_MAX_TGT_INFO )
		m_vecTargetInfo.push_back( info );
}*/

} // namespace amorphous



#endif		/*  __PSEUDOAIRCRAFT_HUD_H__  */
