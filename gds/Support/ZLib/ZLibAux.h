#ifndef  __ZLIBAUX_H__
#define  __ZLIBAUX_H__


/// compress and save file
extern bool SaveFileCompressed( const char *dest_filename, const char *src_filename );

/// load a compressed file, decompress, put on a buffer
/// the pointer to the buffer is returned if the file is
/// successfully loaded & decompressed
extern char *LoadFileDataDecompressed( const char *compressed_filename );


#endif  /*  __ZLIBAUX_H__  */





