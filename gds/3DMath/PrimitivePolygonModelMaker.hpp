#ifndef __PrimitivePolygonModelMaker_HPP__
#define __PrimitivePolygonModelMaker_HPP__


#include <vector>
#include <gds/3DMath/Vector3.hpp>


class PrimitiveModelStyle
{
public:
	enum Name
	{
		EDGE_VERTICES_WELDED,
		EDGE_VERTICES_UNWELDED,
		NUM_STYLES
	};
};



void CreateCylinder( float height, const float *pafRadius, int num_segments,
                     PrimitiveModelStyle::Name style,
					 bool create_top_polygons,
					 bool create_bottom_polygons,
                     std::vector<Vector3>& vecDestPos,
                     std::vector<Vector3>& vecDestNormal,
					 std::vector< std::vector<int> >& vecDestPoly );


void CreateCylinder( const Vector3& vPos0, const Vector3& vPos1, const float *pafRadius, int num_segments,
                     PrimitiveModelStyle::Name style,
					 bool create_top_polygons,
					 bool create_bottom_polygons,
                     std::vector<Vector3>& vecDestPos,
                     std::vector<Vector3>& vecDestNormal,
					 std::vector< std::vector<int> >& vecDestPoly );





#endif /* __PrimitivePolygonModelMaker_HPP__ */