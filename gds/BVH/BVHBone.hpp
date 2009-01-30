#ifndef  __BVHBONE_H__
#define  __BVHBONE_H__

#include <vector>
#include <string>
#include "3DMath/Matrix34.hpp"
#include "3DMath/Quaternion.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/Direct3D9.hpp"

#include "fwd.hpp"


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

class CBVHBone
{
	std::string m_strName;

	Vector3 m_vOffset;

	int m_iNumChannels;
	//int channel[6];	// how to express?

	std::vector<CBVHBone> m_vecChild;

	Matrix34 m_matLocalPose;
	Matrix34 m_matWorldPose;

	Quaternion m_qLocalRot;

	static DWORD ms_dwSkeletonColor;	// shared by all the bones

	static CUnitCube *ms_pUnitCube;

	void SetChannels( char* pcChannel );

public:

	CBVHBone();

	~CBVHBone();

	void LoadFromFile( char* pcTypeAndName, FILE* fp );

	const std::string& GetName() const { return m_strName; }

	void SetName( const std::string& name ) { m_strName = name; }

	inline int GetNumChildren() const { return (int)m_vecChild.size(); }

	inline CBVHBone *GetChild( int i ) { return &m_vecChild[i]; }

	inline Vector3 GetLocalOffset() const { return m_vOffset; }

	void Draw_r( Vector3* pvPrevPosition = NULL, Matrix34* pParentMatrix = NULL );

	void SetMatrixFromBVHData_r( Matrix34* pParentMatrix, float* pafBVHFrameData, int& riCount);

	void GetGlobalPositions_r( std::vector<Vector3>& rvecDestGlobalPositions, Matrix34* pParentMatrix = NULL );

	void GetChannelType_r( std::vector<int>* pvecChannelType );

	int GetNumBones_r();

	int GetNumChannels() const { return m_iNumChannels; }

	const Matrix34& GetTransformationMatrix() const { return m_matLocalPose; }

	Matrix34 *GetWorldTransformationMatrix() { return &m_matWorldPose; }

	void SetPointersToLocalTransformMatrix_r(std::vector<D3DXMATRIX *> *pvecpLocalTransform);
	void SetPointersToGlobalTransformMatrix_r(std::vector<D3DXMATRIX *> *pvecpGlobalTransform);

	void GetLocalTransformMatrices_r( std::vector<D3DXMATRIX>* pvecLocalTransform,
		                              Vector3 vParentBoneGlobalOffset );

	void GetLocalTransforms_r( Matrix34* paDestTransforms, int& rIndex ) const;

	void Scale_r( float factor );

//	CBVHBone operator=(CBVHBone bone);

	void SetSkeletonColor(DWORD dwSkeletonColor) { ms_dwSkeletonColor = dwSkeletonColor; }

	void DrawBoxForBone(D3DXMATRIX &rmatParent, D3DXMATRIX &rmatWorldTransform);

	static CD3DXMeshObject *ms_pTestCube;

	friend class CPVC_JointHub;

	friend class CBVHPlayer;

};

#endif		/*  __BVHBONE_H__  */
