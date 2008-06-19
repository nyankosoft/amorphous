#ifndef  __AnimatedGraphicsManager_H__
#define  __AnimatedGraphicsManager_H__


#include "GraphicsElementManager.h"
#include "GraphicsEffectHandle.h"


#include "../base.h"
#include "3DMath/Vector2.h"
#include "3DMath/Vector3.h"
#include "3DMath/aabb2.h"
#include "3DCommon/fwd.h"
#include "3DCommon/3DGameMath.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/2DFrameRect.h"
#include "3DCommon/2DTriangle.h"
#include "3DCommon/TextureCoord.h"
#include "3DCommon/FloatRGBAColor.h"
#include "3DCommon/FontBase.h"
#include "3DCommon/TextureFont.h"
#include "3DCommon/GraphicsComponentCollector.h"
#include "3DCommon/TextureHandle.h"

#include "GameCommon/fwd.h"
#include "GameCommon/CriticalDamping.h"

#include "Support/FixedVector.h"
#include "Support/Vec3_StringAux.h"
#include "Support/memory_helpers.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <list>
#include <string>


/*
template<class T>
class indexed_object_container<T>
{
	std::vector<T> m_vecContainer;

	std::vector<T> m_vecVacantSlotIndex;
public:

	int push( const T& obj )
	{
		if( 0 < m_vecVacantSlotIndex.size() )
		{
			int index = m_vecVacantSlotIndex.back();
			m_vecContainer[ index ] = obj;

			m_vecVacantSlotIndex.pop_back();

			return index;
		}
		else
		{
			m_vec.push_back( obj );

			return m_vec.back() - 1;
		}

		
	}
};
*/



/**
 * base class for 2d graphics effects
 *
 */
class CGraphicsElementEffectBase
{
protected:

	int m_EffectID;

	double m_fStartTime;
	double m_fEndTime;

	CAnimatedGraphicsManager* m_pManager;

	CGraphicsElement *m_pTargetElement;

	bool m_bInitialized;

	int m_Index; ///< see CGraphiceEffectManagerCallback::OnDestroyed

public:

	CGraphicsElementEffectBase( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: m_EffectID(-1), m_pTargetElement(pTargetElement), m_fStartTime(start_time), m_fEndTime(end_time), m_bInitialized(false), m_Index(-1) {}

	enum TransMode
	{
		TRANS_LINEAR,
		TRANS_CD,
		NUM_TRANS_MODES
	};

	enum CoordType
	{
		COORD_CENTER,
		COORD_TOPLEFT,	///< top-left corner
		NUM_COORD_TYPES
	};

	int GetEffectID() const { return m_EffectID; }

	void SetEffectID( int id ) { m_EffectID = id; }

	int GetIndex() const { return m_Index; }

	void SetIndex( int index ) { m_Index = index; }

	virtual void Update( double current_time, double dt ) = 0;

//	CGraphicsElement *GetElement() { m_pManager->GetGraphicsElementManager()->GetElement(m_TargetElementID); }
	CGraphicsElement *GetElement();

	virtual int GetTransType() const = 0;

	virtual bool IsOver( double current_time ) const { return false; }

	double GetEndTime() const { return m_fEndTime; }

	void SetAnimatedGraphicsManager( CAnimatedGraphicsManager* pMgr ) { m_pManager = pMgr; }

};


class CGraphicsElementEffect_Linear : public CGraphicsElementEffectBase
{
protected:

	bool m_bAppliedAtEndTime;

	inline double GetFraction_Linear( double current_time );

public:

	CGraphicsElementEffect_Linear( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffectBase(pTargetElement, start_time, end_time ), m_bAppliedAtEndTime(false) {}

	virtual int GetTransType() const { return TRANS_LINEAR; }

	virtual bool IsOver( double current_time ) const { return m_fEndTime + 1.0 < current_time && m_bAppliedAtEndTime; }

	bool operator==( const CGraphicsElementEffectBase& effect )
	{
		return m_fEndTime == effect.GetEndTime();
	}

	bool operator<( const CGraphicsElementEffectBase& effect )
	{
		return m_fEndTime < effect.GetEndTime();
	}
};


class CGraphicsElementEffect_CD : public CGraphicsElementEffectBase
{
protected:

public:

	CGraphicsElementEffect_CD( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffectBase(pTargetElement, start_time, end_time ) {}

	virtual int GetTransType() const { return TRANS_CD; }
};


inline double CGraphicsElementEffect_Linear::GetFraction_Linear( double current_time )
{
	double duration = m_fEndTime - m_fStartTime;
	return (current_time - m_fStartTime) / duration;
}


class CE_ColorShift : public CGraphicsElementEffect_Linear
{
	int m_ColorIndex;

	SFloatRGBAColor m_StartColor;
	SFloatRGBAColor m_EndColor;

	int m_TargetChannels;

public:

	CE_ColorShift( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_Linear( pTargetElement, start_time, end_time ),
		m_ColorIndex(0), m_StartColor(SFloatRGBAColor(1,1,1,1)), m_EndColor(SFloatRGBAColor(1,1,1,1)) {}

	enum eTarget
	{
		COMPONENTS_RGB,
		COMPONENTS_RGBA,
		NUM_TARGETS
	};

	void SetTargetChannels( int tgt ) { m_TargetChannels = tgt; }

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};


class CE_AlphaChange : public CGraphicsElementEffect_Linear
{
	int m_ColorIndex;

	float m_fStartAlpha;
	float m_fEndAlpha;

public:

	CE_AlphaChange( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_Linear( pTargetElement, start_time, end_time ),
		m_ColorIndex(0), m_fStartAlpha(0), m_fEndAlpha(0) {}

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};

/*
class CE_FadeIn : public CGraphicsElementEffectBase
{
public:
	void Update( float dt );
};

class CE_FadeOut : public CGraphicsElementEffectBase
{
public:
	void Update( float dt );
};
*/


class CE_Translate : public CGraphicsElementEffect_Linear
{
	Vector2 m_vStart;
	Vector2 m_vEnd;

	int m_TransMode;

//	cdv<Vector2> m_Pos;

public:

