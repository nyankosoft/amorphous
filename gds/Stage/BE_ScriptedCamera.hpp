#ifndef __BE_ScriptedCamera_H__
#define __BE_ScriptedCamera_H__

#include "fwd.hpp"
#include "BaseEntity.hpp"
#include "BaseEntityHandle.hpp"
#include "CopyEntity.hpp"

#include "EntityMotionPathRequest.hpp"

#include "Graphics/FloatRGBColor.hpp"
#include "Graphics/Camera.hpp"

#include "3DMath/Vector2.hpp"
#include "3DMath/Quaternion.hpp"

#include "GameCommon/CriticalDamping.hpp"


class CBE_CameraController;


class CameraTargetHolder
{
public:

	std::string m_TargetName;	///< name of the target to focus. used when the target does not exist when a keyframe is created

	CEntityHandle<> m_Target;	///< entity to focus

	Vector3 m_vTargetPos;

public:

	CameraTargetHolder() : m_vTargetPos(Vector3(0,0,0)) {}

	CameraTargetHolder( Vector3 vTargetPos ) : m_vTargetPos(vTargetPos) {}

	CameraTargetHolder( CCopyEntity* pTarget )
		:
	m_Target(pTarget->Self()),
	m_vTargetPos(Vector3(0,0,0))
	{}

	CameraTargetHolder( const std::string& target_name ) : m_TargetName(target_name), m_vTargetPos(Vector3(0,0,0)) {}

	inline CameraTargetHolder operator+( const CameraTargetHolder& v ) const { return CameraTargetHolder(this->m_vTargetPos + v.m_vTargetPos); }
	inline CameraTargetHolder operator-( const CameraTargetHolder& v ) const { return CameraTargetHolder(this->m_vTargetPos - v.m_vTargetPos); }

	inline CameraTargetHolder operator*( const float f ) const { return CameraTargetHolder(m_vTargetPos * f); }
	inline CameraTargetHolder operator/( const float f ) const { return CameraTargetHolder(m_vTargetPos / f); }
};


inline CameraTargetHolder operator*( const float f, const CameraTargetHolder& v )
{
	return CameraTargetHolder( v.m_vTargetPos * f ); 
}


template <class T>
class KeyFrameParam
{
public:
	float time;

	T val;

	int interpolation_method;
	float tension, continuity, b;

public:

	KeyFrameParam()
		:
	time(0)
	{}
};


template <class T>
class KeyFrameSet
{
protected:

	std::vector< KeyFrameParam<T> > m_vecKeyFrame;

public:

	/**
	 * \param time [in] time for the dest frame
	 * \param dest [out] interpolated frame
	 */
	bool CalcFrame( float time, T& dest )
	{
		size_t i, num_key_frames = m_vecKeyFrame.size();

		if( !IsAvailable( time ) )
			return false;

		// linear interpolation
		// TODO: support some ohter interpolations (e.g., TCB spilne)
		for( i=0; i<num_key_frames-1; i++ )
		{
			if( m_vecKeyFrame[i].time <= time && time < m_vecKeyFrame[i+1].time )
			{
				const T& value0 = m_vecKeyFrame[i].val;
				const T& value1 = m_vecKeyFrame[i+1].val;
				const float t0 = m_vecKeyFrame[i].time;
				const float t1 = m_vecKeyFrame[i+1].time;
				dest = ( (t1 - time) * value0 + (time - t0) * value1 ) / (t1 - t0);
				return true;
			}
		}

		return false;
	}

	bool IsAvailable( float time ) const
	{
		if( m_vecKeyFrame.size() <= 1 )
			return false;
		else if( m_vecKeyFrame[0].time <= time && time < m_vecKeyFrame.back().time )
            return true;
		else
			return false;
	}

	void Reset() { m_vecKeyFrame.resize(0); }

	void AddKeyFrame( float time, const T& key_frame )
	{
		m_vecKeyFrame.push_back( KeyFrameParam<T>() );
		m_vecKeyFrame.back().time = time;
		m_vecKeyFrame.back().val = key_frame;
	}
};


class CPPEffectParams
{
public:

	int flag;

	float blur_x;
	float blur_y;
	SFloatRGBColor monocrhome_color_offset;
	float glare_threshold;
	float noise;
	float stripe;
	float motion_blur_strength;

public:

	CPPEffectParams()
		:
	flag(0),
	blur_x(0),
	blur_y(0),
	monocrhome_color_offset(SFloatRGBColor(0,0,0)),
	glare_threshold(0),
	noise(0),
	stripe(0),
	motion_blur_strength(0)
	{}

	void Clear()
	{
		flag = 0;

		blur_x = blur_y = 0;
		monocrhome_color_offset = SFloatRGBColor(0,0,0);
		glare_threshold = 0;
		noise = 0;
		stripe = 0;
		motion_blur_strength = 0;
	}
};


class CameraParam
{
public:

	float time;

	float fov;
	float nearclip;
	float farclip;
	float aspect_ratio;

	CPPEffectParams pp;

	CEntityHandle<> FocusTarget;

public:

