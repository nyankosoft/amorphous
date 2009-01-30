
#ifndef __KGL_2DARRAY_SERIALIZATION_H__
#define __KGL_2DARRAY_SERIALIZATION_H__

#include "../2DArray.hpp"

#include "Archive.hpp"


namespace GameLib1
{

namespace Serialization
{


template <class T>
inline IArchive& operator & ( IArchive& ar, C2DArray<T>& r2DArray )
{
	int x, y, iSizeX, iSizeY;
	if( ar.GetMode() == IArchive::MODE_OUTPUT )
	{
		// record array size
		iSizeX = r2DArray.size_x();
		iSizeY = r2DArray.size_y();
		ar & iSizeX;
		ar & iSizeY;

		// record each element
		for( y=0; y<iSizeY; y++ )
		{
			for( x=0; x<iSizeX; x++ )
			{
				ar & r2DArray(x,y);
			}
		}
	}
	else // i.e. input mode
	{
		// load array size
		ar & iSizeX;
		ar & iSizeY;
		r2DArray.resize(iSizeX,iSizeY);

		for( y=0; y<iSizeY; y++ )
		{
			for( x=0; x<iSizeX; x++ )
			{
				ar & r2DArray(x,y);
			}
		}
	}

	return ar;
}


}	  /*  Serialization  */

}	  /*  GameLib1  */


#endif  /*  __KGL_2DARRAY_SERIALIZATION_H__  */
