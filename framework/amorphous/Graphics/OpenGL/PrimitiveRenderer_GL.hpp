#ifndef __PrimitiveRenderer_GL_HPP__
#define __PrimitiveRenderer_GL_HPP__


#include "../PrimitiveRenderer.hpp"


namespace amorphous
{


class CPrimitiveRenderer_GL : public PrimitiveRenderer
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

		LOG_GL_ERROR( " Clearing OpenGL errors..." );

		// Unbind GL_ARRAY_BUFFER and GL_ELEMENT_ARRAY_BUFFER to source a standard memory location (RAM).
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		Vector3 points[] = { start, end };
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, points );

		SFloatRGBAColor colors[] = { start_color, end_color };
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, colors );

		LOG_GL_ERROR( " Clearing OpenGL errors before glDrawElements()..." );

		const U16 indices[] = {0,1};
		int num_indices = 2;
		glDrawElements( GL_LINES, num_indices, GL_UNSIGNED_SHORT, indices );

		LOG_GL_ERROR( " Clearing OpenGL errors after glDrawElements()..." );

//		glBegin(GL_LINES);
//		glColor4f( start_color.red, start_color.green, start_color.blue, start_color.alpha );
//		glVertex3f( start.x, start.y, start.z );
//		glColor4f( end_color.red,   end_color.green,   end_color.blue,   end_color.alpha );
//		glVertex3f( end.x,   end.y,   end.z );
//		glEnd();

		return Result::SUCCESS;
	}

	Result::Name DrawLines( const std::vector<Vector3>& points, const SFloatRGBAColor& color )
	{
		LOG_PRINT_ERROR( "Not implemented yet." );

		return Result::UNKNOWN_ERROR;
	}

	Result::Name DrawLines( const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors )
	{
		LOG_PRINT_ERROR( "Not implemented yet." );

		return Result::UNKNOWN_ERROR;
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


} // amorphous



#endif /* __PrimitiveRenderer_GL_HPP__ */
