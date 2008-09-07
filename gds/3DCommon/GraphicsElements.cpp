#include "GraphicsElements.h"
#include "GraphicsElementManager.h"
#include "3DCommon/Font.h"
#include "3DCommon/TextureFont.h"
#include "3DCommon/TrueTypeTextureFont.h"
#include "3DCommon/2DPolygon.h"
#include "Support/Macro.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Profile.h"

#include <algorithm>
using namespace std;


inline int get_num_rows( const string& str )
{
	size_t pos = 0;
	int num_rows = 1;
	while( pos < str.length() && (pos = str.find("\n",pos)) != string::npos )
	{
		num_rows++;
		pos++;
	}

	return num_rows;
}

template<class T>
inline void erase_dupulicate_elements( std::vector<T>& vec )
{
	for( std::vector<T>::iterator itr0 = vec.begin(); itr0 != vec.end(); itr0++ )
	{
//		std::vector<T>::iterator itr1 = itr0++; //< causes error in vector - "vector iterator not dereferencable". Why?
		std::vector<T>::iterator itr1 = itr0+1;
		while( itr1 != vec.end() )
		{
			if( (*itr0) == (*itr1) )
				itr1 = vec.erase( itr1 );
			else
				itr1++;

		}
	}
}


void CGraphicsElement::UpdateTopLeftPos( Vector2 vPos )
{
	Vector2 vSpan = m_AABB.vMax - m_AABB.vMin;
	m_AABB.vMin = vPos;
	m_AABB.vMax = vPos + vSpan;

	SetTopLeftPosInternal( vPos );
}


void CGraphicsElement::SetTopLeftPos( Vector2 vPos )
{
	UpdateTopLeftPos( vPos );

	// Check just in case
	if( 0 <= m_GroupID )
	{
		// Oh, boy... This is owned by a group element
		// - Not really desirable. SetLocalTopLeftPos() should be used instead.
		// - Anyway, need to update the local position as well
		CGE_Group *pOwner = dynamic_cast<CGE_Group *>(m_pManager->GetElement(m_GroupID));
		if( pOwner )
			m_vLocalTopLeftPos = vPos - pOwner->GetLocalOriginInGlobalCoord();
	}

}


void CGraphicsElement::SetLocalTopLeftPos( Vector2 vLocalPos )
{
	m_vLocalTopLeftPos = vLocalPos;

	if( 0 <= m_GroupID )
	{
		// owned by a group element
		// - update the global position from the current local origin of the group element
		CGE_Group *pOwner = dynamic_cast<CGE_Group *>(m_pManager->GetElement(m_GroupID));
		if( pOwner )
		{
			UpdateTopLeftPos( pOwner->GetLocalOriginInGlobalCoord() + m_vLocalTopLeftPos );
		}
	}
	else
	{
		// local pos == global pos
		SetTopLeftPos( m_vLocalTopLeftPos );
	}
}


void CGraphicsElement::SetLayer( int layer_index )
{
	// remove the element from the current layer
	// and place it to the new layer
	m_pManager->SetElementToLayer( this, layer_index );
}



//==========================================================================================
// CGE_Primitive
//==========================================================================================

void CGE_Primitive::DrawPrimitive()
{
	PROFILE_FUNCTION();

	if( 0 <= m_TextureID )
	{
		const CTextureHandle& tex = m_pManager->GetTexture(m_TextureID);
		m_pPrimitive->Draw( tex );
	}
	else
		m_pPrimitive->Draw();	// draw rect without a texture
}


void CGE_Primitive::SetTopLeftPosInternal( Vector2 vPos )
{
	// m_AABB has been updated in CGraphicsElement::SetTopLeftPos().
	// - update the vertex positions
	m_pPrimitive->SetPosition( m_AABB.vMin * m_fScale, m_AABB.vMax * m_fScale );
}



//==========================================================================================
// CGE_Rect
//==========================================================================================

