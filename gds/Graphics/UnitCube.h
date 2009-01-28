#ifndef  __UNITCUBE_H__
#define  __UNITCUBE_H__


#include <d3dx9.h>

#define UNITCUBE_NUM_VERTICES 24



// when the box is rendered as wireframes, the first 8 vertices in the vertex buffer are used

class CUnitCube
{
	// Buffer to hold vertices	
	LPDIRECT3DVERTEXBUFFER9 m_pVB;

	// buffer to hold indices to render the box as wireframes
	// not used when the box is rendered in as polygon;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	short m_RenderMode;

public:
	CUnitCube();
	~CUnitCube();

	HRESULT Init();
	void Release();
	void Draw();

	bool SetUniformColor( float r, float g, float b, float a = 1.0f );

	bool SetDefault3Colors();

	void SetRenderMode( short mode ) { m_RenderMode = mode; }
	short GetRenderMode() { m_RenderMode; }

	enum eUnitCubeRenderMode { RS_POLYGON, RS_WIREFRAME };

};


#endif		/*  __UNITCUBE_H__  */
