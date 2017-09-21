#ifndef  __GraphicsElementManager_H__
#define  __GraphicsElementManager_H__


#include "amorphous/base.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/GraphicsElements.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DFrameRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRoundRect.hpp"
#include "amorphous/Graphics/2DPrimitive/2DTriangle.hpp"
#include "amorphous/Graphics/GraphicsComponentCollector.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Support/FixedVector.hpp"

#include <list>
#include <algorithm>
#include <boost/shared_ptr.hpp>


namespace amorphous
{


class GraphicsElementManagerBase// : public GraphicsComponent
{
public:

	GraphicsElementManagerBase() {}
	virtual ~GraphicsElementManagerBase() {}
	virtual void Release() {}
	virtual void ReleaseGraphicsResources() {}
	virtual void LoadGraphicsResources( const GraphicsParameters& rParam ) {}
/*
	virtual boost::shared_ptr<RectElement> CreateRect( const SRect& rect, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<RectElement> CreateFrameRect( const SRect& rect, const SFloatRGBAColor& color, float frame_width, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<RectElement> CreateRoundRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<RectElement> CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, float frame_width, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<TriangleElement> CreateTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<TriangleElement> CreateFrameTriangle( const SRect& rect, const SFloatRGBAColor& color, float frame_width, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<FillPolygonElement> CreateRegularPolygon( int num_polygon_vertices, int x, int y, int radius, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<TextElement>CreateText( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<TextElement>CreateText( int font_id, const std::string& text, const SRect& textbox, int align_h, int align_v, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<GraphicsElement> CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<GraphicsElementGroup>CreateGroup( std::vector< boost::shared_ptr<GraphicsElement> >& rvecpElement, const SPoint& local_origin ) { return NULL; }
	virtual boost::shared_ptr<GraphicsElementGroup>CreateGroup( boost::shared_ptr<GraphicsElement> *apElement, int num_elements, const SPoint& local_origin ) { return NULL; }
	virtual int LoadTexture( const std::string& tex_filename ) { return -1; }
	virtual int LoadFont( const std::string& font_name ) { return -1; }
	virtual bool LoadFont( int font_id, const std::string& font_name, int font_type, int w, int h, float bold = 0.0f, float italic = 0.0f ) { return false; }
	virtual int LoadTextureFont( const std::string& font_texture_filename, int width, int height, float bold = 0.0f, float italic = 0.0f ) { return -1; }
//	virtual bool SetTexture( int texture_id, int element_id ) { return false; }
//	virtual bool SetTextureCoord( int element_id, const TEXCOORD2& vMin, const TEXCOORD2& vMax ) { return false; }
	virtual const TextureHandle& GetTexture( int tex_id ) { return TextureHandle::Null(); }
	virtual FontBase *GetFont( int font_id ) { return NULL; }
	virtual bool RemoveElement( GraphicsElement*& pElement ) { return false; }
	virtual bool RemoveAllElements() { return false; }
	virtual void Render() {}
	virtual boost::shared_ptr<GraphicsElement> GetElement( int id ) { return NULL; }
	virtual void SetScale( float scale ) {}
	virtual void SetReferenceResolutionWidth( int res_width ) {}

	void SetCallback( GraphicsElementManagerCallbackSharedPtr callback_ptr ) {}*/
};


/**
 About Create*() functions
  - returns a borrowed reference to a graphics element
  - User can
    - release the element by calling GraphicsElementManager::RemoveElement()
    - or just leave them then the dtor of GraphicsElementManager will remove all the element
    - Do not ever use the element once its GraphicsElementManager is released
  - argument of rect or bounding box is local coord by default
    - also set as global coord if the element does not belong to any group element
	  - But isn't this always true because an element cannot be owned by any group the moment it is created?

*/
class GraphicsElementManager : public GraphicsElementManagerBase, public GraphicsComponent
//class GraphicsElementManager : public GraphicsComponent
{
	enum Params
	{
		NUM_MAX_TEXTURES = 64,
		NUM_MAX_LAYERS = 96,
	};

	std::vector< boost::shared_ptr<GraphicsElement> > m_vecpElement;

//	std::vector<CTextureEntity> m_vecTexHandle;	CTextureEntity cannot be used with vector<> - relase & reallocation of vector will screw up the release & load mechanism
	TCFixedVector<TextureHandle,NUM_MAX_TEXTURES> m_vecTexHandle;

	std::vector<FontBase *> m_vecpFont;

	std::vector<Vector2> m_vecOrigFontSize;

	float m_fScale;

	bool m_bAutoScaling;	///< if true, automatically scales graphics elements in LoadGraphicsResources() when the screen resolution is changed

//	int m_ReferenceResolutionWidth;

	class Layer
	{
		std::vector<int> m_vecElement;

	public:
		Layer() {}
		virtual ~Layer() {}

		inline bool RemoveElementFromLayer( boost::shared_ptr<GraphicsElement>& pElement );

		friend class GraphicsElementManager;
	};

	std::vector<Layer> m_vecLayer;

	int m_NumMaxLayers;

	GraphicsElementManagerCallbackSharedPtr m_pCallbackPtr;

private:

	int GetVacantSlotIndex();

	void UpdateElementScales();

	bool RegisterToLayer( int element_index, int layer_index );

	bool InitElement( boost::shared_ptr<GraphicsElement> pElement, int layer_index );

	void RemoveFromLayer( boost::shared_ptr<GraphicsElement> pElement );

//	boost::shared_ptr<RectElement> InitRectElement( int element_index, int layer_index, const SRect& non_scaled_rect, const SFloatRGBAColor& color, C2DPrimitive *pRectPrimitive );

//	boost::shared_ptr<TriangleElement> InitTriangleElement( int element_index, int layer_index, const SRect& non_scaled_rect, const SFloatRGBAColor& color, C2DPrimitive *pRectPrimitive );

	bool InitPrimitiveElement( boost::shared_ptr<PrimitiveElement>, const SRect& non_scaled_rect, const SFloatRGBAColor& color, int layer_index );

	boost::shared_ptr<FillTriangleElement> CreateFillTriangle(
		Vector2 v0, Vector2 v1, Vector2 v2,
		const SFloatRGBAColor& color0,
		const SFloatRGBAColor& color1,
		const SFloatRGBAColor& color2,
		int layer,
		bool store_colors_for_each_vertex
		);

public:

	GraphicsElementManager();

	~GraphicsElementManager();

	void Release();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const GraphicsParameters& rParam );

