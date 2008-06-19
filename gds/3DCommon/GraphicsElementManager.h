#ifndef  __GraphicsElementManager_H__
#define  __GraphicsElementManager_H__


#include "../base.h"
#include "3DMath/Vector2.h"
#include "3DMath/Vector3.h"
#include "3DMath/aabb2.h"
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

#include "GameCommon/CriticalDamping.h"

#include "Support/FixedVector.h"
#include "Support/Vec3_StringAux.h"
#include "Support/memory_helpers.h"

#include <vector>
#include <list>
#include <string>
#include <algorithm>


inline AABB2 AABB2Null()
{
	return AABB2( Vector2(FLT_MAX,FLT_MAX), Vector2(-FLT_MAX,-FLT_MAX) );
}


/**
 terms
 - group: a collection of one or more graphics elements.
   - used to set effect on multiple elements at once
 - layer: holds graphics elements
   - controls rendering order
   - elements in layer 0 are rendered on top (rendered last)
   - higher layer index means the element is behind other elements
   - rendering order of elements in the same layer is not defined. They are rendered in random order.
   - group elements do not belong to any layer




*/
class CGraphicsElement
{
public:

	enum eColor
	{
		COLOR_0,
		COLOR_1,
		COLOR_2,
		COLOR_3,
		NUM_COLORS
	};

protected:

	CGraphicsElementManager *m_pManager;

	SFloatRGBAColor m_aColor[NUM_COLORS];

	int m_TextureID;

	float m_fScale;

	/// holds non-scaled rect
	/// - m_AABB.vMin == non-scaled global top-left position
	AABB2 m_AABB;

	int m_GroupID; ///< group id to which the element belongs. -1 if the element does not belong to any group element

	int m_LayerIndex; ///< layer index to which the element belongs. -1 if the element is a group element

	int m_ElementIndex; ///< index of the element in CGraphicsElementManager::m_vecpElement

	/// top left pos relative to the group element which owns this element
	/// - used by grouped elements
	/// - not used by group elements
	Vector2 m_vLocalTopLeftPos;

protected:

	/// can only be created by CGraphicsElementManager
	/// - make this protected and make CGraphicsElementManager a friend class
	/// \param non_scaled_aabb non-scaled axis-aligned bounding box. Needs to be
	///        initialized in derived class if it is not specified here
	CGraphicsElement( const AABB2& non_scaled_aabb = AABB2Null() )
		:
	m_pManager(NULL),
	m_AABB(non_scaled_aabb),
	m_TextureID(-1),
	m_fScale(1.0f),
	m_GroupID(-1),
	m_ElementIndex(-1),
	m_LayerIndex(-1),
	m_vLocalTopLeftPos(Vector2(0,0))
	{
		for( int i=0; i<NUM_COLORS; i++ )
			m_aColor[i] = SFloatRGBAColor(1,1,1,1);
	}

	virtual ~CGraphicsElement() {}

	virtual void SetTopLeftPosInternal( Vector2 vPos ) {}

private:

	void SetElementIndex( int element_index ) { m_ElementIndex = element_index; }

	void UpdateTopLeftPos( Vector2 vPos );

public:

	virtual void Draw() = 0;

	virtual void SetTexture( int texture_id ) { m_TextureID = texture_id; }

	/// TODO: Decide whether this really has to be virtual
	virtual Vector2 GetTopLeftPos() const { return m_AABB.vMin; }

	virtual Vector2 GetLocalTopLeftPos() const { return m_vLocalTopLeftPos; }

	const AABB2& GetAABB() const { return m_AABB; }

	/// calls SetTopLeftPosInternal() to run routines
	/// specific to each element
	/// - The user should use SetLocalTopLeftPos() if the element is grouped.
	///   The result is undefined if this is called when the local origin of the group element is being changed
	/// \param vPos non-scaled global pos
	void SetTopLeftPos( Vector2 vPos );

	void SetTopLeftPos( SPoint pos ) { SetTopLeftPos( Vector2((float)pos.x,(float)pos.y) ); }

	void SetLocalTopLeftPos( Vector2 vPos );

	void SetLocalTopLeftPos( SPoint pos ) { SetLocalTopLeftPos(Vector2((float)pos.x,(float)pos.y) ); }

	void SetLocalRect( const SRect& local_rect )
	{
		SetSizeLTRB( Vector2((float)local_rect.left,(float)local_rect.top), Vector2((float)local_rect.right,(float)local_rect.bottom) );
		SetLocalTopLeftPos( local_rect.GetTopLeftCorner() );
	}

