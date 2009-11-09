#ifndef __INDEXEDPOLYGON_H__
#define __INDEXEDPOLYGON_H__

#include <vector>
#include <boost/shared_ptr.hpp>

#include "3DMath/ray.hpp"
#include "3DMath/AABB3.hpp"
#include "3DMath/Plane.hpp"
#include "3DMath/Triangle.hpp"
#include "Graphics/General3DVertex.hpp"

#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"
using namespace GameLib1::Serialization;


class CIndexedPolygon : public IArchiveObjectBase
{
	/// Pointer to the vertex buffer (shared pointer).
	/// - Usually shared by polygons that belongs to the same polygon buffer
	/// - When GetVertex() is called, the object will internally
	///   access this buffer and return the reference to the vertex
	boost::shared_ptr<std::vector<CGeneral3DVertex>> m_pVertexBuffer;

	SPlane m_Plane;

	/// axis-aligned bounding box that contains this polygon
	AABB3 m_AABB;

public:

	enum eOnPlaneCondition { OPC_DONTCARE_NORMAL_DIRECTION, OPC_IF_NORMAL_SAME_DIRECTION };

	/// indices to polygon vertices in the vertex buffer
	std::vector<int> m_index;

	/// surface property of the polygon
	int m_MaterialIndex;

public:

	/// Default constructor
	/// - Remember that the vertex buffer must be manually restored
	inline CIndexedPolygon()
		:
	m_MaterialIndex(0)
	{}

	inline CIndexedPolygon( boost::shared_ptr< std::vector<CGeneral3DVertex> > pVertexBuffer )
		:
	m_pVertexBuffer(pVertexBuffer),
	m_MaterialIndex(0)
	{}

	/// creates an indexed triangle
	inline CIndexedPolygon( boost::shared_ptr< std::vector<CGeneral3DVertex> > pVertexBuffer, int i0, int i1, int i2,
		                    int mat_index = 0, const SPlane& plane = SPlane(), const AABB3& aabb = AABB3() );

	/// polygon must be convex
	inline void Split( CIndexedPolygon& front, CIndexedPolygon& back, const SPlane& plane ) const;

	inline void Triangulate( std::vector<CIndexedPolygon>& dest_polygon_buffer ) const;

	const CGeneral3DVertex& GetVertex( int vert_index ) const { return (*m_pVertexBuffer.get())[m_index[vert_index]]; }

	const CGeneral3DVertex& GetVertex( size_t vert_index ) const { return GetVertex( (int)vert_index ); }

	CGeneral3DVertex& Vertex( int vert_index ) { return (*m_pVertexBuffer.get())[m_index[vert_index]]; }

	int GetNumVertices() const { return (int)m_index.size(); }

	const SPlane& GetPlane() const { return m_Plane; }

	const AABB3& GetAABB() const { return m_AABB; }

	inline void UpdateAABB();

	inline bool UpdatePlane();

	inline float CalculateArea() const;

	bool IsOnTriangle( int iTriangleIndex, const Vector3& rvPosition ) const;

	/// Returns true if the given point is on the polygon
	inline bool IsOnPolygon( const Vector3& rvPosition ) const;

	Vector3 GetInterpolatedNormal( const Vector3& rvPosition ) const;

	inline bool SharesPointWith( const CIndexedPolygon& polygon );

	/// Returns true if the line segment hits the polygon
	inline bool ClipLineSegment( const CLineSegment& line_segment, CLineSegmentHit& results ) const;

	/// Added to use CIndexedPolygon with CAABTree
	/// - Not added for actual use
	/// - m_pVertexBuffer would have to be separately serialized and restored
	inline void Serialize( IArchive& ar, const unsigned int version );

	void SetVertexBuffer( boost::shared_ptr< std::vector<CGeneral3DVertex> > pVertexBuffer ) { m_pVertexBuffer = pVertexBuffer; }

	boost::shared_ptr<std::vector<CGeneral3DVertex>> VertexBuffer() { return m_pVertexBuffer; }


//	static std::vector<CGeneral3DVertex>& VertexBuffer() { return (*m_pVertexBuffer); }
};


//============================== inline implementations ==============================

#define NORMAL_EPSILON	0.001


