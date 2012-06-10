#include "ConvexMeshSplitter.hpp"
#include "CustomMesh.hpp"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

using std::vector;
using std::map;
using std::pair;

typedef std::pair<U16,U16> u16_pair;


class are_points_close
{
	const Vector3& m_lhs;
public:

	are_points_close( const Vector3& lhs ) : m_lhs(lhs) {}

	bool operator()( const Vector3& rhs ) const
	{
		return Vec3LengthSq( m_lhs - rhs ) < (0.001f * 0.001f);
	}
};


bool DoesEdgeOverlapsWithTriangle( const std::vector<Vector3>& points, const Vector3& normal, const pair<U16,U16>& edge_to_test, const fixed_vector<U16,3>& triangle_to_test )
{
	Plane edge_plane;
	edge_plane.normal = Vec3Cross( normal, points[edge_to_test.first] - points[edge_to_test.second] );
	edge_plane.normal.Normalize();
	edge_plane.dist = Vec3Dot( edge_plane.normal, points[edge_to_test.first] );

	float d0 = edge_plane.GetDistanceFromPoint( points[ triangle_to_test[0] ] );
	float d1 = edge_plane.GetDistanceFromPoint( points[ triangle_to_test[1] ] );
	float d2 = edge_plane.GetDistanceFromPoint( points[ triangle_to_test[2] ] );

	if( d0 < -0.001f && 0.001f < d1
	 || d1 < -0.001f && 0.001f < d0
	 || d1 < -0.001f && 0.001f < d2
	 || d2 < -0.001f && 0.001f < d1
	 || d2 < -0.001f && 0.001f < d0
	 || d0 < -0.001f && 0.001f < d2 )
	{
		return true;
	}
	else
		return false;
}

bool DoesEdgeOverlapAnyTriangle( const std::vector<Vector3>& points, const Vector3& normal, const std::pair<U16,U16>& edge_to_test, std::vector< fixed_vector<U16,3> >& triangles )
{
	typedef std::pair<U16,U16> edge;

	for( int i=0; i<triangles.size(); i++ )
	{
		bool found_overlap = DoesEdgeOverlapsWithTriangle( points, normal, edge_to_test, triangles[i] );
		if( found_overlap )
			return true;
	}

	return false;
}


bool get_unconnected_point_index( std::vector<char>& processed, U16& dest )
{
	for( size_t i=0; i<processed.size(); i++ )
	{
		if( processed[i] == 0 )
		{
			processed[i] = 1;
			dest = (U16)i;
			return true;
		}
	}

	return false;
}


//void FindAdjacentNonOverlappingTriangle( std::vector< boost::tuple<U16,U16,U16> >& triangles )
void FindAdjacentNonOverlappingTriangle(
	std::vector< fixed_vector<U16,3> >& triangles,
	const std::vector<Vector3>& points,
	const Vector3& normal,
	std::vector<char>& processed
	)
{
	if( triangles.empty() )
		return; // Needs at least one triangle

	typedef std::pair<U16,U16> edge;
	typedef fixed_vector<U16,3> triangle;
//	typedef boost::tuple<U16,U16,U16> triangle;

	const triangle base_triangle = triangles.back();

	for( int i=0; i<3; i++ )
	{
		U16 unconnected_point_index = 0;
		bool found = get_unconnected_point_index( processed, unconnected_point_index );
		if( !found )
			return;

		bool edge_overlaps_with_triangle = false;

		// Check whether the 2 edges not part of the focused triangle cross any triangle created so far.
		for( int j=0; j<2; j++ )
		{
			U16 focused_triangle_point_index = base_triangle[(i+j)%3];
			edge edge_to_test( focused_triangle_point_index, unconnected_point_index );
//			boost::geometry::segment_identifier

			bool found_overlap = DoesEdgeOverlapAnyTriangle( points, normal, edge_to_test, triangles );
			if( found_overlap )
			{
				edge_overlaps_with_triangle = true;
				break;
			}
		}

		if( edge_overlaps_with_triangle == false )
		{
			triangle new_triangle;
			new_triangle.resize( 3 );
			new_triangle[0] = triangles.back()[i];
			new_triangle[0] = triangles.back()[(i+1)%3];
			triangles.push_back( new_triangle );
		}

		triangles.back();
	}
}


