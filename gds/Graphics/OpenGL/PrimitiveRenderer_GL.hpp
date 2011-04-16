#ifndef __PrimitiveRenderer_GL_HPP__
#define __PrimitiveRenderer_GL_HPP__


#include "../PrimitiveRenderer.hpp"


class CPrimitiveRenderer_GL : public CPrimitiveRenderer
{
public:
	CPrimitiveRenderer_GL(){}
	~CPrimitiveRenderer_GL(){}

	Result::Name DrawPoint( const Vector3& pos, const SFloatRGBAColor& color )
	{
		glBegin(GL_POINTS);
		glColor4f( color.fRed, color.fGreen, color.fBlue, color.fAlpha );
		glVertex3f( pos.x, pos.y, pos.z );
		glEnd();

		return Result::SUCCESS;
	}

//	Result::Name DrawPoints();

	Result::Name DrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& color )
	{
		DrawLine( start, end, color, color );
		return Result::SUCCESS;
	}

	Result::Name DrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color )
	{
		glBegin(GL_LINES);
		glColor4f( start_color.fRed, start_color.fGreen, start_color.fBlue, start_color.fAlpha );
		glVertex3f( start.x, start.y, start.z );
		glColor4f( end_color.fRed,   end_color.fGreen,   end_color.fBlue,   end_color.fAlpha );
		glVertex3f( end.x,   end.y,   end.z );
		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawConnectedLines( const std::vector<Vector3>& points, const SFloatRGBAColor& color )
	{
		if( points.size() < 2 )
			return Result::INVALID_ARGS;

		glBegin(GL_LINE_STRIP);
		const size_t num_points = points.size();
		const SFloatRGBAColor line_color = color;
		for( size_t i=0; i<num_points; i++ )
		{
			glColor4f( line_color.fRed, line_color.fGreen, line_color.fBlue, line_color.fAlpha );
			glVertex3f( points[i].x, points[i].y, points[i].z );
		}
		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawConnectedLines( const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors )
	{
		if( points.size() < 2 || points.size() != colors.size() )
			return Result::INVALID_ARGS;

		glBegin(GL_LINE_STRIP);
		const size_t num_points = points.size();
		for( size_t i=0; i<num_points; i++ )
		{
			glColor4f( colors[i].fRed, colors[i].fGreen, colors[i].fBlue, colors[i].fAlpha );
			glVertex3f( points[i].x, points[i].y, points[i].z );
		}
		glEnd();

		return Result::SUCCESS;
	}

/*
	Result::Name DrawLines( PrimitiveType::Name mode, const std::vector<Vector3>& points, const SFloatRGBAColor& color )
	{
		const SFloatRGBAColor line_color = color;

		const int num_line_segments = (int)points.size() / 2;
		glBegin(GL_LINES);
		for( int i=0; i<num_line_segments; i++ )
		{
			const Vector3& p0 = points[i*2];
			const Vector3& p1 = points[i*2+1];
			glColor4f( line_color.fRed, line_color.fGreen, line_color.fBlue, line_color.fAlpha );
			glVertex3f( p0.x, p0.y, p0.z );
			glVertex3f( p1.x, p1.y, p1.z );
		}
		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawLines( PrimitiveType::Name mode, const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors )
	{
		return Result::UNKNOWN_ERROR;
	}*/
};


inline CPrimitiveRenderer_GL& GetPrimitiveRenderer_GL()
{
	static CPrimitiveRenderer_GL s_Renderer;
	return s_Renderer;
}



#endif /* __PrimitiveRenderer_GL_HPP__ */
