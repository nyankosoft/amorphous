#ifndef __amorphous_ViewFrustumTest_HPP__
#define __amorphous_ViewFrustumTest_HPP__

#include <vector>
#include "../Graphics/fwd.hpp"
#include "fwd.hpp"


namespace amorphous
{


#define NUM_MAX_VISIBLEENTITIES 256

#define VFT_IGNORE_NOCLIP_ENTITIES	1


// used to call CStage::GetVisibleEntities(ViewFrustumTest& vf_test);
// in order to collect entites in the view frustum

class ViewFrustumTest
{
public:
	Camera *m_pCamera;

	int m_Flag;

	std::vector<CCopyEntity *> *m_pvecpVisibleEntity;

public:

	inline ViewFrustumTest()
	{
		m_pCamera = NULL;
		m_Flag = 0;
		m_pvecpVisibleEntity = NULL;
	}

	inline void SetCamera(Camera *pCamera) { m_pCamera  = pCamera; }
	inline Camera *GetCamera() { return m_pCamera; }

	inline void SetBuffer( std::vector<CCopyEntity *>& rvecpVisibleEntity ) { m_pvecpVisibleEntity = &rvecpVisibleEntity; }

	inline int GetNumVisibleEntities() { return (int)m_pvecpVisibleEntity->size(); }

	inline CCopyEntity* GetEntity(int i) { return (*m_pvecpVisibleEntity)[i]; }

	inline void AddEntity(CCopyEntity *pCopyEnt) { m_pvecpVisibleEntity->push_back( pCopyEnt ); }

	inline void ClearEntities() { m_pvecpVisibleEntity->resize( 0 ); }
};


//=============================================================================
// inline implementations
//=============================================================================

} // namespace amorphous



#endif  /* __amorphous_ViewFrustumTest_HPP__ */
