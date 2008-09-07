#ifndef  __GraphicsElementManager_H__
#define  __GraphicsElementManager_H__


#include "../base.h"
#include "3DCommon/GraphicsElements.h"
#include "3DCommon/fwd.h"
#include "3DCommon/3DGameMath.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/2DFrameRect.h"
#include "3DCommon/2DRoundRect.h"
#include "3DCommon/2DTriangle.h"
#include "3DCommon/TextureCoord.h"
#include "3DCommon/FloatRGBAColor.h"
#include "3DCommon/FontBase.h"
#include "3DCommon/GraphicsComponentCollector.h"
#include "3DCommon/TextureHandle.h"
#include "3DMath/Vector2.h"
#include "3DMath/Vector3.h"
#include "3DMath/aabb2.h"

// This will load all headers includeing this file...
// How to avoid including "GraphicsEffectManager.h"?
//#include "3DCommon/all.h"


#include "GameCommon/CriticalDamping.h"

#include "Support/FixedVector.h"
#include "Support/Vec3_StringAux.h"
#include "Support/memory_helpers.h"

#include <vector>
#include <list>
#include <string>
#include <algorithm>


class CGraphicsElementManagerBase// : public CGraphicsComponent
{
public:

	CGraphicsElementManagerBase() {}
	virtual ~CGraphicsElementManagerBase() {}
	virtual void Release() {}
	virtual void ReleaseGraphicsResources() {}
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam ) {}

	virtual CGE_Rect *CreateRect( const SRect& rect, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual CGE_Rect *CreateFrameRect( const SRect& rect, const SFloatRGBAColor& color, float border_width, int layer = 0 ) { return NULL; }
	virtual CGE_Rect *CreateRoundRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, int layer = 0 ) { return NULL; }
	virtual CGE_Rect *CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, float border_width, int layer = 0 ) { return NULL; }
	virtual CGE_Triangle *CreateTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual CGE_Triangle *CreateFrameTriangle( const SRect& rect, const SFloatRGBAColor& color, float border_width, int layer = 0 ) { return NULL; }
	virtual CGE_Polygon *CreateRegularPolygon( int num_polygon_vertices, int x, int y, int radius, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual CGE_Text *CreateText( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 ) { return NULL; }
	virtual CGE_Text *CreateTextBox( int font_id, const std::string& text, const SRect& textbox, int align_h, int align_v, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 ) { return NULL; }
	virtual CGraphicsElement *CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual CGE_Group *CreateGroup( std::vector<CGraphicsElement *>& rvecpElement, const SPoint& local_origin ) { return NULL; }
	virtual CGE_Group *CreateGroup( CGraphicsElement** apElement, int num_elements, const SPoint& local_origin ) { return NULL; }
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
	virtual CGraphicsElement *GetElement( int id ) { return NULL; }
	virtual void SetScale( float scale ) {}
	virtual void SetReferenceResolutionWidth( int res_width ) {}

	void SetCallback( CGraphicsElementManagerCallbackSharedPtr callback_ptr ) {}
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

	std::vector<CGraphicsElement *> m_vecpElement;

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

		inline bool RemoveElementFromLayer( CGraphicsElement *pElement );

		friend class CGraphicsElementManager;
	};

	std::vector<CLayer> m_vecLayer;

	int m_NumMaxLayers;

	CGraphicsElementManagerCallbackSharedPtr m_pCallbackPtr;

private:

	int GetVacantSlotIndex();

	void UpdateElementScales();

	bool RegisterToLayer( int element_index, int layer_index );

	void InitElement( CGraphicsElement *pElement, int element_index, int layer_index );

	void RemoveFromLayer( CGraphicsElement *pElement );

    CGE_Rect *InitRectElement( int element_index, int layer_index, const SRect& non_scaled_rect, const SFloatRGBAColor& color, C2DPrimitive *pRectPrimitive );

    CGE_Triangle *InitTriangleElement( int element_index, int layer_index, const SRect& non_scaled_rect, const SFloatRGBAColor& color, C2DPrimitive *pRectPrimitive );

	void InitPrimitiveElement( CGE_Primitive *pElement, C2DPrimitive *pPrimitive, const SRect& non_scaled_rect, const SFloatRGBAColor& color, int element_index, int layer_index );

