#ifndef __SkeletonRenderer_H__
#define __SkeletonRenderer_H__

#include <vector>
#include <memory>

#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/3DMath/ray.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "Skeleton.hpp"


namespace amorphous
{


namespace msynth
{


class SkeletonRenderer
{
	std::weak_ptr<Skeleton> m_pSkeleton;

	std::vector<Matrix34> m_vecBonePose;

	std::vector<Vector3> m_vecBoneSize;
	
	std::vector<CLineSegment> m_vecLineSegment;

//	CUnitCube m_Cube;

	std::shared_ptr<FontBase> m_pFont;

	bool m_DisplayBoneNames;

private:

	void StoreBonePose_r( const Bone& bone, const TransformNode& node, const Matrix34& parent_transform, int depth = 0 );

	void RenderSkeletonAsLines();

	void RenderSkeletonAsBoxes();

public:

	SkeletonRenderer();

	~SkeletonRenderer();

	void UpdateBonePoses( const Keyframe& keyframe );

	void SetSkeleton( std::weak_ptr<Skeleton> pSkeleton ) { m_pSkeleton = pSkeleton; }

//	void SetSkeleton( const Skeleton& skeleton ) { m_pSkeleton = skeleton; }

	void Render();

	void SetDisplayBoneNames( bool display ) { m_DisplayBoneNames = display; }
};


} // namespace msynth

} // namespace amorphous



#endif /* __SkeletonRenderer_H__ */
 