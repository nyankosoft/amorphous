#ifndef  __UNITSPHERE_H__
#define  __UNITSPHERE_H__


#include <d3dx9.h>
#include "3DCommon/FVF_ColorVertex.h"

#define UNITCUBE_NUM_VERTICES 24



// renders a sphere of radius 1

class CUnitSphere
{
	// Buffer to hold vertices
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

	// buffer to hold indices
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	enum eParam
	{
		NUM_VERTICES	= 92,
		NUM_TRIANGLES	= 180,
		NUM_INDICES		= NUM_TRIANGLES * 3,
	};

//	short m_RenderMode;

	void SetVertices( COLORVERTEX* pv );

	void SetIndices( WORD* pw );

public:

	CUnitSphere();

	~CUnitSphere();

	HRESULT Init();

	void Release();
	void Draw();
	bool SetColor( float r, float g, float b, float a = 1.0f );

//	void SetRenderMode( short mode ) { m_RenderMode = mode; }
//	short GetRenderMode() { m_RenderMode; }

};


#endif		/*  __UNITSPHERE_H__  */
