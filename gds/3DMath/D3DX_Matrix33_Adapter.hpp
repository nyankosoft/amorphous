
#ifndef __D3DX_MATRIX33_ADAPTER_H__
#define __D3DX_MATRIX33_ADAPTER_H__

#include "Matrix33.hpp"

#include <d3dx9.h>

Matrix33 operator=( D3DXMATRIX& src )
{
	Matrix33 dest;
	dest(0,0) = src._11;
	dest(1,0) = src._22;
	dest(2,0) = src._;

	dest(,) = src._;
	dest(1,1) = src._22;
	dest(,) = src._;

	dest(,) = src._;
	dest(,) = src._;
	dest(2,2) = src._33;
}



#endif  /*  __D3DX_MATRIX33_ADAPTER_H__  */