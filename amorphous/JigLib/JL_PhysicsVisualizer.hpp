#ifndef  __JL_PHYSICSVISUALIZER_H__
#define  __JL_PHYSICSVISUALIZER_H__


#include "Graphics/GraphicsComponentCollector.hpp"


namespace amorphous
{


class CJL_PhysicsManager;
//class CUnitCube;


#define JL_VIS_CONTACT_POINTS	1
#define JL_VIS_CONTACT_NORMALS	2
#define JL_VIS_AABB				4
#define JL_VIS_VELOCITY			8
#define JL_VIS_ANGULAR_VELOCITY	16


/**
 * CJL_PhysicsVisualizer
 *  - used for visual debugging of physics engines
 *  - displays physics-related information such as velocities,
 *    bounding boxes, contact points, etc.
*/
class CJL_PhysicsVisualizer : public GraphicsComponent
{
	CJL_PhysicsManager *m_pPhysicsManager;

//	CUnitCube *m_pUnitCube;

	int m_RenderStateFlag;

	void DisplayContactPoints();

	void DisplayActorInfo();

public:

	CJL_PhysicsVisualizer( CJL_PhysicsManager *pPhysicsManager );

	~CJL_PhysicsVisualizer();

	void RaiseRenderStateFlag(int Flag) { m_RenderStateFlag |= Flag; }

	void ClearRenderStateFlag(int Flag) { m_RenderStateFlag &= ~Flag; }

	int GetRenderStateFlag() { return m_RenderStateFlag; }

//	void Init();
//	void Release();

	void RenderVisualInfo();


	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const GraphicsParameters& rParam );
};


} // namespace amorphous



#endif		/*  __JL_PHYSICSVISUALIZER_H__  */
