#ifndef  __AnimatedGraphicsManager_H__
#define  __AnimatedGraphicsManager_H__


#include "GraphicsElementManager.hpp"
#include "GraphicsEffectHandle.hpp"

#include "../base.hpp"
#include "../3DMath/Vector2.hpp"
#include "../3DMath/Vector3.hpp"
#include "../3DMath/aabb2.hpp"
#include "../Graphics/fwd.hpp"
#include "../Graphics/2DPrimitive/2DRect.hpp"
#include "../Graphics/2DPrimitive/2DFrameRect.hpp"
#include "../Graphics/2DPrimitive/2DTriangle.hpp"
#include "../Graphics/TextureCoord.hpp"
#include "../Graphics/FloatRGBAColor.hpp"
#include "../Graphics/GraphicsComponentCollector.hpp"
#include "../Graphics/TextureHandle.hpp"
#include "../GameCommon/fwd.hpp"
#include "../GameCommon/CriticalDamping.hpp"
#include "../Support/memory_helpers.hpp"
#include <list>


namespace amorphous
{


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

class GraphicsElementAnimationFlag
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
class GraphicsElementAnimation
{
protected:

	int m_EffectID;

	double m_fStartTime;
	double m_fEndTime;

	GraphicsElementAnimationManager* m_pManager;

	boost::shared_ptr<GraphicsElement> m_pTargetElement;

	bool m_bInitialized;

	int m_Index; ///< see GraphicsElementAnimationCallback::OnDestroyed

	U32 m_Flags;

public:

	GraphicsElementAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: m_EffectID(-1), m_pTargetElement(pTargetElement), m_fStartTime(start_time), m_fEndTime(end_time), m_bInitialized(false), m_Index(-1) {}

	virtual ~GraphicsElementAnimation() {}

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

//	boost::shared_ptr<GraphicsElement> GetElement() { m_pManager->GetGraphicsElementManager()->GetElement(m_TargetElementID); }
	boost::shared_ptr<GraphicsElement> GetElement();

	virtual int GetTransType() const = 0;

	virtual bool IsOver( double current_time ) const { return false; }

	double GetEndTime() const { return m_fEndTime; }

	U32 GetFlags() const { return m_Flags; }

	void SetFlags( U32 flags ) { m_Flags = flags; }

	void SetAnimatedGraphicsManager( GraphicsElementAnimationManager* pMgr ) { m_pManager = pMgr; }

	/// Implemented by non-linear translation effect
	virtual void SetDestPosition( Vector2 vDestPos ) {}

	/// To be implemented by non-linear color shift effect
//	virtual void SetDestColor( const SFloatRGBAColor& dest_color ) {}

	/// To be implemented by non-linear color shift effect
	virtual void SetDestColor( const SFloatRGBAColor& dest_color ) {}
//	virtual void SetDestVertexColor( int vertex, int color_index, const SFloatRGBAColor& dest_color ) {}
};


class GraphicsElementLinearAnimation : public GraphicsElementAnimation
{
protected:

	bool m_bAppliedAtEndTime;

	inline double GetFraction_Linear( double current_time );

public:

	GraphicsElementLinearAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementAnimation(pTargetElement, start_time, end_time ), m_bAppliedAtEndTime(false) {}

	virtual ~GraphicsElementLinearAnimation() {}

	virtual int GetTransType() const { return TRANS_LINEAR; }

	virtual bool IsOver( double current_time ) const { return m_fEndTime + 1.0 < current_time && m_bAppliedAtEndTime; }

	bool operator==( const GraphicsElementAnimation& effect )
	{
		return m_fEndTime == effect.GetEndTime();
	}

	bool operator<( const GraphicsElementAnimation& effect )
	{
		return m_fEndTime < effect.GetEndTime();
	}
};


class GraphicsElementNonLinearAnimation : public GraphicsElementAnimation
{
protected:

public:

	GraphicsElementNonLinearAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementAnimation(pTargetElement, start_time, end_time ) {}

	virtual ~GraphicsElementNonLinearAnimation() {}

	virtual int GetTransType() const { return TRANS_NONLINEAR; }
};


inline double GraphicsElementLinearAnimation::GetFraction_Linear( double current_time )
{
	double duration = m_fEndTime - m_fStartTime;
	return (current_time - m_fStartTime) / duration;
}


class ColorShiftAnimation : public GraphicsElementLinearAnimation
{
	int m_ColorIndex;

	SFloatRGBAColor m_StartColor;
	SFloatRGBAColor m_EndColor;