	CE_Translate( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_Linear( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};


class CE_SizeChange : public CGraphicsElementEffect_Linear
{
	AABB2 m_Start;
	AABB2 m_End;

public:

	CE_SizeChange( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_Linear( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};


class CE_Rotate : public CGraphicsElementEffect_Linear
{
	float m_fStartAngle;
	float m_fEndAngle;

public:

	CE_Rotate( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_Linear( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt )
	{
		if( current_time < m_fStartTime )
			return;
		else if( m_fEndTime < current_time )
		{
			if( !m_bAppliedAtEndTime )
			{
//				GetElement()->SetOrientation( m_vEnd );
				m_bAppliedAtEndTime = true;
			}
			return;
		}
	}

	friend class CAnimatedGraphicsManager;
};


class CE_TextDraw : public CGraphicsElementEffect_Linear
{
	std::string m_OrigText;

	float m_CharsPerSec;

	/// how long the colors spans from solid color to fade out
	int m_FadeLength;

	CGE_Text *m_pTextElement;

public:

	CE_TextDraw( CGraphicsElement *pTargetElement, double start_time )
	:
	CGraphicsElementEffect_Linear( pTargetElement, start_time, 0.0f ), m_CharsPerSec(1), m_FadeLength(0)
	{
		m_pTextElement = dynamic_cast<CGE_Text *>(pTargetElement);

		if( m_pTextElement )
		{
			m_OrigText = m_pTextElement->GetText(); // save the original text

			// clear the text of the target text element
			m_pTextElement->SetText( "" );

//			if( /* currently in animation timeframe */ )
//				UpdateInternal( current_time, 0.0f );
		}
	}

	void SetNumCharsPerSec( int num_chars_per_sec ) { m_CharsPerSec = (float)num_chars_per_sec; }

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};



class CE_Scale : public CGraphicsElementEffect_Linear
{
	float m_fStartScale;
	float m_fEndScale;

	Vector2 m_vCenter;

public:

	CE_Scale( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_Linear( pTargetElement, start_time, end_time ) { m_vCenter = Vector2(0,0); }

	virtual void Update( double current_time, double dt )
	{
		if( current_time < m_fStartTime )
			return;
		else if( m_fEndTime < current_time )
		{
			if( !m_bAppliedAtEndTime )
			{
//				GetElement()->SetScale( m_vEnd );
				m_bAppliedAtEndTime = true;
			}
			return;
		}

		// currently in the animation time frame

		double duration = m_fEndTime - m_fStartTime;
		double f = (current_time - m_fStartTime) / duration;

//		GetElement()->ChangeElementScale( m_vCenter, m_fStartScale + (m_fEndScale - m_fStartScale) * f );
	}

	friend class CAnimatedGraphicsManager;
};


class CE_TranslateCD : public CGraphicsElementEffect_CD
{
	cdv<Vector2> m_Pos;

public:

	CE_TranslateCD( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_CD( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	virtual bool IsOver( double current_time ) const;

	friend class CAnimatedGraphicsManager;
};


class CE_SizeChangeCD : public CGraphicsElementEffect_CD
{
	cdv<Vector2> m_vMin;
	cdv<Vector2> m_vMax;

public:

	CE_SizeChangeCD( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_CD( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	virtual bool IsOver( double current_time ) const;

	friend class CAnimatedGraphicsManager;
};


class CE_ScaleCD : public CGraphicsElementEffect_CD
{
	cdv<float> m_Scale;

	Vector2 m_vCenter;

public:

	CE_ScaleCD( CGraphicsElement *pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect_CD( pTargetElement, start_time, end_time ) { m_vCenter = Vector2(0,0); }

	virtual void Update( double current_time, double dt )
	{
		if( current_time < m_fStartTime )
			return;
		else if( m_Scale.target - m_Scale.current < 0.001f )
			return;
/*		else if( m_fEndTime < current_time )
		{
			GetElement()->SetTopLeftPos( m_Pos.current );
			return;
		}
*/
		// currently in the animation time frame

		m_Scale.Update( (float)dt );

//		GetElement()->ChangeElementScale( m_vCenter, m_Scale.current );
	}

	friend class CAnimatedGraphicsManager;
};


class CE_Blink : public CGraphicsElementEffectBase
{
	float m_fAccumulatedTime; ///< in sec
	float m_fFlipInterval; ///< in sec

	bool m_bLoop;

public:

	CE_Blink( CGraphicsElement *pTargetElement ) : CGraphicsElementEffectBase(pTargetElement, 0.0f, 0.0f ), m_fAccumulatedTime(0), m_fFlipInterval(1.0f) {}
	~CE_Blink() {}
};

/*
class CE_ColorBlink : public CE_Blink
{
	SFloatRGBAColor m_aColor[2];

public:

	CE_ColorBlink() {}
};
*/

class CE_AlphaBlink : public CE_Blink
{
	float m_afAlpha[2];

	/// usually a same value is set to [0] & [1]
	float m_afDuration[2];

public:

	CE_AlphaBlink( CGraphicsElement *pTargetElement )
		:
	CE_Blink( pTargetElement ) 
	{
		m_afAlpha[0] = m_afAlpha[1] = 1.0f;
	}

	virtual void Update( double current_time, double dt );

	/// set the same duration for [0] & [1]
	void SetInterval( float interval )
	{
		m_afDuration[0] = m_afDuration[1] = interval;
	}
};


class CAnimatedGraphicsManagerBase
{
public:

	CAnimatedGraphicsManagerBase() {}
	CAnimatedGraphicsManagerBase( CGraphicsElementManager *pElementManager ) {}
	virtual ~CAnimatedGraphicsManagerBase() { Release(); }
	virtual void Release() {}
	virtual CGraphicsElementManager *GetGraphicsElementManager(){ return NULL; }
	virtual void SetTimeOffset( double time = -1.0 ) {}

	virtual CGraphicsEffectHandle ChangeAlpha( CGraphicsElement *pTargetElement, double start_time, double end_time, int color_index, float start_alpha, float end_alpha, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeAlphaTo( CGraphicsElement *pTargetElement, double start_time, double end_time, int color_index, float end_alpha, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColor( CGraphicsElement *pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColor( CGraphicsElement *pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBColor& start_color, const SFloatRGBColor& end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColorTo( CGraphicsElement *pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBAColor& end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColorTo( CGraphicsElement *pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBColor& end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColorTo( CGraphicsElement *pTargetElement, double start_time, double end_time, int color_index, U32 end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeSize( CGraphicsElement *pTargetElement, double start_time, double end_time, Vector2 vDestMin, Vector2 vDestMax ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ScaleTo( CGraphicsElement *pTargetElement, double start_time, double end_time, float end_scale ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle TranslateTo( CGraphicsElement *pTargetElement, double start_time, double end_time, Vector2 vDestPos, int coord_type, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle TranslateCDV( CGraphicsElement *pTargetElement, double start_time, Vector2 vDestPos, Vector2 vInitVel = Vector2(0,0), float smooth_time = 0.5f, int coord_type = 0 ) { return CGraphicsEffectHandle::Null(); }

	virtual CGraphicsEffectHandle BlinkAlpha( CGraphicsElement *pTargetElement, double start_time, double end_time, double interval, float alpha ) { return CGraphicsEffectHandle::Null(); } 
	virtual CGraphicsEffectHandle BlinkAlpha( CGraphicsElement *pTargetElement, double start_time, double end_time, double duration0, double duration1, float alpha0, float alpha1 ) { return CGraphicsEffectHandle::Null(); } 

	virtual CGraphicsEffectHandle DrawText( CGE_Text *pTargetTextElement, double start_time, int num_chars_per_sec ) { return CGraphicsEffectHandle::Null(); }

	virtual void SetElementPath_Start() {}
	virtual void AddElementPath() {}
	virtual void SetElementPath_End() {}

	virtual CGraphicsEffectHandle SetPosition( CGraphicsElement *pTargetElement, double time, const Vector2& vPos ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle SetColor( CGraphicsElement *pTargetElement, double time, int color_index, const SFloatRGBAColor& color ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle SetColor( CGraphicsElement *pTargetElement, double time, int color_index, const U32& color ) { return CGraphicsEffectHandle::Null(); }
	virtual void UpdateEffects( double dt ) {}
	virtual bool CancelEffect( CGraphicsEffectHandle& effect_handle ) { return false; }
};


/**
 unit of time arg: [sec] by default


*/
class CAnimatedGraphicsManager : public CAnimatedGraphicsManagerBase
{
	CTimer *m_pTimer;

//	std::list<CGraphicsElementEffectBase *> m_vecpEffect;
	std::vector<CGraphicsElementEffectBase *> m_vecpEffect;

	std::vector<int> m_vecVacantSlotIndex;

	double m_fTimeOffset;

	CGraphicsElementManager *m_pGraphicsElementManager;

	/// - incremented every time a graphics effect is created.
	/// - set to graphics effect handle
	int m_NextGraphicsEffectID;

	int GetNextGraphicsEffectID() { return m_NextGraphicsEffectID++; }

	inline int GetVacantSlotIndex();

	inline void RemoveEffect( int effect_index );

	CGraphicsEffectHandle AddGraphicsEffect( CGraphicsElementEffectBase* pEffect );

public:

//	CAnimatedGraphicsManager( CGraphicsElementManager *pElementManager );
	CAnimatedGraphicsManager();

	~CAnimatedGraphicsManager() { Release(); }

	void Release();

	/// returns a borrowed reference to the graphics element manager
	/// NOTE: Returned pointer is a borrowed reference. Do not delete it.
	CGraphicsElementManager *GetGraphicsElementManager(){ return m_pGraphicsElementManager; }

	CGraphicsElement *GetElement( int element_id ) { return m_pGraphicsElementManager->GetElement(element_id); }

	// set the time of this function call as the time origin for subsequent ChangeXXX() calls
	void SetTimeOffset( double time = -1.0 );

//	void AddFadeInEffect( int id, float start, float end, float dest_alpha );
//	void AddFadeOutEffect( int id, float start, float end, float dest_alpha );

	CGraphicsEffectHandle ChangeAlpha( CGraphicsElement *pTargetElement, double start_time, double end_time, int color_index, float start_alpha, float end_alpha, int trans_mode );
	CGraphicsEffectHandle ChangeAlphaTo( CGraphicsElement *pTargetElement, double start_time, double end_time, int color_index, float end_alpha, int trans_mode );

	CGraphicsEffectHandle ChangeColor( CGraphicsElement *pTargetElement, double start_time,  double end_time,
					                   int color_index, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color,
					                   int trans_mode );

	CGraphicsEffectHandle ChangeColor( CGraphicsElement *pTargetElement, double start_time,  double end_time,
					                   int color_index, const SFloatRGBColor& start_color, const SFloatRGBColor& end_color,
					                   int trans_mode );

	CGraphicsEffectHandle ChangeColorTo( CGraphicsElement *pTargetElement, double start_time, double end_time,
						                 int color_index, const SFloatRGBAColor& end_color,
						                 int trans_mode );

	CGraphicsEffectHandle ChangeColorTo( CGraphicsElement *pTargetElement, double start_time, double end_time,
						                 int color_index, const SFloatRGBColor& end_color,
						                 int trans_mode );

	CGraphicsEffectHandle ChangeColorTo( CGraphicsElement *pTargetElement, double start_time, double end_time,
						                 int color_index, U32 end_color,
						                 int trans_mode );

	/// top-left, bottom-right corner of the dest position
	CGraphicsEffectHandle ChangeSize( CGraphicsElement *pTargetElement, double start_time, double end_time, Vector2 vDestMin, Vector2 vDestMax );

	CGraphicsEffectHandle ScaleTo( CGraphicsElement *pTargetElement, double start_time, double end_time, float end_scale );

	/// move an element from its current position to a new destination
	CGraphicsEffectHandle TranslateTo( CGraphicsElement *pTargetElement, double start_time, double end_time, Vector2 vDestPos, int coord_type, int trans_mode );

	/// Translate with Critical Damping (Velocity)
	/// - translate the element from the current position to the dest position 'vDestPos'
	///   with the init velocity 'vInitVel' 
	/// - uses critical damping to calc position
	CGraphicsEffectHandle TranslateCDV( CGraphicsElement *pTargetElement, double start_time, Vector2 vDestPos, Vector2 vInitVel, float smooth_time, int coord_type );

//	CGraphicsEffectHandle BlinkAlpha( CGraphicsElement *pTargetElement, double start_time, double end_time, double interval, float alpha ); 
//	CGraphicsEffectHandle BlinkAlpha( CGraphicsElement *pTargetElement, double start_time, double end_time, double duration0, double duration1, float alpha0, float alpha1 ); 

	/// Used only by text element.
	/// - Effect is terminated after all the entire is drawn
	/// - TODO: Define the behavior when the effect gets canceled when it has not been completely drawn.
	CGraphicsEffectHandle DrawText( CGE_Text *pTargetTextElement, double start_time, int num_chars_per_sec );

//	void SetElementPath_Start();
//	void AddElementPath();
//	void SetElementPath_End();

	CGraphicsEffectHandle SetPosition( CGraphicsElement *pTargetElement, double time, const Vector2& vPos );
	CGraphicsEffectHandle SetColor( CGraphicsElement *pTargetElement, double time, int color_index, const SFloatRGBAColor& color );
	virtual inline CGraphicsEffectHandle SetColor( CGraphicsElement *pTargetElement, double time, int color_index, const U32& color );

	void UpdateEffects( double dt );

	bool CancelEffect( CGraphicsEffectHandle& effect_handle );

	friend class CGraphiceEffectManagerCallback;
};


inline int CAnimatedGraphicsManager::GetVacantSlotIndex()
{
//	if( NUM_MAX_EFFECTS <= m_vecpEffect.size() )
//		return -1;

	if( 0 < m_vecVacantSlotIndex.size() )
	{
		int index = m_vecVacantSlotIndex.back();
		m_vecVacantSlotIndex.pop_back();
		return index;
	}
	else
	{
		m_vecpEffect.push_back( NULL );
		return (int)m_vecpEffect.size() - 1;
	}
}


inline void CAnimatedGraphicsManager::RemoveEffect( int effect_index )
{
	SafeDelete( m_vecpEffect[effect_index] );
	m_vecVacantSlotIndex.push_back( effect_index );
}


inline CGraphicsEffectHandle CAnimatedGraphicsManager::SetColor( CGraphicsElement *pTargetElement, double time, int color_index, const U32& color )
{
	SFloatRGBAColor dest_color;
	dest_color.SetARGB32( color );
	return SetColor( pTargetElement, time, color_index, dest_color );
}



typedef boost::shared_ptr<CAnimatedGraphicsManagerBase> CAnimatedGraphicsManagerSharedPtr;


#endif  /* __AnimatedGraphicsManager_H__ */