	/// Called when the element belongs to a group element and its local origin is changed
	/// \param vLocalOrigin local origin of the group represented in global coordinates
	virtual void UpdateGlobalPositions( Vector2 vLocalOrigin )
	{
		UpdateTopLeftPos( vLocalOrigin + m_vLocalTopLeftPos );
	}

	virtual void ChangeScale( float scale ) {}

	virtual void ChangeElementScale( float scale ) {}

	virtual void SetSizeLTRB( const Vector2& vMin, const Vector2& vMax ) {}

	virtual const SFloatRGBAColor& GetColor( int color_index ) const { return m_aColor[color_index]; }

	/// sets RGBA
	virtual void SetColor( int color_index, const SFloatRGBAColor& color ) { m_aColor[color_index] = color; }

	/// sets RGB
	/// - does not change alpha component
	virtual void SetColor( int color_index, const SFloatRGBColor& color ) { m_aColor[color_index].SetRGB( color.fRed, color.fGreen, color.fBlue ); }

	float GetAlpha( int color_index ) const { return m_aColor[color_index].fAlpha; }

	virtual void SetAlpha(  int color_index, float a ) { m_aColor[color_index].fAlpha = a; }

	inline SFloatRGBAColor GetBlendedColor() const;

	virtual void SetTextureCoord( const TEXCOORD2& vMin, const TEXCOORD2& vMax ) {}

	/// \param stretch_x non-scaled value
	/// \param stretch_y non-scaled value
	virtual void SetTextureCoord( int stretch_x, int stretch_y,
		                           const TEXCOORD2& left_top_offset = TEXCOORD2(0,0),
								   TextureAddress::Mode mode = TextureAddress::Wrap ) {}

	void SetGraphicsElementManager( CGraphicsElementManager *pMgr ) { m_pManager = pMgr; }

	/// place the element in a layer
	void SetLayer( int layer_index );

	/// sets the layer index
	/// NOTE: does not move the element from the current layer
	void SetLayerIndex( int layer_index ) { m_LayerIndex = layer_index; }

	int GetLayerIndex() const { return m_LayerIndex; }

	int GetGroupID() const { return m_GroupID; }

	void SetGroupID( int group_id ) { m_GroupID = group_id; }

	int GetElementIndex() const { return m_ElementIndex; }

	virtual int GetElementType() const = 0;

	enum eType
	{
		TYPE_RECT,
//		TYPE_FRAMERECT,
		TYPE_TRIANGLE,
		TYPE_TEXT,
		TYPE_GROUP,
		NUM_ELEMENT_TYPES
	};

	friend class CGE_Group;
	friend class CGraphicsElementManager;
};


inline SFloatRGBAColor CGraphicsElement::GetBlendedColor() const
{
	return m_aColor[0] * m_aColor[1] * m_aColor[2] * m_aColor[3];
}


class CGE_Primitive : public CGraphicsElement
{
protected:

	/// scaled primitive
	/// - owned reference
	/// - holds scaled border if it is a frame primitive
	/// - holds scaled corner radius if it is a round-cornered primitive
	C2DPrimitive *m_pPrimitive;

	/// Holds non-scaled value of frame border width.
	/// Used by the following primitives
	///   - frame rect
	///   - round frame rect
	int m_OrigBorderWidth; ///< non-scaled border width

	/// Holds non-scaled value of corner radius.
	/// Used by the following primitives.
	///   - round rect
	///   - round frame rect
	int m_CornerRadius;

protected:

	void DrawPrimitive();

	inline void SetBlendedColorToPrimitive();

public:

	/// \param non_scaled_aabb represents a non-scaled rectangular region of the element
	/// \param pPrimitive owned reference to a 2d primitive that holds scaled position
	CGE_Primitive( C2DPrimitive *pPrimitive )
		:
//	CGraphicsElement(non_scaled_aabb),
	m_pPrimitive(pPrimitive),
	m_OrigBorderWidth(1),
	m_CornerRadius(0)
	{
	}

	virtual ~CGE_Primitive() { SafeDelete( m_pPrimitive ); }

	virtual void SetTopLeftPosInternal( Vector2 vPos );

	virtual void ChangeScale( float scale )
	{
		m_fScale = scale;
		m_pPrimitive->SetPosition( m_AABB.vMin * scale, m_AABB.vMax * scale );
	}

