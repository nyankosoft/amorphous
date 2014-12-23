#ifndef __GLProgressiveMeshImpl_HPP__
#define __GLProgressiveMeshImpl_HPP__


#include "GLBasicMeshImpl.hpp"


namespace amorphous
{


/// NOT IMPLEMENTED YET
class CGLProgressiveMeshImpl : public GLBasicMeshImpl
{

public:

	CGLProgressiveMeshImpl();

	CGLProgressiveMeshImpl( const std::string& filename );

	virtual ~CGLProgressiveMeshImpl();

	void Release() {}
};


} // namespace amorphous



#endif	/*  __GLProgressiveMeshImpl_HPP__  */
