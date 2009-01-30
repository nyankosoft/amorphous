
#ifndef  __zpipe_stream_H__
#define  __zpipe_stream_H__

#include "zlib.h"
#include "../stream_buffer.hpp"

extern int z_def( stream_buffer& src_buffer, stream_buffer& dest_buffer, int level = Z_DEFAULT_COMPRESSION );
extern int z_inf( stream_buffer& src_buffer, stream_buffer& dest_buffer );


#endif		/*  __zpipe_stream_H__  */