	int m_TargetChannels;

public:

	ColorShiftAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementLinearAnimation( pTargetElement, start_time, end_time ),
		m_ColorIndex(0), m_StartColor(SFloatRGBAColor(1,1,1,1)), m_EndColor(SFloatRGBAColor(1,1,1,1)) {}

	enum eTarget
	{
		COMPONENTS_RGB,
		COMPONENTS_RGBA,
		NUM_TARGETS
	};

	void SetTargetChannels( int tgt ) { m_TargetChannels = tgt; }

	virtual void Update( double current_time, double dt );

	friend class GraphicsElementAnimationManager;
};


class AlphaShiftAnimation : public GraphicsElementLinearAnimation
{
	int m_ColorIndex;

	float m_fStartAlpha;
	float m_fEndAlpha;

public:

	AlphaShiftAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementLinearAnimation( pTargetElement, start_time, end_time ),
		m_ColorIndex(0), m_fStartAlpha(0), m_fEndAlpha(0) {}

	virtual void Update( double current_time, double dt );

	friend class GraphicsElementAnimationManager;
};

/*
class CE_FadeIn : public GraphicsElementAnimation
{
public:
	void Update( float dt );
};

class CE_FadeOut : public GraphicsElementAnimation
{
public:
	void Update( float dt );
};
*/


class TranslationAnimation : public GraphicsElementLinearAnimation
{
	Vector2 m_vStart;
	Vector2 m_vEnd;

	int m_TransMode;

//	cdv<Vector2> m_Pos;

public:

	TranslationAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementLinearAnimation( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	friend class GraphicsElementAnimationManager;
};


class SizeChangeAnimation : public GraphicsElementLinearAnimation
{
	AABB2 m_Start;
	AABB2 m_End;

public:

	SizeChangeAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementLinearAnimation( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	friend class GraphicsElementAnimationManager;
};


class RotationAnimation : public GraphicsElementLinearAnimation
{
	float m_fStartAngle;
	float m_fEndAngle;

public:

	RotationAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementLinearAnimation( pTargetElement, start_time, end_time ) {}

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

	friend class GraphicsElementAnimationManager;
};


class CE_TextDraw : public GraphicsElementLinearAnimation
{
	float m_CharsPerSec;

	/// how long the colors spans from solid color to fade out
	int m_FadeLength;

	std::string m_OrigText;

	std::vector<int> m_vec;

	boost::shared_ptr<TextElement> m_pTextElement;

public:

	CE_TextDraw( boost::shared_ptr<TextElement> pTargetElement, double start_time );

	void SetNumCharsPerSec( int num_chars_per_sec ) { m_CharsPerSec = (float)num_chars_per_sec; }

	virtual void Update( double current_time, double dt );

	friend class GraphicsElementAnimationManager;
};



class ScalingAnimation : public GraphicsElementLinearAnimation
{
	float m_fStartScale;
	float m_fEndScale;

	Vector2 m_vCenter;

public:

	ScalingAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementLinearAnimation( pTargetElement, start_time, end_time ) { m_vCenter = Vector2(0,0); }

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

	friend class GraphicsElementAnimationManager;
};


class CE_NonLinearVertexColorShift : public GraphicsElementNonLinearAnimation
{
	cdv<SFloatRGBAColor> m_Color;

	int m_ColorIndex;

	int m_VertexIndex;

	/// The user must not remove polygon element until the effect is done. How's that?
	/// - How does the user know if the effect is still doing its job or done?
	boost::shared_ptr<PolygonElement> m_pPolygonElement;

public:

	CE_NonLinearVertexColorShift( boost::shared_ptr<PolygonElement> pTargetElement, double start_time )
		: GraphicsElementNonLinearAnimation( pTargetElement, start_time, start_time + 10000.0 ), m_pPolygonElement(pTargetElement) {}

	virtual void Update( double current_time, double dt );

	void SetDestColor( const SFloatRGBAColor& dest_color );

	// Does this thing have any use?
	// - It's not likely that the user wants to change target vertex / color index later.
//	void SetDestVertexColor( int vertex, int color_index, const SFloatRGBAColor& dest_color );

	friend class GraphicsElementAnimationManager;
};


class NonLinearTranslationAnimation : public GraphicsElementNonLinearAnimation
{
	cdv<Vector2> m_Pos;

public:

	NonLinearTranslationAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementNonLinearAnimation( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	virtual bool IsOver( double current_time ) const;

	void SetDestPosition( Vector2 vDestPos ) { m_Pos.target = vDestPos; }

	friend class GraphicsElementAnimationManager;
};


class SizeChangeAnimationCD : public GraphicsElementNonLinearAnimation
{
	cdv<Vector2> m_vMin;
	cdv<Vector2> m_vMax;

public:

	SizeChangeAnimationCD( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementNonLinearAnimation( pTargetElement, start_time, end_time ) {}

	virtual void Update( double current_time, double dt );

	virtual bool IsOver( double current_time ) const;

	friend class GraphicsElementAnimationManager;
};


class NonLinearScalingAnimation : public GraphicsElementNonLinearAnimation
{
	cdv<float> m_Scale;

	Vector2 m_vCenter;

public:

	NonLinearScalingAnimation( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time )
		: GraphicsElementNonLinearAnimation( pTargetElement, start_time, end_time ) { m_vCenter = Vector2(0,0); }

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

	friend class GraphicsElementAnimationManager;
};


class CE_Blink : public GraphicsElementNonLinearAnimation
{
protected:

	double m_fAccumulatedTime; ///< in sec

	/// usually a same value is set to [0] & [1]
	double m_afDuration[2];

	int m_ColorIndex;

//	double m_fFlipInterval; ///< in sec

//	bool m_bLoop;

public:

	CE_Blink( boost::shared_ptr<GraphicsElement> pTargetElement ) : GraphicsElementNonLinearAnimation(pTargetElement, 0.0f, 10000.0f ), m_fAccumulatedTime(0) {}
	~CE_Blink() {}

	friend class GraphicsElementAnimationManager;
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

	CE_AlphaBlink( boost::shared_ptr<GraphicsElement> pTargetElement )
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

	friend class GraphicsElementAnimationManager;
};


class CE_SineWave : public GraphicsElementNonLinearAnimation
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

	CE_SineWave( boost::shared_ptr<GraphicsElement> pTargetElement )
		:
	GraphicsElementNonLinearAnimation(pTargetElement, 0.0f, 10000.0f )
	{}

	virtual ~CE_SineWave()
	{}
};


class CE_SineWaveAlphaChange : public CE_SineWave
{
	float m_afAlpha[2];

public:

	CE_SineWaveAlphaChange( boost::shared_ptr<GraphicsElement> pTargetElement )
		:
	CE_SineWave( pTargetElement ) 
	{
		m_afAlpha[0] = m_afAlpha[1] = 1.0f;
	}

	void Update( double current_time, double dt );

	friend class GraphicsElementAnimationManager;
};


class CE_SineWaveColorChange : public CE_SineWave
{
	SFloatRGBAColor m_aColor[2];

public:

	CE_SineWaveColorChange( boost::shared_ptr<GraphicsElement> pTargetElement )
		:
	CE_SineWave( pTargetElement ) 
	{
		m_aColor[0] = m_aColor[1] = SFloatRGBAColor::White();
	}

	void Update( double current_time, double dt );

	friend class GraphicsElementAnimationManager;
};


class GraphicsElementAnimationManagerBase
{
public:

	GraphicsElementAnimationManagerBase() {}
	GraphicsElementAnimationManagerBase( GraphicsElementManager *pElementManager ) {}
	virtual ~GraphicsElementAnimationManagerBase() { Release(); }
	virtual void Release() {}
	virtual boost::shared_ptr<GraphicsElementManager> GetGraphicsElementManager() { return boost::shared_ptr<GraphicsElementManager>(); }
	virtual void SetTimeOffset( double time = -1.0 ) {}