	//
	// functions to create primitives
	//

	boost::shared_ptr<CombinedRectElement> CreateRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );
	boost::shared_ptr<FillRectElement> CreateFillRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, int layer = 0 );
	boost::shared_ptr<FrameRectElement> CreateFrameRect( const SRect& rect, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );

	boost::shared_ptr<CombinedRoundRectElement> CreateRoundRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, const SFloatRGBAColor& frame_color_0, float corner_radius, float frame_width, int layer = 0 );
	boost::shared_ptr<RoundFillRectElement> CreateRoundFillRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, float corner_radius, int layer = 0 );
	boost::shared_ptr<RoundFrameRectElement> CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& frame_color_0, float corner_radius, float frame_width, int layer = 0 );

	boost::shared_ptr<CombinedTriangleElement> CreateTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& fill_color_0, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );
	boost::shared_ptr<FillTriangleElement> CreateFillTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& fill_color_0, int layer = 0 );
	boost::shared_ptr<FrameTriangleElement> CreateFrameTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );

//	boost::shared_ptr<TriangleElement> CreateTriangle( const SRect& rect, const SFloatRGBAColor& fill_color_0, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );
//	boost::shared_ptr<TriangleElement> CreateFillTriangle( const SRect& rect, const SFloatRGBAColor& fill_color_0, int layer = 0 );
//	boost::shared_ptr<TriangleElement> CreateFrameTriangle( const SRect& rect, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );

	///  NOT IMPLEMENTED
	/// - Does CreatePolygon() suffice or CreateTriangle() should be available?
	boost::shared_ptr<GraphicsElement> CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer = 0 );

	/**
	  Note that the colors are stored separately for each vertex
	*/
	boost::shared_ptr<FillTriangleElement> CreateFillTriangle(
		Vector2 v0, Vector2 v1, Vector2 v2,
		const SFloatRGBAColor& vertex0_color,
		const SFloatRGBAColor& vertex1_color,
		const SFloatRGBAColor& vertex2_color,
		int layer = 0
		);

	boost::shared_ptr<FillTriangleElement> CreateFillTriangle( Vector2 v0, Vector2 v1, Vector2 v2, const SFloatRGBAColor& color = SFloatRGBAColor::White(), int layer = 0 );

	boost::shared_ptr<FillPolygonElement> CreateRegularPolygon( int num_polygon_vertices, int x, int y, int radius, CRegularPolygonStyle::Name style, const SFloatRGBAColor& color, int layer = 0 );

	/// \param font_w, font_h font size. default size is used when set to zero or omitted.
	/// Default font size is the size of font obtained from element manager by the font id of the text element
	boost::shared_ptr<TextElement> CreateText( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 );

	/// \param font_w, font_h same as CreateText()
	boost::shared_ptr<TextElement> CreateText( int font_id, const std::string& text, const SRect& textbox, int align_h, int align_v, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 );

	boost::shared_ptr<GraphicsElementGroup> CreateGroup( std::vector< boost::shared_ptr<GraphicsElement> >& rvecpElement, const SPoint& local_origin );

	boost::shared_ptr<GraphicsElementGroup>CreateGroup( boost::shared_ptr<GraphicsElement> *apElement, int num_elements, const SPoint& local_origin );

	inline boost::shared_ptr<GraphicsElement> GetElement( int id );


	bool LoadTexture( int texture_id, const std::string& tex_filename );

	int LoadTexture( const std::string& tex_filename );

	/// load TrueType / texture font
	/// - Not preferable IF since user has to specify font_type
	/// - Font type should be determined automatically from font_name
	bool LoadFont( int font_id, const std::string& font_name, int font_type, int w, int h, float bold = 0.0f, float italic = 0.0f, float shadow = 0.0f );

	/// Load a font
	/// - Let user specify the id
	/// - Useful when the user defines enums of font ids and control the id to font mapppings
	/// - Returns true on success
	bool LoadFont( int font_id, const std::string& font_name, int width, int height, float bold = 0.0f, float italic = 0.0f, float shadow = 0.0f );

	/// Load a font
	/// - Let system determine the font id
	/// - Returns a valid font id on succcess
	/// - Returns -1 on failure
	int LoadFont( const std::string& font_name, int width, int height, float bold = 0.0f, float italic = 0.0f, float shadow = 0.0f );

	int LoadTextureFont( const std::string& font_texture_filename, int width, int height, float bold = 0.0f, float italic = 0.0f, float shadow = 0.0f );

	inline const TextureHandle& GetTexture( int tex_id );

	/// \return borrowed reference to a font object
	inline FontBase *GetFont( int font_id );

	bool RemoveElement( boost::shared_ptr<GraphicsElement> pElement );

	/// deletes all the graphics elements.
	/// vector elements are not resized to zero
	bool RemoveAllElements();

	void SetElementToLayer( boost::shared_ptr<GraphicsElement> pElement, int layer_index );

	void Render();

	void SetScale( float scale );	///< used to adjust the scale manually

