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
		glColor4f( color.red, color.green, color.blue, color.alpha );
		glVertex3f( pos.x, pos.y, pos.z );
		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawPoints( const std::vector<Vector3>& points, const SFloatRGBAColor& color )
	{
		glBindTexture( GL_TEXTURE_2D, 0 );

		glBegin(GL_POINTS);
		const size_t num_points = points.size();
		const SFloatRGBAColor c = color;
		for( size_t i=0; i<num_points; i++ )
		{
			glColor4f( c.red, c.green, c.blue, c.alpha );
			glVertex3f( points[i].x, points[i].y, points[i].z );
		}
		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& color )
	{
		DrawLine( start, end, color, color );
		return Result::SUCCESS;
	}

	Result::Name DrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color )
	{
		glBindTexture( GL_TEXTURE_2D, 0 );

		glBegin(GL_LINES);
		glColor4f( start_color.red, start_color.green, start_color.blue, start_color.alpha );
		glVertex3f( start.x, start.y, start.z );
		glColor4f( end_color.red,   end_color.green,   end_color.blue,   end_color.alpha );
		glVertex3f( end.x,   end.y,   end.z );
		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawConnectedLines( const std::vector<Vector3>& points, const SFloatRGBAColor& color )
	{
		if( points.size() < 2 )
			return Result::INVALID_ARGS;

		glBindTexture( GL_TEXTURE_2D, 0 );

		glBegin(GL_LINE_STRIP);
		const size_t num_points = points.size();
		const SFloatRGBAColor line_color = color;
		for( size_t i=0; i<num_points; i++ )
		{
			glColor4f( line_color.red, line_color.green, line_color.blue, line_color.alpha );
			glVertex3f( points[i].x, points[i].y, points[i].z );
		}
		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawConnectedLines( const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors )
	{
		if( points.size() < 2 || points.size() != colors.size() )
			return Result::INVALID_ARGS;

		glBindTexture( GL_TEXTURE_2D, 0 );

		glBegin(GL_LINE_STRIP);
		const size_t num_points = points.size();
		for( size_t i=0; i<num_points; i++ )
		{
			glColor4f( colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha );
			glVertex3f( points[i].x, points[i].y, points[i].z );
		}
		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawRect( const Vector3 *positions, const Vector3 *normals, const SFloatRGBAColor *diffuse_colors, const TEXCOORD2 *tex_coords_0 )
	{
		return Result::UNKNOWN_ERROR;
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
			glColor4f( line_color.red, line_color.green, line_color.blue, line_color.alpha );
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
