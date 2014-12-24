#ifndef __GLProgressiveMeshImpl_HPP__
#define __GLProgressiveMeshImpl_HPP__


#include "GLBasicMeshImpl.hpp"


namespace amorphous
{


/// NOT IMPLEMENTED YET
class GLProgressiveMeshImpl : public GLBasicMeshImpl
{

public:

	GLProgressiveMeshImpl();

	GLProgressiveMeshImpl( const std::string& filename );

	virtual ~GLProgressiveMeshImpl();

	void Release() {}
};


} // namespace amorphous



#endif	/*  __GLProgressiveMeshImpl_HPP__  */