enum ePolygonStatus { POLYGON_ONPLANE, POLYGON_FRONT, POLYGON_BACK, POLYGON_INTERSECTING };


inline CIndexedPolygon::CIndexedPolygon( boost::shared_ptr< std::vector<CGeneral3DVertex> > pVertexBuffer, int i0, int i1, int i2,
										 int mat_index, const SPlane& plane, const AABB3& aabb )
:
m_pVertexBuffer(pVertexBuffer),
m_MaterialIndex(mat_index),
m_Plane(plane),
m_AABB(aabb)
{
	m_index.resize(3);
	m_index[0] = i0;
	m_index[1] = i1;
	m_index[2] = i2;
}


inline void CIndexedPolygon::Split( CIndexedPolygon& front, CIndexedPolygon& back, const SPlane& plane ) const
{
	size_t i, num_orig_verts = m_index.size();  //the number of points of this face
//	SFloatRGBColor col, col0, col1;

	std::vector<CGeneral3DVertex>& vert_buffer = *m_pVertexBuffer.get();

	// copy the properties of the source polygon. e.g.) material index
	front = *this;
	back = *this;

	// clear vertex indices
	front.m_index.resize(0);
	back.m_index.resize(0);

	//Initialization for 'front' and 'back': delete the vertices and copy the plane from the original face

	//check if each edge of this face and the 'cutplane' intersect
	for( i=0; i<num_orig_verts; i++)
	{
		const CGeneral3DVertex& mv0 = vert_buffer[m_index[i]];
		const CGeneral3DVertex& mv1 = vert_buffer[m_index[(i+1) % num_orig_verts]];
		const Vector3& p0 = mv0.m_vPosition;      /// pick up 2 vertices on the polygon face
		const Vector3& p1 = mv1.m_vPosition;      /// see if they are in front of, behind or crossing the plane 
		int c0 = ClassifyPoint( plane, p0 );
		int c1 = ClassifyPoint( plane, p1 );

		if( ( c0 == PNT_FRONT && c1 == PNT_BACK )
		||  ( c0 == PNT_BACK && c1 == PNT_FRONT ) )
		{
			// need to split the edge
			float d0 = plane.GetDistanceFromPoint( p0 );
			float d1 = plane.GetDistanceFromPoint( p1 );
			float f = - d0 / ( d1 - d0 );
//			Vector3 pn = p0 + ( p1 - p0 ) * f;  //the cross point between linesegment (p1 - p0) and cutplane

			// calculate properties for the new vertex at the cross point.
			// vertex color, tex coord, etc.
			CGeneral3DVertex new_vert;

			new_vert = mv0 + ( mv1 - mv0 ) * f;

			Vec3Normalize( new_vert.m_vNormal, new_vert.m_vNormal );

//			new_vert.OrthonormalizeLocalSpace();

			// push the new vertex to the buffer
			// note that 2 vertices are added for each split position
			int new_vert_index = (int)vert_buffer.size();
			vert_buffer.push_back( new_vert );
			vert_buffer.push_back( new_vert );

//			vert0_index = 
			if( c0 == PNT_FRONT )
				front.m_index.push_back( m_index[i] );  // add P0 vertex to CFace front
//				front.m_pVertices.push_back( mv0 );  // add P0 vertex to CFace front
			if( c0 == PNT_BACK )
				back.m_index.push_back( m_index[i] );
//				back.m_pVertices.push_back( mv0 );
			front.m_index.push_back( new_vert_index );
			back.m_index.push_back( new_vert_index + 1 );
//			front.m_pVertices.push_back(new_vert);
//			back.m_pVertices.push_back(new_vert);
			continue;
		}
		if( c0 == PNT_FRONT ) front.m_index.push_back( m_index[i] );	// front -> on
		if( c0 == PNT_BACK  ) back.m_index.push_back( m_index[i] );		// back -> on
		if( c0 == PNT_ONPLANE )
		{
			front.m_index.push_back( m_index[i] );
			back.m_index.push_back( m_index[i] );
		}
	}
}


