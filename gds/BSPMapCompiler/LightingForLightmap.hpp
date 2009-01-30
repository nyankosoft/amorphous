#ifndef  __MAPCOMPILER_LIGHTINGFORLIGHTMAP_H__
#define  __MAPCOMPILER_LIGHTINGFORLIGHTMAP_H__


#include <vector>
using namespace std;

class CBaseLight;

#include "Lightmap.hpp"
#include "MapFace.hpp"

//#include "3DMath/PolygonMesh.hpp"

//==========================================================================================
// CLightingForLightmap
//==========================================================================================

template <class T>
class CLightingForLightmap
{
protected:

	/// pointer to the array of pointers to light objects  (ライトオブジェクトのポインタの配列、へのポインタ)
	vector<CBaseLight *>* m_pvecpLight;

public:

	/// set lights
	/// maintains the pointer only and light objects are not copied
	virtual void SetLights( vector<CBaseLight *>& rvecpLight ) { m_pvecpLight = &rvecpLight; }

	virtual bool Calculate( vector<CLightmap>& rvecLightmap, CPolygonMesh<T>& rMesh ) = 0;
};


#endif  /*  __MAPCOMPILER_LIGHTINGFORLIGHTMAP_H__  */
