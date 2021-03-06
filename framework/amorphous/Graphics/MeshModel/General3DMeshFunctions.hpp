#ifndef __General3DMeshFunctions_HPP__
#define __General3DMeshFunctions_HPP__


#include <vector>
#include "amorphous/Graphics/fwd.hpp"


namespace amorphous
{


void GetConnectedSets( const General3DMesh& src_mesh, std::vector< std::vector<int> >& connected_polygon_sets );
void GetConnectedSets( const General3DMesh& src_mesh, std::vector< std::shared_ptr<General3DMesh> >& pDestMeshes );

} // namespace amorphous



#endif /* __General3DMeshFunctions_HPP__ */

