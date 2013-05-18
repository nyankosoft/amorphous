#ifndef  __BVHBONE_H__
#define  __BVHBONE_H__


#include "fwd.hpp"
#include "../Support/StringAux.hpp"
#include "../3DMath/Matrix34.hpp"
#include "../3DMath/Matrix44.hpp"
#include "../3DMath/Quaternion.hpp"
#include "../Graphics/FloatRGBAColor.hpp"
#include "../Graphics/MeshObjectHandle.hpp"


namespace amorphous
{


enum eChannelType
{
	CT_POSITION_X = 0,
	CT_POSITION_Y,
	CT_POSITION_Z,

	CT_ROTATION_X,
	CT_ROTATION_Y,
	CT_ROTATION_Z
};



class CPVC_JointHub;

class BVHBone
{
	std::string m_strName;

	Vector3 m_vOffset;

	int m_iNumChannels;
	//int channel[6];	// how to express?

	std::vector<BVHBone> m_vecChild;

	Matrix34 m_matLocalPose;
	Matrix34 m_matWorldPose;

	Quaternion m_qLocalRot;

	static SFloatRGBAColor ms_dwSkeletonColor;	// shared by all the bones

//	static CUnitCube *ms_pUnitCube;

	void SetChannels( char* pcChannel );

public:

	BVHBone();

	~BVHBone();

	void Reset();

	void LoadFromFile( char* pcTypeAndName, FILE* fp );

	const std::string& GetName() const { return m_strName; }

	void SetName( const std::string& name ) { m_strName = name; }

	inline int GetNumChildren() const { return (int)m_vecChild.size(); }

	inline BVHBone *GetChild( int i ) { return &m_vecChild[i]; }

	inline const BVHBone *GetChild( int i ) const { return &m_vecChild[i]; }

	inline Vector3 GetLocalOffset() const { return m_vOffset; }

	void Draw_r( Vector3* pvPrevPosition = NULL, Matrix34* pParentMatrix = NULL );

	void SetMatrixFromBVHData_r( Matrix34* pParentMatrix, float* pafBVHFrameData, int& riCount);

	void GetGlobalPositions_r( std::vector<Vector3>& rvecDestGlobalPositions, Matrix34* pParentMatrix = NULL ) const;

	void GetChannelType_r( std::vector<int>* pvecChannelType ) const;

	int GetNumBones_r() const;

	int GetNumChannels() const { return m_iNumChannels; }

	const Matrix34& GetTransformationMatrix() const { return m_matLocalPose; }

	Matrix34 *GetWorldTransformationMatrix() { return &m_matWorldPose; }

	void SetPointersToLocalTransformMatrix_r( std::vector<Matrix34 *> *pvecpLocalTransform );
	void SetPointersToGlobalTransformMatrix_r( std::vector<Matrix34 *> *pvecpGlobalTransform );

	void GetLocalTransformMatrices_r( std::vector<Matrix34>* pvecLocalTransform,
		                              Vector3 vParentBoneGlobalOffset );

	void GetLocalTransforms_r( Matrix34* paDestTransforms, int& rIndex ) const;

	void Scale_r( float factor );

	void SetSkeletonColor(U32 dwSkeletonColor) { ms_dwSkeletonColor.SetARGB32( dwSkeletonColor ); }

	void DrawBoxForBone(Matrix44 &rmatParent, Matrix44 &rmatWorldTransform);

//	static MeshHandle ms_TestCube;

	friend class CPVC_JointHub;

	friend class BVHPlayer;

};

} // namespace amorphous


#endif		/*  __BVHBONE_H__  */
