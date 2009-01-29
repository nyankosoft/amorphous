#ifndef  __SKYBOX_H__
#define  __SKYBOX_H__

#include "BSPStaticGeometry.h"

#define  MAX_SKYBOXPOLYGONS	6


class CBSPMap;

class CSkybox
{
	CSG_Polygon m_aSkyboxPolygon[MAX_SKYBOXPOLYGONS];
	int m_iNumPolygons;

public:
	CSkybox();
	~CSkybox();

	/// render skybox with fixed function shader
	void Render( Vector3& rvOffset = D3DXVECTOR3(0,0,0) );

	/// render skybox with HLSL
//	void Render_Effect( D3DXVECTOR3& rvOffset = D3DXVECTOR3(0,0,0) );

	void SetSkyboxPolygons( CSG_Polygon* paPolygon, int iNumPolygons );

	friend class CBSPMap;

};



#endif		/*  __SKYBOX_H__  */

