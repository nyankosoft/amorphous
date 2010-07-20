#ifndef __LWS_Items_HPP__
#define __LWS_Items_HPP__


//=====================================================================
// structures and classes to store various items and components
// in LightWave scene files. (*.lws files)
//=====================================================================


#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>
#include "3DMath/Matrix34.hpp"


class CTextFileScanner;


class CLWS_Keyframe
{
public:

	float fValue;
	float fTime;
	int iSpantype;
	float fCurveParam[6];

public:

	CLWS_Keyframe()
		:
	fValue(0),
	fTime(0),
	iSpantype(0)
	{
		memset( fCurveParam, 0, sizeof(fCurveParam) );
	}
};


class CLWS_Channel
{
public:

	std::vector<CLWS_Keyframe> vecKey;
	int iPreBehavior;
	int iPostBehavior;

public:

	CLWS_Channel()
		:
	iPreBehavior(0),
	iPostBehavior(0)
	{}

	void Load(FILE* fp);

	void Quantize( float q );
};



//===============================================================================================
// CLWS_Item
//===============================================================================================

class CLWS_Item
{
protected:

	enum Params
	{
		NUM_MAX_CHANNELS = 9,
	};

	int m_iNumChannels;
	CLWS_Channel m_aChannel[NUM_MAX_CHANNELS];

	/// parent item info
	int m_iParentType;
	int m_iParentIndex;

	/// parent id - added for lws files with version 5 or later.
	/// For prior versions, this id is decomposed into type and index above
	int m_ParentID;

	/// used by scene file with version 5 or later
	int m_ItemID;

	/// child item info
	std::vector<int> m_vecChildType;
	std::vector<int> m_vecChildIndex;

	float m_afPivotRotationAngle[3];

//	boost::shared_ptr<CLWS_Item> m_pParent;
	CLWS_Item *m_pParent;

protected:

	void LoadChannelBlocksFromFile( int iNumChannels, FILE* fp );

	bool LoadChannels( CTextFileScanner& scanner );

	void CheckChannelBlock( char* pcFirstLine, FILE* fp );

	void AddChildItemInfo( int type, int index ) { m_vecChildType.push_back(type); m_vecChildIndex.push_back(index); }

	friend class CLightWaveSceneLoader;

public:

	enum ItemType
	{
		TYPE_OBJECT,
		TYPE_LIGHT,
		TYPE_CAMERA,
		TYPE_BONE,
		NUM_ITEM_TYPES
	};

public:

	CLWS_Item();

	virtual ItemType GetItemType() const = 0;

	int GetItemID() const { return m_ItemID; }

	void SetItemID( int item_id ) { m_ItemID = item_id; }

	virtual bool LoadFromFile( CTextFileScanner& scanner );

	float GetValueAt( int channel, float fTime );

	/// For the moment, this function returns the position at frame 0,
	/// and time in keyframes ('fTime') is ignored. 
	Vector3 GetPositionAt( float fTime );

	Vector3 GetPositionAtKeyframe( int keyframe );

	void GetOrientationAt( float fTime, Matrix33& matOrient );

	inline Matrix33 GetOrientationAt( float fTime );

	void GetOrientationAtKeyframe( int keyframe, Matrix33& matOrient );

	void GetPoseAt( float fTime, Matrix34& rDestPose );

	void GetPoseAtKeyframe( int keyframe, Matrix34& rDestPose );

	/// returns the number of the keyframes of the item
	/// returns the maximum number of keyframes if channels have different numbers of keyframes
	int GetNumKeyFrames();

	/// returns the position at the specified keyframe
	/// TODO: The return value will be incorrect
	/// when channels have different numbers of keyframes
	Vector3 GetPositionAtKeyFrame( int iKeyFrame );

	/// calculates differences of each angle component between fStartTime and fEndTime
	Matrix33 GetDeltaOrientation( float fStartTime, float fEndTime );

	float GetTimeAtKeyFrame( int iKeyFrame );

	bool HasParent() { if( 0 <= m_iParentType && 0 <= m_iParentIndex ) return true; else return false; }

	int GetParentType() const { return m_iParentType; }

	int GetParentIndex() const { return m_iParentIndex; }

	int GetNumChildren() const { return (int)m_vecChildType.size(); }
	int GetChildType( int i ) const { return m_vecChildType[i]; }
	int GetChildIndex( int i ) const { return m_vecChildIndex[i]; }

	int GetParentID() const { return m_ParentID; }

	CLWS_Item *GetParent() { return m_pParent; }

	int GetNumChannels() const { return m_iNumChannels; }

