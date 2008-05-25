#ifndef __BE_ScriptedCamera_H__
#define __BE_ScriptedCamera_H__

#include "BaseEntity.h"
#include "BaseEntityHandle.h"
#include "CopyEntity.h"

#include "EntityMotionPathRequest.h"

#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/Camera.h"

#include "3DMath/Vector2.h"


class CBE_CameraController;


class CameraTargetHolder
{
public:

	string m_TargetName;	///< name of the target to focus. used when the target is not found when a keyframe is created

	CCopyEntity* m_pTarget;	///< target to focus

	Vector3 m_vTargetPos;

public:

	CameraTargetHolder() : m_pTarget(NULL), m_vTargetPos(Vector3(0,0,0)) {}
	CameraTargetHolder( Vector3 vTargetPos ) : m_pTarget(NULL), m_vTargetPos(vTargetPos) {}
	CameraTargetHolder( CCopyEntity* pTarget ) : m_pTarget(pTarget), m_vTargetPos(Vector3(0,0,0)) {}

	CameraTargetHolder( const std::string& target_name ) : m_TargetName(target_name), m_pTarget(NULL), m_vTargetPos(Vector3(0,0,0)) {}

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

		if( num_key_frames == 0 )
			return false;
		else if( num_key_frames == 1 )
			return false;
		else if( time < m_vecKeyFrame[0].time || m_vecKeyFrame.back().time < time )
			return false;
		else
		{
			// linear interpolation
			// TODO: support some ohter interpolations (e.g. TCB spilne)
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
	}

	bool IsAvailable( float time )
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

	CCopyEntity* pFocusTarget;

public:

	CameraParam()
		:
	time(0.0f),
	fov(3.141592f / 3.0f),
	nearclip(0.05f),
	farclip(200.0f),
	aspect_ratio(4.0f / 3.0f),
	pFocusTarget(NULL)
	{}
};


class CEntitySet;

class FocusTargetFrameSet : public KeyFrameSet<CameraTargetHolder>
{
public:

	bool UpdateFocusTargetPositions( float time )
	{
		if( !IsAvailable( time ) )
			return false;

		size_t i, num_key_frames = m_vecKeyFrame.size();

		if( num_key_frames == 0 )
			return false;
		else if( num_key_frames == 1 )
			return false;
		else
		{
			// linear interpolation
			// TODO: support some ohter interpolations (e.g. TCB spilne)
			for( i=0; i<num_key_frames-1; i++ )
			{
				if( m_vecKeyFrame[i].time <= time && time < m_vecKeyFrame[i+1].time )
				{
					for( int j=0; j<2; j++ )
					{
						if( IsValidEntity(m_vecKeyFrame[i+j].val.m_pTarget) )
						{
							m_vecKeyFrame[i+j].val.m_vTargetPos = m_vecKeyFrame[i+j].val.m_pTarget->Position();
						}
					}
					return true;
				}
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


class CBEC_ScriptedCameraExtraData
{
public:

	CBEC_MotionPath Path;

	CameraParam DefaultParam;

	CPPEffectParams PPEffectParams;

	CCamera Camera;

	CScriptCameraKeyFrames KeyFrames;

	bool m_InitializedAtCutsceneStart;

	bool m_bInUse;

//	LPDIRECT3DTEXTURE9 pSceneTexture;

	CBEC_ScriptedCameraExtraData()
		:
	m_InitializedAtCutsceneStart(false),
	m_bInUse(false)
	{}

};


class CBE_ScriptedCamera : public CBaseEntity
{
private:

	CCamera m_Camera;

//	CBEC_MotionPath m_Path;

	std::vector<CBEC_ScriptedCameraExtraData> m_vecExtraData;

private:

	void AddExtraData();

	inline int GetNewExtraDataIndex();

	inline CBEC_ScriptedCameraExtraData& GetExtraData( CCopyEntity *pCopyEnt ) { return m_vecExtraData[pCopyEnt->iExtraDataIndex]; }

	void UpdateCameraParams( CCopyEntity* pCopyEnt );

public:

	CBE_ScriptedCamera();
	~CBE_ScriptedCamera() {}

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame

//	void Draw(CCopyEntity* pCopyEnt);

	void RenderStage(CCopyEntity* pCopyEnt);

	void CreateRenderTasks(CCopyEntity* pCopyEnt);

    virtual void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);


//	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_SCRIPTEDCAMERA; }

//	virtual void Serialize( IArchive& ar, const unsigned int version );
	
	virtual void UpdateCamera( CCopyEntity *pCopyEnt ) {}
	virtual CCamera *GetCamera() { return &m_Camera; }

	friend CBE_CameraController;
};


inline int CBE_ScriptedCamera::GetNewExtraDataIndex()
{
	size_t i, num_extra_data = m_vecExtraData.size();
	for( i=0; i<num_extra_data; i++ )
	{
		if( !m_vecExtraData[i].m_bInUse )
			return (int)i;
	}

	// add new extra data
	AddExtraData();

	return (int)num_extra_data;
}


#endif  /*  __BE_ScriptedCamera_H__  */
