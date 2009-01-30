#include "Graphics/GraphicsElementManager.hpp"
#include "Graphics/Font/Font.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Graphics/Font/TrueTypeTextureFont.hpp"
#include "Graphics/2DPrimitive/2DPolygon.hpp"
#include "GraphicsElementManager.hpp"
#include "Support/Macro.h"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"

#include <algorithm>
using namespace std;


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

	// set as local coord
	// the element is owned by a group: local coord
	// the element is not owned by any group: global coord
	pElement->SetLocalTopLeftPos( non_scaled_rect.GetTopLeftCorner() );

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
	CGE_Triangle *pTriangleElement = new CGE_Triangle( color, pTrianglePrimitive, non_scaled_rect );

	pTriangleElement->m_AABB = AABB2( Vector2((float)non_scaled_rect.left,(float)non_scaled_rect.top), Vector2((float)non_scaled_rect.right,(float)non_scaled_rect.bottom) );

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

	C2DFrameRect *p2DFrameRect = new C2DFrameRect( rect * m_fScale, color.GetARGB32(), border_width * m_fScale );

	return InitRectElement( index, layer, rect, color, p2DFrameRect );
}


CGE_Rect *CGraphicsElementManager::CreateRoundRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	C2DRoundRect *p2DRoundRect = new C2DRoundRect( rect * m_fScale, color.GetARGB32(), (int)(corner_radius * m_fScale) );

	return InitRectElement( index, layer, rect, color, p2DRoundRect );
}


CGE_Rect *CGraphicsElementManager::CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, float border_width, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	C2DRoundFrameRect *p2DRoundFrameRect = new C2DRoundFrameRect( rect * m_fScale, color.GetARGB32(), (int)(corner_radius * m_fScale), (int)(border_width * m_fScale) );

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


CGE_Polygon *CGraphicsElementManager::CreateRegularPolygon( int num_polygon_vertices, int x, int y, int radius, CRegularPolygonStyle::Name style, const SFloatRGBAColor& color, int layer )
{
	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	C2DRegularPolygon *p2DRegularPolygon = new C2DRegularPolygon();
	p2DRegularPolygon->MakeRegularPolygon( num_polygon_vertices, Vector2((float)x, (float)y) * m_fScale, (int)(radius * m_fScale), style );

	SRect non_scaled_rect = RectLTRB( x - radius, y - radius, x + radius, y + radius );
	CGE_Polygon *pPolygonElement = new CGE_Polygon( color, p2DRegularPolygon, non_scaled_rect );

	InitPrimitiveElement( pPolygonElement, p2DRegularPolygon, non_scaled_rect, color, index, layer );

	return pPolygonElement;
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

	// manager must be set to the element before calling SetLocalTopLeftPos()
	// because it calls UpdateTextAlignment(), which calls CGraphicsElementManager::GetFont()
	pTextElement->SetLocalTopLeftPos( textbox.GetTopLeftCorner() );

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

	// need to call this after group id is set to each element
	pGroupElement->SetLocalOrigin( local_origin );

	return pGroupElement;
}


bool CGraphicsElementManager::LoadTexture( int texture_id, const std::string& tex_filename )
{
	const int required_size = texture_id + 1;
	const int shortage = required_size - (int)m_vecTexHandle.size();
	for( int i=0; i<shortage; i++ )
		m_vecTexHandle.push_back( CTextureHandle() );

	return m_vecTexHandle[texture_id].Load( tex_filename );
}


int CGraphicsElementManager::LoadTexture( const std::string& tex_filename )
{
	m_vecTexHandle.push_back( CTextureHandle() );

	bool res = m_vecTexHandle.back().Load( tex_filename );

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
	CTrueTypeTextureFont *pTTFont = NULL;
	int w_scaled = (int)(w * m_fScale);
	int h_scaled = (int)(h * m_fScale);
	const int ttf_resolution = 64;

	if( (int)m_vecpFont.size() <= font_id )
	{
		size_t num_to_append = font_id + 1 - m_vecpFont.size();
		m_vecpFont.insert(        m_vecpFont.end(),        num_to_append, NULL );
		m_vecOrigFontSize.insert( m_vecOrigFontSize.end(), num_to_append, Vector2(0,0) );
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

	case CFontBase::FONTTYPE_TRUETYPETEXTURE:
		pTTFont = new CTrueTypeTextureFont();
		pTTFont->InitFont( font_name, ttf_resolution, w_scaled, h_scaled );
		pTTFont->SetItalic( italic );
		m_vecpFont[font_id] = pTTFont;
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