bool Get3PointsToCreateTriangle( const std::vector<Vector3>& points, U16& i0, U16& i1, U16& i2 )
{
	for( size_t i=0; i<points.size(); i++ )
	{
		for( size_t j=i+1; j<points.size(); j++ )
		{
			for( size_t k=j+1; k<points.size(); k++ )
			{
				const Vector3 cross = Vec3Cross( points[j] - points[i], points[k] - points[i] );
				if( 0.000001f < cross.GetLengthSq() )
				{
					i0 = i;
					i1 = j;
					i2 = k;
					return true;
				}
			}
		}
	}

	return true;
}


/// \param points [in] 
/// \param triangles [out] 
//void CreateTrianglesOnSplitSurface( const std::vector<Vector3>& points, std::vector< boost::tuple<U16,U16,U16> >& triangles )
void CreateTrianglesOnSplitSurface( const std::vector<Vector3>& points, const Vector3& normal, std::vector< fixed_vector<U16,3> >& triangles )
{
	typedef std::pair<U16,U16> edge;
//	typedef boost::tuple<U16,U16,U16> triangle;
	typedef fixed_vector<U16,3> triangle;

	vector<char> connected;
	connected.resize( points.size(), 0 );

	if( points.size() < 3 )
		return;

	U16 i0=0, i1=0, i2=0;
	bool found = Get3PointsToCreateTriangle( points, i0, i1, i2 );
	if( !found )
		return;

	triangle first_triangle;
	first_triangle.resize( 3 );
	first_triangle[0] = i0;
	first_triangle[1] = i1;
	first_triangle[2] = i2;
	triangles.push_back( first_triangle );
	connected[i0] = 1;
	connected[i1] = 1;
	connected[i2] = 1;

	// For each edge, find an adjacent triangle which does not overlap
	// with any of the triangles created so far.
	FindAdjacentNonOverlappingTriangle( triangles, points, normal, connected );
}


void CreatePolygonsOnSplitSurface()
{
}


/*
When an edge is split by the split plane, 4 vertices are added.
1. Vertex for the split polygon in front of the split plane
2. Vertex for the split polygon behind the split plane
3. Vertex for the polygon created on the split plane and added to the mesh in front of the split plane
4. Vertex for the polygon created on the split plane and added to the mesh behind the split plane

Need to sort the vertices created as 3 and 4 to connect them in the right orders?
*/



