
#include "3DCommon/all.h"
#include "GraphicsElementManager.h"
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

CGE_Triangle::CGE_Triangle( /*const AABB2& non_scaled_aabb,*/ const SFloatRGBAColor& color0, C2DPrimitive *pPrimitive )
:
CGE_Primitive(pPrimitive),
m_pTriangle(NULL),
m_pFTriangle(NULL),
m_RFpTriangle(NULL)
{
/*	AABB2 aabb;
	aabb.Nullify();
	for( int i=0; i<3; i++ )
		aabb.AddPoint( pPrimitive->GetPosition2D(i) );

	m_AABB = aabb;*/
	// ^^^^^^^^^^^^^^ Wrong! m_AABB holds a non-scaled bounding box!!!

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
	SetRenderStatesForTextureFont( AlphaBlend::InvSrcAlpha );

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
		vLocalTextOffset.x = (float)( box_width/2  - (text_length * font_width)/2 );
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


void CGE_Group::Draw()
{
	// do nothing
	// - drawing calls are managed by CGraphicsElementManager
	// - CBE_Group is not intended to batch the draw calls
	//   - elements in a group might belong to different layers, and in such a case
	//     rendering order need to be managed the same way as non-grouped elements
}



//=====================================================================
// CGraphicsElementManager
//=====================================================================

CGraphicsElementManager::CGraphicsElementManager()
{
	m_vecpElement.resize( 256, NULL );

	m_fScale = 1.0f;

	m_bAutoScaling = true;

	if( m_bAutoScaling )
		m_fScale = GetScreenWidth() / (float)GetReferenceScreenWidth();


	m_NumMaxLayers = NUM_MAX_LAYERS;

	m_vecLayer.resize( m_NumMaxLayers );
}


CGraphicsElementManager::~CGraphicsElementManager()
{
	Release();

	ReleaseGraphicsResources();
}


void CGraphicsElementManager::Release()
{
	RemoveAllElements(); // This will SafeDelete() all the elements of m_vecpElement

	SafeDeleteVector( m_vecpFont );
}


void CGraphicsElementManager::ReleaseGraphicsResources()
{
	size_t i, num = m_vecpFont.size();
	for( i=0; i<num; i++ )
		m_vecpFont[i]->Release();
}


void CGraphicsElementManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	size_t i, num = m_vecpFont.size();
	for( i=0; i<num; i++ )
		m_vecpFont[i]->Reload();

	if( m_bAutoScaling )
	{
		m_fScale = GetScreenWidth() / (float)GetReferenceScreenWidth();
		UpdateElementScales();
	}
}


int CGraphicsElementManager::GetVacantSlotIndex()
{
	int i, num = (int)m_vecpElement.size();
	for( i=0; i<num; i++ )
	{
		if( !m_vecpElement[i] )
			return i;
	}

	int index = (int)m_vecpElement.size();

//	CGraphicsElemectBase *p = NULL;
//	m_vecpElement.push_back( p );
	m_vecpElement.push_back( NULL );

	return index;
}


void CGraphicsElementManager::UpdateElementScales()
{
	float scale = m_fScale;
	int i, num_elements = (int)m_vecpElement.size();
	for( i=0; i<num_elements; i++ )
	{
		if( m_vecpElement[i] )
			m_vecpElement[i]->ChangeScale( scale );
	}
}


bool CGraphicsElementManager::RegisterToLayer( int element_index, int layer_index )
{
	if( layer_index < 0 || m_NumMaxLayers <= layer_index )
		return false;

//	if( m_vecLayer.size() <= (size_t)layer_index )
//	{
//		int i, num_extra_layers = layer_index - (int)m_vecLayer.size() + 1;
//		for( i=0; i<num_extra_layers; i++ )
//			m_vecLayer.push_back( CGraphicsElementManager::CLayer() );
//	}

	m_vecLayer[layer_index].m_vecElement.push_back( element_index );

	return true;
}


