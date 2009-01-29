#ifndef __SkeletonRenderer_H__
#define __SkeletonRenderer_H__

#include <vector>
#include <boost/weak_ptr.hpp>

#include "3DMath/Matrix34.h"
#include "Graphics/UnitCube.h"
#include "MotionSynthesis/Skeleton.h"
#include "MotionSynthesis/MotionPrimitive.h"


namespace msynth
{


class CSkeletonRenderer
{
	boost::weak_ptr<CSkeleton> m_pSkeleton;

	std::vector<Matrix34> m_vecBonePose;

	std::vector<Vector3> m_vecBoneSize;

	CUnitCube m_Cube;

private:

	void StoreBonePose_r( const CBone& bone, const CTransformNode& node, const Matrix34& parent_transform );

public:

	CSkeletonRenderer();

	~CSkeletonRenderer();

	void UpdateBonePoses( const CKeyframe& keyframe );

	void SetSkeleton( boost::weak_ptr<CSkeleton> pSkeleton ) { m_pSkeleton = pSkeleton; }

//	void SetSkeleton( const CSkeleton& skeleton ) { m_pSkeleton = skeleton; }

	void Render();
};


} // namespace msynth


#endif /* __SkeletonRenderer_H__ */
 