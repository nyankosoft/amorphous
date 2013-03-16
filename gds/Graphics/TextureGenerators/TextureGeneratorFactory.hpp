#ifndef __HeaderName_HPP__
#define __HeaderName_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../../Support/Serialization/ArchiveObjectFactory.hpp"


namespace amorphous
{

using namespace serialization;


class TextureGeneratorFactory : public IArchiveObjectFactory
{
public:

	IArchiveObjectBase *CreateObject(const unsigned int id);
};


} // namespace amorphous



#endif /* __HeaderName_HPP__ */