CGE_Rect::CGE_Rect( /*const AABB2& non_scaled_aabb,*/ const SFloatRGBAColor& color0, C2DPrimitive *pPrimitive )
:
CGE_Primitive(pPrimitive)
{
//	m_AABB = AABB2( pPrimitive->GetPosition2D(0), pPrimitive->GetPosition2D(2) );
	m_aColor[0] = color0;
	ChangeScale( m_fScale );

	const int num_corners = 4;
	for( int i=0; i<num_corners; i++ )
		m_aCornerColor[i] = SFloatRGBAColor(1,1,1,1);
}


void CGE_Rect::Draw()
{
	PROFILE_FUNCTION();

	if( /* use_corner_colors == */ true
	 && m_pPrimitive->GetPrimitiveType() != C2DPrimitive::TYPE_ROUNDRECT )
	{
		// set separate colors to each corner
		// - not available for C2DRoundRect
		SFloatRGBAColor blended_color = GetBlendedColor();
		const int num_corners = 4;
		for( int i=0; i<num_corners; i++ )
		{
			m_pPrimitive->SetCornerColor( i, blended_color * m_aCornerColor[i] );
		}
	}
	else
		SetBlendedColorToPrimitive();

	DrawPrimitive();
}


//==========================================================================================
// CGE_Triangle
//==========================================================================================

CGE_Triangle::CGE_Triangle( const SFloatRGBAColor& color0, C2DPrimitive *pPrimitive, const SRect& non_scaled_rect )
:
CGE_Primitive(pPrimitive),
m_pTriangle(NULL),
m_pFTriangle(NULL),
m_RFpTriangle(NULL)
{
	m_AABB = AABB2( Vector2((float)non_scaled_rect.left,(float)non_scaled_rect.top), Vector2((float)non_scaled_rect.right,(float)non_scaled_rect.bottom) );

//	m_pTriangle   = dynamic_cast<C2DTriangle *>(pPrimitive),
//	m_pFTriangle  = dynamic_cast<C2DFrameTriangle *>(pPrimitive),
//	m_RFpTriangle = dynamic_cast<C2DRoundFrameTriangle *>(pPrimitive)

	m_aColor[0] = color0;
	ChangeScale( m_fScale );
}


void CGE_Triangle::Draw()
{
	SetBlendedColorToPrimitive();

	DrawPrimitive();
}



//==========================================================================================
// CGE_Polygon
//==========================================================================================

CGE_Polygon::CGE_Polygon( const SFloatRGBAColor& color0, C2DPrimitive *pPrimitive, const SRect& non_scaled_rect )
:
CGE_Primitive(pPrimitive)
{
	m_pRegularPolygon = dynamic_cast<C2DRegularPolygon *>(pPrimitive);
	m_aColor[0] = color0;

//	ChangeScale( m_fScale );
}


void CGE_Polygon::Draw()
{
	SetBlendedColorToPrimitive();

	m_pRegularPolygon->Draw();
}


//==========================================================================================
// CGE_Text
//==========================================================================================

/*
CGE_Text::CGE_Text( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color0 )
:
m_FontID(font_id),
m_Text(text),
m_vTextPos(Vector2(x,y))
{
	m_AABB.vMin = m_vTextPos;
	m_TextAlignH = TAL_LEFT;
	m_TextAlignV = TAL_TOP;
	m_aColor[0] = color0;
	ChangeScale( m_fScale );

//		m_AABB.vMax = 
}
*/

