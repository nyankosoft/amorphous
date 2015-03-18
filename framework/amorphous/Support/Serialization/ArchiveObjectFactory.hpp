#ifndef  __amorphous_ArchiveObjectFactory_HPP__
#define  __amorphous_ArchiveObjectFactory_HPP__


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


#endif		/*  __amorphous_ArchiveObjectFactory_HPP__  */
