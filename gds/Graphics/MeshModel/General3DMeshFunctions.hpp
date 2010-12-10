#ifndef __General3DMeshFunctions_HPP__
#define __General3DMeshFunctions_HPP__


#include <vector>
#include "../fwd.hpp"


void GetConnectedSets( const CGeneral3DMesh& src_mesh, std::vector< std::vector<int> >& connected_polygon_sets );
void GetConnectedSets( const CGeneral3DMesh& src_mesh, std::vector< boost::shared_ptr<CGeneral3DMesh> >& pDestMeshes );


#endif /* __General3DMeshFunctions_HPP__ */

