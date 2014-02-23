#ifndef  __KGL_ARCHIVEOBJECTFACTORY_H__
#define  __KGL_ARCHIVEOBJECTFACTORY_H__


namespace amorphous
{

namespace serialization
{


class IArchiveObjectBase;

class IArchiveObjectFactory
{

public:
//	IArchiveObjectFactory() {}

	virtual ~IArchiveObjectFactory() {}

	virtual IArchiveObjectBase *CreateObject(const unsigned int id) = 0;

};


} // namespace serialization

} // namespace amorphous

#endif		/*  __KGL_ARCHIVEOBJECTFACTORY_H__  */
