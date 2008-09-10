#include "GraphicsEffectManager.h"
#include "GraphicsElementManager.h"
#include "GameCommon/Timer.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Vec3_StringAux.h"

#include <algorithm>
using namespace std;


/**
- Releasing effects
  - Certain effects are not released automatically and the user is responsible for releasing them.
    Those are,
    - Non-linear effects created with DONT_RELEASE flag
      - Linear effects cannot be created with DONT_RELEASE flag
    - Looped effects
      - They keep repeating the same effects until the user explicitly releases them.

- Multi-thread issues
  - The following functions must be called by a single thread
    - AnimatedGraphicsManager::UpdateEffects()
	- CGraphicsEffectHandle::Set*()
    - 
*/


const CGraphicsEffectHandle CGraphicsEffectHandle::ms_NullHandle;


void CGraphicsEffectHandle::SetDestPosition( const Vector2& vDestPos )
{
	CGraphicsElementEffect *pEffect = m_pManager->GetEffect(*this);
	if( pEffect )
		pEffect->SetDestPosition( vDestPos );
}


void CGraphicsEffectHandle::SetDestColor( const SFloatRGBAColor& dest_color )
{
//	CGraphicsElementEffect *pEffect = m_pManager->GetEffect(*this);
//	if( pEffect )
//		pEffect->ChangeDestColor( dest_color );
}


void CGraphicsEffectHandle::ChangeDestVertexTexCoord( int vertex, const TEXCOORD2& tex_coord )
{
//	CGraphicsElementEffect *pEffect = m_pManager->GetEffect(*this);
//	if( pEffect )
//		pEffect->ChangeDestVertexTexCoord( vertex, tex_coord );
}


void CE_ColorShift::Update( double current_time, double dt )
{
	if( current_time < m_fStartTime )
		return;
	else if( m_fEndTime < current_time )
	{
		if( !m_bAppliedAtEndTime )
		{
			// set the color at the end time and mark the effect as done
			GetElement()->SetColor( m_ColorIndex, m_EndColor );
			m_bAppliedAtEndTime = true;
		}
		return;
	}

	// currently in the animation time frame
	if( !m_bInitialized )
	{
		m_StartColor = GetElement()->GetColor( m_ColorIndex );
		m_bInitialized = true;
	}

	float frac = (float)GetFraction_Linear( current_time );
	if( m_TargetChannels == COMPONENTS_RGBA )
		GetElement()->SetColor( m_ColorIndex, m_StartColor + (m_EndColor - m_StartColor) * frac );
	else
		GetElement()->SetColor( m_ColorIndex, m_StartColor.GetRGBColor() + (m_EndColor.GetRGBColor() - m_StartColor.GetRGBColor()) * frac );

//		GetElement()->SetColor( Interpolate_Linear( m_StartColor, m_EndColor, current_time ) );
}


void CE_AlphaChange::Update( double current_time, double dt )
{
	if( current_time < m_fStartTime )
		return;
	else if( m_fEndTime < current_time )
	{
		if( !m_bAppliedAtEndTime )
		{
			GetElement()->SetAlpha( m_ColorIndex, m_fEndAlpha );
			m_bAppliedAtEndTime = true;
		}
		return;
	}

	GetElement()->SetAlpha( m_ColorIndex, m_fStartAlpha + (m_fEndAlpha - m_fStartAlpha) * (float)GetFraction_Linear( current_time ) );
}


void CE_Translate::Update( double current_time, double dt )
{
	if( current_time < m_fStartTime )
		return;
	else if( m_fEndTime < current_time )
	{
		if( !m_bAppliedAtEndTime )
		{
			GetElement()->SetTopLeftPos( m_vEnd );
			m_bAppliedAtEndTime = true;
		}
		return;
	}

	// currently in the animation time frame

	GetElement()->SetTopLeftPos( m_vStart + (m_vEnd - m_vStart) * (float)GetFraction_Linear( current_time ) );
}


