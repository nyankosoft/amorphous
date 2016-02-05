#include "GLTextureUtilities.hpp"
#include "GLGraphicsDevice.hpp"
#include "Support/BitmapImage.hpp"


namespace amorphous
{

using namespace std;


bool SaveGL2DTextureToImageFile( GLuint texture, int width, int height, GLenum src_format, GLenum src_type, const std::string& image_filepath )
{
	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	glBindTexture( GL_TEXTURE_2D, texture );

	LOG_GL_ERROR( "glBindTexture() failed." );

	uint pixel_data_size = 16;
	if(src_format == GL_RGBA)
	{
		if(src_type == GL_UNSIGNED_BYTE)
			pixel_data_size = 4;
	}

	vector<uchar> pixels;
	pixels.resize( width * height * pixel_data_size, 0 );

	glGetTexImage(
		GL_TEXTURE_2D,   // GLenum target,
 		0,               // GLint level,
		src_format,      // GLenum format,
 		src_type,        // GLenum type,
 		&pixels[0]       // GLvoid *img
	);

	LOG_GL_ERROR( "glGetTexImage() failed." );

	int bpp          = (src_format == GL_RGBA || src_format == GL_BGRA) ? 32 : 24;
	int num_channels = (src_format == GL_RGBA || src_format == GL_BGRA) ? 4 : 3;
//	const int width  = m_TextureDesc.Width;
//	const int height = m_TextureDesc.Height;
	BitmapImage img( width, height, bpp );
	for( int y=0; y<height; y++ )
//	for( int y=height-1; 0<=y; y-- )
	{
		for( int x=0; x<width; x++ )
		{
			int offset = ((height-y-1) * width + x) * num_channels;
			U8 r = pixels[offset + 0];
			U8 g = pixels[offset + 1];
			U8 b = pixels[offset + 2];

			if( num_channels == 4 )
			{
				U8 a = pixels[offset + 3];
				img.SetPixel( x, y, r, g, b, a );
			}
			else
			{
				img.SetPixel( x, y, r, g, b );
			}
		}
	}

	img.SaveToFile( image_filepath );

	return true;
}


} // namespace amorphous