	const CLWS_Channel& GetChannel( int i ) const { return m_aChannel[i]; }

	void QuantizeRotations( float q );
	void QuantizeTranslations( float q );

//	Matrix33 GetOrientationAtKeyFrame( int iKeyFrame );
};

inline Matrix33 CLWS_Item::GetOrientationAt( float fTime )
{
	Matrix33 matOrient = Matrix33Identity();
	GetOrientationAt( fTime, matOrient );
	return matOrient;
}


//===============================================================================================
// CLWS_ObjectLayer
//===============================================================================================

class CLWS_ObjectLayer : public CLWS_Item
{
	std::string m_strObjectFilename;

	int m_iLayerNumber;

	bool m_bNullObject;

public:

	CLWS_ObjectLayer();

	ItemType GetItemType() const { return TYPE_OBJECT; }

	bool LoadFromFile( CTextFileScanner& scanner );

	std::string& GetObjectFilename() { return m_strObjectFilename; }

	int GetLayerNumber() const { return m_iLayerNumber; }

	void SetNullObject( bool b ) { m_bNullObject = b; }

	friend class CLightWaveSceneLoader;
};



//===============================================================================================
// CLWS_Bone
//===============================================================================================

class CLWS_Bone : public CLWS_Item
{
	std::string m_strBoneName;

//	int m_iLayerNumber;

//	bool m_bNullObject;

	float m_fBoneRestLength;

	Vector3 m_vBoneRestPosition;

//	Vector3 m_vBoneRestDirection;

	/// rotation angles in radians
	/// - CLWS_Bone::LoadFromFile() converts the value from degree to radian
	/// - LWS file stores this value in degrees, even though Key values in rotation channels
	///   are in radian.
	float m_afBoneRestAngle[3];

	std::string m_strBoneWeightMapName;

	std::vector< boost::shared_ptr<CLWS_Bone> > m_vecpChildBone;

	boost::shared_ptr<CLWS_Bone> m_pParentBone;

public:

	CLWS_Bone();

	ItemType GetItemType() const { return TYPE_BONE; }

	bool LoadFromFile( CTextFileScanner& scanner );

	const std::string& GetBoneName() const { return m_strBoneName; }

	std::vector< boost::shared_ptr<CLWS_Bone> >& ChildBone() { return m_vecpChildBone; }

	boost::shared_ptr<CLWS_Bone> GetParentBone() { return m_pParentBone; }

	float GetBoneRestLength() const { return m_fBoneRestLength; }

	Vector3 GetBoneRestPosition() const { return m_vBoneRestPosition; }

//	Vector3 GetBoneRestDirection() const { return m_vBoneRestDirection; }

	float GetBoneRestAngle( int i ) const { return m_afBoneRestAngle[i]; }

	Matrix33 GetOrientationFromRestOrientationAt( float fTime );

	void GetOffsetOrientationAt( float fTime, Matrix33& matOrient );
	Matrix33 GetOffsetOrientationAt( float fTime ) { Matrix33 ret = Matrix33Identity(); GetOffsetOrientationAt(fTime,ret); return ret; }

	Matrix33 GetBoneRestOrientation() const;

	friend class CLightWaveSceneLoader;
};



//===============================================================================================
// CLWS_Light
//===============================================================================================

class CLWS_Light : public CLWS_Item
{
	std::string m_strLightName;

	float m_afLightColor[3];
	float m_fLightIntensity;

	int m_LightType;

public:

	enum eLWS_LightType
	{
		TYPE_INVALID,
		TYPE_DISTANT,
		TYPE_POINT,
		TYPE_SPOT,
		TYPE_LINEAR,
		TYPE_AREA
	};

public:

	CLWS_Light();

	ItemType GetItemType() const { return TYPE_LIGHT; }

	std::string& GetName() { return m_strLightName; }

	float* GetColor() { return m_afLightColor; }

	float GetIntensity() const { return m_fLightIntensity; }

	int GetType() const { return m_LightType; }

	virtual bool LoadFromFile( CTextFileScanner& scanner );
};



//===============================================================================================
// CLWS_Fog
//===============================================================================================

class CLWS_Fog
{
public:

	enum eLWS_FogType
	{
		TYPE_OFF,
		TYPE_LINEAR,
		TYPE_NONLINEAR1,
		TYPE_NONLINEAR2
	};

public:

	int iType;
	float fMinDist;
	float fMaxDist;
	float fMinAmount;
	float fMaxAmount;
	float afColor[3];	// [0]:red / [1]:green / [2]:blue

	CLWS_Fog();
};


#endif /* __LWS_Items_HPP__ */
