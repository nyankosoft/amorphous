
#include "zlib.h"
#include "stdio.h"


#define ZAUX_CHUNK 16384


static size_t GetFileSize( FILE *fp )
{
	char buffer[ZAUX_CHUNK];
	size_t read_bytes;
	size_t count = 0;

	// save the current position
	fpos_t pos;
	fgetpos( fp, &pos );

	do
	{
		read_bytes = fread( buffer, 1, ZAUX_CHUNK, fp );
		count++;
	}
	while( read_bytes == ZAUX_CHUNK );

	// reset the file pointer
	fsetpos( fp, &pos );

	return ZAUX_CHUNK * (count-1) + read_bytes;
}


/// compress and save file
bool SaveFileCompressed( const char *dest_filename, const char *src_filename )
{
	FILE *fp_src = fopen( src_filename, "rb" );

	if( !fp_src )
		return false;

	unsigned long file_size = GetFileSize( fp_src );

	if( file_size == 0 )
		return false;

	// the size of the source file has been confirmed
	// load the data to the buffer

	char *pSrcBuffer = new char [file_size];
	char *pDestBuffer = new char [compressBound(file_size)];
	unsigned long compressed_size = compressBound(file_size);

	// copy the content of the source file to the source buffer
	fread( pSrcBuffer, file_size, 1, fp_src );

    int err = compress((Bytef*)pDestBuffer, &compressed_size,
		               (const Bytef*)pSrcBuffer, file_size);

	FILE *fp_dest = fopen( dest_filename, "wb" );

	// write the size of the original file at the head of the compressed file
	fwrite( &file_size, sizeof(unsigned long), 1, fp_dest );

	// write the compressed data
	fwrite( pDestBuffer, compressed_size, 1, fp_dest );

	delete [] pSrcBuffer;
	delete [] pDestBuffer;

	fclose(fp_src);
	fclose(fp_dest);

	return true;

/*	int ret, flush;
	unsigned have;
	z_stream strm;
	char in[CHUNK];
	char out[CHUNK];

	// allocate deflate state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return ret;

	// compress until end of file
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source))
		{
			(void)deflateEnd(&strm);
			return Z_ERRNO;
		}
		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		// run deflate() on input until output buffer not full, finish
		//	compression if all of source has been read in
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);    // no bad return value
			assert(ret != Z_STREAM_ERROR);  // state not clobbered
			have = CHUNK - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)deflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);
		assert(strm.avail_in == 0);     // all input will be used

		// done when last data in file processed
	} while (flush != Z_FINISH);
	assert(ret == Z_STREAM_END);        // stream will be complete

	// clean up and return
	(void)deflateEnd(&strm);
	return Z_OK;
	*/
}



/// load a compressed file, decompress, put on a buffer
/// the pointer to the buffer is returned if the file is
/// successfully loaded & decompressed
char *LoadFileDataDecompressed( const char *compressed_filename )
{
	FILE *fp_src = fopen( compressed_filename, "rb" );

	if( !fp_src )
        return NULL;

	unsigned long decompressed_size;
	fread( &decompressed_size, sizeof(unsigned long), 1, fp_src );
	char *pDestBuffer = new char [decompressed_size];

	// get the size of the compressed data
	int compressed_size = GetFileSize( fp_src );

	if( compressed_size == 0 )
		return NULL;

	char *pSrcBuffer = new char [compressed_size];
	fread( pSrcBuffer, compressed_size, 1, fp_src );

	uncompress( (Bytef *)pDestBuffer, &decompressed_size, (Bytef *)pSrcBuffer, compressed_size );

	fclose( fp_src );
	delete [] pSrcBuffer;

	return pDestBuffer;
}