void CGE_Text::Draw()
{
	PROFILE_FUNCTION();

	CFontBase *pFont = m_pManager->GetFont(m_FontID);
	if( !pFont )
		return;

	int orig_font_w=0, orig_font_h=0;
	if( 0 < m_FontWidth && 0 < m_FontHeight )
	{
		// override the default font size with the element-specific font size
		pFont->GetFontSize( orig_font_w, orig_font_h );
//		pFont->SetFontSize( m_FontWidth, m_FontHeight );
		pFont->SetFontSize( m_ScaledWidth, m_ScaledHeight );
	}

	// set texture stage states
	// this is not necessary when the font is not a texture font
	SetRenderStatesForTextureFont( m_DestAlphaBlendMode );

	pFont->SetFontColor( GetBlendedColor().GetARGB32() );

	pFont->DrawText( m_Text, m_vScaledPos );

	// restore the original font size if the text has its own font size
	if( 0 < m_FontWidth && 0 < m_FontHeight )
		pFont->SetFontSize( orig_font_w, orig_font_h );
}


void CGE_Text::SetTextAlignment( int horizontal_alignment, int vertical_alignment )
{
	m_TextAlignH = horizontal_alignment;
	m_TextAlignV = vertical_alignment;

	UpdateTextAlignment();
}


void CGE_Text::UpdateTextAlignment()
{
	Vector2 vDiagonal = m_AABB.vMax - m_AABB.vMin;
	int box_width  = (int)vDiagonal.x;
	int box_height = (int)vDiagonal.y;

	CFontBase *pFont = m_pManager->GetFont( m_FontID );
	if( !pFont )
		return;

	int font_width  = 0 < m_FontWidth  ? m_FontWidth  : pFont->GetFontWidth();
	int font_height = 0 < m_FontHeight ? m_FontHeight : pFont->GetFontHeight();
	const int text_length = (int)m_Text.length();

	Vector2 vLocalTextOffset = Vector2(0,0);
	switch(m_TextAlignH)
	{
	case CGE_Text::TAL_LEFT:
		vLocalTextOffset.x = 0;
		break;
	case CGE_Text::TAL_CENTER:
		vLocalTextOffset.x = (float)( box_width/2  - pFont->GetTextWidth(m_Text.c_str())/2 );
		break;
	case CGE_Text::TAL_RIGHT:
		// --- NOT IMPLEMENTED ---
		break;
	default:
		break;
	}
	switch(m_TextAlignV)
	{
	case CGE_Text::TAL_TOP:
		vLocalTextOffset.y = 0;	// param.m_TextMargin
		break;
	case CGE_Text::TAL_CENTER:
//		param.m_vTextOffset.y = (float)(control_height - font_height) * 0.5f;
		vLocalTextOffset.y = (float)( box_height/2 - font_height*get_num_rows(m_Text)/2 );
		break;
	case CGE_Text::TAL_BOTTOM:
		// --- NOT IMPLEMENTED ---
		break;
	default:
		break;
	}

	m_vTextPos = vLocalTextOffset + m_AABB.vMin;

	m_vScaledPos = m_vTextPos * m_fScale;
}


//==================================================================================
// CGE_Group
//==================================================================================

inline void CGE_Group::RemoveInvalidElements()
{
	// remove NULL elements
	vector<CGraphicsElement *>::iterator itr = m_vecpElement.begin();
	while( itr != m_vecpElement.end() )
	{
		if( (*itr) == NULL )
			itr = m_vecpElement.erase( itr );
		else
			itr++;
	}

	// remove the same elements
	erase_dupulicate_elements( m_vecpElement );
}

/*
CGE_Group::CGE_Group( std::vector<CGraphicsElement *>& rvecpElement )
:
m_vecpElement(rvecpElement)
{
	RemoveInvalidElements();

	UpdateAABB();

	// use the top-left corner as the local origin
	SetLocalOrigin( GetTopLeftPos() );

	// set local top-left potitions for grouped elements
	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
		(*itr)->m_vLocalTopLeftPos = (*itr)->GetTopLeftPos() - GetTopLeftPos();
	}
}
*/


