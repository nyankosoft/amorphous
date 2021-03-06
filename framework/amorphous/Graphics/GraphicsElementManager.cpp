#include "GraphicsElementManager.hpp"
#include "Font/TextureFont.hpp"
#include "Font/FontFactory.hpp"
#include "2DPrimitive/2DPolygon.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/Profile.hpp"


namespace amorphous
{

using namespace std;


//=====================================================================
// GraphicsElementManager
//=====================================================================


GraphicsElementManager::GraphicsElementManager()
{
	m_vecpElement.resize( 256, shared_ptr<GraphicsElement>() );

	m_fScale = 1.0f;

	m_bAutoScaling = true;

	if( m_bAutoScaling )
		m_fScale = GetScreenWidth() / (float)GetReferenceScreenWidth();


	m_NumMaxLayers = NUM_MAX_LAYERS;

	m_vecLayer.resize( m_NumMaxLayers );
}


GraphicsElementManager::~GraphicsElementManager()
{
	Release();

	ReleaseGraphicsResources();
}


void GraphicsElementManager::Release()
{
	RemoveAllElements(); // This will SafeDelete() all the elements of m_vecpElement

	SafeDeleteVector( m_vecpFont );
}


void GraphicsElementManager::ReleaseGraphicsResources()
{
	size_t i, num = m_vecpFont.size();
	for( i=0; i<num; i++ )
		m_vecpFont[i]->Release();
}


void GraphicsElementManager::LoadGraphicsResources( const GraphicsParameters& rParam )
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


int GraphicsElementManager::GetVacantSlotIndex()
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
	m_vecpElement.push_back( shared_ptr<GraphicsElement>() );

	return index;
}


void GraphicsElementManager::UpdateElementScales()
{
	float scale = m_fScale;
	int i, num_elements = (int)m_vecpElement.size();
	for( i=0; i<num_elements; i++ )
	{
		if( m_vecpElement[i] )
			m_vecpElement[i]->ChangeScale( scale );
	}
}


bool GraphicsElementManager::RegisterToLayer( int element_index, int layer_index )
{
	if( layer_index < 0 || m_NumMaxLayers <= layer_index )
		return false;

//	if( m_vecLayer.size() <= (size_t)layer_index )
//	{
//		int i, num_extra_layers = layer_index - (int)m_vecLayer.size() + 1;
//		for( i=0; i<num_extra_layers; i++ )
//			m_vecLayer.push_back( GraphicsElementManager::Layer() );
//	}

	m_vecLayer[layer_index].m_vecElement.push_back( element_index );

	return true;
}


bool GraphicsElementManager::InitElement( shared_ptr<GraphicsElement> pElement, int layer_index )
{
	int element_index = GetVacantSlotIndex();

	m_vecpElement[element_index] = pElement;

	m_vecpElement[element_index]->SetGraphicsElementManager( this );
	m_vecpElement[element_index]->ChangeScale( m_fScale );
	m_vecpElement[element_index]->SetElementIndex( element_index );

	pElement->m_pSelf = pElement;

//	if( pElement->GetElementType() != GraphicsElement::TYPE_GROUP )
	if( pElement->BelongsToLayer() )
	{
		// GraphicsElementGroup and CombinedPrimitiveElement do not belong to layer

		bool res = RegisterToLayer( element_index, layer_index );
		if( !res )
			return false;

		m_vecpElement[element_index]->SetLayerIndex( layer_index );
	}

	return true;
}


bool GraphicsElementManager::InitPrimitiveElement( shared_ptr<PrimitiveElement> pElement,
													const SRect& non_scaled_rect,
												    const SFloatRGBAColor& color,
													int layer_index )
{
	bool res = InitElement( pElement, layer_index );
	if( !res )
		return false;

	pElement->SetColor( 0, color );

	AABB2 non_scaled_aabb;
	non_scaled_aabb.vMin.x = (float)non_scaled_rect.left;
	non_scaled_aabb.vMin.y = (float)non_scaled_rect.top;
	non_scaled_aabb.vMax.x = (float)non_scaled_rect.right;
	non_scaled_aabb.vMax.y = (float)non_scaled_rect.bottom;

	// set as local coord
	// the element is owned by a group: local coord
	// the element is not owned by any group: global coord
	pElement->SetLocalTopLeftPos( non_scaled_rect.GetTopLeftCorner() );

	pElement->SetSizeLTRB( non_scaled_aabb.vMin, non_scaled_aabb.vMax );

	return true;
}