void CE_SizeChange::Update( double current_time, double dt )
{
	if( current_time < m_fStartTime )
		return;
	else if( m_fEndTime < current_time )
	{
		if( !m_bAppliedAtEndTime )
		{
			GetElement()->SetSizeLTRB( m_End.vMin, m_End.vMax );
			m_bAppliedAtEndTime = true;
		}
		return;
	}

	// currently in the animation time frame

	double f = GetFraction_Linear( current_time );

	Vector2 vMin = m_Start.vMin + ( m_End.vMin - m_Start.vMin ) * (float)f;
	Vector2 vMax = m_Start.vMax + ( m_End.vMax - m_Start.vMax ) * (float)f;
	GetElement()->SetSizeLTRB( vMin, vMax );
}


void CE_TextDraw::Update( double current_time, double dt )
{
	if( m_FadeLength == 0 )
	{
		// no fade effect
		// - update the text

		int num_current_chars = (int)( m_CharsPerSec * ( current_time - m_fStartTime ) );

		if( num_current_chars < 0 )
			num_current_chars = 0;

		m_pTextElement->SetText( m_OrigText.substr(0,num_current_chars) );

		if( (int)m_OrigText.length() <= num_current_chars )
		{
			// rendered the complete original text
			// - terminate the effect
			m_bAppliedAtEndTime = true;
		}
	}
	else
	{
//		m_pTextElement->SetCharColorBuffer(  );
	}
}


void CE_NonLinearVertexColorShift::Update( double current_time, double dt )
{
	if( current_time < m_fStartTime )
		return;

	SFloatRGBAColor diff = m_Color.target - m_Color.current;
	if( diff.fRed   < 0.001f
	 && diff.fGreen < 0.001f
	 && diff.fBlue  < 0.001f
	 && diff.fAlpha < 0.001f )
		return;

	m_Color.Update( (float)dt );

//	LOG_PRINT_CAUTION( to_string(current_time) + " - " + to_string(m_Pos.current) );

	m_pPolygonElement->SetVertexColor( m_VertexIndex, m_ColorIndex, m_Color.current );
}


void CE_NonLinearVertexColorShift::SetDestColor( const SFloatRGBAColor& dest_color )
//void CE_NonLinearVertexColorShift::SetDestVertexColor( int color_index, const SFloatRGBAColor& dest_color )
{
	// What to do with the color index
	// - Already determined when the effect is created
	// - Should not be included in the arguments
	m_Color.target = dest_color;
}


//==========================================================================
// CE_TranslateNonLinear
//==========================================================================

void CE_TranslateNonLinear::Update( double current_time, double dt )
{
	if( current_time < m_fStartTime )
		return;
	else if( Vec2LengthSq( m_Pos.target - m_Pos.current ) < 0.001f )
		return;
/*	else if( m_fEndTime < current_time )
	{
		GetElement()->SetTopLeftPos( m_Pos.current );
		return;
	}
*/
	// currently in the animation time frame

	m_Pos.Update( (float)dt );

//	LOG_PRINT_CAUTION( to_string(current_time) + " - " + to_string(m_Pos.current) );

	GetElement()->SetTopLeftPos( m_Pos.current );
}


bool CE_TranslateNonLinear::IsOver( double current_time ) const
{
	return Vec2LengthSq( m_Pos.target - m_Pos.current ) < 0.1f;
}


void CE_SizeChangeCD::Update( double current_time, double dt )
{
	if( current_time < m_fStartTime )
		return;
	else if( Vec2LengthSq( m_vMin.target - m_vMin.current ) < 0.001f 
		&& Vec2LengthSq( m_vMax.target - m_vMax.current ) < 0.001f )
		return;

	// currently in the animation time frame

	m_vMin.Update( (float)dt );
	m_vMax.Update( (float)dt );

	GetElement()->SetSizeLTRB( m_vMin.current, m_vMax.current );
}