inline void CIndexedPolygon::Triangulate( std::vector<CIndexedPolygon>& dest_polygon_buffer ) const
{
	if( m_index.size() <= 2 )
		return;

	if( m_index.size() <= 3 )
	{
		dest_polygon_buffer.push_back( *this );
		return;
	}

	size_t i, num_tris = m_index.size() - 2;
	for( i=0; i<num_tris; i++ )
	{
		dest_polygon_buffer.push_back(
			CIndexedPolygon( m_pVertexBuffer, m_index[0], m_index[i+1], m_index[i+2], m_MaterialIndex, m_Plane, m_AABB ) );
	}

	/// experimental
	/// - how to triangulate a concave polygon?
/*
	// copy of vertex indices
	vector<int> index_copy = m_index;
	while( 3 < index_copy.size() )
	{
		// find a corner that has the minimum angle
		// - index[1] is the focused vertex index
		float angle = 0, min_angle = 2.0f * (float)PI;
		size_t min_angle_vert_index[3] = { 0, 1, 2 };
		size_t i, num_vertices = index_copy.size();
		size_t index[3];
		for( i=0; i<num_vertices; i++ )
		{
			index[0] = i;
			index[1] = (i+1) % num_vertices;
			index[2] = (i+2) % num_vertices;
			const Vector3& pos0 = ((*m_pVertexBuffer)[index_copy[index[0]]]).m_vPosition;
			const Vector3& pos1 = ((*m_pVertexBuffer)[index_copy[index[1]]]).m_vPosition;
			const Vector3& pos2 = ((*m_pVertexBuffer)[index_copy[index[2]]]).m_vPosition;

			if( 0.0f < Vec3Dot( Vec3Cross( pos0 - pos1, pos2 - pos1 ), m_Plane.normal ) )
				continue; // The angle is greater than ( 2 * PI ). Skip this corner.

			angle = Vec3GetAngleBetween( pos0 - pos1, pos2 - pos1 );

			if( angle == 0.0f )
				int BreakHere = 1;

			if( angle < min_angle )
			{
				min_angle = angle;
				for( int j=0; j<3; j++ )
					min_angle_vert_index[j] = index[j];
			}
		}

		dest_polygon_buffer.push_back( *this );

		dest_polygon_buffer.back().m_index.resize( 3 );
		dest_polygon_buffer.back().m_index[0] = index_copy[min_angle_vert_index[0]];
		dest_polygon_buffer.back().m_index[1] = index_copy[min_angle_vert_index[1]];
		dest_polygon_buffer.back().m_index[2] = index_copy[min_angle_vert_index[2]];

		index_copy.erase( index_copy.begin() + min_angle_vert_index[1] );
	}

	dest_polygon_buffer.push_back( *this );
	dest_polygon_buffer.back().m_index = index_copy;*/
}


inline void CIndexedPolygon::UpdateAABB()
{
	m_AABB.Nullify();

	const size_t num_vertices = m_index.size();
	for( size_t i=0; i<num_vertices; i++ )
	{
		m_AABB.AddPoint( GetVertex(i).m_vPosition );
	}
}


inline bool CIndexedPolygon::UpdatePlane()
{
	if( m_index.size() <= 2 )
		return false;

	static const double s_NormalEpsilon = 0.0001;

	/// Find 3 vertices that are not on a single line
	/// - If a polygon has more than 4 vertices, some of its vertices may
	///   be on a straight line.

	Vector3 p0,p1,out;
	Vector3 normal(0.0f, 0.0f, 0.0f);
	Vector3 vZeroVector(0.0f, 0.0f, 0.0f);
	float dist;
	size_t i=0,j,k;
	const size_t num_vertices = m_index.size();
	while( (fabs(normal.x) < s_NormalEpsilon)
		&& (fabs(normal.y) < s_NormalEpsilon)
		&& (fabs(normal.z) < s_NormalEpsilon)
		&& (i < num_vertices) )
	{
	 
		j = (i+1) % num_vertices;
		k = (i+2) % num_vertices;
		p0 = GetVertex(i).m_vPosition - GetVertex(j).m_vPosition;
		p1 = GetVertex(k).m_vPosition - GetVertex(j).m_vPosition;
		Vec3Cross(normal, p1, p0 );
		Vec3Normalize(out, normal );
		i++;
	}

	if( normal == vZeroVector )
	{
		return false;
	}

	normal = out;
	dist = Vec3Dot( GetVertex(0).m_vPosition, normal );   //Distance between (0,0,0) and this plane

	m_Plane.normal = normal;
	m_Plane.dist = dist;

	if( fabs( fabs(normal.x) - 1.0 ) < NORMAL_EPSILON )
		m_Plane.type = 0;	// plane is perpendicular to x-axis
	else if( fabs( fabs(normal.y) - 1.0 ) < NORMAL_EPSILON )
		m_Plane.type = 1;	// plane is perpendicular to y-axis
	else if( fabs( fabs(normal.z) - 1.0 ) < NORMAL_EPSILON )
		m_Plane.type = 2;	// plane is perpendicular to z-axis
	else if( normal.y == 0.0f )
		m_Plane.type = 3;	// plane is vertical (y == 0)
	else
		m_Plane.type = 5;

	return true;
}