/// Note that return value does not represent an edge, but it represents 2 indices.
std::pair<U16,U16> AddSplitVertices(
	const CCustomMesh& src,
	float f,
	U16 vertex_index0,
	U16 vertex_index1,
	const Plane& split_plane,
	CCustomMesh& dest_front,
	CCustomMesh& dest_back,
	std::vector<U16>& split_surface_vertex_indices_front,
	std::vector<U16>& split_surface_vertex_indices_back
	)
{
	bool add_vertices_for_each_split_edges = false;

	U16 new_index0 = dest_front.GetNumVertices();
	U16 new_index1 = dest_back.GetNumVertices();

	if( add_vertices_for_each_split_edges )
	{
		split_surface_vertex_indices_front.push_back( new_index0 + 1 );
		split_surface_vertex_indices_back.push_back( new_index1 + 1 );
	}

	int num_vertices_to_add = add_vertices_for_each_split_edges ? 2 : 1;
	dest_front.AddVertices( num_vertices_to_add );
	dest_back.AddVertices(  num_vertices_to_add );

	Vector3 p0 = src.GetPosition( vertex_index0 );
	Vector3 p1 = src.GetPosition( vertex_index1 );
	Vector3 p = p0 * (1.0f - f) + p1 * f;
	dest_front.SetPosition( new_index0, p );
	dest_back.SetPosition(  new_index1, p );
	if( add_vertices_for_each_split_edges )
	{
		dest_front.SetPosition( new_index0+1, p );
		dest_back.SetPosition(  new_index1+1, p );
	}

	if( src.GetVertexFormatFlags() & VFF::NORMAL )
	{
		Vector3 n0 = src.GetNormal( vertex_index0 );
		Vector3 n1 = src.GetNormal( vertex_index1 );
		Vector3 n = n0 * (1.0f - f) + n1 * f;
		dest_front.SetNormal( new_index0, n );
		dest_back.SetNormal(  new_index1, n );
		if( add_vertices_for_each_split_edges )
		{
			dest_front.SetNormal( new_index0+1, -split_plane.normal );
			dest_back.SetNormal(  new_index1+1,  split_plane.normal );
		}
	}

	if( src.GetVertexFormatFlags() & VFF::BINORMAL )
	{
	}

	if( src.GetVertexFormatFlags() & VFF::TANGENT )
	{
	}

	if( src.GetVertexFormatFlags() & VFF::DIFFUSE_COLOR )
	{
		SFloatRGBAColor c0 = src.GetDiffuseColor( vertex_index0 );
		SFloatRGBAColor c1 = src.GetDiffuseColor( vertex_index1 );
		SFloatRGBAColor c = c0 * (1.0f - f) + c1 * f;
		dest_front.SetDiffuseColor( new_index0, c );
		dest_back.SetDiffuseColor(  new_index1, c );
		if( add_vertices_for_each_split_edges )
		{
			dest_front.SetDiffuseColor( new_index0+1, c );
			dest_back.SetDiffuseColor(  new_index1+1, c );
		}
	}

	U32 tex_coord_flags[] =
	{
		VFF::TEXCOORD2_0,
		VFF::TEXCOORD2_1,
		VFF::TEXCOORD2_2,
		VFF::TEXCOORD2_3
	};

	for( int i=0; i<numof(tex_coord_flags); i++ )
	{
		if( src.GetVertexFormatFlags() & tex_coord_flags[i] )
		{
			TEXCOORD2 t0 = src.Get2DTexCoord( vertex_index0, i );
			TEXCOORD2 t1 = src.Get2DTexCoord( vertex_index1, i );
			TEXCOORD2 t = t0 * (1.0f - f) + t1 * f;
			dest_front.Set2DTexCoord( new_index0, t, i );
			dest_back.Set2DTexCoord(  new_index1, t, i );
			if( add_vertices_for_each_split_edges )
			{
				dest_front.Set2DTexCoord( new_index0+1, t, i );
				dest_back.Set2DTexCoord(  new_index1+1, t, i );
			}
		}
	}

	return pair<U16,U16>( new_index0, new_index1 );
}


void TriangulateIndices( std::vector<U16>& indices )
{
	vector<U16> dest;
	for( size_t i=0; i<indices.size()-2; i++ )
	{
		dest.push_back( 0 );
		dest.push_back( (U16)i+1 );
		dest.push_back( (U16)i+2 );
	}

	indices = dest;
}


