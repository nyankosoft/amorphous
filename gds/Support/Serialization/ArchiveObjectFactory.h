#ifndef  __KGL_ARCHIVEOBJECTFACTORY_H__
#define  __KGL_ARCHIVEOBJECTFACTORY_H__


namespace GameLib1
{

namespace Serialization
{


class IArchiveObjectBase;

class IArchiveObjectFactory
{

public:
//	IArchiveObjectFactory() {}
//	~IArchiveObjectFactory() {}

	virtual IArchiveObjectBase *CreateObject(const unsigned int id) = 0;

};


}	/*  Serialization  */

}	/*  GameLib1  */

#endif		/*  __KGL_ARCHIVEOBJECTFACTORY_H__  */
