#include "GraphicsElements.hpp"
#include "GraphicsElementManager.hpp"
#include "Graphics/Font/Font.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Graphics/Font/TrueTypeTextureFont.hpp"
#include "Graphics/2DPrimitive/2DPolygon.hpp"
#include "Support/Macro.h"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"

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


void CGraphicsElement::UpdateTransform( const Matrix23& parent_transform )
{
	m_ParentTransform = parent_transform;

	Matrix23 global_transform = CalculateGlobalTransform();
	if( global_transform.matOrient == Matrix22Identity() )
	{
		// no rotation
		// - Need to update only the translation
		UpdatePositionsInternalForNonRotatedElement( global_transform.vPosition );
	}
	else
	{
		// The global transform includes rotation.
		UpdatePositionsInternal( global_transform );
	}

}


Matrix23 CGraphicsElement::CalculateGlobalTransform()
{
	return m_ParentTransform * m_LocalTransform;
}

/*
void CGraphicsElement::SetTopLeftPos( Vector2 vGlobalPos )
{
	// local position of this element = (the inverse of the parent transform) * global position
	m_ParentTransform.InvTransform( m_vLocalTopLeftPos, vGlobalPos );

	UpdateTopLeftPos( vGlobalPos );

	// Check just in case
	if( 0 <= m_GroupID )
	{
		// Oh, boy... This is owned by a group element
		// - It's not really desirable to call SetTopLeftPos() of a grouped element.
		//   SetLocalTopLeftPos() should be used instead.
		// - Anyway, need to update the local position as well
		CGE_Group *pOwner = dynamic_cast<CGE_Group *>(m_pManager->GetElement(m_GroupID));
		if( pOwner )
			m_vLocalTopLeftPos = vGlobalPos - pOwner->GetLocalOriginInGlobalCoord();
	}

}
*/

void CGraphicsElement::SetLocalTopLeftPos( Vector2 vLocalPos )
{
	m_vLocalTopLeftPos = vLocalPos;

	Vector2 vSpan = m_LocalAABB.vMax - m_LocalAABB.vMin;
	m_LocalAABB.vMin = vLocalPos;
	m_LocalAABB.vMax = m_LocalAABB.vMin + vSpan;

	UpdateTransform( m_ParentTransform );
}