/// Called when a triangle is crossing the split planne and needs to be split.
void CConvexMeshSplitter::SplitTriangle(
	const CCustomMesh& src,
	const vector<char>& vertex_sides,
	const vector< std::pair<uint,uint> >& new_vertex_indices,
	int triangle_index,
	const Plane& split_plane,
	CCustomMesh& dest_front,
	CCustomMesh& dest_back,
	vector<U16>& front_polygon_indices,
	vector<U16>& back_polygon_indices,
	vector<U16>& split_surface_vertex_indices_front,
	vector<U16>& split_surface_vertex_indices_back
	)
{
	// These should be member variables because they are accessed from multiple functions?
	vector<U16> front_split_indices, back_split_indices;

//	map< u16_pair, CEdgeSplitInfo >& edge_split_info = m_EdgeToEdgeSplitInfo;

	using std::map;
	using std::pair;

	vector<Vector3>& split_surface_points = m_SplitSurfacePoints;

	for( int i=0; i<3; i++ )
	{
		int i0 = triangle_index * 3 + i;
		int i1 = triangle_index * 3 + (i+1) % 3;
		const U16 vertex_index0 = src.GetIndex( i0 );
		const U16 vertex_index1 = src.GetIndex( i1 );

		char vs0 = vertex_sides[ src.GetIndex( i0 ) ];
		char vs1 = vertex_sides[ src.GetIndex( i1 ) ];

//		if( vs0 * vs1 != -1 )
//			continue; // The edge is not crossing the split plane

		if( vs0 == 1 && vs1 == 1 )
		{
			int new_vertex_index = new_vertex_indices[vertex_index0].first;
			if( new_vertex_index == -1 )
				return;

			front_polygon_indices.push_back( (U16)new_vertex_index );
		}
		else if( vs0 == -1 && vs1 == -1 )
		{
			int new_vertex_index = new_vertex_indices[vertex_index0].second;
			if( new_vertex_index == -1 )
				return;

			back_polygon_indices.push_back( (U16)new_vertex_index );
		}
		else
		{
			// vs0 * vs1 == -1, i.e. the edge is crossing the split plane

			std::pair<U16,U16> front_and_back_indices;

			u16_pair edge( u16_pair(vertex_index0,vertex_index1) );
			map< u16_pair, CEdgeSplitInfo >::iterator itr
				= m_EdgeToEdgeSplitInfo.find( edge );

			if( itr != m_EdgeToEdgeSplitInfo.end() )
			{
				// The edge has already been split.
//				continue;
				front_and_back_indices.first  = itr->second.front_vertex_index;
				front_and_back_indices.second = itr->second.back_vertex_index;
			}
			else
			{
				// This edge crossing the split plane and it has not been split yet.
//				m_EdgeToEdgeSplitInfo[edge] = CEdgeSplitInfo();

				// Calculate the split point on the edge
				Vector3 p0 = src.GetPosition( src.GetIndex( i0 ) );
				Vector3 p1 = src.GetPosition( src.GetIndex( i1 ) );
				float d0 = split_plane.GetDistanceFromPoint( p0 );
				float d1 = split_plane.GetDistanceFromPoint( p1 );
//				const Vector3 p = p0 * d1 + p1 * d0 / (d0 - d1);
				const float f = d0 / (d0 - d1);
				const Vector3 p = p0 * (1.0f - f) + p1 * f;

				bool create_single_convex_polygon_on_split_surface = false;
				if( create_single_convex_polygon_on_split_surface )
				{
					std::vector<Vector3>::iterator itr
						= std::find_if( split_surface_points.begin(), split_surface_points.end(), are_points_close(p) );

					if( itr == split_surface_points.end() )
						split_surface_points.push_back( p );
				}

//				edge_split_info.find( u16_pair(vertex_index0,vertex_index1) );

				front_and_back_indices
					= AddSplitVertices( src, f, vertex_index0, vertex_index1, split_plane, dest_front, dest_back, split_surface_vertex_indices_front, split_surface_vertex_indices_back );

//				results.front_points.push_back( p );
//				results.back_points.push_back( p );
				if( vs0 == 1 )
				{
//					front_split_indices.push_back( new_front_index++ );
//					front_split_indices.push_back( new_index_0 );
//					back_split_indices.push_back( new_index_1 );

					int new_vertex_index = new_vertex_indices[vertex_index0].first;
					if( new_vertex_index == -1 )
						return;

					front_polygon_indices.push_back( (U16)new_vertex_index );
					front_polygon_indices.push_back( front_and_back_indices.first );
					back_polygon_indices.push_back( front_and_back_indices.second );
//					back_polygon_indices.push_back( vertex_index1 );
				}
				else
				{
//					back_split_indices.push_back( new_front_index++ );
//					back_split_indices.push_back( new_index_1 );
//					front_split_indices.push_back( new_index_0 );

					int new_vertex_index = new_vertex_indices[vertex_index0].second;
					if( new_vertex_index == -1 )
						return;

//					front_polygon_indices.push_back( vertex_index1 );
					front_polygon_indices.push_back( front_and_back_indices.first );
					back_polygon_indices.push_back( (U16)new_vertex_index );
					back_polygon_indices.push_back( front_and_back_indices.second );
				}
			}
		}
	}

//	if( 3 < front_split_indices.size() )
//		TriangulateIndices( front_split_indices );

//	if( 3 < back_split_indices.size() )
//		TriangulateIndices( back_split_indices );
}


