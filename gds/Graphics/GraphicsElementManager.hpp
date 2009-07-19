#ifndef  __GraphicsElementManager_H__
#define  __GraphicsElementManager_H__


#include "../base.hpp"
#include "Graphics/GraphicsElements.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/3DGameMath.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/2DPrimitive/2DFrameRect.hpp"
#include "Graphics/2DPrimitive/2DRoundRect.hpp"
#include "Graphics/2DPrimitive/2DTriangle.hpp"
#include "Graphics/TextureCoord.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "Graphics/Font/FontBase.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/TextureHandle.hpp"
#include "3DMath/Vector2.hpp"
#include "3DMath/Vector3.hpp"
#include "3DMath/aabb2.hpp"

// This will load all headers includeing this file...
// How to avoid including "GraphicsEffectManager.hpp"?
//#include "Graphics/all.hpp"


#include "GameCommon/CriticalDamping.hpp"

#include "Support/FixedVector.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "Support/memory_helpers.hpp"

#include <list>
#include <algorithm>
#include <boost/shared_ptr.hpp>


class CGraphicsElementManagerBase// : public CGraphicsComponent
{
public:

	CGraphicsElementManagerBase() {}
	virtual ~CGraphicsElementManagerBase() {}
	virtual void Release() {}
	virtual void ReleaseGraphicsResources() {}
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam ) {}
/*
	virtual boost::shared_ptr<CRectElement> CreateRect( const SRect& rect, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CRectElement> CreateFrameRect( const SRect& rect, const SFloatRGBAColor& color, float frame_width, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CRectElement> CreateRoundRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CRectElement> CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, float frame_width, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CTriangleElement> CreateTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CTriangleElement> CreateFrameTriangle( const SRect& rect, const SFloatRGBAColor& color, float frame_width, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CFillPolygonElement> CreateRegularPolygon( int num_polygon_vertices, int x, int y, int radius, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CTextElement>CreateText( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CTextElement>CreateText( int font_id, const std::string& text, const SRect& textbox, int align_h, int align_v, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CGraphicsElement> CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual boost::shared_ptr<CGraphicsElementGroup>CreateGroup( std::vector< boost::shared_ptr<CGraphicsElement> >& rvecpElement, const SPoint& local_origin ) { return NULL; }
	virtual boost::shared_ptr<CGraphicsElementGroup>CreateGroup( boost::shared_ptr<CGraphicsElement> *apElement, int num_elements, const SPoint& local_origin ) { return NULL; }
	virtual int LoadTexture( const std::string& tex_filename ) { return -1; }
	virtual int LoadFont( const std::string& font_name ) { return -1; }
	virtual bool LoadFont( int font_id, const std::string& font_name, int font_type, int w, int h, float bold = 0.0f, float italic = 0.0f ) { return false; }
	virtual int LoadTextureFont( const std::string& font_texture_filename, int width, int height, float bold = 0.0f, float italic = 0.0f ) { return -1; }
//	virtual bool SetTexture( int texture_id, int element_id ) { return false; }
//	virtual bool SetTextureCoord( int element_id, const TEXCOORD2& vMin, const TEXCOORD2& vMax ) { return false; }
	virtual const CTextureHandle& GetTexture( int tex_id ) { return CTextureHandle::Null(); }
	virtual CFontBase *GetFont( int font_id ) { return NULL; }
	virtual bool RemoveElement( CGraphicsElement*& pElement ) { return false; }
	virtual bool RemoveAllElements() { return false; }
	virtual void Render() {}
	virtual boost::shared_ptr<CGraphicsElement> GetElement( int id ) { return NULL; }
	virtual void SetScale( float scale ) {}
	virtual void SetReferenceResolutionWidth( int res_width ) {}

	void SetCallback( CGraphicsElementManagerCallbackSharedPtr callback_ptr ) {}*/
};