void CGraphicsElementManager::InitElement( CGraphicsElement *pElement, int element_index, int layer_index )
{
	m_vecpElement[element_index]->SetGraphicsElementManager( this );
	m_vecpElement[element_index]->ChangeScale( m_fScale );
	m_vecpElement[element_index]->SetElementIndex( element_index );

	if( pElement->GetElementType() != CGraphicsElement::TYPE_GROUP )
		m_vecpElement[element_index]->SetLayerIndex( layer_index );
}


void CGraphicsElementManager::InitPrimitiveElement( CGE_Primitive *pElement,
												    C2DPrimitive *pPrimitive,
													const SRect& non_scaled_rect,
												    const SFloatRGBAColor& color,
													int element_index,
													int layer_index )
{
	float z = 0;

	AABB2 non_scaled_aabb;
	non_scaled_aabb.vMin.x = (float)non_scaled_rect.left;
	non_scaled_aabb.vMin.y = (float)non_scaled_rect.top;
	non_scaled_aabb.vMax.x = (float)non_scaled_rect.right;
	non_scaled_aabb.vMax.y = (float)non_scaled_rect.bottom;

	pPrimitive->SetTextureUV( TEXCOORD2( 0.0f, 0.0f ), TEXCOORD2( 1.0f, 1.0f ) );
	pPrimitive->SetZDepth( z );
	pPrimitive->SetColor( color );

	m_vecpElement[element_index] = pElement;

	InitElement( pElement, element_index, layer_index );

	pElement->SetSizeLTRB( non_scaled_aabb.vMin, non_scaled_aabb.vMax );
}


CGE_Rect *CGraphicsElementManager::InitRectElement( int element_index, int layer_index,
													const SRect& non_scaled_rect,
													const SFloatRGBAColor& color,
													C2DPrimitive *pRectPrimitive )
{

	CGE_Rect *pRectElement = new CGE_Rect( color, pRectPrimitive );

	InitPrimitiveElement( pRectElement, pRectPrimitive, non_scaled_rect, color, element_index, layer_index );

	return pRectElement;
}


CGE_Triangle *CGraphicsElementManager::InitTriangleElement( int element_index, int layer_index,
														    const SRect& non_scaled_rect,
															const SFloatRGBAColor& color,
													        C2DPrimitive *pTrianglePrimitive )
{
	CGE_Triangle *pTriangleElement = new CGE_Triangle( color, pTrianglePrimitive );

	InitPrimitiveElement( pTriangleElement, pTrianglePrimitive, non_scaled_rect, color, element_index, layer_index );

	return pTriangleElement;
}


CGE_Rect *CGraphicsElementManager::CreateRect( const SRect& rect, const SFloatRGBAColor& color, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	C2DRect *p2DRect = new C2DRect( rect * m_fScale, color.GetARGB32() );

	return InitRectElement( index, layer, rect, color, p2DRect );
}


CGE_Rect *CGraphicsElementManager::CreateFrameRect( const SRect& rect, const SFloatRGBAColor& color, float border_width, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	C2DFrameRect *p2DFrameRect = new C2DFrameRect( rect * m_fScale, color.GetARGB32(), border_width );

	return InitRectElement( index, layer, rect, color, p2DFrameRect );
}


CGE_Rect *CGraphicsElementManager::CreateRoundRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	C2DRoundRect *p2DRoundRect = new C2DRoundRect( rect * m_fScale, color.GetARGB32(), (int)corner_radius );

	return InitRectElement( index, layer, rect, color, p2DRoundRect );
}


CGE_Rect *CGraphicsElementManager::CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, float border_width, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	C2DRoundFrameRect *p2DRoundFrameRect = new C2DRoundFrameRect( rect * m_fScale, color.GetARGB32(), (int)corner_radius, (int)border_width );

	return InitRectElement( index, layer, rect, color, p2DRoundFrameRect );
}