void CGraphicsElement::SetSizeLTRB( const Vector2& vLocalMin, const Vector2& vLocalMax )
{
	m_LocalAABB.vMin = vLocalMin;
	m_LocalAABB.vMax = vLocalMax;

	UpdateTransform( m_ParentTransform );
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


/// Use this if the primitive is not rotated ( CalculateGlobalTransform().matOrient == Matrix22Identity() ).
void CGE_Primitive::UpdatePositionsInternalForNonRotatedElement( const Vector2& vGlobalTranslation )
{
	m_pPrimitive->SetPosition(
		( vGlobalTranslation + m_LocalAABB.vMin ) * m_fScale,
		( vGlobalTranslation + m_LocalAABB.vMax ) * m_fScale
		);
}


void CGE_Primitive::SetTopLeftPosInternal( Vector2 vPos )
{
	// m_LocalAABB has been updated in CGraphicsElement::SetTopLeftPos().
	// - update the vertex positions
//	m_pPrimitive->SetPosition( m_LocalAABB.vMin * m_fScale, m_LocalAABB.vMax * m_fScale );

	UpdatePositionsInternal( m_ParentTransform * m_LocalTransform );
}


void CGE_Primitive::ChangeScale( float scale )
{
	m_fScale = scale;

	UpdatePositionsInternal( m_ParentTransform * m_LocalTransform );
//	m_pPrimitive->SetPosition( m_LocalAABB.vMin * scale, m_LocalAABB.vMax * scale );
}



//==========================================================================================
// CGE_Rect
//==========================================================================================

CGE_Rect::CGE_Rect( /*const AABB2& non_scaled_aabb,*/ const SFloatRGBAColor& color0, C2DPrimitive *pPrimitive )
:
CGE_Primitive(pPrimitive)
{
//	m_LocalAABB = AABB2( pPrimitive->GetPosition2D(0), pPrimitive->GetPosition2D(2) );
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


void CGE_Rect::SetLocalTransform( const Matrix23& local_transform )
{
	CGraphicsElement::SetLocalTransform( local_transform );

	UpdatePositionsInternal( m_ParentTransform * local_transform );
}


void CGE_Rect::UpdatePositionsInternal( const Matrix23& global_transform )
{
	const float s = m_fScale;
	Vector2 v;
//	Vector2 vCenter = GetLocalRotationCenterPos();
//	Vector2 vScaledCenter = vCenter * s;
	Vector2 vMin = m_LocalAABB.vMin;// - vCenter;
	Vector2 vMax = m_LocalAABB.vMax;// - vCenter;
	Vector2 avVertPos[] =
	{
		Vector2( vMin.x, vMin.y ),
		Vector2( vMax.x, vMin.y ),
		Vector2( vMax.x, vMax.y ),
		Vector2( vMin.x, vMax.y )
	};

	for( int i=0; i<4; i++ )
	{
		v = global_transform * avVertPos[i];

		m_pPrimitive->SetVertexPosition( i, v * s );
	}
/*
	if( fabs(m_fRotationAngle) < 0.0001 )
	{
		m_pPrimitive->Rotate( deg_to_rad(m_fRotationAngle) );
	}
	else
	{
		m_pPrimitive->Rotate( deg_to_rad(m_fRotationAngle) );
	}*/
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
	m_LocalAABB = AABB2( Vector2((float)non_scaled_rect.left,(float)non_scaled_rect.top), Vector2((float)non_scaled_rect.right,(float)non_scaled_rect.bottom) );

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


void CGE_Triangle::SetVertexPosition( int index, const Vector2& vPos )
{
//	m_pPrimitive->SetVertexPosition( index, vPos * m_fScale );

	UpdatePositionsInternal( m_ParentTransform * m_LocalTransform );
}


void CGE_Triangle::UpdatePositionsInternal( const Matrix23& global_transform )
{
//	if( m_pTriangle )
//		for( int i=0; i<3; i++ ) m_pTriangle->SetVertexPosition( i, global_transform * m_avVertexPosition[i] );
//	else if( m_pFTriangle )
//		for( int i=0; i<3; i++ ) m_pTriangle->SetVertexPosition( i, global_transform * m_avVertexPosition[i] );
//	else if( m_RFpTriangle )
//		for( int i=0; i<3; i++ ) m_pTriangle->SetVertexPosition( i, global_transform * m_avVertexPosition[i] );
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


void CGE_Polygon::SetRadius( int vertex, int radius )
{
	m_pRegularPolygon->SetRadius( vertex, (int)(radius * m_fScale) );
}


Vector2 CGE_Polygon::GetVertexPos( int vertex )
{
	return m_pRegularPolygon->GetVertexPos( vertex ) / m_fScale;
}


void CGE_Polygon::SetVertexColor( int vertex, int color_index, const SFloatRGBAColor& color )
{
	m_pRegularPolygon->SetVertexColor( vertex, color );
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
	m_LocalAABB.vMin = m_vTextPos;
	m_TextAlignH = TAL_LEFT;
	m_TextAlignV = TAL_TOP;
	m_aColor[0] = color0;
	ChangeScale( m_fScale );

//		m_LocalAABB.vMax = 
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


void CGE_Text::UpdateLocalTextOffset()
{
	CFontBase *pFont = m_pManager->GetFont( m_FontID );
	if( !pFont )
		return;

	Vector2 vDiagonal = m_LocalAABB.vMax - m_LocalAABB.vMin;
	int box_width  = (int)vDiagonal.x;
	int box_height = (int)vDiagonal.y;
	const float scale = m_fScale;

	// non-scaled font width & height
	int font_width  = 0 < m_FontWidth  ? m_FontWidth  : ( pFont->GetFontWidth()  / scale ); 
	int font_height = 0 < m_FontHeight ? m_FontHeight : ( pFont->GetFontHeight() / scale );
	const int text_length = (int)m_Text.length();
	float font_scale = 1.0f;
	Vector2 vLocalTextOffset = Vector2(0,0);

	switch(m_TextAlignH)
	{
	case CGE_Text::TAL_LEFT:
		vLocalTextOffset.x = 0;
		break;
	case CGE_Text::TAL_CENTER:
		font_scale = font_width / ((float)pFont->GetFontWidth() / scale);
		vLocalTextOffset.x = (float)( box_width/2  - (pFont->GetTextWidth(m_Text.c_str())/scale) * font_scale / 2 );
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

	m_vLocalTextOffset = vLocalTextOffset;
}


void CGE_Text::UpdateTextAlignment()
{
	UpdateLocalTextOffset();

	Matrix23 global_transform = CalculateGlobalTransform();

	// TODO: support rotation
	m_vTextPos = m_vLocalTextOffset + m_LocalAABB.vMin + global_transform.vPosition;

	m_vScaledPos = m_vTextPos * m_fScale;
}


/// TODO: support rotation
void CGE_Text::UpdatePositionsInternal( const Matrix23& global_transform )
{
	UpdatePositionsInternalForNonRotatedElement( global_transform.vPosition );
}


void CGE_Text::UpdatePositionsInternalForNonRotatedElement( const Vector2& vGlobalTranslation )
{
	Vector2 vSpan = m_LocalAABB.vMax - m_LocalAABB.vMin;
//	m_FontWidth  = (int)( vSpan.x / (float)m_Text.length() ); 
//	m_FontHeight = (int)vSpan.y;// vSpan.x / (float)GetNumRows(m_Text)

	m_vTextPos = m_vLocalTextOffset + m_LocalAABB.vMin + vGlobalTranslation;

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
}


Vector2 CGE_Group::GetLocalOriginInGlobalCoord() const
{
	return m_ParentTransform * m_LocalTransform * GetLocalTopLeftPos();
/*
	if( 0 <= m_GroupID )
	{
		CGE_Group *pParentGroup = dynamic_cast<CGE_Group *>(m_pManager->GetElement( m_GroupID ));
		if( pParentGroup )
			return pParentGroup->GetLocalOriginInGlobalCoord() + GetLocalTopLeftPos();
		else
			return GetLocalTopLeftPos();
	}
	else
	{
		// not owned by any group
		// - i.e.) m_vLocalOrigin is in global coord
		return GetLocalTopLeftPos();
	}*/
}


void CGE_Group::SetLocalOrigin( Vector2 vLocalOrigin )
{
//	m_vLocalOrigin = vLocalOrigin;
	m_vLocalTopLeftPos = vLocalOrigin;

	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
//		(*itr)->UpdateGlobalPositions( GetLocalOriginInGlobalCoord() );
		(*itr)->UpdateTransform(
			m_ParentTransform
		  * m_LocalTransform
		  * Matrix23( vLocalOrigin, Matrix22Identity() )
		  );
	}
}


void CGE_Group::SetLocalTopLeftPos( Vector2 vPos )
{
	SetLocalOrigin( vPos );
}


void CGE_Group::UpdateTransform( const Matrix23& parent_transform )
{
	m_ParentTransform = parent_transform;

	Matrix23 global_transform
		= m_ParentTransform
		* m_LocalTransform
		* Matrix23( GetLocalTopLeftPos(), Matrix22Identity() );

//	UpdatePositionsInternal( global_transform );

	vector<CGraphicsElement *>::iterator itr;
	for( itr = m_vecpElement.begin(); itr != m_vecpElement.end(); itr++ )
	{
		(*itr)->UpdateTransform( global_transform );
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
	m_LocalAABB.vMin = vMin;
	m_LocalAABB.vMax = vMax;

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
	//   - rationale: elements in a group might belong to different layers, and in such a case
	//                rendering order need to be managed the same way as non-grouped elements
}