inline bool CIndexedPolygon::IsOnPolygon( const Vector3& rvPosition ) const
{
	const int num_triangles = GetNumVertices() - 2;
	for( int i=0; i<num_triangles; i++ )
	{
		if( IsOnTriangle( i, rvPosition ) )
			return true;
	}

	return false;
}


inline bool CIndexedPolygon::SharesPointWith( const CIndexedPolygon& polygon )
{
	const size_t num_vertices0 = m_index.size();
	const size_t num_vertices1 = polygon.m_index.size();
	for( size_t i=0; i<num_vertices0; i++)
	{
		for( size_t j=0; j<num_vertices1; j++)
		{
			if( m_index[i] == polygon.m_index[j] )
			{
				// TODO: compare vertices
				return true;
			}
		}
	}

	return false;
}


inline bool CIndexedPolygon::ClipLineSegment( const CLineSegment& line_segment, CLineSegmentHit& results ) const
{
	if( m_index.size() <= 2 )
		return false;

	// check triangles
	// - The polygon must be convex
	bool hit;
	const Vector3 vStart = line_segment.vStart;
	Vector3 vGoal  = line_segment.vGoal;

	const size_t num_triangles = m_index.size() - 2;
	for( size_t i=0; i<num_triangles; i++)
	{

		CTriangle triangle(
			GetVertex(0).m_vPosition,
			GetVertex(i+1).m_vPosition,
			GetVertex(i+2).m_vPosition,
			GetPlane().normal );

		hit = triangle.RayIntersect( vStart, vGoal );

		if( hit )
		{
			Vector3 vOrigTrace = line_segment.vGoal - line_segment.vStart;
			results.vEnd = vGoal;
			results.fFraction = Vec3Dot( results.vEnd - line_segment.vStart, vOrigTrace ) / Vec3LengthSq( vOrigTrace );
			return true;
		}
	}

	return false;
}


inline float CIndexedPolygon::CalculateArea() const
{
	float fCos, fSin;
	Vector3 vEdge1, vEdge2;
	float fEdgeLen1, fEdgeLen2;
	float fArea = 0.0f;

	const size_t num_vertices = m_index.size();
	for( size_t i=1; i<num_vertices - 1; i++)
	{
		vEdge1 = GetVertex(i).m_vPosition   - GetVertex(0).m_vPosition;
		vEdge2 = GetVertex(i+1).m_vPosition - GetVertex(0).m_vPosition;
		fEdgeLen1 = Vec3Length(vEdge1);
		fEdgeLen2 = Vec3Length(vEdge2);
		vEdge1 = vEdge1 / fEdgeLen1;
		vEdge2 = vEdge2 / fEdgeLen2;
		fCos = Vec3Dot(vEdge1, vEdge2);
		fSin = sqrtf( 1.0f - fCos * fCos );

		fArea += fEdgeLen1 * fEdgeLen2 * fSin / 2.0f;
	}

	return fArea;
}


inline void CIndexedPolygon::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_Plane;

	ar & m_AABB;

	ar & m_index;

	/// surface property of the polygon
	ar & m_MaterialIndex;
}


