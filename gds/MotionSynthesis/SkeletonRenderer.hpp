#ifndef __SkeletonRenderer_H__
#define __SkeletonRenderer_H__

#include <vector>
#include <boost/weak_ptr.hpp>

#include "3DMath/Matrix34.hpp"
#include "3DMath/ray.hpp"
#include "Graphics/UnitCube.hpp"
#include "MotionSynthesis/Skeleton.hpp"
#include "MotionSynthesis/MotionPrimitive.hpp"


namespace msynth
{


class CSkeletonRenderer
{
	boost::weak_ptr<CSkeleton> m_pSkeleton;

	std::vector<Matrix34> m_vecBonePose;

	std::vector<Vector3> m_vecBoneSize;
	
	std::vector<CLineSegment> m_vecLineSegment;

//	CUnitCube m_Cube;

private:

	void StoreBonePose_r( const CBone& bone, const CTransformNode& node, const Matrix34& parent_transform, int depth = 0 );

	void RenderSkeletonAsLines();

	void RenderSkeletonAsBoxes();

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
 