CGE_Triangle *CGraphicsElementManager::CreateTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& color, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	C2DTriangle *p2DTriangle = new C2DTriangle( dir, rect * m_fScale, color.GetARGB32() );

	return InitTriangleElement( index, layer, rect, color, p2DTriangle );
}


CGE_Triangle *CGraphicsElementManager::CreateFrameTriangle( const SRect& rect, const SFloatRGBAColor& color, float border_width, int layer )
{
	LOG_PRINT_ERROR( "Not implemented yet!" );
	return NULL;
}


CGE_Text *CGraphicsElementManager::CreateText( int font_id, const string& text, float x, float y,
										       const SFloatRGBAColor& color, int font_w, int font_h, int layer )
{
	CFontBase *pFont = this->GetFont(font_id);
	if( pFont )
	{
		Vector2 non_scaled_size = m_vecOrigFontSize[font_id];
		if( font_w <= 0 ) font_w = (int)non_scaled_size.x;// pFont->GetFontWidth();
		if( font_h <= 0 ) font_h = (int)non_scaled_size.y;// pFont->GetFontHeight();
	}

	SRect rect;
	rect.left   = (int)x;
	rect.top    = (int)y;
	rect.right  = (int)x + font_w * (int)text.length();
	rect.bottom = (int)y + font_h;
	return CreateTextBox(
		font_id,
		text,
		rect,
		CGE_Text::TAL_LEFT,
		CGE_Text::TAL_TOP,
		color,
		font_w,
		font_h,
		layer );
/*	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	CGE_Text *pTextElement = new CGE_Text( font_id, text, x, y, color );
	pTextElement->SetFontSize( font_w, font_h );
	m_vecpElement[index] = pTextElement;

	InitElement( pTextElement, index, layer );

	return pTextElement;
*/
}


CGE_Text *CGraphicsElementManager::CreateTextBox( int font_id, const std::string& text,
												  const SRect& textbox, int align_h, int align_v,
												  const SFloatRGBAColor& color, int font_w, int font_h, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	AABB2 aabb;
	aabb.vMin = Vector2( (float)textbox.left,  (float)textbox.top );
	aabb.vMax = Vector2( (float)textbox.right, (float)textbox.bottom );
	CGE_Text *pTextElement = new CGE_Text( font_id, text, aabb, align_h, align_v, color );
	pTextElement->SetFontSize( font_w, font_h );
	m_vecpElement[index] = pTextElement;

	InitElement( pTextElement, index, layer );

	pTextElement->UpdateTextAlignment();

	// need to scale text element?

	return pTextElement;
}


CGraphicsElement *CGraphicsElementManager::CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer )
{
//	if( !RegisterToLayer( index, layer ) )
//		return -1;

	return NULL;
}


CGraphicsElement *CGraphicsElementManager::CreatePolygon()
{
	return NULL;
}


CGE_Group *CGraphicsElementManager::CreateGroup( CGraphicsElement** apElement, int num_elements, const SPoint& local_origin )
{
	vector<CGraphicsElement *> vecpElement;
	vecpElement.resize(num_elements);
	for( int i=0; i<num_elements; i++ )
	{
		vecpElement[i] = apElement[i];
	}

	return CreateGroup( vecpElement, local_origin );
}


CGE_Group *CGraphicsElementManager::CreateGroup( std::vector<CGraphicsElement *>& rvecpElement, const SPoint& local_origin )
{
	int index = GetVacantSlotIndex();

	CGE_Group *pGroupElement = new CGE_Group( rvecpElement, Vector2((float)local_origin.x,(float)local_origin.y) );
	m_vecpElement[index] = pGroupElement;

	// do not register to layer
	// - group element does not belong to layer
	m_vecpElement[index]->SetGraphicsElementManager( this );
	m_vecpElement[index]->ChangeScale( m_fScale );
	m_vecpElement[index]->SetElementIndex( index );

	// TODO: set group id to each grouped element
	size_t i, num_grouped_elements = rvecpElement.size();
	for( i=0; i<num_grouped_elements; i++ )
		rvecpElement[i]->SetGroupID( index );

	return pGroupElement;
}


