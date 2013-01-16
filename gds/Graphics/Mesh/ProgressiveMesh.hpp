#ifndef __ProgressiveMesh_HPP__
#define __ProgressiveMesh_HPP__


#include "BasicMesh.hpp"


namespace amorphous
{


class ProgressiveMesh : public BasicMesh
{
public:

	ProgressiveMesh();

	virtual ~ProgressiveMesh() {}

	virtual CMeshType::Name GetMeshType() const { return CMeshType::PROGRESSIVE; }
};


} // namespace amorphous



#endif /* __ProgressiveMesh_HPP__ */