void CalculateVertexSides(
	const CCustomMesh& src,
	const Plane& split_plane,
	vector<char>& vertex_sides,
	int& num_front_vertices,
	int& num_back_vertices,
	int& num_on_plane_vertices
)
{
	static const int not_sorted = 0x0F;

	const int num_vertices = src.GetNumVertices();
	vertex_sides.resize( num_vertices, not_sorted );

	for( int i=0; i<num_vertices; i++ )
	{
		const Vector3 pos = src.GetPosition(i);
		float d = split_plane.GetDistanceFromPoint( pos );
		if( 0.001f < d )
		{
			vertex_sides[i] = 1; // front
			num_front_vertices++;
		}
		else if( d < -0.001f )
		{
			vertex_sides[i] = -1; // back
			num_back_vertices++;
		}
		else
		{
			vertex_sides[i] = 0; // on plane
			num_on_plane_vertices++;
		}
	}
}


Result::Name CConvexMeshSplitter::SplitMeshByPlane( const CCustomMesh& src, const Plane& split_plane, CCustomMesh& dest_front, CCustomMesh& dest_back )
{
	const int num_vertices = src.GetNumVertices();
	vector<char> vertex_sides;

	vector< std::pair<uint,uint> > new_vertex_indices;
	new_vertex_indices.resize( num_vertices, std::pair<int,int>(-1,-1) );

	// new_vertex_indices[?].second is rarely used.

	int num_front_vertices    = 0;
	int num_back_vertices     = 0;
	int num_on_plane_vertices = 0;

	CalculateVertexSides( src, split_plane, vertex_sides, num_front_vertices, num_back_vertices, num_on_plane_vertices );

	if( num_front_vertices == 0
	 && num_back_vertices  == 0 )
	{
		// The whole mesh is on the plane
//		return 0;
		Result::UNKNOWN_ERROR;
	}
	else if( num_front_vertices == 0 )
	{
//		return -1;
		Result::UNKNOWN_ERROR;
	}
	else if( num_back_vertices == 0 )
	{
//		return 1;
		Result::UNKNOWN_ERROR;
	}

	// The mesh 'src' is crossing 'split_plane'

//	dest_front.SetNumVertices( num_front_vertices + num_on_plane_vertices );
//	dest_back.SetNumVertices(  num_back_vertices  + num_on_plane_vertices );

	// Init the vertex buffers of the dest meshes.
	// The size of the vertex buffer allocated here does not consider the vertices added
	// as a result of splitting. The rooms for split vertices are created later.
	const U32 vf_flags = src.GetVertexFormatFlags();
	dest_front.InitVertexBuffer( num_front_vertices + num_on_plane_vertices, vf_flags );
	dest_back.InitVertexBuffer(  num_back_vertices  + num_on_plane_vertices, vf_flags );

	// Copy materials
	dest_front.Materials() = src.GetMaterials();
	dest_back.Materials()  = src.GetMaterials();

	uint dest_front_vertex_index = 0;
	uint dest_back_vertex_index = 0;

	// Copy vertices to dest_front & dest_back
	for( int i=0; i<num_vertices; i++ )
	{
		if( vertex_sides[i] == 1 )
		{
			new_vertex_indices[i].first  = dest_front_vertex_index;
			src.CopyVertexTo( i, dest_front, dest_front_vertex_index++ );
//			new_vertex_indices[i].first
//			dest_front.AddVertex(  );
		}
		else if( vertex_sides[i] == -1 )
		{
			new_vertex_indices[i].second = dest_back_vertex_index;
			src.CopyVertexTo( i, dest_back,  dest_back_vertex_index++ );
//			back_front.AddVertex(  );
		}
		else
		{
			// The vertex is on the split plane.
			// Copy the vertex to both of the meshes
			new_vertex_indices[i].first  = dest_front_vertex_index;
			new_vertex_indices[i].second = dest_back_vertex_index;
			src.CopyVertexTo( i, dest_front, dest_front_vertex_index++ );
			src.CopyVertexTo( i, dest_back,  dest_back_vertex_index++ );
		}
	}

	vector<U16> front_mesh_indices, back_mesh_indices;
	front_mesh_indices.reserve( num_front_vertices );
	back_mesh_indices.reserve( num_back_vertices );

	vector<U16> front_polygon_vertex_indices;
	vector<U16> back_polygon_vertex_indices;

	vector<U16> split_surface_vertex_indices_front;
	vector<U16> split_surface_vertex_indices_back;

	const int num_src_triangles = src.GetNumIndices() / 3;
	for( int i=0; i<num_src_triangles; i++ )
	{
		const int index0 = src.GetIndex( i*3   );
		const int index1 = src.GetIndex( i*3+1 );
		const int index2 = src.GetIndex( i*3+2 );
		const int v0_stat = vertex_sides[index0];
		const int v1_stat = vertex_sides[index1];
		const int v2_stat = vertex_sides[index2];

		if( v0_stat == 1
		 && v1_stat == 1
		 && v2_stat == 1 )
		{
			// the triangle is in front of the split plane
			// dest_front takes the 3 vertices
			front_mesh_indices.push_back( new_vertex_indices[index0].first );
			front_mesh_indices.push_back( new_vertex_indices[index1].first );
			front_mesh_indices.push_back( new_vertex_indices[index2].first );
		}
		else if( v0_stat == -1
		      && v1_stat == -1
		      && v2_stat == -1 )
		{
			// the triangle is behind the split plane
			// dest_back takes the 3 vertices
			back_mesh_indices.push_back( new_vertex_indices[index0].second );
			back_mesh_indices.push_back( new_vertex_indices[index1].second );
			back_mesh_indices.push_back( new_vertex_indices[index2].second );
		}
		else if( v0_stat * v1_stat == -1
		      || v1_stat * v2_stat == -1
		      || v2_stat * v0_stat == -1 )
		{
			// There is at least one pair of triangle vertices which are on different sides on the plane
			// -> The triangle is crossing the split plane
			// -> Split the triangle
			front_polygon_vertex_indices.resize( 0 );
			back_polygon_vertex_indices.resize( 0 );
			SplitTriangle(
				src,
				vertex_sides,
				new_vertex_indices,
				i,
				split_plane,
				dest_front,
				dest_back,
				front_polygon_vertex_indices,
				back_polygon_vertex_indices,
				split_surface_vertex_indices_front,
				split_surface_vertex_indices_back
				);

			// Triangulate the front and back polygons
			for( int k=0; k<(int)front_polygon_vertex_indices.size() - 2; k++ )
			{
				front_mesh_indices.push_back( front_polygon_vertex_indices[0] );
				front_mesh_indices.push_back( front_polygon_vertex_indices[k+1] );
				front_mesh_indices.push_back( front_polygon_vertex_indices[k+2] );
			}

			for( int k=0; k<(int)back_polygon_vertex_indices.size() - 2; k++ )
			{
				back_mesh_indices.push_back( back_polygon_vertex_indices[0] );
				back_mesh_indices.push_back( back_polygon_vertex_indices[k+1] );
				back_mesh_indices.push_back( back_polygon_vertex_indices[k+2] );
			}
		}
	}

	bool add_vertices_for_each_split_edge = false;
	if( add_vertices_for_each_split_edge )//options.close_split_surfaces )
	{
		CreatePolygonsOnSplitSurface();

		typedef fixed_vector<U16,3> triangle;

		vector<Vector3> front_points;
		front_points.resize( split_surface_vertex_indices_front.size() );
		for( size_t i=0; i<split_surface_vertex_indices_front.size(); i++ )
			front_points[i] = dest_front.GetPosition( split_surface_vertex_indices_front[i] );

		vector<Vector3> back_points;
		back_points.resize( split_surface_vertex_indices_back.size() );
		for( size_t i=0; i<split_surface_vertex_indices_back.size(); i++ )
			back_points[i] = dest_back.GetPosition( split_surface_vertex_indices_back[i] );

		vector<triangle> front_triangles, back_triangles;
		CreateTrianglesOnSplitSurface( front_points, split_plane.normal, front_triangles );
		CreateTrianglesOnSplitSurface( back_points,  split_plane.normal, back_triangles );

		for( size_t i=0; i<front_triangles.size(); i++ )
		{
			front_mesh_indices.push_back( front_triangles[i][0] );
			front_mesh_indices.push_back( front_triangles[i][1] );
			front_mesh_indices.push_back( front_triangles[i][2] );
		}

		for( size_t i=0; i<back_triangles.size(); i++ )
		{
			back_mesh_indices.push_back( back_triangles[i][0] );
			back_mesh_indices.push_back( back_triangles[i][1] );
			back_mesh_indices.push_back( back_triangles[i][2] );
		}
	}

	bool create_single_convex_polygon_on_split_surface = false;
	if( create_single_convex_polygon_on_split_surface )
	{
		typedef fixed_vector<U16,3> triangle;

		const int num_points = m_SplitSurfacePoints.size();

		vector<triangle> triangles;
		CreateTrianglesOnSplitSurface( m_SplitSurfacePoints, split_plane.normal, triangles );
		const U16 front_mesh_vertex_index_offset = (U16)dest_front.GetNumVertices();
		for( size_t i=0; i<triangles.size(); i++ )
		{
			// Add triangle indices
//			front_mesh_indices.push_back( triangles[i][0] + front_mesh_vertex_index_offset );
//			front_mesh_indices.push_back( triangles[i][1] + front_mesh_vertex_index_offset );
//			front_mesh_indices.push_back( triangles[i][2] + front_mesh_vertex_index_offset );
		}

		for( int i=0; i<num_points; i++ )
		{
			// Add vertices
		}
	}

	dest_front.SetIndices( front_mesh_indices );
	dest_back.SetIndices(  back_mesh_indices );

	LOG_PRINTF_VERBOSE(( "front: %u vertices / %u triangles", dest_front.GetNumVertices(), dest_front.GetNumIndices() / 3 ));
	LOG_PRINTF_VERBOSE(( "back:  %u vertices / %u triangles", dest_back.GetNumVertices(),  dest_back.GetNumIndices() / 3 ));

	return Result::SUCCESS;
}


Result::Name CConvexMeshSplitter::SplitMesh( const CCustomMesh& src, const Plane& split_plane )
{
	m_EdgeToEdgeSplitInfo.clear();
	m_SplitSurfacePoints.clear();

	m_MeshSplitResults.m_pBackMesh.reset(  new CCustomMesh );
	m_MeshSplitResults.m_pFrontMesh.reset( new CCustomMesh );

	return SplitMeshByPlane(
		src,
		split_plane,
		*(m_MeshSplitResults.m_pFrontMesh),
		*(m_MeshSplitResults.m_pBackMesh)
		);
}
