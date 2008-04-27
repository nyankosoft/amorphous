#ifndef __CD3DXFRAME_H__
#define __CD3DXFRAME_H__

#include <d3dx9.h>

class CD3DXFrame: public D3DXFRAME
{
public:
	CD3DXFrame();
	~CD3DXFrame();
	void SetName( LPCTSTR Name );

public:
	D3DXMATRIXA16 CombinedTransformationMatrix;
};

#endif // __CD3DXFRAME_H__
