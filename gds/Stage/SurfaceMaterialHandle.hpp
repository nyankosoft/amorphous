#ifndef  __SurfaceMaterialHandle_H__
#define  __SurfaceMaterialHandle_H__


#include "Support/NamedResourceHandle.hpp"

class CSurfaceMaterialManager;

//typedef CNamedResourceHandle<CSurfaceMaterialManager> CSurfaceMaterialHandle;

class CSurfaceMaterialHandle : public CNamedResourceHandle
{
public:

	friend class CSurfaceMaterialManager;
};


#endif		/*  __SurfaceMaterialHandle_H__  */