/**
 About Create*() functions
  - returns a borrowed reference to a graphics element
  - User can
    - release the element by calling CGraphicsElementManager::RemoveElement()
    - or just leave them then the dtor of CGraphicsElementManager will remove all the element
    - Do not ever use the element once its CGraphicsElementManager is released
  - argument of rect or bounding box is local coord by default
    - also set as global coord if the element does not belong to any group element
	  - But isn't this always true because an element cannot be owned by any group the moment it is created?

*/
class CGraphicsElementManager : public CGraphicsElementManagerBase, public CGraphicsComponent
//class CGraphicsElementManager : public CGraphicsComponent
{
	enum Params
	{
		NUM_MAX_TEXTURES = 64,
		NUM_MAX_LAYERS = 96,
	};

	std::vector< boost::shared_ptr<CGraphicsElement> > m_vecpElement;

//	std::vector<CTextureEntity> m_vecTexHandle;	CTextureEntity cannot be used with vector<> - relase & reallocation of vector will screw up the release & load mechanism
	TCFixedVector<CTextureHandle,NUM_MAX_TEXTURES> m_vecTexHandle;

	std::vector<CFontBase *> m_vecpFont;

	std::vector<Vector2> m_vecOrigFontSize;

	float m_fScale;

	bool m_bAutoScaling;	///< if true, automatically scales graphics elements in LoadGraphicsResources() when the screen resolution is changed

//	int m_ReferenceResolutionWidth;

	class CLayer
	{
		std::vector<int> m_vecElement;

	public:
		CLayer() {}
		virtual ~CLayer() {}

		inline bool RemoveElementFromLayer( boost::shared_ptr<CGraphicsElement>& pElement );

		friend class CGraphicsElementManager;
	};

	std::vector<CLayer> m_vecLayer;

	int m_NumMaxLayers;

	CGraphicsElementManagerCallbackSharedPtr m_pCallbackPtr;

private:

	int GetVacantSlotIndex();

	void UpdateElementScales();

	bool RegisterToLayer( int element_index, int layer_index );

	bool InitElement( boost::shared_ptr<CGraphicsElement> pElement, int layer_index );

	void RemoveFromLayer( boost::shared_ptr<CGraphicsElement> pElement );

//	boost::shared_ptr<CRectElement> InitRectElement( int element_index, int layer_index, const SRect& non_scaled_rect, const SFloatRGBAColor& color, C2DPrimitive *pRectPrimitive );

//	boost::shared_ptr<CTriangleElement> InitTriangleElement( int element_index, int layer_index, const SRect& non_scaled_rect, const SFloatRGBAColor& color, C2DPrimitive *pRectPrimitive );

	bool InitPrimitiveElement( boost::shared_ptr<CPrimitiveElement>, const SRect& non_scaled_rect, const SFloatRGBAColor& color, int layer_index );

public:

	CGraphicsElementManager();

	~CGraphicsElementManager();

	void Release();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	//
	// functions to create primitives
	//

	boost::shared_ptr<CCombinedRectElement> CreateRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );
	boost::shared_ptr<CFillRectElement> CreateFillRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, int layer = 0 );
	boost::shared_ptr<CFrameRectElement> CreateFrameRect( const SRect& rect, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );

	boost::shared_ptr<CCombinedRoundRectElement> CreateRoundRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, const SFloatRGBAColor& frame_color_0, float corner_radius, float frame_width, int layer = 0 );
	boost::shared_ptr<CRoundFillRectElement> CreateRoundFillRect( const SRect& rect, const SFloatRGBAColor& fill_color_0, float corner_radius, int layer = 0 );
	boost::shared_ptr<CRoundFrameRectElement> CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& frame_color_0, float corner_radius, float frame_width, int layer = 0 );

	boost::shared_ptr<CCombinedTriangleElement> CreateTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& fill_color_0, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );
	boost::shared_ptr<CFillTriangleElement> CreateFillTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& fill_color_0, int layer = 0 );
	boost::shared_ptr<CFrameTriangleElement> CreateFrameTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );

//	boost::shared_ptr<CTriangleElement> CreateTriangle( const SRect& rect, const SFloatRGBAColor& fill_color_0, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );
//	boost::shared_ptr<CTriangleElement> CreateFillTriangle( const SRect& rect, const SFloatRGBAColor& fill_color_0, int layer = 0 );
//	boost::shared_ptr<CTriangleElement> CreateFrameTriangle( const SRect& rect, const SFloatRGBAColor& frame_color_0, float frame_width, int layer = 0 );

	///  NOT IMPLEMENTED
	/// - Does CreatePolygon() suffice or CreateTriangle() should be available?
	boost::shared_ptr<CGraphicsElement> CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer = 0 );

	boost::shared_ptr<CFillPolygonElement> CreateRegularPolygon( int num_polygon_vertices, int x, int y, int radius, CRegularPolygonStyle::Name style, const SFloatRGBAColor& color, int layer = 0 );

	/// \param font_w, font_h font size. default size is used when set to zero or omitted.
	/// Default font size is the size of font obtained from element manager by the font id of the text element
	boost::shared_ptr<CTextElement> CreateText( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 );

	/// \param font_w, font_h same as CreateText()
	boost::shared_ptr<CTextElement> CreateText( int font_id, const std::string& text, const SRect& textbox, int align_h, int align_v, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 );

	boost::shared_ptr<CGraphicsElementGroup> CreateGroup( std::vector< boost::shared_ptr<CGraphicsElement> >& rvecpElement, const SPoint& local_origin );

	boost::shared_ptr<CGraphicsElementGroup>CreateGroup( boost::shared_ptr<CGraphicsElement> *apElement, int num_elements, const SPoint& local_origin );

	inline boost::shared_ptr<CGraphicsElement> GetElement( int id );


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

	inline const CTextureHandle& GetTexture( int tex_id );

	/// \return borrowed reference to a font object
	inline CFontBase *GetFont( int font_id );

//	bool RemoveElement( CGraphicsElementGroup*& pGroupElement );
	bool RemoveElement( boost::shared_ptr<CGraphicsElementGroup>& pGroupElement );

	bool RemoveElement( boost::shared_ptr<CGraphicsElement>& pElement );

	/// deletes all the graphics elements.
	/// vector elements are not resized to zero
	bool RemoveAllElements();

	void SetElementToLayer( boost::shared_ptr<CGraphicsElement> pElement, int layer_index );

	void Render();

	void SetScale( float scale );	///< used to adjust the scale manually

//	void SetReferenceResolutionWidth( int res_width ) { m_ReferenceResolutionWidth = res_width; }

	void SetCallback( CGraphicsElementManagerCallbackSharedPtr callback_ptr ) { m_pCallbackPtr = callback_ptr; }
};


// ==================================== inline implementations ====================================

inline bool CGraphicsElementManager::CLayer::RemoveElementFromLayer( boost::shared_ptr<CGraphicsElement>& pElement )
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

inline boost::shared_ptr<CGraphicsElement> CGraphicsElementManager::GetElement( int id )
{
	return m_vecpElement[id];
}


inline const CTextureHandle& CGraphicsElementManager::GetTexture( int tex_id )
{
	if( 0 <= tex_id && tex_id < m_vecTexHandle.size() )
		return m_vecTexHandle[tex_id];
	else
		return CTextureHandle::Null();
}


inline CFontBase *CGraphicsElementManager::GetFont( int font_id )
{
	if( font_id < 0 || (int)m_vecpFont.size() <= font_id )
		return NULL;
	else
		return m_vecpFont[font_id];
}


#endif  /* __GraphicsElementManager_H__ */
