
#include "GM_PaintBar.h"
#include "GM_PaintBarDesc.h"
#include "GM_Dialog.h"

#include "GameInput/InputHandler.h"

#include <algorithm>


//=====================================================================================
// Segment
//=====================================================================================

inline bool CGM_PaintBar::Segment::OverlapsWith( Segment& region )
{
	if( region.right < left || right < region.left)
		return false;
	else
		return true;

/*
	if( region.left <= left && left < region.right )		return true;
	else if( region.left <= right && right < region.right )	return true;
	else		return false;
*/

}


bool CGM_PaintBar::Segment::ContainsPosition( int  x )
{
	if( left <= x && x <= right )
		return true;
	else
		return false;
}


inline void CGM_PaintBar::Segment::MergeWith( Segment& region )
{
	left  = left < region.left ? left : region.left;
	right = right > region.right ? right : region.right;

//	left = Min( left, region.left );
//	right = Max( right, region.right );
}


//=====================================================================================
// CGM_PaintBar
//=====================================================================================

CGM_PaintBar::CGM_PaintBar( CGM_Dialog *pDialog, CGM_PaintBarDesc *pDesc )
: CGM_Control( pDialog, pDesc )
{
	m_BarFlag = pDesc->BarTypeFlag;

	m_BarRect = *(pDesc->pBarRect);

	m_bPressed = false;

	m_iSegmentEpsilon = 3;	// pDesc->iSegmentEpsilon;

}


//void CGM_PaintBar::UpdateRects(){}


bool CGM_PaintBar::HandleMouseInput( SInputData& input )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

	SPoint pt = SPoint( input.GetParamH16(), input.GetParamL16() );

	int segment_type;

    switch( input.iGICode )
    {
	case GIC_MOUSE_BUTTON_L:
	case GIC_MOUSE_BUTTON_R:

		// set segment type
		if( input.iGICode == GIC_MOUSE_BUTTON_L )
			segment_type = 0;
		else
			segment_type = 1;

        if( input.iType == ITYPE_KEY_PRESSED )
		{

			if( m_BoundingBox.ContainsPoint( pt ) )
			{
				CheckDeleteSegment( pt.x, segment_type );

				// Pressed while inside the control
				m_bPressed = true;
//				SetCapture( DXUTGetHWND() );

				if( !m_bHasFocus )
					m_pDialog->RequestFocus( this );

				m_iStartPosition = pt.x - m_BoundingBox.left;
				m_iCurrentPosition = m_iStartPosition;

				return true;
			}
			break;
        }

		else if( input.iType == ITYPE_KEY_RELEASED )
		{
            if( m_bPressed )
            {
                m_bPressed = false;
//				ReleaseCapture();

				m_iCurrentPosition = pt.x - m_BoundingBox.left;

				Segment segment;
				segment.left = m_iStartPosition;
				segment.right = m_iCurrentPosition;

				if( abs(segment.left - segment.right) < m_iSegmentEpsilon )
					return true;	// segment too small

				if( segment.right < segment.left )
					swap( segment.right, segment.left );

				Clamp( segment.left, segment.right );

				// set segment type
//				if( input.iGICode == GIC_MOUSE_BUTTON_L )					segment.type = 0;
//				else					segment.type = 1;

				segment.type = segment_type;

				AddSegment( segment );
//				m_pDialog->ClearFocus();
//				m_pDialog->SendEvent( GM_EVENT_SLIDER_VALUE_CHANGED, true, this );

                return true;
            }
        }
        break;

//	case GIC_MOUSE_BUTTON_M:
//	case GIC_MOUSE_BUTTON_R:
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( !m_BoundingBox.ContainsPoint( pt ) )
				return false;

			CheckDeleteSegment( pt.x );

			return true;
		}
		break;
*/
	case GIC_MOUSE_AXIS_X:
	case GIC_MOUSE_AXIS_Y:
		if( m_bPressed )
		{
			// update current position
			m_iCurrentPosition = (pt.x - m_BoundingBox.left);

			if( m_iCurrentPosition < 0 )
				m_iCurrentPosition = 0;
			if( m_BoundingBox.GetWidth() < m_iCurrentPosition )
				m_iCurrentPosition = m_BoundingBox.GetWidth();

			return true;
		}
		break;

    }

	return false;
}