/*
shared_ptr<RectElement> GraphicsElementManager::InitRectElement( int element_index, int layer_index,
													const SRect& non_scaled_rect,
													const SFloatRGBAColor& color,
													C2DPrimitive *pRectPrimitive )
{

	shared_ptr<RectElement> pRectElement( new RectElement( color, pRectPrimitive ) );

	bool res = InitPrimitiveElement( pRectElement, pRectPrimitive, non_scaled_rect, color, element_index, layer_index );

	return res ? pRectElement : shared_ptr<RectElement>();
}


shared_ptr<RectElement> GraphicsElementManager::InitRoundRectElement( int element_index, int layer_index,
													const SRect& non_scaled_rect,
													const SFloatRGBAColor& color,
													C2DPrimitive *pRectPrimitive )
{
	shared_ptr<RoundRectElement> pRectElement( new RoundRectElement( color, pRectPrimitive ) );

	InitPrimitiveElement( pRectElement, pRectPrimitive, non_scaled_rect, color, element_index, layer_index );

	return pRectElement;
}


shared_ptr<TriangleElement> GraphicsElementManager::InitTriangleElement( int element_index, int layer_index,
														    const SRect& non_scaled_rect,
															const SFloatRGBAColor& color,
													        C2DPrimitive *pTrianglePrimitive )
{
	shared_ptr<TriangleElement> pTriangleElement( new TriangleElement( color, pTrianglePrimitive, non_scaled_rect ) );

	pTriangleElement->m_LocalAABB = AABB2( Vector2((float)non_scaled_rect.left,(float)non_scaled_rect.top), Vector2((float)non_scaled_rect.right,(float)non_scaled_rect.bottom) );

	InitPrimitiveElement( pTriangleElement, pTrianglePrimitive, non_scaled_rect, color, element_index, layer_index );

	return pTriangleElement;
}
*/

shared_ptr<CombinedRectElement> GraphicsElementManager::CreateRect( const SRect& rect,
															  const SFloatRGBAColor& fill_color_0,
															  const SFloatRGBAColor& frame_color_0,
															  float frame_width,
															  int layer )
{
/*	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return shared_ptr<RectElement>();
*/

	shared_ptr<FillRectElement> pFill = CreateFillRect( rect, fill_color_0, layer );
	shared_ptr<FrameRectElement> pFrame = CreateFrameRect( rect, frame_color_0, frame_width, layer );

//	C2DRect *p2DRect = new C2DRect( rect * m_fScale, fill_color_0.GetARGB32() );
//	C2DFrameRect *p2DFrameRect = new C2DFrameRect( rect * m_fScale, frame_color_0.GetARGB32() );

//	shared_ptr<RectElement> pRectElement( new RectElement( color, pRectPrimitive ) );
//	shared_ptr<RectElement> pRectElement( new RectElement( non_scaled_rect, m_fScale ) );
	shared_ptr<CombinedRectElement> pRectElement( new CombinedRectElement( rect, m_fScale, pFill, pFrame ) );

	bool res = InitElement( pRectElement, 0 );

//	bool res = InitPrimitiveElement( pRectElement, layer_index );

	return res ? pRectElement : shared_ptr<CombinedRectElement>();
}


shared_ptr<FillRectElement> GraphicsElementManager::CreateFillRect( const SRect& rect, const SFloatRGBAColor& color, int layer )
{
	shared_ptr<FillRectElement> pFillRectElement( new FillRectElement( rect, m_fScale ) );

	bool res = InitPrimitiveElement( pFillRectElement, rect, color, layer );

	return res ? pFillRectElement : shared_ptr<FillRectElement>();
}


shared_ptr<FrameRectElement> GraphicsElementManager::CreateFrameRect( const SRect& rect, const SFloatRGBAColor& color, float frame_width, int layer )
{
	shared_ptr<FrameRectElement> pFrameRectElement( new FrameRectElement( rect, m_fScale ) );

	bool res = InitPrimitiveElement( pFrameRectElement, rect, color, layer );

	pFrameRectElement->SetFrameWidth( (int)frame_width );

	return res ? pFrameRectElement : shared_ptr<FrameRectElement>();
}


