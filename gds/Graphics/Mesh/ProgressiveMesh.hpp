#ifndef __ProgressiveMesh_HPP__
#define __ProgressiveMesh_HPP__


#include "BasicMesh.hpp"


namespace amorphous
{


class CProgressiveMesh : public CBasicMesh
{
public:

	CProgressiveMesh();

	virtual ~CProgressiveMesh() {}

	virtual CMeshType::Name GetMeshType() const { return CMeshType::PROGRESSIVE; }
};


} // namespace amorphous



#endif /* __ProgressiveMesh_HPP__ */
