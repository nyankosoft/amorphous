#ifndef __MeshArchiveToGeneral3DMeshConverer_HPP__
#define __MeshArchiveToGeneral3DMeshConverer_HPP__


#include "../../base.hpp"
#include "../fwd.hpp"
using namespace MeshModel;


class CMeshArchiveToGeneral3DMeshConverer
{
public:
	CMeshArchiveToGeneral3DMeshConverer(){}
	~CMeshArchiveToGeneral3DMeshConverer(){}

	/**
	Limitations
	- Only copies positions of vertices

	*/
	Result::Name Convert( const C3DMeshModelArchive& src, CGeneral3DMesh& dest );
};



#endif /* __MeshArchiveToGeneral3DMeshConverer_HPP__ */