shared_ptr<CombinedRoundRectElement> GraphicsElementManager::CreateRoundRect( const SRect& rect,
															  const SFloatRGBAColor& fill_color_0,
															  const SFloatRGBAColor& frame_color_0,
															  float corner_radius,
															  float frame_width,
															  int layer )
{
/*	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return shared_ptr<RectElement>();*/

//	C2DRect *p2DRect = new C2DRect( rect * m_fScale, fill_color_0.GetARGB32() );
//	C2DFrameRect *p2DFrameRect = new C2DFrameRect( rect * m_fScale, frame_color_0.GetARGB32() );

	shared_ptr<RoundFillRectElement> pFill = CreateRoundFillRect( rect, fill_color_0, corner_radius, layer );
	shared_ptr<RoundFrameRectElement> pFrame = CreateRoundFrameRect( rect, frame_color_0, corner_radius, frame_width, layer );

	shared_ptr<CombinedRoundRectElement> pRoundRectElement( new CombinedRoundRectElement( rect, m_fScale, pFill, pFrame ) );

	bool res = InitElement( pRoundRectElement, layer );

//	InitPrimitiveElement( pRoundRectElement, rect, color, layer );

	return res ? pRoundRectElement : shared_ptr<CombinedRoundRectElement>();
}


std::shared_ptr<RoundFillRectElement> GraphicsElementManager::CreateRoundFillRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, float corner_radius, int layer )
{
	shared_ptr<RoundFillRectElement> pRoundFillRectElement( new RoundFillRectElement( rect, m_fScale, corner_radius ) );

	bool res = InitPrimitiveElement( pRoundFillRectElement, rect, fill_color_0, layer );

	return res ? pRoundFillRectElement : shared_ptr<RoundFillRectElement>();
}


shared_ptr<RoundFrameRectElement> GraphicsElementManager::CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, float frame_width, int layer )
{
/*	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return shared_ptr<RectElement>();*/

	shared_ptr<RoundFrameRectElement> pRoundFrameRectElement( new RoundFrameRectElement( rect, m_fScale, corner_radius ) );

	pRoundFrameRectElement->SetCornerRadius( corner_radius );
	pRoundFrameRectElement->SetFrameWidth( (int)frame_width );

	bool res = InitPrimitiveElement( pRoundFrameRectElement, rect, color, layer );

	return res ? pRoundFrameRectElement : shared_ptr<RoundFrameRectElement>();

//	return InitRectElement( index, layer, rect, color, p2DRoundFrameRect );
}


//shared_ptr<TriangleElement> GraphicsElementManager::CreateTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& color, int layer )
shared_ptr<CombinedTriangleElement> GraphicsElementManager::CreateTriangle( C2DTriangle::Direction dir,
																	 const SRect& rect,
																	 const SFloatRGBAColor& fill_color_0,
																	 const SFloatRGBAColor& frame_color_0,
																	 float frame_width,
																	 int layer )
{
/*	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return shared_ptr<TriangleElement>();*/

	shared_ptr<FillTriangleElement> pFill = CreateFillTriangle( dir, rect, fill_color_0, layer );
	shared_ptr<FrameTriangleElement> pFrame = CreateFrameTriangle( dir, rect, frame_color_0, frame_width, layer );

	shared_ptr<CombinedTriangleElement> pTriangleElement( new CombinedTriangleElement( rect, m_fScale, pFill, pFrame ) );

//	pTriangleElement->m_LocalAABB = AABB2( Vector2((float)non_scaled_rect.left,(float)non_scaled_rect.top), Vector2((float)non_scaled_rect.right,(float)non_scaled_rect.bottom) );

//	InitPrimitiveElement( pTriangleElement, non_scaled_rect, color, element_index, layer );
	bool res = InitElement( pTriangleElement, layer );

	return res ? pTriangleElement : shared_ptr<CombinedTriangleElement>();
}


shared_ptr<FillTriangleElement> GraphicsElementManager::CreateFillTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& color, int layer )
{
	shared_ptr<FillTriangleElement> pFillTriangleElement( new FillTriangleElement( dir, rect, m_fScale ) );
	bool res = InitPrimitiveElement( pFillTriangleElement, rect, color, layer );

	return res ? pFillTriangleElement : shared_ptr<FillTriangleElement>();
/*
	return CreateTriangle(
		dir,
		rect,
		color,
		SFloatRGBAColor::White(),
		0, // frame width 0 means no frame
		layer );*/
}