bool CE_SizeChangeCD::IsOver( double current_time ) const
{
	return ( Vec2LengthSq( m_vMin.target - m_vMin.current ) < 0.1f
		&& Vec2LengthSq( m_vMax.target - m_vMax.current ) < 0.1f );
}


void CE_AlphaBlink::Update( double current_time, double dt )
{
}


class CGraphiceEffectManagerCallback : public CGraphicsElementManagerCallback
{
	/// borrowed reference
	CAnimatedGraphicsManager *m_pEffectMgr;

public:

	CGraphiceEffectManagerCallback( CAnimatedGraphicsManager *pEffectMgr ) : m_pEffectMgr(pEffectMgr) {}
	virtual ~CGraphiceEffectManagerCallback() {}

//	virtual void OnCreated( CGraphicsElement *pElement ) {}

	/// delete graphics effects which are currently being apllied to the deleted element
	virtual void OnDestroyed( CGraphicsElement *pElement );

};


void CGraphiceEffectManagerCallback::OnDestroyed( CGraphicsElement *pElement )
{
	if( m_pEffectMgr->m_vecpEffect.size() == 0 )
		return;

	vector<CGraphicsElementEffect *>::iterator itr = m_pEffectMgr->m_vecpEffect.begin();

	while( itr != m_pEffectMgr->m_vecpEffect.end() )
	{
		if( (*itr) && (*itr)->GetElement() == pElement )
		{
			// target element is being released
			// - release the effect as well
			m_pEffectMgr->RemoveEffect( (*itr)->GetIndex() );
		}

		itr++;
	}
}


CGraphicsElement *CGraphicsElementEffect::GetElement()
{
	return m_pTargetElement;
//	return m_pManager->GetGraphicsElementManager()->GetElement(m_TargetElementID);
}


//=====================================================================
// CAnimatedGraphicsManager
//=====================================================================

//CAnimatedGraphicsManager::CAnimatedGraphicsManager( CGraphicsElementManager *pElementManager )
//:
//m_pGraphicsElementManager(pElementManager)
CAnimatedGraphicsManager::CAnimatedGraphicsManager()
{
	m_pTimer = new CTimer();
	m_pTimer->Start();

	SetTimeOffset();

	m_pGraphicsElementManager = new CGraphicsElementManager();

	// register callback that releases the effect when its target element gets released
	CGraphicsElementManagerCallbackSharedPtr pCallback
		= CGraphicsElementManagerCallbackSharedPtr( new CGraphiceEffectManagerCallback( this ) );

	m_pGraphicsElementManager->SetCallback( pCallback );

	m_NextGraphicsEffectID = 0;

	// for vector<CGraphicsElementEffect *>
	m_vecpEffect.resize( 32, NULL );
	int num_effects = (int)m_vecpEffect.size();
	for( int i=num_effects-1; 0<=i; i-- )
	{
		m_vecVacantSlotIndex.push_back( i );
	}
}


void CAnimatedGraphicsManager::Release()
{
//	SafeDeleteVector( m_vecpEffect );
	vector<CGraphicsElementEffect *>::iterator itr;
	for( itr = m_vecpEffect.begin();
		 itr != m_vecpEffect.end();
		 itr++ )
	{
		SafeDelete( *itr );
	}

	SafeDelete( m_pGraphicsElementManager );

	SafeDelete( m_pTimer );
}