bool CGraphicsElementManager::LoadTexture( int texture_id, const std::string& tex_filename )
{
	if( (int)m_vecTexHandle.size() <= texture_id )
	{
		for( int i=0; i<texture_id - (int)m_vecTexHandle.size() + 1; i++ )
			m_vecTexHandle.push_back( CTextureHandle() );
	}

	m_vecTexHandle[texture_id].filename = tex_filename;

	return m_vecTexHandle[texture_id].Load();
}


int CGraphicsElementManager::LoadTexture( const std::string& tex_filename )
{
	m_vecTexHandle.push_back( CTextureHandle() );
	m_vecTexHandle.back().filename = tex_filename;

	bool res = m_vecTexHandle.back().Load();

	if( res )
		return (int)(m_vecTexHandle.size() - 1);
	else
	{
		m_vecTexHandle.back().Release();	// need to manually release since pop_back() of FixedVector does not delete the object
		m_vecTexHandle.pop_back();
		LOG_PRINT_WARNING( " - Cannot load a texture: " + tex_filename );
        return -1;
	}
}


bool CGraphicsElementManager::LoadFont( int font_id, const string& font_name, int font_type, int w, int h, float bold, float italic )
{
	CFont *pFont = NULL;
	CTextureFont *pTexFont = NULL;
	int w_scaled = (int)(w * m_fScale);
	int h_scaled = (int)(h * m_fScale);

	if( (int)m_vecpFont.size() <= font_id )
	{
		for( size_t i=0; i<font_id - m_vecpFont.size() + 1; i++ )
		{
			m_vecpFont.push_back( NULL );
			m_vecOrigFontSize.push_back( Vector2(0,0) );
		}
	}

	SafeDelete( m_vecpFont[font_id] );

	m_vecOrigFontSize[font_id] = Vector2( (float)w, (float)h );

	switch( font_type )
	{
	case CFontBase::FONTTYPE_NORMAL:
		pFont = new CFont();
		pFont->InitFont( font_name, w_scaled, h_scaled );
		m_vecpFont[font_id] = pFont;
		break;

	case CFontBase::FONTTYPE_TEXTURE:
		pTexFont = new CTextureFont();
		pTexFont->InitFont( font_name, w_scaled, h_scaled );
		pTexFont->SetItalic( italic );
		m_vecpFont[font_id] = pTexFont;
		break;

	default:
		return false;
	}

	return true;
}


int CGraphicsElementManager::LoadTextureFont( const string& font_texture_filename,
											  int width, int height,
											  float bold, float italic )
{
	LOG_PRINT( " - Loading a texture for font: " + font_texture_filename );

	CTextureFont* pFont = new CTextureFont();
	bool res = pFont->InitFont( font_texture_filename, (int)(width * m_fScale), (int)(height * m_fScale) );
	if( res )
	{
		// save the original size of the font
		// used when the scaling is requested
		m_vecOrigFontSize.push_back( Vector2( (float)width, (float)height ) );

//		pFont->SetBold( bold );
		pFont->SetItalic( italic );

		m_vecpFont.push_back( pFont );
		return (int)m_vecpFont.size() - 1;
	}
	else
	{
		LOG_PRINT_ERROR( " - Cannot load a texture for font: " + font_texture_filename );
		SafeDelete( pFont );
		return -1;
	}
}


bool CGraphicsElementManager::RemoveElement( CGE_Group*& pGroupElement )
{
	CGraphicsElement *pElement = pGroupElement;
	pGroupElement = NULL;
	return RemoveElement( pElement );
}