//	void SetReferenceResolutionWidth( int res_width ) { m_ReferenceResolutionWidth = res_width; }

	void SetCallback( GraphicsElementManagerCallbackSharedPtr callback_ptr ) { m_pCallbackPtr = callback_ptr; }
};


// ==================================== inline implementations ====================================

inline bool GraphicsElementManager::Layer::RemoveElementFromLayer( boost::shared_ptr<GraphicsElement>& pElement )
{
	int element_index = pElement->GetElementIndex();

	size_t i, num_elements = m_vecElement.size();
	for( i=0; i<num_elements; i++ )
	{
		if( element_index == m_vecElement[i] )
		{
			m_vecElement.erase( m_vecElement.begin() + i );
			return true;
		}
	}

	return false;
}

inline boost::shared_ptr<GraphicsElement> GraphicsElementManager::GetElement( int id )
{
	return m_vecpElement[id];
}


inline const TextureHandle& GraphicsElementManager::GetTexture( int tex_id )
{
	if( 0 <= tex_id && tex_id < m_vecTexHandle.size() )
		return m_vecTexHandle[tex_id];
	else
		return TextureHandle::Null();
}


inline FontBase *GraphicsElementManager::GetFont( int font_id )
{
	if( font_id < 0 || (int)m_vecpFont.size() <= font_id )
		return NULL;
	else
		return m_vecpFont[font_id];
}

} // namespace amorphous



#endif  /* __GraphicsElementManager_H__ */