	virtual GraphicsElementAnimationHandle ChangeAlpha( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, float start_alpha, float end_alpha, int trans_mode ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeAlphaTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, float end_alpha, int trans_mode ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeColor( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color, int trans_mode ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeColor( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBColor& start_color, const SFloatRGBColor& end_color, int trans_mode ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeColorTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBAColor& end_color, int trans_mode ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeColorTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBColor& end_color, int trans_mode ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeColorTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, U32 end_color, int trans_mode ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeSize( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, Vector2 vDestMin, Vector2 vDestMax ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ScaleTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, float end_scale ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle TranslateTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, Vector2 vDestPos, int coord_type, int trans_mode ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle TranslateNonLinear( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, Vector2 vDestPos, Vector2 vInitVel = Vector2(0,0), float smooth_time = 0.5f, int coord_type = 0, U32 flags = 0 ) { return GraphicsElementAnimationHandle::Null(); }

	virtual GraphicsElementAnimationHandle BlinkAlpha( boost::shared_ptr<GraphicsElement> pTargetElement, double interval, int color_index = 0 ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle BlinkAlpha( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, double interval, float alpha ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle BlinkAlpha( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, double duration0, double duration1, float alpha0, float alpha1 ) { return GraphicsElementAnimationHandle::Null(); } 

	virtual GraphicsElementAnimationHandle ChangeAlphaInSineWave( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double period, int color_index, float alpha0, float alpha1, int num_periods = -1 ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeColorInSineWave( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double period, int color_index, const SFloatRGBAColor& color0, const SFloatRGBAColor& color1, int num_periods = -1 ) { return GraphicsElementAnimationHandle::Null(); }

	virtual GraphicsElementAnimationHandle DrawText( boost::shared_ptr<TextElement> pTargetTextElement, double start_time, int num_chars_per_sec ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle ChangeVertexColorNonLinear( boost::shared_ptr<PolygonElement> pTargetTextElement, double start_time,
		int color_index, int vertex,
		const SFloatRGBAColor& dest_color, const SFloatRGBAColor& color_change_velocity ) { return GraphicsElementAnimationHandle::Null(); }

	virtual void SetElementPath_Start() {}
	virtual void AddElementPath() {}
	virtual void SetElementPath_End() {}

	virtual GraphicsElementAnimationHandle SetPosition( boost::shared_ptr<GraphicsElement> pTargetElement, double time, const Vector2& vPos ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle SetColor( boost::shared_ptr<GraphicsElement> pTargetElement, double time, int color_index, const SFloatRGBAColor& color ) { return GraphicsElementAnimationHandle::Null(); }
	virtual GraphicsElementAnimationHandle SetColor( boost::shared_ptr<GraphicsElement> pTargetElement, double time, int color_index, const U32& color ) { return GraphicsElementAnimationHandle::Null(); }
	virtual void UpdateEffects( double dt ) {}
	virtual bool CancelEffect( GraphicsElementAnimationHandle& effect_handle ) { return false; }
};


/**
 unit of time arg: [sec] by default


*/
class GraphicsElementAnimationManager : public GraphicsElementAnimationManagerBase
{
	Timer *m_pTimer;

//	std::list<GraphicsElementAnimation *> m_vecpEffect;
	std::vector<GraphicsElementAnimation *> m_vecpEffect;

	std::vector<int> m_vecVacantSlotIndex;

	double m_fTimeOffset;

	boost::shared_ptr<GraphicsElementManager> m_pGraphicsElementManager;

	/// - incremented every time a graphics effect is created.
	/// - set to graphics effect handle
	int m_NextGraphicsEffectID;

	int GetNextGraphicsEffectID() { return m_NextGraphicsEffectID++; }

	inline int GetVacantSlotIndex();

	inline void RemoveEffect( int effect_index );

	GraphicsElementAnimationHandle AddGraphicsEffect( GraphicsElementAnimation* pEffect );

	/// Used by effect handle
	/// - Never let any other thread call Update() while this is called and effect handles finish
	///   necessary operations.
	///   Reason: effect instance may be removed in Update() after effect pointer is returned
	///   by this function
	GraphicsElementAnimation *GetEffect( GraphicsElementAnimationHandle& effect_handle );

public:

//	GraphicsElementAnimationManager( GraphicsElementManager *pElementManager );
	GraphicsElementAnimationManager();

	~GraphicsElementAnimationManager() { Release(); }

	void Release();

	/// returns a borrowed reference to the graphics element manager
	/// NOTE: Returned pointer is a borrowed reference. Do not delete it.
	boost::shared_ptr<GraphicsElementManager> GetGraphicsElementManager() { return m_pGraphicsElementManager; }

	boost::shared_ptr<GraphicsElement> GetElement( int element_id ) { return m_pGraphicsElementManager->GetElement(element_id); }

	// set the time of this function call as the time origin for subsequent ChangeXXX() calls
	void SetTimeOffset( double time = -1.0 );

//	void AddFadeInEffect( int id, float start, float end, float dest_alpha );
//	void AddFadeOutEffect( int id, float start, float end, float dest_alpha );

	GraphicsElementAnimationHandle ChangeAlpha( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, float start_alpha, float end_alpha, int trans_mode );
	GraphicsElementAnimationHandle ChangeAlphaTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, float end_alpha, int trans_mode );

	GraphicsElementAnimationHandle ChangeColor( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time,  double end_time,
					                   int color_index, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color,
					                   int trans_mode );

	GraphicsElementAnimationHandle ChangeColor( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time,  double end_time,
					                   int color_index, const SFloatRGBColor& start_color, const SFloatRGBColor& end_color,
					                   int trans_mode );

	GraphicsElementAnimationHandle ChangeColorTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time,
						                 int color_index, const SFloatRGBAColor& end_color,
						                 int trans_mode );

	GraphicsElementAnimationHandle ChangeColorTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time,
						                 int color_index, const SFloatRGBColor& end_color,
						                 int trans_mode );

	GraphicsElementAnimationHandle ChangeColorTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time,
						                 int color_index, U32 end_color,
						                 int trans_mode );

	/// top-left, bottom-right corner of the dest position
	GraphicsElementAnimationHandle ChangeSize( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, Vector2 vDestMin, Vector2 vDestMax );

	GraphicsElementAnimationHandle ScaleTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, float end_scale );

	/// move an element from its current position to a new destination
	GraphicsElementAnimationHandle TranslateTo( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, Vector2 vDestPos, int coord_type, int trans_mode );

	/// Translate with Critical Damping (Velocity)
	/// - Translate the element from the current position to the dest position 'vDestPos'
	///   with the init velocity 'vInitVel' 
	/// - Uses critical damping to calculate the position of the target element
	GraphicsElementAnimationHandle TranslateNonLinear( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, Vector2 vDestPos, Vector2 vInitVel, float smooth_time, int coord_type, U32 flags );

	/// blink the element by alternating its alpha component between the current value and zero
	/// - The effect is looped. The user needs cancel the effect explicitly to stop the blink effect
	GraphicsElementAnimationHandle BlinkAlpha( boost::shared_ptr<GraphicsElement> pTargetElement, double interval, int color_index );

//	GraphicsElementAnimationHandle BlinkAlpha( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, double interval, float alpha ); 
//	GraphicsElementAnimationHandle BlinkAlpha( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, double duration0, double duration1, float alpha0, float alpha1 ); 

	GraphicsElementAnimationHandle ChangeAlphaInSineWave( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double period, int color_index, float alpha0, float alpha1, int num_periods );

	GraphicsElementAnimationHandle ChangeColorInSineWave( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double period, int color_index, const SFloatRGBAColor& color0, const SFloatRGBAColor& color1, int num_periods );

	/// Used only by text element.
	/// - Effect is terminated after the entire text is drawn
	/// - TODO: Define the behavior when the effect gets canceled before it finish drawing the complete text.
	GraphicsElementAnimationHandle DrawText( boost::shared_ptr<TextElement> pTargetTextElement, double start_time, int num_chars_per_sec );

	GraphicsElementAnimationHandle ChangeVertexColorNonLinear( boost::shared_ptr<PolygonElement> pTargetTextElement, double start_time,
		int color_index, int vertex,
		const SFloatRGBAColor& dest_color, const SFloatRGBAColor& color_change_velocity );

//	void SetElementPath_Start();
//	void AddElementPath();
//	void SetElementPath_End();

	GraphicsElementAnimationHandle SetPosition( boost::shared_ptr<GraphicsElement> pTargetElement, double time, const Vector2& vPos );
	GraphicsElementAnimationHandle SetColor( boost::shared_ptr<GraphicsElement> pTargetElement, double time, int color_index, const SFloatRGBAColor& color );
	virtual inline GraphicsElementAnimationHandle SetColor( boost::shared_ptr<GraphicsElement> pTargetElement, double time, int color_index, const U32& color );

	void UpdateEffects( double dt );

	bool CancelEffect( GraphicsElementAnimationHandle& effect_handle );

	friend class GraphicsElementAnimationHandle;
	friend class GraphicsElementAnimationCallback;
};


inline int GraphicsElementAnimationManager::GetVacantSlotIndex()
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


inline void GraphicsElementAnimationManager::RemoveEffect( int effect_index )
{
	SafeDelete( m_vecpEffect[effect_index] );
	m_vecVacantSlotIndex.push_back( effect_index );
}


inline GraphicsElementAnimationHandle GraphicsElementAnimationManager::SetColor( boost::shared_ptr<GraphicsElement> pTargetElement, double time, int color_index, const U32& color )
{
	SFloatRGBAColor dest_color;
	dest_color.SetARGB32( color );
	return SetColor( pTargetElement, time, color_index, dest_color );
}



typedef boost::shared_ptr<GraphicsElementAnimationManagerBase> GraphicsElementAnimationManagerSharedPtr;

} // namespace amorphous



#endif  /* __AnimatedGraphicsManager_H__ */
