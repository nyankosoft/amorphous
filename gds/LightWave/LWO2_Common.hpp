#ifndef __LWO2_COMMON_H__
#define __LWO2_COMMON_H__


#include <stdio.h>
#include <stdlib.h>


namespace amorphous
{


typedef unsigned short UINT2;
typedef unsigned long UINT4;


#define MAKE_ID(a,b,c,d)	\
	((unsigned long) (a)<<24 | (unsigned long) (b)<<16 | (unsigned long) (c)<<8 | (unsigned long) (d))



/* Universal IFF identifiers */
#define ID_FORM		MAKE_ID('F','O','R','M')
#define ID_LWO2		MAKE_ID('L','W','O','2')

// Primary chunk ID
#define ID_LAYR		MAKE_ID('L','A','Y','R')
#define ID_TAGS		MAKE_ID('T','A','G','S')
#define ID_CLIP		MAKE_ID('C','L','I','P')
#define ID_SURF		MAKE_ID('S','U','R','F')
#define ID_VMAP		MAKE_ID('V','M','A','P')
#define ID_VMAD		MAKE_ID('V','M','A','D')
#define ID_BBOX		MAKE_ID('B','B','O','X')

// PTAG subchunks
#define ID_PART		MAKE_ID('P','A','R','T')
#define ID_SMGP		MAKE_ID('S','M','G','P')
#define ID_BNUP		MAKE_ID('B','N','U','P')
#define ID_BONE		MAKE_ID('B','O','N','E')
#define ID_BNWT		MAKE_ID('B','N','W','T')

// Primary chunks that are found in each layer, per-layer chuncks
#define ID_PNTS		MAKE_ID('P','N','T','S')
#define ID_POLS		MAKE_ID('P','O','L','S')
#define ID_PTAG		MAKE_ID('P','T','A','G')
#define ID_VMAP		MAKE_ID('V','M','A','P')

// POLS TYPE
#define ID_FACE		MAKE_ID('F','A','C','E')
#define ID_BONE		MAKE_ID('B','O','N','E')

// VMAP type
#define ID_TXUV		MAKE_ID('T','X','U','V')
#define ID_RGBA		MAKE_ID('R','G','B','A')
#define ID_PICK		MAKE_ID('P','I','C','K')	// point selection set
#define ID_WGHT		MAKE_ID('W','G','H','T')

// CLIP subchunk
#define ID_STIL		MAKE_ID('S','T','I','L')

// SURF subchunk
#define ID_VCOL		MAKE_ID('V','C','O','L')
#define ID_BLOK		MAKE_ID('B','L','O','K')
#define ID_IMAG		MAKE_ID('I','M','A','G')
#define ID_SMAN		MAKE_ID('S','M','A','N')
#define ID_CMNT		MAKE_ID('C','M','N','T')
#define ID_COLR		MAKE_ID('C','O','L','R')
#define ID_DIFF		MAKE_ID('D','I','F','F')
#define ID_SPEC		MAKE_ID('S','P','E','C')
#define ID_LUMI		MAKE_ID('L','U','M','I')
#define ID_REFL		MAKE_ID('R','E','F','L')
#define ID_TRAN		MAKE_ID('T','R','A','N')
#define ID_TRNL		MAKE_ID('T','R','N','L')
#define ID_GLOS		MAKE_ID('G','L','O','S')
#define ID_BUMP		MAKE_ID('B','U','M','P')

#define ID_IMAP		MAKE_ID('I','M','A','P')
#define ID_PROC		MAKE_ID('P','R','O','C')
#define ID_GRAD		MAKE_ID('G','R','A','D')
#define ID_SHDR		MAKE_ID('S','H','D','R')

#define ID_CHAN		MAKE_ID('C','H','A','N')
#define ID_ENAB		MAKE_ID('E','N','A','B')
#define ID_OPAC		MAKE_ID('O','P','A','C')
#define ID_AXIS		MAKE_ID('A','X','I','S')

#define ID_DESC		MAKE_ID('D','E','S','C')
#define ID_TEXT		MAKE_ID('T','E','X','T')

#define ID_FLAG		MAKE_ID('F','L','A','G')

/**  IMAGE MAP  **/
//#define ID_VMAP		MAKE_ID('V','M','A','P')


// Swap 4 bytes
inline unsigned long MSB4 (unsigned long w)
{
	unsigned long tmp;
	tmp =  (w & 0x000000ff);                           // (w & 11111111)
	tmp = ((w & 0x0000ff00) >> 0x08) | (tmp << 0x08);  // (w & 1111111100000000 ) >> 8
	tmp = ((w & 0x00ff0000) >> 0x10) | (tmp << 0x08);  // (w & 111111110000000000000000 ) >> 16
	tmp = ((w & 0xff000000) >> 0x18) | (tmp << 0x08);  // (w & 11111111000000000000000000000000 ) >> 24
	return tmp;
}


inline unsigned short MSB2 (unsigned short w)
{
	unsigned short tmp;
	tmp =  (w & 0x00ff);                          // (w & 11111111)
	tmp = ((w & 0xff00) >> 0x08) | (tmp << 0x08); // (w & 1111111100000000)
	return tmp;
}

/*
UINT4 Read4Bytes(FILE* fp){
	UINT4 uiRead;
	fread( &uiRead, sizeof( UINT4 ), 1, fp );  //fread( void *buffer, size_t size, size_t count, FILE *stream );
	return uiRead;
}

UINT2 Read2Bytes(FILE* fp){
	UINT2 wRead;
	fread( &wRead, sizeof( UINT2 ), 1, fp );  //fread( void *buffer, size_t size, size_t count, FILE *stream );
	return wRead;
}
*/

//Read 4 bytes in Big-Endian order, 
//then change the order into Little-Endian and return the 4 bytes.
inline UINT4 ReadBE4BytesIntoLE(FILE* fp)
{
	UINT4 ui;
	fread( &ui, sizeof( UINT4 ), 1, fp );//	ui = Read4Bytes(fp);
	ui = MSB4(ui);
	return ui;
}

inline UINT2 ReadBE2BytesIntoLE(FILE* fp)
{
	UINT2 w;
	fread( &w, sizeof( UINT2 ), 1, fp );  //	w = Read2Bytes(fp);
	w = MSB2(w);
	return w;
}

// Read a variable length index.
// A variable length index may be 2 bytes or 4 bytes.
extern UINT4 ReadVLIndex(FILE* fp, int *pIndexSize);

// read a string name
extern int ReadName(char* pName, FILE* fp);

// Progress the file pointer	'fp' by 'count' bytes 
extern void AdvanceFP(FILE *fp, UINT4 count);


} // amorphous



#endif  /*  __LWO2_COMMON_H__  */
