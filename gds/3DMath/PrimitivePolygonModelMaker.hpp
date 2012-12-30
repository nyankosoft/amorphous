#ifndef __PrimitivePolygonModelMaker_HPP__
#define __PrimitivePolygonModelMaker_HPP__


#include <vector>
#include "Vector3.hpp"
#include "PolygonModelStyle.hpp"


namespace amorphous
{


/// \param height                 [in] height of the cylinder
/// \param pafRadius              [in] 2 radii of the cylinder
/// \param num_segments           [in] The number of divisions around the cylinder. The higher value geenrates smoother & better looking cylinder. Settings this to 4 generates a box.
/// \param num_divisions          [in] The number of divisions on the side polygons along the axis of the cylinder. TODO: rename this to 'num_segments'
/// \param create_top_polygons    [in] Creates polygons for the top face if set to true.
/// \param top_style              [in] Whether the vertices around the rim of the top face should be merged or not. Ignored when create_top_polygons is set to false.
/// \param create_bottom_polygons [in] Creates polygons for the bottom face if set to true.
/// \param bottom_style           [in] Whether the vertices around the rim of the bottom face should be merged or not. Ignored when create_bottom_polygons is set to false.
/// \param vecDestPos    [out] points
/// \param vecDestNormal [out] normals
/// \param vecDestPoly   [out] polygons
void CreateCylinder( float height, const float *pafRadius,
                     int num_segments, int num_divisions,
					 bool create_top_polygons,
                     PolygonModelStyle::Name top_style,
					 bool create_bottom_polygons,
                     PolygonModelStyle::Name bottom_style,
                     std::vector<Vector3>& vecDestPos,
                     std::vector<Vector3>& vecDestNormal,
					 std::vector< std::vector<int> >& vecDestPoly );


void CreateCylinder( const Vector3& vPos0, const Vector3& vPos1, const float *pafRadius,
                     int num_segments, int num_divisions,
					 bool create_top_polygons,
                     PolygonModelStyle::Name top_style,
					 bool create_bottom_polygons,
                     PolygonModelStyle::Name bottom_style,
                     std::vector<Vector3>& vecDestPos,
                     std::vector<Vector3>& vecDestNormal,
					 std::vector< std::vector<int> >& vecDestPoly );


void CreatePipe( float length, const float outer_radius, const float inner_radius,
                 int num_segments, int num_divisions,
                 bool create_top_polygons,
                 PolygonModelStyle::Name top_style,
                 bool create_bottom_polygons,
                 PolygonModelStyle::Name bottom_style,
                 std::vector<Vector3>& vecDestPos,
                 std::vector<Vector3>& vecDestNormal,
                 std::vector< std::vector<int> >& vecDestPoly );


} // namespace amorphous



#endif /* __PrimitivePolygonModelMaker_HPP__ */