CGE_Group::CGE_Group( std::vector<CGraphicsElement *>& rvecpElement, Vector2 vLocalOrigin )
:
//m_vLocalOrigin(vLocalOrigin),
m_vecpElement(rvecpElement)
{
	m_vLocalTopLeftPos = vLocalOrigin;

	RemoveInvalidElements();

	UpdateAABB();

	// set local top-left potitions for grouped elements
/*	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
		(*itr)->m_vLocalTopLeftPos = (*itr)->GetTopLeftPos() - m_vLocalOrigin;
	}
	*/

	// update global positions of grouped elements
	SetLocalOrigin( vLocalOrigin );
}


CGE_Group::~CGE_Group()
{
	// release the elements from the group
	// - does not release the element itself from the graphics element manager
	// - they just get 'ungrouped'.
	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
		(*itr)->m_GroupID = -1;
	}
}


void CGE_Group::SetTopLeftPosInternal( Vector2 vGlobalPos )
{
/*	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
//		Vector2 vLocalTopLeftPos = (*itr)->GetTopLeftPos() - GetTopLeftPos();
//		(*itr)->SetTopLeftPos( vPos + vLocalTopLeftPos );
///		(*itr)->SetTopLeftPos( vPos + (*itr)->m_vLocalTopLeftPos );
		(*itr)->UpdateGlobalPositions( vGlobalPos + v );
	}*/
}


Vector2 CGE_Group::GetLocalOriginInGlobalCoord() const
{
	if( 0 <= m_GroupID )
	{
		CGE_Group *pParentGroup = dynamic_cast<CGE_Group *>(m_pManager->GetElement( m_GroupID ));
		if( pParentGroup )
			return pParentGroup->GetLocalOriginInGlobalCoord() + m_vLocalTopLeftPos;
		else
			return m_vLocalTopLeftPos;
	}
	else
	{
		// not owned by any group
		// - i.e.) m_vLocalOrigin is in global coord
		return m_vLocalTopLeftPos;
	}
}


void CGE_Group::UpdateGlobalPositions( Vector2 vLocalOrigin )
{
	SetLocalTopLeftPos( m_vLocalTopLeftPos );
}


void CGE_Group::SetLocalTopLeftPos( Vector2 vPos )
{
	m_vLocalTopLeftPos = vPos;
//	CGraphicsElement::SetLocalTopLeftPos( vPos );

	// update the global positions of all the child elements
	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
		(*itr)->UpdateGlobalPositions( GetLocalOriginInGlobalCoord() );
	}
}


void CGE_Group::ChangeScale( float scale )
{
//	int i, num_elements = m_vecElementID.size();
//	for( i=0; i<num_elements; i++ )

	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
		(*itr)->ChangeScale( scale );
}


void CGE_Group::ChangeElementScale( float scale )
{
}


void CGE_Group::SetSizeLTRB( const Vector2& vMin, const Vector2& vMax )
{
	m_AABB.vMin = vMin;
	m_AABB.vMax = vMax;

	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin();
		itr != m_vecpElement.end();
		itr++ )
	{
		(*itr)->SetSizeLTRB( vMin, vMax );
	}
}


void CGE_Group::SetColor( int color_index, const SFloatRGBAColor& color )
{
	CGraphicsElement::SetColor( color_index, color );

	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin();
		itr != m_vecpElement.end();
		itr++ )
	{
		(*itr)->SetColor( color_index, color );
	}
}


void CGE_Group::SetAlpha( int color_index, float a )
{
	CGraphicsElement::SetAlpha( color_index, a );

	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin();
		itr != m_vecpElement.end();
		itr++ )
	{
		(*itr)->SetAlpha( color_index, a );
	}

}


void CGE_Group::SetDestAlphaBlendMode( AlphaBlend::Mode mode )
{
	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin();
		itr != m_vecpElement.end();
		itr++ )
	{
		(*itr)->SetDestAlphaBlendMode( mode );
	}

}


void CGE_Group::Draw()
{
	// do nothing
	// - drawing calls are managed by CGraphicsElementManager
	// - CBE_Group is not intended to batch the draw calls
	//   - elements in a group might belong to different layers, and in such a case
	//     rendering order need to be managed the same way as non-grouped elements
}