shared_ptr<FrameTriangleElement> GraphicsElementManager::CreateFrameTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& color, float frame_width, int layer )
{
	return shared_ptr<FrameTriangleElement>();
/*
	return CreateTriangle(
		dir,
		rect,
		SFloatRGBAColor::White(),
		color,
		frame_width,
		layer );*/
}


shared_ptr<FillPolygonElement> GraphicsElementManager::CreateRegularPolygon( int num_polygon_vertices, int x, int y, int radius, CRegularPolygonStyle::Name style, const SFloatRGBAColor& color, int layer )
{
/*	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return shared_ptr<PolygonElement>();
*/
	SRect non_scaled_rect = RectLTRB( x - radius, y - radius, x + radius, y + radius );
//	shared_ptr<PolygonElement> pPolygonElement = new FillPolygonElement( color, non_scaled_rect );
	shared_ptr<FillPolygonElement> pPolygonElement( new FillPolygonElement( num_polygon_vertices, Vector2((float)x,(float)y), radius, style, m_fScale ) );

	InitPrimitiveElement( pPolygonElement, non_scaled_rect, color, layer );

	return pPolygonElement;
}


shared_ptr<TextElement> GraphicsElementManager::CreateText( int font_id, const string& text, float x, float y,
										       const SFloatRGBAColor& color, int font_w, int font_h, int layer )
{
	FontBase *pFont = this->GetFont(font_id);
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
	return CreateText(
		font_id,
		text,
		rect,
		TextElement::TAL_LEFT,
		TextElement::TAL_TOP,
		color,
		font_w,
		font_h,
		layer );

/*	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return NULL;

	shared_ptr<TextElement>pTextElement = new TextElement( font_id, text, x, y, color );
	pTextElement->SetFontSize( font_w, font_h );
	m_vecpElement[index] = pTextElement;

	InitElement( pTextElement, index, layer );

	return pTextElement;
*/
}


shared_ptr<TextElement> GraphicsElementManager::CreateText( int font_id, const std::string& text,
												  const SRect& textbox, int align_h, int align_v,
												  const SFloatRGBAColor& color, int font_w, int font_h, int layer )
{
/*	int index = GetVacantSlotIndex();

	if( !RegisterToLayer( index, layer ) )
		return shared_ptr<TextElement>();
*/
	AABB2 aabb;
	aabb.vMin = Vector2( (float)textbox.left,  (float)textbox.top );
	aabb.vMax = Vector2( (float)textbox.right, (float)textbox.bottom );
	shared_ptr<TextElement> pTextElement( new TextElement( font_id, text, aabb, align_h, align_v, color ) );
	pTextElement->SetFontSize( font_w, font_h );

	bool res = InitElement( pTextElement, layer );
	if( !res )
		return shared_ptr<TextElement>();

	// manager must be set to the element before calling SetLocalTopLeftPos()
	// because it calls UpdateTextAlignment(), which calls GraphicsElementManager::GetFont()
	pTextElement->SetLocalTopLeftPos( textbox.GetTopLeftCorner() );

	pTextElement->UpdateTextAlignment();

	// need to scale text element?

	return pTextElement;
}


shared_ptr<GraphicsElement> GraphicsElementManager::CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer )
{
//	if( !RegisterToLayer( index, layer ) )
//		return -1;

	return shared_ptr<GraphicsElement>();
}