	virtual void SetSizeLTRB( const Vector2& vMin, const Vector2& vMax )
	{
		m_AABB.vMin = vMin;
		m_AABB.vMax = vMax;
		m_pPrimitive->SetPosition( m_AABB.vMin * m_fScale, m_AABB.vMax * m_fScale );
	}

	virtual void SetTextureCoord( const TEXCOORD2& vMin, const TEXCOORD2& vMax ) { m_pPrimitive->SetTextureUV( vMin, vMax ); }

	virtual void SetTextureCoord( int stretch_x, int stretch_y,
		                           const TEXCOORD2& left_top_offset,
								   TextureAddress::Mode mode )
	{
		m_pPrimitive->SetTextureCoords( (int)(stretch_x * m_fScale), (int)(stretch_y * m_fScale), left_top_offset, mode );
	}
};


//=============================== inline implementations ===============================

inline void CGE_Primitive::SetBlendedColorToPrimitive()
{
	// update vertex colors
	// the same color is set to all the 4 vertices of the primitive
	m_pPrimitive->SetColor( GetBlendedColor() );
}



/**
  - graphics element for rectangle
*/
class CGE_Rect : public CGE_Primitive
{

	/// blended with CGraphicsElement::m_aColor[]
	SFloatRGBAColor m_aCornerColor[4];

public:


	/// \param pPrimitive - owned reference
	CGE_Rect( const SFloatRGBAColor& color0, C2DPrimitive *pPrimitive );

	virtual void Draw();

//	void SetHFrameTextureCoord();
//	void SetVFrameTextureCoord();

	virtual int GetElementType() const { return TYPE_RECT; }

	/// not available for C2DRoundRect
	void SetCornerColor( int corner_index, SFloatRGBAColor& color ) { m_aCornerColor[corner_index] = color; }
};


class CGE_Triangle : public CGE_Primitive
{
	/// hold one of the following 3 primitives

	C2DTriangle *m_pTriangle;
	C2DFrameTriangle *m_pFTriangle;
	C2DRoundFrameTriangle *m_RFpTriangle;

public:

	CGE_Triangle( const SFloatRGBAColor& color0, C2DPrimitive *pPrimitive, const SRect& non_scaled_rect );

	virtual void Draw();

	virtual int GetElementType() const { return TYPE_TRIANGLE; }
};


/*
class CGE_Circle : public CGraphicsElement
{
//	C2DCircle *m_pCircle; ???
	// or
//	C2DEllipse *m_pEllipse; ???

public:

	/// \param pPrimitive - owned reference
	CGE_Circle( C2DPrimitive *pPrimitive, const SFloatRGBAColor& color0 );

	virtual ~CGE_Circle() { SafeDelete( m_pPrimitive ); }

	virtual void Draw();

//	virtual Vector2 GetTopLeftPos() const { return ???; }

	virtual void SetTopLeftPosInternal( Vector2 vPos )
	{
	}

	virtual void ChangeScale( float scale )
	{
	}

	virtual void SetSizeLTRB( const Vector2& vMin, const Vector2& vMax )
	{
	}

	virtual void SetTextureCoord( const TEXCOORD2& vMin, const TEXCOORD2& vMax ) { m_pPrimitive->SetTextureUV( vMin, vMax ); }

	virtual int GetElementType() const { return TYPE_???; }

	/// not available for C2DRoundRect
	void SetCornerColor( int corner_index, SFloatRGBAColor& color ) { m_aCornerColor[corner_index] = color; }

}
*/



class CGE_Text : public CGraphicsElement
{
	int m_FontID;

	std::string m_Text;

	/// top left corner pos of the text
	Vector2 m_vTextPos;

	Vector2 m_vScaledPos;

	/// scaled font size. when set to 0, size of the font obtained by 'font_id' is used
	int m_ScaledWidth, m_ScaledHeight;

	/// non-scaled font size
	int m_FontWidth, m_FontHeight;

	int m_TextAlignH;
	int m_TextAlignV;

public:

	enum eTextAlignment { TAL_LEFT, TAL_TOP, TAL_CENTER, TAL_RIGHT, TAL_BOTTOM, NUM_TEXT_ALIGNMENTS };

	CGE_Text( int font_id, const std::string& text, const AABB2& textbox, int align_h, int align_v, const SFloatRGBAColor& color0 )
		: m_FontID(font_id), m_Text(text), m_vTextPos(textbox.vMin)
	{
		m_AABB = textbox;
		m_TextAlignH = align_h;
		m_TextAlignV = align_v;
		m_aColor[0] = color0;
		ChangeScale( m_fScale );
	}

//	CGE_Text( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color0 );

