
#ifndef  __zpipe_stream_H__
#define  __zpipe_stream_H__

#include "zlib.h"
#include "../stream_buffer.hpp"

extern int z_def( amorphous::stream_buffer& src_buffer, amorphous::stream_buffer& dest_buffer, int level = Z_DEFAULT_COMPRESSION );
extern int z_inf( amorphous::stream_buffer& src_buffer, amorphous::stream_buffer& dest_buffer );


#endif		/*  __zpipe_stream_H__  */


