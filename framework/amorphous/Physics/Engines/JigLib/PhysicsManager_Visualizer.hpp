
#ifndef  __PhysicsManager_Visualizer_H__
#define  __PhysicsManager_Visualizer_H__


//#include <vector>
//using namespace std;


#include "amorphous/Graphics/GraphicsComponentCollector.hpp"


namespace amorphous
{


class CJL_PhysicsManager;
class CUnitCube;


#define JL_VIS_CONTACT_POINTS	1
#define JL_VIS_CONTACT_NORMALS	2
#define JL_VIS_AABB				4
#define JL_VIS_VELOCITY			8
#define JL_VIS_ANGULAR_VELOCITY	16


class CPhysicsManager_Visualizer : public CGraphicsComponent
{
	CJL_PhysicsManager *m_pPhysicsManager;
	CUnitCube *m_pUnitCube;

	int m_RenderStateFlag;

	void DisplayContactPoints();
	void DisplayActorInfo();

public:
	CPhysicsManager_Visualizer( CJL_PhysicsManager *pPhysicsManager );
	~CPhysicsManager_Visualizer();

	void RaiseRenderStateFlag(int Flag) { m_RenderStateFlag |= Flag; }
	void ClearRenderStateFlag(int Flag) { m_RenderStateFlag &= ~Flag; }
	int GetRenderStateFlag() { return m_RenderStateFlag; }

//	void Init();
//	void Release();

	void RenderVisualInfo();


	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

};


} // amorphous



#endif		/*  __PhysicsManager_Visualizer_H__  */