void CGM_PaintBar::Clamp( int& left, int& right )
{
	if( left < 0 )
		left = 0;
	if( m_BoundingBox.GetWidth() < right )
		right = m_BoundingBox.GetWidth();
}


void CGM_PaintBar::CheckDeleteSegment( const int pos_x, const int non_target_type )
{
	int i=0, iNumSegs = m_vecPaintedRegion.size();
	while( i<iNumSegs )
//	for( i=0; i<iNumSegs; i++ )
	{
		if( m_vecPaintedRegion[i].ContainsPosition( pos_x - m_BoundingBox.left ) &&
			m_vecPaintedRegion[i].type != non_target_type )
		{
			// found a segment to delete
			m_vecPaintedRegion.erase( m_vecPaintedRegion.begin() + i );
			iNumSegs = m_vecPaintedRegion.size();
			i=0;
		}

		i++;
	}
}


void CGM_PaintBar::AddSegment( Segment& region )
{
	if( m_BarFlag & TYPE_MERGE_OVERLAPS )
	{
		int i=0;
		int iNumRegions = m_vecPaintedRegion.size();
		while( i<iNumRegions )
//		for( i=0; i<iNumRegions; i++ )
		{
			if( m_vecPaintedRegion[i].OverlapsWith(region) )
			{
				region.MergeWith( m_vecPaintedRegion[i] );
				m_vecPaintedRegion.erase( m_vecPaintedRegion.begin() + i );

				// restart search from the first element
				i = 0;
				iNumRegions = m_vecPaintedRegion.size();

			}
			else
				i++;
		}
	}

	m_vecPaintedRegion.push_back( region );
}


void CGM_PaintBar::Render( float fElapsedTime )
{

	// debug
	D3DXVECTOR2 vTopLeft     = D3DXVECTOR2( m_BoundingBox.left,  m_BoundingBox.top );

	int	iState = GM_STATE_NORMAL;

/*	DWORD dwColor = D3DCOLOR_ARGB( (int)(m_BarRect.m_FontColor.m_aColor[iState].fAlpha * 255.0f),
		                           (int)(m_BarRect.m_FontColor.m_aColor[iState].fRed   * 255.0f),
								   (int)(m_BarRect.m_FontColor.m_aColor[iState].fGreen * 255.0f),
								   (int)(m_BarRect.m_FontColor.m_aColor[iState].fBlue  * 255.0f) );
*/
//	CFont *pFont = m_pDialog->GetFont( m_DefaultFont.m_iFontIndex );
//	pFont->DrawText( m_strTitle.c_str(), vTopLeft, dwColor );

	m_BarRect.m_RectColor.Blend( GM_STATE_NORMAL, fElapsedTime, 1.0f );
	m_BarRect.Draw( m_pDialog );

	C2DRect rect;
	int i, iNumRegions = m_vecPaintedRegion.size();
	int left, right, top, bottom;
	top    = m_BarRect.GetRect().top;
	bottom = m_BarRect.GetRect().bottom;

	DWORD dwColorSet0[2] = { 0xA010E010, 0xA0E01010 };	// green, red
	DWORD dwColorSet1[2] = { 0xA0D0D0D0, 0xA0D0D0D0 };	// gray,  gray
	DWORD *pColor = NULL;

	if( m_BarFlag & TYPE_MERGE_OVERLAPS )
		pColor = dwColorSet0;
	else
		pColor = dwColorSet1;

	for( i=0; i<iNumRegions; i++ )
	{
		Segment& highlight = m_vecPaintedRegion[i];
		left  = m_BoundingBox.left + highlight.left;
		right = m_BoundingBox.left + highlight.right;
		rect.SetPosition( D3DXVECTOR2( left, top+1 ), D3DXVECTOR2( right, bottom-1 ) );

		rect.SetColor( pColor[highlight.type] );

		rect.Draw();
	}

	if( m_bPressed )
	{
		left = m_BoundingBox.left + m_iStartPosition;
		right = m_BoundingBox.left + m_iCurrentPosition;
//		left  = m_BarRect.GetRect().left + m_iStartPosition;
//		right = m_BarRect.GetRect().left + m_iCurrentPosition;
		if( right < left )
			swap( left, right );

		rect.SetColor( 0xA020FF20 );
		rect.SetPosition( D3DXVECTOR2( left, top ), D3DXVECTOR2( right, bottom ) );
		rect.Draw();
	}

}