void CAnimatedGraphicsManager::SetTimeOffset( double time )
{
	if( time < 0 )
		m_fTimeOffset = m_pTimer->GetTime();
	else
		m_fTimeOffset = time;
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeAlpha( CGraphicsElement *pTargetElement,
											double start_time,  double end_time,
											int color_index, float start_alpha, float end_alpha,
											int trans_mode )
{
	CE_AlphaChange *p = new CE_AlphaChange( pTargetElement, m_fTimeOffset + start_time, m_fTimeOffset + end_time );
	p->m_ColorIndex = color_index;
	p->m_fStartAlpha = start_alpha;
	p->m_fEndAlpha   = end_alpha;

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeAlphaTo( CGraphicsElement *pTargetElement,
											  double start_time, double end_time,
											  int color_index, float end_alpha,
											  int trans_mode )
{
	return ChangeAlpha( pTargetElement, start_time, end_time,
		color_index,
		pTargetElement->GetAlpha(color_index),
		end_alpha,
		trans_mode );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeColor( CGraphicsElement *pTargetElement,
											double start_time, double end_time,
											int color_index,
											const SFloatRGBAColor& start_color,
											const SFloatRGBAColor& end_color,
											int trans_mode )
{
	CE_ColorShift *p = new CE_ColorShift( pTargetElement, m_fTimeOffset + start_time, m_fTimeOffset + end_time );
	p->m_ColorIndex = color_index;
	p->m_StartColor = start_color;
	p->m_EndColor   = end_color;

	p->SetTargetChannels( CE_ColorShift::COMPONENTS_RGBA );

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeColor( CGraphicsElement *pTargetElement,
											double start_time, double end_time,
											int color_index,
											const SFloatRGBColor& start_color,
											const SFloatRGBColor& end_color,
											int trans_mode )
{
	CE_ColorShift *p = new CE_ColorShift( pTargetElement, m_fTimeOffset + start_time, m_fTimeOffset + end_time );

	p->m_ColorIndex = color_index;

	p->m_StartColor.fRed   = start_color.fRed;
	p->m_StartColor.fGreen = start_color.fGreen;
	p->m_StartColor.fBlue  = start_color.fBlue;

	p->m_EndColor.fRed   = end_color.fRed;
	p->m_EndColor.fGreen = end_color.fGreen;
	p->m_EndColor.fBlue  = end_color.fBlue;

	// do not change alpha component
	p->SetTargetChannels( CE_ColorShift::COMPONENTS_RGB );

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeColorTo( CGraphicsElement *pTargetElement,
											  double start_time, double end_time,
											  int color_index, const SFloatRGBAColor& end_color,
											  int trans_mode )
{
	return ChangeColor( pTargetElement, start_time, end_time,
		color_index,
		pTargetElement->GetColor(color_index),
		end_color,
		trans_mode );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeColorTo( CGraphicsElement *pTargetElement,
											  double start_time, double end_time,
											  int color_index, const SFloatRGBColor& end_color,
											  int trans_mode )
{
	return ChangeColor( pTargetElement, start_time, end_time,
		color_index,
		pTargetElement->GetColor(color_index).GetRGBColor(),
		end_color,
		trans_mode );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeColorTo( CGraphicsElement *pTargetElement,
											  double start_time, double end_time,
											  int color_index, U32 end_color,
											  int trans_mode )
{
	SFloatRGBAColor dest_end_color;
	dest_end_color.SetARGB32( end_color );

	return ChangeColorTo( pTargetElement, start_time, end_time,
		color_index, dest_end_color,
		trans_mode );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::TranslateNonLinear( CGraphicsElement *pTargetElement,
											 double start_time,
											 Vector2 vDestPos,
											 Vector2 vInitVel,
											 float smooth_time,
											 int coord_type,
											 U32 flags )
{
//	LOG_PRINT_CAUTION( to_string(vDestPos) );

	switch( coord_type )
	{
	case CGraphicsElementEffect::COORD_CENTER:
//		dest_x = dest_x - element.GetWidth() * 0.5f;
//		dest_y = dest_y - element.GetHeight() * 0.5f;
		break;
	case CGraphicsElementEffect::COORD_TOPLEFT:
	default:
		break;
	}

	CE_TranslateNonLinear *p = new CE_TranslateNonLinear( pTargetElement, m_fTimeOffset + start_time, m_fTimeOffset + start_time + 1000.0 );
	p->m_Pos.current = pTargetElement->GetTopLeftPos();
	p->m_Pos.target  = vDestPos;
	p->m_Pos.vel     = vInitVel;
	p->m_Pos.smooth_time = smooth_time;
	p->SetFlags( flags );

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::TranslateTo( CGraphicsElement *pTargetElement,
											double start_time, double end_time,
											Vector2 vDestPos,
											int coord_type, int trans_mode )
{
//	Move( id, start_time, end_time,
//		GraphicsElementManager.GetElement()->GetPos( id ),
//		dest_pos,
//		trans_mode );

	switch( coord_type )
	{
	case CGraphicsElementEffect::COORD_CENTER:
//		dest_x = dest_x - element.GetWidth() * 0.5f;
//		dest_y = dest_y - element.GetHeight() * 0.5f;
		break;
	case CGraphicsElementEffect::COORD_TOPLEFT:
	default:
		break;
	}

	CE_Translate *p = new CE_Translate( pTargetElement, m_fTimeOffset + start_time, m_fTimeOffset + end_time );
	p->m_vStart = pTargetElement->GetTopLeftPos();
	p->m_vEnd   = vDestPos;

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeSize( CGraphicsElement *pTargetElement,
										  double start_time, double end_time,
											 Vector2 vDestMin, Vector2 vDestMax )
{
	CE_SizeChange *p = new CE_SizeChange( pTargetElement, m_fTimeOffset + start_time, m_fTimeOffset + end_time );
	p->m_Start = pTargetElement->GetAABB();
	p->m_End   = AABB2( vDestMin, vDestMax );

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ScaleTo( CGraphicsElement *pTargetElement, double start_time, double end_time, float end_scale )
{
	CE_Scale *p = new CE_Scale( pTargetElement, m_fTimeOffset + start_time, m_fTimeOffset + end_time );
//	p->m_vStart = pTargetElement->GetTopLeftPos();
//	p->m_vEnd   = vDestPos;

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::SetPosition( CGraphicsElement *pTargetElement, double time, const Vector2& vPos )
{
	return CGraphicsEffectHandle::Null();
}


CGraphicsEffectHandle CAnimatedGraphicsManager::SetColor( CGraphicsElement *pTargetElement, double time, int color_index, const SFloatRGBAColor& color )
{
	if( !pTargetElement )
		return CGraphicsEffectHandle::Null();

	double time_offset = m_fTimeOffset;
	time = time_offset + time;

	CE_ColorShift *p = new CE_ColorShift( pTargetElement, time, time );
	p->m_ColorIndex = color_index;
	p->m_StartColor = pTargetElement->GetColor(color_index);
	p->m_EndColor = color;

	return AddGraphicsEffect( p );
}


CE_TextDraw::CE_TextDraw( CGE_Text *pTargetElement, double start_time )
:
CGraphicsElementLinearEffect( pTargetElement, start_time, 0.0f ), m_CharsPerSec(1), m_FadeLength(0)
{
	m_pTextElement = pTargetElement;

	if( m_pTextElement )
	{
		m_OrigText = m_pTextElement->GetText(); // save the original text

		// clear the text of the target text element
		m_pTextElement->SetText( "" );		 // This causes the memory leak. Why?
		//m_pTextElement->SetText( string() ); // Does not fix the problem. Causes a similar memory leak

//		if( /* currently in animation timeframe */ )
//			UpdateInternal( current_time, 0.0f );
	}
}


CGraphicsEffectHandle CAnimatedGraphicsManager::DrawText( CGE_Text *pTargetTextElement, double start_time, int num_chars_per_sec )
{
	if( !pTargetTextElement )//|| pTargetTextElement->GetType() != CGraphicsElement::TEXT )
		return CGraphicsEffectHandle::Null();

	CE_TextDraw *p = new CE_TextDraw( pTargetTextElement, m_fTimeOffset + start_time );

	p->m_CharsPerSec = (float)num_chars_per_sec;

	p->SetNumCharsPerSec( num_chars_per_sec );

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::ChangeVertexColorNonLinear( CGE_Polygon *pTargetPolygonElement,
																		    double start_time,
																			int color_index,
																			int vertex,
																			const SFloatRGBAColor& dest_color,
																			const SFloatRGBAColor& color_change_velocity )
{
	if( !pTargetPolygonElement )
		return CGraphicsEffectHandle::Null();

	CE_NonLinearVertexColorShift *p = new CE_NonLinearVertexColorShift( pTargetPolygonElement,  m_fTimeOffset + start_time );
	p->m_ColorIndex  = color_index;
	p->m_VertexIndex = vertex;
	p->SetDestColor( dest_color );
	p->m_Color.vel = color_change_velocity;

	return AddGraphicsEffect( p );
}


CGraphicsEffectHandle CAnimatedGraphicsManager::AddGraphicsEffect( CGraphicsElementEffect* pEffect )
{
	pEffect->SetAnimatedGraphicsManager( this );

//	m_vecpEffect.push_back( p ); // for list<CGraphicsElementEffect *>

	// for vector<CGraphicsElementEffect *>

	int index = GetVacantSlotIndex();

	m_vecpEffect[index] = pEffect;

	int id = GetNextGraphicsEffectID();

	pEffect->SetEffectID( id );

	// used in CGraphiceEffectManagerCallback::OnDestroyed()
	pEffect->SetIndex( index );

	return CGraphicsEffectHandle( this, index, id );
}


void CAnimatedGraphicsManager::UpdateEffects( double dt )
{
	const double current_time = m_pTimer->GetTime();

	vector<CGraphicsElementEffect *>::iterator itr;
	for( itr = m_vecpEffect.begin();
		 itr != m_vecpEffect.end();
		 itr++ )
	{
		if( *itr )
			(*itr)->Update( current_time, dt );
	}

//	std::sort( m_vecpEffect.begin(), m_vecpEffect.end() );

	// remove finished effect objects
	size_t i, num_effects = m_vecpEffect.size();
	for( i=0; i<num_effects; i++ )
	{
		if( m_vecpEffect[i]
		 && m_vecpEffect[i]->IsOver( current_time )
		 && !(m_vecpEffect[i]->GetFlags() & CGraphicsElementEffectFlag::DONT_RELEASE) )
		{
			SafeDelete( m_vecpEffect[i] );
			m_vecVacantSlotIndex.push_back( (int)i );
		}
	}
}


bool CAnimatedGraphicsManager::CancelEffect( CGraphicsEffectHandle& effect_handle )
{
	if( GetEffect(effect_handle) )
	{
		RemoveEffect( effect_handle.m_EffectIndex );
		return true;
	}
	else
		return false;
}


CGraphicsElementEffect *CAnimatedGraphicsManager::GetEffect( CGraphicsEffectHandle& effect_handle )
{
	int index = effect_handle.m_EffectIndex;
	if( index < 0 || (int)m_vecpEffect.size() <= index )
		return NULL;

	if( !m_vecpEffect[index] )
		return NULL;

	if( m_vecpEffect[index]->GetEffectID() != effect_handle.m_EffectID )
		return NULL; // ids do not match - the effect has been already released

	return m_vecpEffect[index];
}


/*
void CAnimatedGraphicsManager::UpdateEffects( double dt )
{
	vector<CGraphicsElementEffect *>::iterator itr;
	for( itr = m_vecpEffect.begin();
		 itr != m_vecpEffect.end();
		 itr++ )
	{
		if( *itr )
			(*itr)->Update( (double)m_pTimer->GetTime(), dt );
	}

//	std::sort( m_vecpEffect.begin(), m_vecpEffect.end() );

	const double current_time = m_pTimer->GetTime();

	// remove finished effect objects
	for( itr = m_vecpEffect.begin();
		 itr != m_vecpEffect.end();
		 // do not increment 'itr'  )
	{
		if( *itr && (*itr)->IsOver( current_time ) )
		{
			SafeDelete( *itr ); // TODO: check this: Is this a reference of the actual vector element or a copy?
			itr = m_vecpEffect.erase( itr );
		}
		else
			itr++;
	}
}
*/
