
#ifndef  __DYNAMICLIGHTMANAGERFORSTATICGEOMETRY_H__
#define  __DYNAMICLIGHTMANAGERFORSTATICGEOMETRY_H__

#include <vector>
using namespace std;

#include <d3dx9tex.h>

class CBSPMap;
class CLightEntity;
class CLightEntityManager;
class CSG_Polygon;

#define NUM_MAX_DYNAMIC_LIGHTS_PER_FRAME 4	//32


class CDynamicLightManagerForStaticGeometry
{
//	CBSPMap *m_pMap;	// pointer to static geometry manager class

	CLightEntityManager *m_pLightEntityManager;

	CLightEntity *m_apDynamicLight[NUM_MAX_DYNAMIC_LIGHTS_PER_FRAME];

	vector<int> m_veciLitPolygonIndex[NUM_MAX_DYNAMIC_LIGHTS_PER_FRAME];

	/// the number of dynamic lights handled by this class during a frame.
	/// updated every frame
	/// NUM_MAX_DYNAMIC_LIGHTS_PER_FRAME is the maximum value of this variable
	int m_iNumCurrentDynamicLights;

public:

	CDynamicLightManagerForStaticGeometry();
	~CDynamicLightManagerForStaticGeometry();

	void SetLightEntityManager(CLightEntityManager *pLightEntityManager) { m_pLightEntityManager = pLightEntityManager; }

	void SetDynamicLights(CBSPMap *pMap);

	void SetDynamicLight( CLightEntity& rLight, CSG_Polygon& rPolygon, CBSPMap *pMap );

	void ClearDynamicLightEffectsFromPolygons(CBSPMap *pMap);
};



#endif		/*  __DYNAMICLIGHTMANAGERFORSTATICGEOMETRY_H__  */