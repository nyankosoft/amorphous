#ifndef  __AnimatedGraphicsManager_H__
#define  __AnimatedGraphicsManager_H__


#include "GraphicsElementManager.hpp"
#include "GraphicsEffectHandle.hpp"


#include "../base.hpp"
#include "3DMath/Vector2.hpp"
#include "3DMath/Vector3.hpp"
#include "3DMath/aabb2.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/3DGameMath.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/2DPrimitive/2DFrameRect.hpp"
#include "Graphics/2DPrimitive/2DTriangle.hpp"
#include "Graphics/TextureCoord.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "Graphics/Font/FontBase.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/TextureHandle.hpp"

#include "GameCommon/fwd.hpp"
#include "GameCommon/CriticalDamping.hpp"

#include "Support/FixedVector.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "Support/memory_helpers.hpp"

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

class CGraphicsElementEffectFlag
{
public:
	enum Name
	{
		DONT_RELEASE = ( 1 << 0 ), ///< can be used with non-linear effects. User must manually release the effect if he wants to remove it before releasing the effect manager.
	};
};


/**
 * base class for 2d graphics effects
 *
 */
class CGraphicsElementEffect
{
protected:

	int m_EffectID;

	double m_fStartTime;
	double m_fEndTime;

	CAnimatedGraphicsManager* m_pManager;

	boost::shared_ptr<CGraphicsElement> m_pTargetElement;

	bool m_bInitialized;

	int m_Index; ///< see CGraphiceEffectManagerCallback::OnDestroyed

	U32 m_Flags;

public:

	CGraphicsElementEffect( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: m_EffectID(-1), m_pTargetElement(pTargetElement), m_fStartTime(start_time), m_fEndTime(end_time), m_bInitialized(false), m_Index(-1) {}

	virtual ~CGraphicsElementEffect() {}

	enum TransMode
	{
		TRANS_LINEAR,
		TRANS_NONLINEAR, ///< Effect uses critical damping and transfroms non-linearly
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

//	boost::shared_ptr<CGraphicsElement> GetElement() { m_pManager->GetGraphicsElementManager()->GetElement(m_TargetElementID); }
	boost::shared_ptr<CGraphicsElement> GetElement();

	virtual int GetTransType() const = 0;

	virtual bool IsOver( double current_time ) const { return false; }

	double GetEndTime() const { return m_fEndTime; }

	U32 GetFlags() const { return m_Flags; }

	void SetFlags( U32 flags ) { m_Flags = flags; }

	void SetAnimatedGraphicsManager( CAnimatedGraphicsManager* pMgr ) { m_pManager = pMgr; }

	/// Implemented by non-linear translation effect
	virtual void SetDestPosition( Vector2 vDestPos ) {}

	/// To be implemented by non-linear color shift effect
//	virtual void SetDestColor( const SFloatRGBAColor& dest_color ) {}

	/// To be implemented by non-linear color shift effect
	virtual void SetDestColor( const SFloatRGBAColor& dest_color ) {}
//	virtual void SetDestVertexColor( int vertex, int color_index, const SFloatRGBAColor& dest_color ) {}
};


class CGraphicsElementLinearEffect : public CGraphicsElementEffect
{
protected:

	bool m_bAppliedAtEndTime;

	inline double GetFraction_Linear( double current_time );

public:

	CGraphicsElementLinearEffect( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect(pTargetElement, start_time, end_time ), m_bAppliedAtEndTime(false) {}

	virtual ~CGraphicsElementLinearEffect() {}

	virtual int GetTransType() const { return TRANS_LINEAR; }

	virtual bool IsOver( double current_time ) const { return m_fEndTime + 1.0 < current_time && m_bAppliedAtEndTime; }

	bool operator==( const CGraphicsElementEffect& effect )
	{
		return m_fEndTime == effect.GetEndTime();
	}

	bool operator<( const CGraphicsElementEffect& effect )
	{
		return m_fEndTime < effect.GetEndTime();
	}
};


class CGraphicsElementNonLinearEffect : public CGraphicsElementEffect
{
protected:

public:

	CGraphicsElementNonLinearEffect( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementEffect(pTargetElement, start_time, end_time ) {}

	virtual ~CGraphicsElementNonLinearEffect() {}

	virtual int GetTransType() const { return TRANS_NONLINEAR; }
};


inline double CGraphicsElementLinearEffect::GetFraction_Linear( double current_time )
{
	double duration = m_fEndTime - m_fStartTime;
	return (current_time - m_fStartTime) / duration;
}


class CE_ColorShift : public CGraphicsElementLinearEffect
{
	int m_ColorIndex;

	SFloatRGBAColor m_StartColor;
	SFloatRGBAColor m_EndColor;

	int m_TargetChannels;

public:

	CE_ColorShift( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementLinearEffect( pTargetElement, start_time, end_time ),
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


class CE_AlphaChange : public CGraphicsElementLinearEffect
{
	int m_ColorIndex;

	float m_fStartAlpha;
	float m_fEndAlpha;

public:

	CE_AlphaChange( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementLinearEffect( pTargetElement, start_time, end_time ),
		m_ColorIndex(0), m_fStartAlpha(0), m_fEndAlpha(0) {}

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};

/*
class CE_FadeIn : public CGraphicsElementEffect
{
public:
	void Update( float dt );
};

class CE_FadeOut : public CGraphicsElementEffect
{
public:
	void Update( float dt );
};
*/


class CE_Translate : public CGraphicsElementLinearEffect
{
	Vector2 m_vStart;
	Vector2 m_vEnd;

	int m_TransMode;

//	cdv<Vector2> m_Pos;

public:

	CE_Translate( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementLinearEffect( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};


class CE_SizeChange : public CGraphicsElementLinearEffect
{
	AABB2 m_Start;
	AABB2 m_End;

public:

	CE_SizeChange( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementLinearEffect( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};


class CE_Rotate : public CGraphicsElementLinearEffect
{
	float m_fStartAngle;
	float m_fEndAngle;

public:

	CE_Rotate( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementLinearEffect( pTargetElement, start_time, end_time ) {}

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


class CE_TextDraw : public CGraphicsElementLinearEffect
{
	float m_CharsPerSec;

	/// how long the colors spans from solid color to fade out
	int m_FadeLength;

	std::string m_OrigText;

	std::vector<int> m_vec;

	boost::shared_ptr<CTextElement> m_pTextElement;

public:

	CE_TextDraw( boost::shared_ptr<CTextElement> pTargetElement, double start_time );

	void SetNumCharsPerSec( int num_chars_per_sec ) { m_CharsPerSec = (float)num_chars_per_sec; }

	virtual void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};



class CE_Scale : public CGraphicsElementLinearEffect
{
	float m_fStartScale;
	float m_fEndScale;

	Vector2 m_vCenter;

public:

	CE_Scale( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementLinearEffect( pTargetElement, start_time, end_time ) { m_vCenter = Vector2(0,0); }

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


class CE_NonLinearVertexColorShift : public CGraphicsElementNonLinearEffect
{
	cdv<SFloatRGBAColor> m_Color;

	int m_ColorIndex;

	int m_VertexIndex;

	/// The user must not remove polygon element until the effect is done. How's that?
	/// - How does the user know if the effect is still doing its job or done?
	boost::shared_ptr<CPolygonElement> m_pPolygonElement;

public:

	CE_NonLinearVertexColorShift( boost::shared_ptr<CPolygonElement> pTargetElement, double start_time )
		: CGraphicsElementNonLinearEffect( pTargetElement, start_time, start_time + 10000.0 ), m_pPolygonElement(pTargetElement) {}

	virtual void Update( double current_time, double dt );

	void SetDestColor( const SFloatRGBAColor& dest_color );

	// Does this thing have any use?
	// - It's not likely that the user wants to change target vertex / color index later.
//	void SetDestVertexColor( int vertex, int color_index, const SFloatRGBAColor& dest_color );

	friend class CAnimatedGraphicsManager;
};


class CE_TranslateNonLinear : public CGraphicsElementNonLinearEffect
{
	cdv<Vector2> m_Pos;

public:

	CE_TranslateNonLinear( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementNonLinearEffect( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	virtual bool IsOver( double current_time ) const;

	void SetDestPosition( Vector2 vDestPos ) { m_Pos.target = vDestPos; }

	friend class CAnimatedGraphicsManager;
};


class CE_SizeChangeCD : public CGraphicsElementNonLinearEffect
{
	cdv<Vector2> m_vMin;
	cdv<Vector2> m_vMax;

public:

	CE_SizeChangeCD( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementNonLinearEffect( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	virtual bool IsOver( double current_time ) const;

	friend class CAnimatedGraphicsManager;
};


class CE_ScaleCD : public CGraphicsElementNonLinearEffect
{
	cdv<float> m_Scale;

	Vector2 m_vCenter;

public:

	CE_ScaleCD( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time )
		: CGraphicsElementNonLinearEffect( pTargetElement, start_time, end_time ) { m_vCenter = Vector2(0,0); }

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


class CE_Blink : public CGraphicsElementNonLinearEffect
{
protected:

	double m_fAccumulatedTime; ///< in sec

	/// usually a same value is set to [0] & [1]
	double m_afDuration[2];

	int m_ColorIndex;

//	double m_fFlipInterval; ///< in sec

//	bool m_bLoop;

public:

	CE_Blink( boost::shared_ptr<CGraphicsElement> pTargetElement ) : CGraphicsElementNonLinearEffect(pTargetElement, 0.0f, 10000.0f ), m_fAccumulatedTime(0) {}
	~CE_Blink() {}

	friend class CAnimatedGraphicsManager;
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

public:

	CE_AlphaBlink( boost::shared_ptr<CGraphicsElement> pTargetElement )
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

	friend class CAnimatedGraphicsManager;
};


class CE_SineWave : public CGraphicsElementNonLinearEffect
{
protected:

	float m_fPeriod; ///< [sec]

	int m_ColorIndex;

	float CalculateFactor( double current_time, double dt )
	{
		double offset = 0.5 * PI; // make alpha0/color0 first appear in full brightness
		double x = current_time * 2.0 * PI / m_fPeriod + offset;
		double y = sin( x );

		return (float)( ( y + 1.0 ) * 0.5 );
	}

public:

	CE_SineWave( boost::shared_ptr<CGraphicsElement> pTargetElement )
		:
	CGraphicsElementNonLinearEffect(pTargetElement, 0.0f, 10000.0f )
	{}

	virtual ~CE_SineWave()
	{}
};


class CE_SineWaveAlphaChange : public CE_SineWave
{
	float m_afAlpha[2];

public:

	CE_SineWaveAlphaChange( boost::shared_ptr<CGraphicsElement> pTargetElement )
		:
	CE_SineWave( pTargetElement ) 
	{
		m_afAlpha[0] = m_afAlpha[1] = 1.0f;
	}

	void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
};


class CE_SineWaveColorChange : public CE_SineWave
{
	SFloatRGBAColor m_aColor[2];

public:

	CE_SineWaveColorChange( boost::shared_ptr<CGraphicsElement> pTargetElement )
		:
	CE_SineWave( pTargetElement ) 
	{
		m_aColor[0] = m_aColor[1] = SFloatRGBAColor::White();
	}

	void Update( double current_time, double dt );

	friend class CAnimatedGraphicsManager;
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

	virtual CGraphicsEffectHandle ChangeAlpha( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, float start_alpha, float end_alpha, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeAlphaTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, float end_alpha, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBColor& start_color, const SFloatRGBColor& end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColorTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBAColor& end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColorTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBColor& end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColorTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, U32 end_color, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeSize( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, Vector2 vDestMin, Vector2 vDestMax ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ScaleTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, float end_scale ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle TranslateTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, Vector2 vDestPos, int coord_type, int trans_mode ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle TranslateNonLinear( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, Vector2 vDestPos, Vector2 vInitVel = Vector2(0,0), float smooth_time = 0.5f, int coord_type = 0, U32 flags = 0 ) { return CGraphicsEffectHandle::Null(); }

	virtual CGraphicsEffectHandle BlinkAlpha( boost::shared_ptr<CGraphicsElement> pTargetElement, double interval, int color_index = 0 ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle BlinkAlpha( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, double interval, float alpha ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle BlinkAlpha( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, double duration0, double duration1, float alpha0, float alpha1 ) { return CGraphicsEffectHandle::Null(); } 

	virtual CGraphicsEffectHandle ChangeAlphaInSineWave( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double period, int color_index, float alpha0, float alpha1, int num_periods = -1 ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeColorInSineWave( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double period, int color_index, const SFloatRGBAColor& color0, const SFloatRGBAColor& color1, int num_periods = -1 ) { return CGraphicsEffectHandle::Null(); }

	virtual CGraphicsEffectHandle DrawText( boost::shared_ptr<CTextElement> pTargetTextElement, double start_time, int num_chars_per_sec ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle ChangeVertexColorNonLinear( boost::shared_ptr<CPolygonElement> pTargetTextElement, double start_time,
		int color_index, int vertex,
		const SFloatRGBAColor& dest_color, const SFloatRGBAColor& color_change_velocity ) { return CGraphicsEffectHandle::Null(); }

	virtual void SetElementPath_Start() {}
	virtual void AddElementPath() {}
	virtual void SetElementPath_End() {}

	virtual CGraphicsEffectHandle SetPosition( boost::shared_ptr<CGraphicsElement> pTargetElement, double time, const Vector2& vPos ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle SetColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double time, int color_index, const SFloatRGBAColor& color ) { return CGraphicsEffectHandle::Null(); }
	virtual CGraphicsEffectHandle SetColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double time, int color_index, const U32& color ) { return CGraphicsEffectHandle::Null(); }
	virtual void UpdateEffects( double dt ) {}
	virtual bool CancelEffect( CGraphicsEffectHandle& effect_handle ) { return false; }
};


/**
 unit of time arg: [sec] by default


*/
class CAnimatedGraphicsManager : public CAnimatedGraphicsManagerBase
{
	CTimer *m_pTimer;

//	std::list<CGraphicsElementEffect *> m_vecpEffect;
	std::vector<CGraphicsElementEffect *> m_vecpEffect;

	std::vector<int> m_vecVacantSlotIndex;

	double m_fTimeOffset;

	CGraphicsElementManager *m_pGraphicsElementManager;

	/// - incremented every time a graphics effect is created.
	/// - set to graphics effect handle
	int m_NextGraphicsEffectID;

	int GetNextGraphicsEffectID() { return m_NextGraphicsEffectID++; }

	inline int GetVacantSlotIndex();

	inline void RemoveEffect( int effect_index );

	CGraphicsEffectHandle AddGraphicsEffect( CGraphicsElementEffect* pEffect );

	/// Used by effect handle
	/// - Never let any other thread call Update() while this is called and effect handles finish
	///   necessary operations.
	///   Reason: effect instance may be removed in Update() after effect pointer is returned
	///   by this function
	CGraphicsElementEffect *GetEffect( CGraphicsEffectHandle& effect_handle );

public:

//	CAnimatedGraphicsManager( CGraphicsElementManager *pElementManager );
	CAnimatedGraphicsManager();

	~CAnimatedGraphicsManager() { Release(); }

	void Release();

	/// returns a borrowed reference to the graphics element manager
	/// NOTE: Returned pointer is a borrowed reference. Do not delete it.
	CGraphicsElementManager *GetGraphicsElementManager(){ return m_pGraphicsElementManager; }

	boost::shared_ptr<CGraphicsElement> GetElement( int element_id ) { return m_pGraphicsElementManager->GetElement(element_id); }

	// set the time of this function call as the time origin for subsequent ChangeXXX() calls
	void SetTimeOffset( double time = -1.0 );

//	void AddFadeInEffect( int id, float start, float end, float dest_alpha );
//	void AddFadeOutEffect( int id, float start, float end, float dest_alpha );

	CGraphicsEffectHandle ChangeAlpha( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, float start_alpha, float end_alpha, int trans_mode );
	CGraphicsEffectHandle ChangeAlphaTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, float end_alpha, int trans_mode );

	CGraphicsEffectHandle ChangeColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time,  double end_time,
					                   int color_index, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color,
					                   int trans_mode );

	CGraphicsEffectHandle ChangeColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time,  double end_time,
					                   int color_index, const SFloatRGBColor& start_color, const SFloatRGBColor& end_color,
					                   int trans_mode );

	CGraphicsEffectHandle ChangeColorTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time,
						                 int color_index, const SFloatRGBAColor& end_color,
						                 int trans_mode );

	CGraphicsEffectHandle ChangeColorTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time,
						                 int color_index, const SFloatRGBColor& end_color,
						                 int trans_mode );

	CGraphicsEffectHandle ChangeColorTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time,
						                 int color_index, U32 end_color,
						                 int trans_mode );

	/// top-left, bottom-right corner of the dest position
	CGraphicsEffectHandle ChangeSize( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, Vector2 vDestMin, Vector2 vDestMax );

	CGraphicsEffectHandle ScaleTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, float end_scale );

	/// move an element from its current position to a new destination
	CGraphicsEffectHandle TranslateTo( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, Vector2 vDestPos, int coord_type, int trans_mode );

	/// Translate with Critical Damping (Velocity)
	/// - Translate the element from the current position to the dest position 'vDestPos'
	///   with the init velocity 'vInitVel' 
	/// - Uses critical damping to calculate the position of the target element
	CGraphicsEffectHandle TranslateNonLinear( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, Vector2 vDestPos, Vector2 vInitVel, float smooth_time, int coord_type, U32 flags );

	/// blink the element by alternating its alpha component between the current value and zero
	/// - The effect is looped. The user needs cancel the effect explicitly to stop the blink effect
	CGraphicsEffectHandle BlinkAlpha( boost::shared_ptr<CGraphicsElement> pTargetElement, double interval, int color_index );

//	CGraphicsEffectHandle BlinkAlpha( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, double interval, float alpha ); 
//	CGraphicsEffectHandle BlinkAlpha( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, double duration0, double duration1, float alpha0, float alpha1 ); 

	CGraphicsEffectHandle ChangeAlphaInSineWave( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double period, int color_index, float alpha0, float alpha1, int num_periods );

	CGraphicsEffectHandle ChangeColorInSineWave( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double period, int color_index, const SFloatRGBAColor& color0, const SFloatRGBAColor& color1, int num_periods );

	/// Used only by text element.
	/// - Effect is terminated after the entire text is drawn
	/// - TODO: Define the behavior when the effect gets canceled before it finish drawing the complete text.
	CGraphicsEffectHandle DrawText( boost::shared_ptr<CTextElement> pTargetTextElement, double start_time, int num_chars_per_sec );

	CGraphicsEffectHandle ChangeVertexColorNonLinear( boost::shared_ptr<CPolygonElement> pTargetTextElement, double start_time,
		int color_index, int vertex,
		const SFloatRGBAColor& dest_color, const SFloatRGBAColor& color_change_velocity );

//	void SetElementPath_Start();
//	void AddElementPath();
//	void SetElementPath_End();

	CGraphicsEffectHandle SetPosition( boost::shared_ptr<CGraphicsElement> pTargetElement, double time, const Vector2& vPos );
	CGraphicsEffectHandle SetColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double time, int color_index, const SFloatRGBAColor& color );
	virtual inline CGraphicsEffectHandle SetColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double time, int color_index, const U32& color );

	void UpdateEffects( double dt );

	bool CancelEffect( CGraphicsEffectHandle& effect_handle );

	friend class CGraphicsEffectHandle;
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


inline CGraphicsEffectHandle CAnimatedGraphicsManager::SetColor( boost::shared_ptr<CGraphicsElement> pTargetElement, double time, int color_index, const U32& color )
{
	SFloatRGBAColor dest_color;
	dest_color.SetARGB32( color );
	return SetColor( pTargetElement, time, color_index, dest_color );
}



typedef boost::shared_ptr<CAnimatedGraphicsManagerBase> CAnimatedGraphicsManagerSharedPtr;


#endif  /* __AnimatedGraphicsManager_H__ */