	/**
	 draws text
	 - text color set for this text and not restored
	 - text font size - set for this text if the font size has been specified explicitly
	   and the original font size is restored after draw
	 */
	virtual void Draw();

	virtual void SetTopLeftPosInternal( Vector2 vPos )
	{
		UpdateTextAlignment();
	}

	virtual void ChangeScale( float scale )
	{
		m_fScale = scale;
		m_vScaledPos = m_vTextPos * scale;
		m_ScaledWidth = (int)(m_FontWidth * scale);
		m_ScaledHeight = (int)(m_FontHeight * scale);
	}

	virtual void SetSizeLTRB( const Vector2& vMin, const Vector2& vMax )
	{
		m_AABB.vMin = vMin;
		m_AABB.vMax = vMax;
		m_vTextPos = vMin;
		Vector2 vSpan = vMax - vMin;
		m_FontWidth = (int)( vSpan.x / (float)m_Text.length() ); 
		m_FontHeight =(int)vSpan.y;// vSpan.x / (float)GetNumRows(m_Text)

		m_vScaledPos = m_vTextPos * m_fScale;
	}

	void SetFontID( int font_id ) { m_FontID = font_id; }
	
	const std::string& GetText() const { return m_Text; }

	void SetText( const std::string& text ) { m_Text = text; }

	void SetFontSize( int w, int h ) { m_FontWidth = w; m_FontHeight = h; }

	/// \param horizontal_alignment CGE_Text::TAL_LEFT, TAL_CENTER or TAL_RIGHT
	/// \param vertical_alignment CGE_Text::TAL_TOP, TAL_CENTER or TAL_BOTTOM
	void SetTextAlignment( int horizontal_alignment, int vertical_alignment );

	void UpdateTextAlignment();

	virtual int GetElementType() const { return TYPE_TEXT; }
};


class CGE_Group : public CGraphicsElement
{
	std::vector<CGraphicsElement *> m_vecpElement;

	/// origin of local top-left positions which are grouped by the group element.
	/// - Represented in global screen coordinates
//	Vector2 m_vLocalOrigin;

private:

	inline void RemoveInvalidElements();

	inline void UpdateAABB();

public:

//	CGE_Group( std::vector<CGraphicsElement *>& rvecpElement );

	/// Calculates the local top-left positions of the specified graphics elements from vLocalOrigin
	/// \param vLocalOrigin local origin in global screen coordinates
	CGE_Group( std::vector<CGraphicsElement *>& rvecpElement, Vector2 vLocalOrigin );

	virtual ~CGE_Group();

	virtual void Draw();

	Vector2 GetTopLeftPos() const { return GetLocalOriginInGlobalCoord(); }

	void SetLocalTopLeftPos( Vector2 vPos );

	void SetLocalTopLeftPos( SPoint pos ) { SetLocalTopLeftPos( Vector2((float)pos.x,(float)pos.y) ); }

	Vector2 GetLocalOriginInLocalCoord() const { return m_vLocalTopLeftPos;/* m_vLocalOrigin;*/ }

	Vector2 GetLocalOriginInGlobalCoord() const;

	void UpdateGlobalPositions( Vector2 vLocalOrigin );

	/// Updates the global coordinates of grouped elements in this call or later?
	/// - in this call: not good for performance
	/// - later: you need to set a flag such as 'm_bNeedToUpdateGlobalPositionsOfGroupedElements' to true.
	///   Later, before the rendering begins, update global positions of the grouped elements if the flag is true
	/// NOTE: Called from ctor
	inline void SetLocalOrigin( Vector2 vLocalOrigin );

	void SetLocalOrigin( SPoint local_origin ) { SetLocalOrigin( Vector2((float)local_origin.x,(float)local_origin.y) ); }

	virtual void SetTopLeftPosInternal( Vector2 vPos );

	virtual void ChangeScale( float scale );

	virtual void ChangeElementScale( float scale );

	virtual void SetSizeLTRB( const Vector2& vMin, const Vector2& vMax );

	virtual void SetColor( int color_index, const SFloatRGBAColor& color );

	virtual void SetAlpha( int color_index, float a );

	virtual int GetElementType() const { return TYPE_GROUP; }

	inline void RemoveElementFromGroup( CGraphicsElement *pElement );

	std::vector<CGraphicsElement *>& GetElementBuffer() { return m_vecpElement; }
};