protected:

	// 19:15 2007/07/29 changed to a non-singleton class
//$	static CSingleton<CGraphicsElementManager> m_obj;

public:

	CGraphicsElementManager();

//$	static CGraphicsElementManager* Get() { return m_obj.get(); }

	~CGraphicsElementManager();

	void Release();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	//
	// functions to reate primitives
	//

	CGE_Rect *CreateRect( const SRect& rect, const SFloatRGBAColor& color, int layer = 0 );

	CGE_Rect *CreateFrameRect( const SRect& rect, const SFloatRGBAColor& color, float border_width, int layer = 0 );

	CGE_Rect *CreateRoundRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, int layer = 0 );

	CGE_Rect *CreateRoundFrameRect( const SRect& rect, const SFloatRGBAColor& color, float corner_radius, float border_width, int layer = 0 );

	CGE_Triangle *CreateTriangle( C2DTriangle::Direction dir, const SRect& rect, const SFloatRGBAColor& color, int layer = 0 );

	CGE_Triangle *CreateFrameTriangle( const SRect& rect, const SFloatRGBAColor& color, float border_width, int layer = 0 );

	///  NOT IMPLEMENTED
	/// - Does CreatePolygon() suffice or CreateTriangle() should be available?
	CGraphicsElement *CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer = 0 );

	CGE_Polygon *CreateRegularPolygon( int num_polygon_vertices, int x, int y, int radius, CRegularPolygonStyle::Name style, const SFloatRGBAColor& color, int layer = 0 );

	/// \param font_w, font_h font size. default size is used when set to zero or omitted.
	/// Default font size is the size of font obtained from element manager by the font id of the text element
	CGE_Text *CreateText( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 );

	/// \param font_w, font_h same as CreateText()
	CGE_Text *CreateTextBox( int font_id, const std::string& text, const SRect& textbox, int align_h, int align_v, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 );

	CGE_Group *CreateGroup( std::vector<CGraphicsElement *>& rvecpElement, const SPoint& local_origin );

	CGE_Group *CreateGroup( CGraphicsElement** apElement, int num_elements, const SPoint& local_origin );

	inline CGraphicsElement *GetElement( int id );


	bool LoadTexture( int texture_id, const std::string& tex_filename );

	int LoadTexture( const std::string& tex_filename );

	/// load TrueType / texture font
	bool LoadFont( int font_id, const std::string& font_name, int font_type, int w, int h, float bold = 0.0f, float italic = 0.0f );

	int LoadTextureFont( const std::string& font_texture_filename, int width, int height, float bold = 0.0f, float italic = 0.0f );

	inline const CTextureHandle& GetTexture( int tex_id );

	/// \return borrowed reference to a font object
	inline CFontBase *GetFont( int font_id );

	bool RemoveElement( CGE_Group*& pGroupElement );
	bool RemoveElement( CGraphicsElement*& pElement );

	/// deletes all the graphics elements.
	/// vector elements are not resized to zero
	bool RemoveAllElements();

	void SetElementToLayer( CGraphicsElement *pElement, int layer_index );

	void Render();

	void SetScale( float scale );	///< used to adjust the scale manually

//	void SetReferenceResolutionWidth( int res_width ) { m_ReferenceResolutionWidth = res_width; }

	void SetCallback( CGraphicsElementManagerCallbackSharedPtr callback_ptr ) { m_pCallbackPtr = callback_ptr; }
};


// ==================================== inline implementations ====================================

inline bool CGraphicsElementManager::CLayer::RemoveElementFromLayer( CGraphicsElement *pElement )
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

inline CGraphicsElement *CGraphicsElementManager::GetElement( int id )
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
