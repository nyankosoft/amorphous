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

	virtual MeshType::Name GetMeshType() const { return MeshType::PROGRESSIVE; }
};


} // namespace amorphous



#endif /* __ProgressiveMesh_HPP__ */
