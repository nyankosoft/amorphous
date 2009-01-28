#ifndef  __JL_PHYSICSVISUALIZER_D3D_H__
#define  __JL_PHYSICSVISUALIZER_D3D_H__


//#include <vector>
//using namespace std;


#include "Graphics/GraphicsComponentCollector.h"


class CJL_PhysicsManager;
class CUnitCube;


#define JL_VIS_CONTACT_POINTS	1
#define JL_VIS_CONTACT_NORMALS	2
#define JL_VIS_AABB				4
#define JL_VIS_VELOCITY			8
#define JL_VIS_ANGULAR_VELOCITY	16


/**
 * CJL_PhysicsVisualizer_D3D
 *  - used for visual debugging of JigLib physics library
 *  - displays physics-related information such as velocities,
 *    bounding boxes, contact points, etc.
*/
class CJL_PhysicsVisualizer_D3D : public CGraphicsComponent
{
	CJL_PhysicsManager *m_pPhysicsManager;

	CUnitCube *m_pUnitCube;

	int m_RenderStateFlag;

	void DisplayContactPoints();

	void DisplayActorInfo();

public:

	CJL_PhysicsVisualizer_D3D( CJL_PhysicsManager *pPhysicsManager );

	~CJL_PhysicsVisualizer_D3D();

	void RaiseRenderStateFlag(int Flag) { m_RenderStateFlag |= Flag; }

	void ClearRenderStateFlag(int Flag) { m_RenderStateFlag &= ~Flag; }

	int GetRenderStateFlag() { return m_RenderStateFlag; }

//	void Init();
//	void Release();

	void RenderVisualInfo();


	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

};



#endif		/*  __JL_PHYSICSVISUALIZER_D3D_H__  */
