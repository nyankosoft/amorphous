#ifndef __amorphous_Graphics_StringAux_HPP__
#define __amorphous_Graphics_StringAux_HPP__


#include <string>
#include "FloatRGBAColor.hpp"
#include "TextureCoord.hpp"


namespace amorphous
{


inline std::string to_string( const SFloatRGBColor& c, int precision = 3, int num_zfills = 0 )
{
	std::string fmt_float = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";
	std::string fmt_buffer = "( " + fmt_float + ", " + fmt_float + ", " + fmt_float + " )";

	char buffer[80];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, fmt_buffer.c_str(), c.red, c.green, c.blue );

	return std::string(buffer);
}


inline std::string to_string( const SFloatRGBAColor& c, int precision = 3, int num_zfills = 0 )
{
	std::string fmt_float = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";
	std::string fmt_buffer = "( " + fmt_float + ", " + fmt_float + ", " + fmt_float + ", " + fmt_float + " )";

	char buffer[80];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, fmt_buffer.c_str(), c.red, c.green, c.blue, c.alpha );

	return std::string(buffer);
}


inline std::string to_string( const TEXCOORD2& tex, int precision = 3, int num_zfills = 0 )
{
	std::string fmt_float = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";
	std::string fmt_buffer = "( " + fmt_float + ", " + fmt_float + " )";

	char buffer[32];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, fmt_buffer.c_str(), tex.u, tex.v );

	return std::string(buffer);
}


} // namespace amorphous


#endif /* __amorphous_Graphics_StringAux_HPP__ */