inline void CGE_Group::UpdateAABB()
{
	m_AABB.Nullify();
	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
		m_AABB.MergeAABB( (*itr)->GetAABB() );
	}
}


inline void CGE_Group::SetLocalOrigin( Vector2 vLocalOrigin )
{
//	m_vLocalOrigin = vLocalOrigin;
	m_vLocalTopLeftPos = vLocalOrigin;

	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
		(*itr)->UpdateGlobalPositions( GetLocalOriginInGlobalCoord() );
	}
}


inline void CGE_Group::RemoveElementFromGroup( CGraphicsElement *pElement )
{
	// remove the arg element from this group
	// - does not release the element
	std::vector<CGraphicsElement *>::iterator itr = std::find( m_vecpElement.begin(), m_vecpElement.end(), pElement );
	if( itr != m_vecpElement.end() )
		m_vecpElement.erase( itr );
	else
		LOG_PRINT_ERROR( "Failed to find a graphics element in the list of its owner group element." );
}

// provides callback mechanism invoked when an element is created / destroyed
// - used by animated graphics manager to remove effects when the targt element is removed
class CGraphicsElementManagerCallback
{
public:

	CGraphicsElementManagerCallback() {}
	virtual ~CGraphicsElementManagerCallback() {}

	virtual void OnCreated( CGraphicsElement *pElement ) {}

	/// called right before the element is released in CGraphicsElementManager::RemoveElement()
	virtual void OnDestroyed( CGraphicsElement *pElement ) {}
};


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
	virtual CGE_Text *CreateText( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 ) { return NULL; }
	virtual CGE_Text *CreateTextBox( int font_id, const std::string& text, const SRect& textbox, int align_h, int align_v, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 ) { return NULL; }
	virtual CGraphicsElement *CreateTriangle( Vector2 *pVertex, const SFloatRGBAColor& color, int layer = 0 ) { return NULL; }
	virtual CGraphicsElement *CreatePolygon() { return NULL; }
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

	/// \param font_w, font_h font size. default size is used when set to zero or omitted.
	/// Default font size is the size of font obtained from element manager by the font id of the text element
	CGE_Text *CreateText( int font_id, const std::string& text, float x, float y, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 );

	/// \param font_w, font_h same as CreateText()
	CGE_Text *CreateTextBox( int font_id, const std::string& text, const SRect& textbox, int align_h, int align_v, const SFloatRGBAColor& color, int font_w = 0, int font_h = 0, int layer = 0 );

	/// NOT IMPLEMENTED
	CGraphicsElement *CreatePolygon();

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



/*
class CGE_FrameRect : public CGraphicsElement
{
	C2DFrameRect m_FrameRect; ///< scaled rect with scaled border width

public:

	CGE_FrameRect( const C2DFrameRect& framerect, const SFloatRGBAColor& color0 )
		: m_FrameRect(framerect)
	{
		m_AABB = AABB2(framerect.GetCornerPos2D(0),framerect.GetCornerPos2D(2));
		m_OrigBorderWidth = framerect.GetBorderWidth();
		m_aColor[0] = color0;
		ChangeScale( m_fScale );
	}

//	virtual void Draw();
	virtual void Draw()
	{
		SFloatRGBAColor blended_color = GetBlendedColor();
		m_FrameRect.SetColor( blended_color.GetARGB32() );

		m_FrameRect.Draw();
	}

	virtual void SetTopLeftPosInternal( Vector2 vPos )
	{
		Vector2 vSpan = m_AABB.vMax - m_AABB.vMin;
		m_AABB.vMin = vPos;
		m_AABB.vMax = vPos + vSpan;
		m_FrameRect.SetPosition( m_AABB.vMin * m_fScale, m_AABB.vMax * m_fScale );
	}

	virtual void ChangeScale( float scale )
	{
		m_fScale = scale;
		m_FrameRect.SetPosition( m_AABB.vMin * scale, m_AABB.vMax * scale );
		m_FrameRect.SetBorderWidth( (int)(m_OrigBorderWidth * scale) );
	}

	/// does not change border width
	virtual void SetSizeLTRB( const Vector2& vMin, const Vector2& vMax )
	{
		m_AABB.vMin = vMin;
		m_AABB.vMax = vMax;
		m_FrameRect.SetPosition( m_AABB.vMin * m_fScale, m_AABB.vMax * m_fScale );
	}

	virtual int GetElementType() const { return TYPE_FRAMERECT; }
};
*/



#endif  /* __GraphicsElementManager_H__ */
