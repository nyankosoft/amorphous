#ifndef __GLTextureUtilities_HPP__
#define __GLTextureUtilities_HPP__


//#include "../../base.hpp" // syntax errors in <gl/gl.h> if this is not included on VC++ express 2010.
#include <string>
#include "amorphous/Graphics/OpenGL/GLHeaders.h"


namespace amorphous
{


bool SaveGL2DTextureToImageFile( GLuint texture, int width, int height, GLenum src_format, GLenum src_type, const std::string& image_filepath );


} // namespace amorphous


#endif /* __GLTextureUtilities_HPP__ */
