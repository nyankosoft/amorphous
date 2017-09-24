#ifndef  __SurfaceMaterialHandle_H__
#define  __SurfaceMaterialHandle_H__


#include "amorphous/Support/NamedResourceHandle.hpp"


namespace amorphous
{

class CSurfaceMaterialManager;

//typedef CNamedResourceHandle<CSurfaceMaterialManager> CSurfaceMaterialHandle;

class CSurfaceMaterialHandle : public CNamedResourceHandle
{
public:

	friend class CSurfaceMaterialManager;
};

} // amorphous



#endif		/*  __SurfaceMaterialHandle_H__  */