shared_ptr<FillTriangleElement> GraphicsElementManager::CreateFillTriangle(
	Vector2 v0, Vector2 v1, Vector2 v2,
	const SFloatRGBAColor& color0,
	const SFloatRGBAColor& color1,
	const SFloatRGBAColor& color2,
	int layer,
	bool store_colors_for_each_vertex
	)
{
	AABB2 aabb;
	aabb.Nullify();
	aabb.AddPoint( v0 );
	aabb.AddPoint( v1 );
	aabb.AddPoint( v2 );

	SRect rect;
	rect.left   = (int)aabb.vMin.x;
	rect.top    = (int)aabb.vMin.y;
	rect.right  = (int)aabb.vMax.x;
	rect.bottom = (int)aabb.vMax.y;

	shared_ptr<FillTriangleElement> pFillTriangleElement( new FillTriangleElement( v0, v1, v2, rect, m_fScale ) );

	bool res = InitPrimitiveElement( pFillTriangleElement, rect, SFloatRGBAColor::White(), layer );

	if( !res )
		return shared_ptr<FillTriangleElement>();

	if( store_colors_for_each_vertex )
	{
		pFillTriangleElement->SetCornerColor( 0, color0 );
		pFillTriangleElement->SetCornerColor( 1, color1 );
		pFillTriangleElement->SetCornerColor( 2, color2 );
	}
	else
	{
		// Assumes all the colors are the same.
		int color_index = 0;
		pFillTriangleElement->SetColor( color_index, color0 );
	}

	return pFillTriangleElement;
}


shared_ptr<FillTriangleElement> GraphicsElementManager::CreateFillTriangle(
	Vector2 v0, Vector2 v1, Vector2 v2,
	const SFloatRGBAColor& vertex0_color,
	const SFloatRGBAColor& vertex1_color,
	const SFloatRGBAColor& vertex2_color,
	int layer
	)
{
	return CreateFillTriangle( v0, v1, v2, vertex0_color, vertex1_color, vertex2_color, layer, true );
}


shared_ptr<FillTriangleElement> GraphicsElementManager::CreateFillTriangle( Vector2 v0, Vector2 v1, Vector2 v2, const SFloatRGBAColor& color, int layer )
{
	return CreateFillTriangle( v0, v1, v2, color, color, color, layer, false );
}


shared_ptr<GraphicsElementGroup> GraphicsElementManager::CreateGroup( shared_ptr<GraphicsElement> *apElement, int num_elements, const SPoint& local_origin )
{
	vector< shared_ptr<GraphicsElement> > vecpElement;
	vecpElement.resize(num_elements);
	for( int i=0; i<num_elements; i++ )
	{
		vecpElement[i] = apElement[i];
	}

	return CreateGroup( vecpElement, local_origin );
}