	CameraParam()
		:
	time(0.0f),
	fov(3.141592f / 3.0f),
	nearclip(0.05f),
	farclip(200.0f),
//	aspect_ratio(4.0f / 3.0f),
	aspect_ratio( CGraphicsComponent::GetAspectRatio() )
	{}
};


class FocusTargetFrameSet : public KeyFrameSet<CameraTargetHolder>
{
public:

	/// get the positions of the target entities of the current key frames
	bool UpdateFocusTargetPositions( float time )
	{
		if( !IsAvailable( time ) )
			return false;

		size_t i, num_key_frames = m_vecKeyFrame.size();

		// linear interpolation
		// TODO: support some ohter interpolations (e.g. TCB spilne)
		for( i=0; i<num_key_frames-1; i++ )
		{
			if( m_vecKeyFrame[i].time <= time && time < m_vecKeyFrame[i+1].time )
			{
				for( int j=0; j<2; j++ )
				{
					CCopyEntity *pTarget = m_vecKeyFrame[i+j].val.m_Target.GetRawPtr();

					if( pTarget )
					{
						m_vecKeyFrame[i+j].val.m_vTargetPos = pTarget->Position();
					}
				}
				return true;
			}
		}

		return false;
	}

	void UpdateFocusTargetEntities( CEntitySet *pEntitySet );

};


class CCameraProperty
{
public:
	KeyFrameSet<float> fov;
	KeyFrameSet<float> nearclip;
	KeyFrameSet<float> farclip;
	KeyFrameSet<float> aspect_ratio;

	FocusTargetFrameSet FocusTarget;

public:

	void Reset()
	{
		fov.Reset();
		nearclip.Reset();
		farclip.Reset();
		aspect_ratio.Reset();
		FocusTarget.Reset();
	}
};


class CScreenEffectProperty
{
public:
	// effects
	KeyFrameSet<Vector2> Blur;
	KeyFrameSet<float> MotionBlurStrength;
	KeyFrameSet<SFloatRGBColor> MonochromeColorOffset;
	KeyFrameSet<float> GlareThreshold;

	KeyFrameSet<float> Noise;
	KeyFrameSet<float> Stripe;

	KeyFrameSet<SFloatRGBColor> FadeColor;

public:

	void Reset()
	{
		Blur.Reset();
		MotionBlurStrength.Reset();
		MonochromeColorOffset.Reset();
		GlareThreshold.Reset();

		Noise.Reset();
		Stripe.Reset();

		FadeColor.Reset();
	}
};


class CScriptCameraKeyFrames
{
public:
	CCameraProperty Camera;
	CScreenEffectProperty Effect;

	void Reset() { Camera.Reset(); Effect.Reset(); }
};


/**
 Scripted camera entities are terminated when they finish following the given path


*/
class CScriptedCameraEntity : public CCopyEntity
{
	CCamera m_Camera;

	CBEC_MotionPath m_Path;

	CameraParam m_DefaultParam;

	CPPEffectParams m_PPEffectParams;

	CScriptCameraKeyFrames m_KeyFrames;

	bool m_InitializedAtCutsceneStart;

	/// set to true after 'm_Path' set
	/// - e.g., through a game message
	/// - prevent the base entity treating this as expired before receiving motion path
	/// - What about m_KeyFrames?
	bool m_Initialized;

//	cdv<Vector3> m_vCamPos;

	cdv<Matrix33> m_CamOrient;

//	LPDIRECT3DTEXTURE9 pSceneTexture;

private:

	void UpdateCameraOrientationByFocusTarget( float current_time );

public:

	CScriptedCameraEntity()
		:
	m_InitializedAtCutsceneStart(false),
	m_Initialized(false)
	{
		m_CamOrient.target  = Matrix33Identity();
		m_CamOrient.current = Matrix33Identity();//.FromRotationMatrix( Matrix33Identity() );
		m_CamOrient.vel = Matrix33Identity();//Quaternion(0,0,0,0);
		m_CamOrient.smooth_time = 0.3f;
	}

	void Update( float dt );

	void RenderStage();

	void CreateRenderTasks();

	void HandleMessage( SGameMessage& msg );

	void UpdateCameraParams();

	const CBEC_MotionPath& GetPath() const { return m_Path; }
};


class CBE_ScriptedCamera : public CBaseEntity
{
private:

	void UpdateCameraParams( CCopyEntity* pCopyEnt );

public:

	CBE_ScriptedCamera();

	~CBE_ScriptedCamera() {}

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act( CCopyEntity* pCopyEnt );	//behavior in in one frame

//	void Draw(CCopyEntity* pCopyEnt);

	void RenderStage(CCopyEntity* pCopyEnt);

	void CreateRenderTasks(CCopyEntity* pCopyEnt);

    void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) { pCopyEnt_Self->HandleMessage( rGameMessage ); }


//	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_SCRIPTEDCAMERA; }

//	virtual void Serialize( IArchive& ar, const unsigned int version );
	
	virtual void UpdateCamera( CCopyEntity *pCopyEnt ) {}
//	virtual CCamera *GetCamera() { return &m_Camera; }

	friend CBE_CameraController;
};


#endif  /*  __BE_ScriptedCamera_H__  */
