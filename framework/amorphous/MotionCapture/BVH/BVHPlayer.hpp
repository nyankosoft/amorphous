#ifndef  __BVHPLAYER_H__
#define  __BVHPLAYER_H__


#include "BVHBone.hpp"


namespace amorphous
{


/// Holds translations / rotations for each joint at a frame in a single array of float values.
/// Translations are in meters. angles are in radians.
struct SBVHFrameData
{
	/// stores positions & rotation angles in a single float array
	std::vector<float> m_vecfChannelValue;

	inline int GetNumChannles() const { return (int)m_vecfChannelValue.size(); }

	inline float GetValue( int i ) const { return m_vecfChannelValue[i]; }
};


class BVHPlayer
{
protected:

	/// root joint
	BVHBone m_RootBone;

	/// stores original filename
	std::string m_strBVHFilename;

	/// stores motion data
	std::vector<SBVHFrameData> m_vecFrame;

	/// stores the channel type info
	std::vector<int> m_vecChannelType;

	/// used to temporarily hold interpolated frame values
	std::vector<float> m_vecfInterpolatedFrame;

	/// how many seconds per frame
	/// this is usually 0.033333333[sec]
	float m_fFrameTime;

private:

	void LoadSkeleton( FILE *fp );

	void LoadFrameData( FILE *fp );

public:

	BVHPlayer();

	~BVHPlayer();

	void Reset();

	/// load and store ".bvh" file
	virtual bool LoadBVHFile( const std::string& filename );

	const std::string& GetBVHFileName() const { return m_strBVHFilename; }

	/// set pose at 'fTime'
	bool SetWorldTransformation( float fTime );

	/// render the currently set pose
	virtual bool Render();

	/// render pose at 'fTime'
	virtual bool Render( float fTime );


	inline BVHBone *GetRootBone() { return &m_RootBone; }

	inline const BVHBone *GetRootBone() const { return &m_RootBone; }

	inline int GetNumBones() const { return m_RootBone.GetNumBones_r(); }

	/// basic bvh file information
	inline int GetNumTotalFrames() const { return (int)m_vecFrame.size(); }
	inline float GetFrameTime() const { return m_fFrameTime; }
	inline float GetTotalPlayTime() const { return (float)m_vecFrame.size() * m_fFrameTime; }

	void GetGlobalPositionsAtFrame( int iFrame, std::vector<Vector3>& rvecDestGlobalPositions );

	Matrix34 GetBodyCenterTransformationMatrixAt( int iFrame );

	Vector3 GetBodyCenterPosition( int iFrame );	//get the position of the body center in world coordinate

	SBVHFrameData& GetFrameData( int iFrame ) { return m_vecFrame[iFrame]; }
	std::vector<SBVHFrameData>& GetFrameData() { return m_vecFrame; }

	void DeleteFrames( int iStartFrame, int iEndFrame );
	virtual void DeleteAllFrames() { m_vecFrame.clear(); }
	void ClearStartPositionOffset( int iStartFrame, int iEndFrame );
	void MoveOffset( int iFrame, Vector3 vOffset,
		float fRotAngleZ = 0, float fRotAngleX = 0, float fRotAngleY = 0 );

	void CopyFramesTo( int iStartFrame, int iEndFrame, BVHPlayer& dest_bvh_player ) const;

	void CopySkeletonTo( BVHPlayer& dest_bvh_player ) const;

	/// returns transforms at each joint
	void GetLocalTransforms( Matrix34* paDestTransform ) const ;

	void Scale( float factor );

//	void SetBoneOffsetMatricesForXMesh( char* pcFilename );
};

} // namespace amorphous



#endif		/*  __BVHPLAYER_H__  */