shared_ptr<GraphicsElementGroup> GraphicsElementManager::CreateGroup( vector< shared_ptr<GraphicsElement> >& rvecpElement, const SPoint& local_origin )
{
	int index = GetVacantSlotIndex();

	shared_ptr<GraphicsElementGroup> pGroupElement( new GraphicsElementGroup( rvecpElement, Vector2((float)local_origin.x,(float)local_origin.y) ) );
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


bool GraphicsElementManager::LoadTexture( int texture_id, const std::string& tex_filename )
{
	const int required_size = texture_id + 1;
	const int shortage = required_size - (int)m_vecTexHandle.size();
	for( int i=0; i<shortage; i++ )
		m_vecTexHandle.push_back( TextureHandle() );

	return m_vecTexHandle[texture_id].Load( tex_filename );
}


int GraphicsElementManager::LoadTexture( const std::string& tex_filename )
{
	m_vecTexHandle.push_back( TextureHandle() );

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


bool GraphicsElementManager::LoadFont( int font_id, const string& font_name, int font_type, int w, int h, float bold, float italic, float shadow )
{
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

	FontFactory font_factory;
	FontBase *pFont = font_factory.CreateFontRawPtr( font_name );
	if( !pFont )
		return false;

	pFont->SetFontSize( w_scaled, h_scaled );

	pFont->SetItalic( italic );

	m_vecpFont[font_id] = pFont;

	return true;
}


bool GraphicsElementManager::LoadFont( int font_id, const std::string& font_name, int width, int height, float bold, float italic, float shadow )
{
	FontBase::FontType font_type;
	if( font_name.rfind( ".ttf" ) == font_name.length() - 4 )
		font_type = FontBase::FONTTYPE_TRUETYPETEXTURE;
	else if( font_name.rfind( ".bmp" ) == font_name.length() - 4
		|| font_name.rfind( ".jpg" ) == font_name.length() - 4
		|| font_name.rfind( ".tga" ) == font_name.length() - 4
		|| font_name.rfind( ".dds" ) == font_name.length() - 4 )
	{
		font_type = FontBase::FONTTYPE_TEXTURE;
	}
	else
	{
		font_type = FontBase::FONTTYPE_UTF;
	}

	return LoadFont( font_id, font_name, font_type, width, height, bold, italic, shadow );
}


int GraphicsElementManager::LoadFont( const std::string& font_name, int width, int height, float bold, float italic, float shadow )
{
	int font_id = (int)m_vecpFont.size(); // add a new entry for the new font

	bool res = LoadFont( font_id, font_name, width, height, bold, italic, shadow );

	return res ? font_id : -1;
}


int GraphicsElementManager::LoadTextureFont( const string& font_texture_filename,
											  int width, int height,
											  float bold, float italic, float shadow )
{
	LOG_PRINT( " - Loading a texture for font: " + font_texture_filename );

	TextureFont* pFont = new TextureFont();
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


bool GraphicsElementManager::RemoveElement( shared_ptr<GraphicsElement> pElement )
{
	if( !pElement // invalid argument
	 || !(pElement->m_pManager) ) // already released
	{
		return false;
	}

	pElement->OnRemovalRequested();

	int element_id = pElement->GetElementIndex();

	// callback triggered by element removal
	// - used by effect manager to remove effect which is currently being applied to removed element 'pElement'
	if( m_pCallbackPtr.get() )
		m_pCallbackPtr->OnDestroyed( pElement );

//	if( pElement->GetElementType() != GraphicsElement::TYPE_GROUP )
	if( pElement->BelongsToLayer() )
	{
		RemoveFromLayer( pElement );
	}

	// remove the element from the group to which it currently belongs
	int group_id = pElement->GetGroupID();
	shared_ptr<GraphicsElementGroup> pGroup;
	if( 0 <= group_id
	 && GetElement(group_id)->GetElementType() == GraphicsElement::TYPE_GROUP
	 && ( pGroup = dynamic_pointer_cast< GraphicsElementGroup, GraphicsElement >(GetElement(group_id)) ) )
	{
		pGroup->RemoveElementFromGroup( pElement );
	}

//	SAFE_DELETE( m_vecpElement[element_id] );
	m_vecpElement[element_id].reset();

	pElement->Release();

	return true;
}

/*
bool GraphicsElementManager::RemoveElement( std::shared_ptr<CombinedPrimitiveElement> pElement )
{
	std::shared_ptr<GraphicsElement> pFilElement   = pElement->FillElement();
	std::shared_ptr<GraphicsElement> pFrameElement = pElement->FrameElement();
	RemoveElement( pFilElement );
	RemoveElement( pFrameElement );

	std::shared_ptr<GraphicsElement> pCombinedPrimitiveElement   = pElement;
	RemoveElement( pCombinedPrimitiveElement );

	return true;
}
*/

bool GraphicsElementManager::RemoveAllElements()
{
	size_t i, num = m_vecpElement.size();
	for( i=0; i<num; i++ )
	{
		RemoveElement( m_vecpElement[i] );
		m_vecpElement[i].reset();
	}

	return true;
}


// remove an element from its current layer
// - does not release the element
void GraphicsElementManager::RemoveFromLayer( std::shared_ptr<GraphicsElement> pElement )
{
	int layer_index = pElement->GetLayerIndex();
	if( layer_index < 0 || (int)m_vecLayer.size() <= layer_index )
		return;

	Layer& current_layer = m_vecLayer[layer_index];

	bool res = current_layer.RemoveElementFromLayer( pElement );

	if( res )
		pElement->SetLayerIndex( -1 );
}


void GraphicsElementManager::SetElementToLayer( std::shared_ptr<GraphicsElement> pElement, int layer_index )
{
	if( layer_index < 0 || NUM_MAX_LAYERS <= layer_index )
		return;

	if( (int)m_vecLayer.size() <= layer_index )
	{
		// add layer(s)
		// - Note that the previous check 'NUM_MAX_LAYERS <= layer_index'
		//   guarantees that size of 'm_vecLayer' does not excceed NUM_MAX_LAYERS
		while( (int)m_vecLayer.size() <= layer_index )
			m_vecLayer.push_back( Layer() );
	}

	RemoveFromLayer( pElement );

	pElement->SetLayerIndex( layer_index );

	m_vecLayer[layer_index].m_vecElement.push_back( pElement->GetElementIndex() );
}


void GraphicsElementManager::Render()
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


void GraphicsElementManager::SetScale( float scale )
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


} // namespace amorphous