bool CGraphicsElementManager::RemoveElement( CGraphicsElement*& pElement )
{
	if( !pElement )
		return false;

	int element_id = pElement->GetElementIndex();

	// callback triggered by element removal
	// - used by effect manager to remove effect which is currently being applied to removed element 'pElement'
	if( m_pCallbackPtr.get() )
		m_pCallbackPtr->OnDestroyed( pElement );

	if( pElement->GetElementType() != CGraphicsElement::TYPE_GROUP )
	{
		// remove the element from the layer
		int layer_index = pElement->GetLayerIndex();
		CLayer& rLayer = m_vecLayer[layer_index];
//		rLayer.m_vecElement

        vector<int>::iterator itrTarget = find( rLayer.m_vecElement.begin(), rLayer.m_vecElement.end(), layer_index );

		if( itrTarget != rLayer.m_vecElement.end() )
		{
			rLayer.m_vecElement.erase( itrTarget );
		}
	}

	// remove the element from the group to which it currently belongs
	int group_id = pElement->GetGroupID();
	CGE_Group *pGroup = NULL;
	if( 0 <= group_id
	 && GetElement(group_id)->GetElementType() == CGraphicsElement::TYPE_GROUP
	 && ( pGroup = dynamic_cast<CGE_Group *>(GetElement(group_id)) ) )
	{
		pGroup->RemoveElementFromGroup( pElement );
	}

	SAFE_DELETE( m_vecpElement[element_id] );

	pElement = NULL;

	return true;
}


bool CGraphicsElementManager::RemoveAllElements()
{
	size_t i, num = m_vecpElement.size();
	for( i=0; i<num; i++ )
	{
		RemoveElement( m_vecpElement[i] );
	}

	return true;
}


// remove an element from its current layer
// - does not release the element
void CGraphicsElementManager::RemoveFromLayer( CGraphicsElement *pElement )
{
	int layer_index = pElement->GetLayerIndex();
	if( layer_index < 0 || (int)m_vecLayer.size() <= layer_index )
		return;

	CLayer& current_layer = m_vecLayer[layer_index];

	bool res = current_layer.RemoveElementFromLayer( pElement );

	if( res )
		pElement->SetLayerIndex( -1 );
}


void CGraphicsElementManager::SetElementToLayer( CGraphicsElement *pElement, int layer_index )
{
	if( layer_index < 0 || NUM_MAX_LAYERS <= layer_index )
		return;

	if( (int)m_vecLayer.size() <= layer_index )
	{
		// add layer(s)
		// - Note that the previous check 'NUM_MAX_LAYERS <= layer_index'
		//   guarantees that size of 'm_vecLayer' does not excceed NUM_MAX_LAYERS
		while( (int)m_vecLayer.size() <= layer_index )
			m_vecLayer.push_back( CLayer() );
	}

	RemoveFromLayer( pElement );

	pElement->SetLayerIndex( layer_index );

	m_vecLayer[layer_index].m_vecElement.push_back( pElement->GetElementIndex() );
}


void CGraphicsElementManager::Render()
{
	int i, num_layers = (int)m_vecLayer.size();
	int j, num_elements;
	for( i=num_layers-1; 0<=i; i-- )
	{
		num_elements = (int)m_vecLayer[i].m_vecElement.size();
		for( j=0; j<num_elements; j++ )
		{
			GetElement( m_vecLayer[i].m_vecElement[j] )->Draw();
		}
	}

/*	size_t i, num = m_vecpElement.size();
	for( i=0; i<num; i++ )
	{
		if( m_vecpElement[i] )
			m_vecpElement[i]->Draw();
	}*/
}


void CGraphicsElementManager::SetScale( float scale )
{
	m_fScale = scale;

	size_t i, num = m_vecpElement.size();
	for( i=0; i<num; i++ )
	{
		if( m_vecpElement[i] )
			m_vecpElement[i]->ChangeScale( scale );
	}

	size_t num_fonts = m_vecpFont.size();
	for( i=0; i<num_fonts; i++ )
	{
		const Vector2 font_size = m_vecOrigFontSize[i] * scale;

		if( m_vecpFont[i] )
			m_vecpFont[i]->SetFontSize( (int)font_size.x, (int)font_size.y );
	}
}
