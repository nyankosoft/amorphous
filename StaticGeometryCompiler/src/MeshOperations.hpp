#ifndef  __MeshOperations_HPP__
#define  __MeshOperations_HPP__



#include <vector>
#include "gds/Graphics/fwd.hpp"
#include "gds/3DMath/Sphere.hpp"


class CConnectedSet
{
public:
	std::vector<int> vecPoint;
	std::vector<int> vecPoly;
};


void GetConnectedSets( CGeneral3DMesh& mesh,
					   std::vector<CConnectedSet>& vecConnected );

bool AreVerticesOnSphere( const CGeneral3DMesh& mesh, // [in]
						  const std::vector<int>& vertex_indices, // [in]
						  Sphere& sphere, // [out]
						  float radius_error_tolerance = 0.01f ); // [in]


#endif /* __MeshOperations_HPP__ */
