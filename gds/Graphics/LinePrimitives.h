#ifndef  __LinePrimitives_H__
#define  __LinePrimitives_H__


#include <vector>
#include "Graphics/all.h"
#include "3DMath/Matrix34.h"


/**
 line type primtives
 - mainly for visual debugging
*/
class CLinePrimitives
{
protected:

	std::vector<TEXTUREVERTEX> m_vecPoint;

public:

	CLinePrimitives() {}

	CLinePrimitives( const std::vector<Vector3>& vecPoints, U32 color = 0xFFFFFFFF) { SetPoints( vecPoints, color ); }

	virtual void SetPoints( const std::vector<Vector3>& vecPoints, U32 color = 0xFFFFFFFF );

	virtual ~CLinePrimitives() {}

//	void SetColor( U32 color );

	virtual void Draw() = 0;

	void Clear() { m_vecPoint.resize(0); }
};


/**
 A line strip is a primitive that is composed of connected line segments.
 */
class CLineStrip : public CLinePrimitives
{
public:

	CLineStrip() {}

	CLineStrip( const std::vector<Vector3>& vecPoints, U32 color )
		:
	CLinePrimitives( vecPoints, color ) {}

//	void SetPoints( const std::vector<Vector3>& vecPoints, U32 color );

	void SetColor( U32 color );

	void Draw();
};


class CLineList : public CLinePrimitives
{
public:

	CLineList() {}

	CLineList( const std::vector<Vector3>& vecPoints, U32 color )
		:
	CLinePrimitives( vecPoints, color ) {}

	void SetLineSegmentColor( int line_segment_index, U32 color );

	void AddLineSegment( Vector3 start, Vector3 end, U32 color );

//	void SetPoints( const std::vector<Vector3>& vecPoints, U32 color );

	void Draw();
};


inline void GetCoordAxesDisplay( CLineList& dest, const Matrix34& pose, float axis_length = 1.0f )
{
	float a = axis_length;

	dest.AddLineSegment( pose * Vector3(0,0,0), pose * Vector3(a,0,0), 0xFFFF0000 );
	dest.AddLineSegment( pose * Vector3(0,0,0), pose * Vector3(0,a,0), 0xFF00FF00 );
	dest.AddLineSegment( pose * Vector3(0,0,0), pose * Vector3(0,0,a), 0xFF0000FF );
}


inline void GetCoordAxesDisplay( CLineList& dest, const std::vector<Matrix34>& vecPose, float axis_length = 1.0f )
{
	const size_t num = vecPose.size();
	for( size_t i=0; i<num; i++ )
	{
		GetCoordAxesDisplay( dest, vecPose[i], axis_length );
	}
}


inline void DrawCoordAxes( const Matrix34& pose, float axis_length = 1.0f )
{
	CLineList axes;
	GetCoordAxesDisplay( axes, pose, axis_length );
	axes.Draw();
}


#endif /* __LinePrimitives_H__ */
