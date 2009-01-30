#ifndef  __2DPolygon_H__
#define  __2DPolygon_H__


#include "2DRoundRect.hpp"


/**
  polygon

 */
class C2DRegularPolygon : public C2DRoundRect
{
protected:

	/// polygon vertices (= buffer size + 1)
	int m_NumPolygonVertices;

	Vector2 m_vCenterPosition;

	std::vector<int> m_vecRadius;

	CRegularPolygonStyle::Name m_InitStyle;

protected:

	void ResizeBuffer();

	virtual int GetNumVertices() const { return m_NumPolygonVertices + 2; }

	virtual void CalculateLocalVertexPositions();

	virtual void UpdateVertexPositions();

//	inline virtual void UpdateColor();

public:

	inline C2DRegularPolygon();

//	inline C2DRegularPolygon( int num_vertices, const Vector2& vCenter, int radius, RegularPolygonStyle::Name style = CRegularPolygonStyle::VERTEX_AT_TOP );

	virtual ~C2DRegularPolygon() {}

	virtual int unsigned GetPrimitiveType() const { return C2DPrimitive::TYPE_POLYGON; }

	/// calls ResizeBuffer() internally
	/// - must not be called from constructor
//	virtual void SetZDepth(float fZValue);

//	void ScalePosition( float fScale );

	void MakeRegularPolygon( int num_polygon_vertices, int x, int y, int radius, CRegularPolygonStyle::Name style = CRegularPolygonStyle::VERTEX_AT_TOP ) { MakeRegularPolygon( num_polygon_vertices, Vector2((float)x,(float)y), radius, style ); }

	void MakeRegularPolygon( int num_polygon_vertices, const Vector2& vCenter, int radius, CRegularPolygonStyle::Name style = CRegularPolygonStyle::VERTEX_AT_TOP );

	void SetRadius( int radius );

	/// change the radius at a vertex
	/// - This makes the polygon non-regular
	/// - Added to draw the cobweb chart
	void SetRadius( int vertex, int radius );

	Vector2 GetVertexPos( int vertex );

	void SetVertexColor( int vertex, const SFloatRGBAColor& color );
};


// =================================== inline implementations =================================== 

inline C2DRegularPolygon::C2DRegularPolygon()
:
m_NumPolygonVertices(6),
m_vCenterPosition(Vector2(0,0))
{
	SetColor( 0xFFFFFFFF );
}


#endif		/*  __2DPolygon_H__  */