/// \param src_polygon_buffer [in] array of polygons to triangulate
/// \param dest_polygon_buffer [out] array of polygons that stores the triangulated polygons
inline void Triangulate( std::vector<CIndexedPolygon>& dest_polygon_buffer,
						 const std::vector<CIndexedPolygon>& src_polygon_buffer )
{
	const size_t num_pols = src_polygon_buffer.size();
	for( size_t i=0; i<num_pols; i++ )
	{
		src_polygon_buffer[i].Triangulate( dest_polygon_buffer );
	}
}


inline AABB3 GetAABB( const std::vector<CIndexedPolygon>& polygon_buffer )
{
	AABB3 aabb;
	aabb.Nullify();

	size_t i, num_pols = polygon_buffer.size();
	size_t j, num_verts;
	for( i=0; i<num_pols; i++ )
	{
		// polygon_buffer[i].Update();
		// aabb.MergeAABB( polygon_buffer[i].GetAABB() );

		num_verts = polygon_buffer[i].m_index.size();
		for( j=0; j<num_verts; j++ )
		{
			aabb.AddPoint( polygon_buffer[i].GetVertex((int)j).m_vPosition );
		}
	}

	return aabb;
}


inline void UpdateAABBs( std::vector<CIndexedPolygon>& polygon_buffer )
{
	size_t i, num_pols = polygon_buffer.size();
	for( i=0; i<num_pols; i++ )
	{
		polygon_buffer[i].UpdateAABB();
	}
}


inline bool AreOnSamePlane( const CIndexedPolygon& polygon0, const CIndexedPolygon& polygon1 )
{
	if( fabs(polygon1.GetPlane().dist - polygon0.GetPlane().dist) < 0.000001
	 && Vec3LengthSq( polygon1.GetPlane().normal - polygon0.GetPlane().normal ) < 0.000001 )
	{
		return true;
	}
	else
	{
		return false;
	}
}


/**
 * OPC_DONTCARE_NORMAL_DIRECTION
 *	the face is marked as on-plane if it is on the plane without any regard to normal direction
 * OPC_IF_NORMAL_SAME_DIRECTION (default)
 *	the face is regarded as on-plane if its normal is in the same direction as the plane
 */
inline int	ClassifyPolygon( const SPlane& plane,
						     const CIndexedPolygon& polygon,
							 int iOnPlaneCondition = CIndexedPolygon::OPC_DONTCARE_NORMAL_DIRECTION )
{
	int front = 0;
	int back = 0;
	int num_vertices = (int)polygon.m_index.size();

	for(int i=0; i<num_vertices; i++)
	{
		switch( ClassifyPoint( plane, polygon.GetVertex(i).m_vPosition ) )
		{
		case PNT_FRONT:		front++;	break;
		case PNT_BACK:		back++;		break;
		case PNT_ONPLANE:	// ignore vertrices on the plane
		default:;
		}
	}

	if( front == 0 && back == 0 )
	{

//		if( iOnPlaneCondition == OPC_DONTCARE_NORMAL_DIRECTION )
			return POLYGON_ONPLANE;		//the face is on-plane regardless of its normal direction

/*		SPlane &plane2 = face.GetPlane();

		else if( fabs( plane.normal.x - plane2.normal.x ) < NORMAL_EPSILON 
		&&  fabs( plane.normal.y - plane2.normal.y ) < NORMAL_EPSILON 
		&&  fabs( plane.normal.z - plane2.normal.z ) < NORMAL_EPSILON )
		{
			return POLYGON_ONPLANE;	// the 2 normals face the same direction
		}
		else
		{	// Consider the vertex to be behind the palne if the plane and the polygon
			// face the opposite directions
			return POLYGON_BACK;	//the 2 normals face the opposite directions
		}*/
	}
	if( front != 0 && back != 0 )
		return POLYGON_INTERSECTING;
	if( front > 0 )
		return POLYGON_FRONT;
	return POLYGON_BACK;
}


extern void UnweldVerticesOfPolygonsOnDifferentPlanes( std::vector<CIndexedPolygon>& polygon_buffer );

extern void UnweldVerticesBetween2GroupsOfPolygons( std::vector<CIndexedPolygon>& polygon_buffer,
													const std::vector<int>& polygon_indices0,
													const std::vector<int>& polygon_indices1 );



#endif  /* __INDEXEDPOLYGON_H__*/
