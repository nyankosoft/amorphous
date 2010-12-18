#ifndef __VIEWFRUSTUMTEST_H__
#define __VIEWFRUSTUMTEST_H__

#include <vector>
#include "../Graphics/fwd.hpp"
#include "fwd.hpp"


#define NUM_MAX_VISIBLEENTITIES 256

#define VFT_IGNORE_NOCLIP_ENTITIES	1


// used to call CStage::GetVisibleEntities(CViewFrustumTest& vf_test);
// in order to collect entites in the view frustum

class CViewFrustumTest
{
public:
	CCamera *m_pCamera;

	int m_Flag;

	std::vector<CCopyEntity *> *m_pvecpVisibleEntity;

public:

	inline CViewFrustumTest()
	{
		m_pCamera = NULL;
		m_Flag = 0;
		m_pvecpVisibleEntity = NULL;
	}

	inline void SetCamera(CCamera *pCamera) { m_pCamera  = pCamera; }
	inline CCamera *GetCamera() { return m_pCamera; }

	inline void SetBuffer( std::vector<CCopyEntity *>& rvecpVisibleEntity ) { m_pvecpVisibleEntity = &rvecpVisibleEntity; }

	inline int GetNumVisibleEntities() { return (int)m_pvecpVisibleEntity->size(); }

	inline CCopyEntity* GetEntity(int i) { return (*m_pvecpVisibleEntity)[i]; }

	inline void AddEntity(CCopyEntity *pCopyEnt) { m_pvecpVisibleEntity->push_back( pCopyEnt ); }

	inline void ClearEntities() { m_pvecpVisibleEntity->resize( 0 ); }
};


//=============================================================================
// inline implementations
//=============================================================================


#endif  /* __VIEWFRUSTUMTEST_H__ */