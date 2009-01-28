//--------------------------------------------------------------------------------------
// File: PackedFile.cpp
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//#include "../../DXUT/DXUT.h"
#include "PackedFile.h"
#include "../SafeDelete.h"

//#include "SDKMisc.h"

using namespace std;


//--------------------------------------------------------------------------------------
CPackedFile::CPackedFile() :
m_pChunks(NULL),
m_pFileIndices(NULL),
m_pMappedChunks(NULL),
m_hFileMapping(0),
m_hFile(0)
{
	ZeroMemory( &m_FileHeader, sizeof(PACKED_FILE_HEADER) );
}

//--------------------------------------------------------------------------------------
CPackedFile::~CPackedFile()
{
	UnloadPackedFile();
}

//--------------------------------------------------------------------------------------
UINT64 GetSize( const char* szFile )
{
	UINT64 Size = 0;

	HANDLE hFile = CreateFileA( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( INVALID_HANDLE_VALUE != hFile )
	{
		LARGE_INTEGER FileSize;
		GetFileSizeEx( hFile, &FileSize );
		Size = FileSize.QuadPart;
		CloseHandle(hFile);
	}

	return Size;
}

//--------------------------------------------------------------------------------------
UINT64 GetSize( WCHAR* szFile )
{
	UINT64 Size = 0;

//	HANDLE hFile = CreateFile( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	HANDLE hFile = CreateFileW( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( INVALID_HANDLE_VALUE != hFile )
	{
		LARGE_INTEGER FileSize;
		GetFileSizeEx( hFile, &FileSize );
		Size = FileSize.QuadPart;
		CloseHandle(hFile);
	}

	return Size;
}

//--------------------------------------------------------------------------------------
// Align the input offset to the specified granularity (see CreatePackedFile below)
//--------------------------------------------------------------------------------------
UINT64 AlignToGranularity( UINT64 Offset, UINT64 Granularity )
{
	UINT64 floor = Offset / Granularity;
	return (floor+1)*Granularity;
}

//--------------------------------------------------------------------------------------
// Write bytes into the file until the granularity is reached (see CreatePackedFile below)
//--------------------------------------------------------------------------------------
UINT64 FillToGranularity( HANDLE hFile, UINT64 CurrentOffset, UINT64 Granularity )
{
	UINT64 NewOffset = AlignToGranularity( CurrentOffset, Granularity );
	UINT64 NumBytes = NewOffset - CurrentOffset;

	DWORD dwWritten;
	BYTE Zero = 0;
	for( UINT64 i=0; i<NumBytes; i++ )
	{
		if( !WriteFile( hFile, &Zero, sizeof(BYTE), &dwWritten, NULL ) )
			return 0;
	}

	return NewOffset;
}

/**
 * Creates a packed file.  The file is a flat uncompressed file containing all resources
 * needed for the sample.  The file consists of chunks of data.  Each chunk represents
 * a mappable window that can be accessed by MapViewOfFile.  Since MapViewOfFile can
 * only map a view onto a file in 64k granularities, each chunk must start on a 64k
 * boundary.  The packed file also creates an index.  This index is loaded into memory
 * at startup and is not memory mapped.  The index is used to find the locations of 
 * resource files within the packed file.
 */
struct STRING { WCHAR str[MAX_PATH]; };
bool CPackedFile::CreatePackedFile( //ID3D10Device* pDev10,
								    //IDirect3DDevice9* pDev9,
									const string& packfilename,// WCHAR* szFileName,
									vector<string> src_filename,
									vector<string> dest_filename )
//									LEVEL_ITEM* pLevelItems,
//									UINT NumLevelItems )
{
	bool bRet = false;
	HANDLE hFile;

	m_FileHeader.NumFiles = 0;

//	CDXUTSDKMesh meshFile;

	vector<FILE_INDEX*> TempFileIndices;
	vector<CHUNK_HEADER*> TempHeaderList;
//	vector<STRING> FullFilePath;
	vector<string> FullFilePath;

//	STRING strFullPathW;
	string strFullPath;
//	WCHAR strTempName[MAX_PATH];
	FILE_INDEX* pindex;

	// Add meshes
//	for( UINT i=0; i<NumLevelItems; i++ )
	size_t num_src_files = src_filename.size();
	for( size_t i=0; i<num_src_files; i++ )
	{
		// Find the file
//		if( SUCCEEDED(DXUTFindDXSDKMediaFileCch( strFullPathW.str, MAX_PATH, pLevelItems[i].szName )) )
		strFullPath = src_filename[i];
		if( true )
		{
//			UINT64 size = GetSize( strFullPathW.str );
			UINT64 size = GetSize( strFullPath.c_str() );
			if( size > 0 )
			{
				pindex = new FILE_INDEX;

				pindex->FileSize = size;
/*
				// Try to load the actual mesh
				if( pDev10 )
				{
					if( FAILED( meshFile.Create( pDev10, strFullPathW.str ) ) )	{SafeDelete( pindex );	continue;}
				}
				else if( pDev9 )
				{
					if( FAILED( meshFile.Create( pDev9, strFullPathW.str ) ) )	{SafeDelete( pindex );	continue;}
				}
*/
//				StringCchCopy( pindex->szFileName, MAX_PATH, pLevelItems[i].szName );
				strncpy( pindex->szFileName, dest_filename[i].c_str(), MAX_PATH );

//				FullFilePath.push_back( strFullPathW );
				FullFilePath.push_back( src_filename[i].c_str() );

				// Get the mesh path
//				WCHAR* strPath = meshFile.GetMeshPathW();

				// Add this mesh
				TempFileIndices.push_back( pindex );
/*
				// Find the textures
				for( UINT i=0; i<meshFile.GetNumMaterials(); i++ )
				{
					SDKMESH_MATERIAL* pMat = meshFile.GetMaterial(i);
					if( pMat->DiffuseTexture[0] != 0 )
					{
						MultiByteToWideChar( CP_ACP, 0, pMat->DiffuseTexture, -1, strTempName, MAX_PATH );
						StringCchPrintf( strFullPathW.str, MAX_PATH, L"%s%s", strPath, strTempName );

						size = GetSize( strFullPathW.str );
						if( size > 0 )
						{
							pindex = new FILE_INDEX;
							StringCchPrintf( pindex->szFileName, MAX_PATH, strTempName );
							pindex->FileSize = size;
							TempFileIndices.Add( pindex );
							FullFilePath.Add( strFullPathW );
						}
					}
					if( pMat->NormalTexture[0] != 0 )
					{
						MultiByteToWideChar( CP_ACP, 0, pMat->NormalTexture, -1, strTempName, MAX_PATH );
						// rest same as DiffuseTexture case
					}
					if( pMat->SpecularTexture[0] != 0 )
					{
						MultiByteToWideChar( CP_ACP, 0, pMat->SpecularTexture, -1, strTempName, MAX_PATH );
						// rest same as DiffuseTexture case
					}
				}

				// Destroy this mesh
				meshFile.Destroy();*/
			}
		}
	}

	UINT64 ChunkSize = 128*1024*1024;	// Fixed 128 meg chunks
										// This will vary according to application needs
	SYSTEM_INFO SystemInfo;
	GetSystemInfo( &SystemInfo );
	UINT64 Granularity = SystemInfo.dwAllocationGranularity; // Allocation granularity (always 64k)
	UINT64 CurrentChunk = 0;
	CHUNK_HEADER* pchunkheader = new CHUNK_HEADER;
	pchunkheader->ChunkOffset = 0;
	pchunkheader->ChunkSize = 0;

	// Calculate all of the file chunk indices and offsets needed
	for( size_t i=0; i<TempFileIndices.size(); i++ )
	{
		pindex = TempFileIndices.at(i);

		if( pchunkheader->ChunkSize + pindex->FileSize > ChunkSize )
		{
			TempHeaderList.push_back( pchunkheader );

			pchunkheader = new CHUNK_HEADER;
			pchunkheader->ChunkOffset = 0;
			pchunkheader->ChunkSize = 0;
			CurrentChunk ++;
		}

		pindex->ChunkIndex = CurrentChunk;
		pindex->OffsetIntoChunk = pchunkheader->ChunkSize;
		pchunkheader->ChunkSize += pindex->FileSize;
	}
	if( pchunkheader->ChunkSize > 0 )
		TempHeaderList.push_back( pchunkheader );
	else
		SafeDelete( pchunkheader );

	UINT64 IndexSize = sizeof( PACKED_FILE_HEADER ) + sizeof( CHUNK_HEADER )*TempHeaderList.size() + sizeof( FILE_INDEX )*TempFileIndices.size();
	UINT64 ChunkOffset = AlignToGranularity( IndexSize, Granularity );
	
	// Align chunks to the proper granularities
	for( size_t i=0; i<TempHeaderList.size(); i++ )
	{
		pchunkheader = TempHeaderList.at(i);
		pchunkheader->ChunkOffset = ChunkOffset;

		if( i < TempHeaderList.size() )
			ChunkOffset += AlignToGranularity( pchunkheader->ChunkSize, Granularity );
	}
	
	// Fill in the header data
	m_FileHeader.FileSize = ChunkOffset;
	m_FileHeader.ChunkSize = ChunkSize;
	m_FileHeader.NumChunks = TempHeaderList.size();
	m_FileHeader.NumFiles = TempFileIndices.size();
	m_FileHeader.Granularity = Granularity;

	// Open the file
	hFile = CreateFile( packfilename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
//	hFile = CreateFileW( szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( INVALID_HANDLE_VALUE == hFile )
		return bRet;

	// write the header
	DWORD dwWritten;
	DWORD dwRead;
	if( !WriteFile( hFile, &m_FileHeader, sizeof(PACKED_FILE_HEADER), &dwWritten, NULL ) )
		goto Error;

	// write out chunk headers
	for( size_t i=0; i<TempHeaderList.size(); i++ )
	{
		pchunkheader = TempHeaderList.at(i);
		if( !WriteFile( hFile, pchunkheader, sizeof(CHUNK_HEADER), &dwWritten, NULL ) )
			goto Error;
	}

	// write the index
	for( size_t i=0; i<TempFileIndices.size(); i++ )
	{
		pindex = TempFileIndices.at(i);
		if( !WriteFile( hFile, pindex, sizeof(FILE_INDEX), &dwWritten, NULL ) )
			goto Error;
	}

	// Fill in up to the granularity
	UINT64 CurrentFileSize = IndexSize;
	CurrentFileSize = FillToGranularity( hFile, CurrentFileSize, Granularity );
	if( 0 == CurrentFileSize )
		goto Error;

	// Write out the files
	CurrentChunk = 0;
	for( size_t i=0; i<TempFileIndices.size(); i++ )
	{
		pindex = TempFileIndices.at(i);

		if( pindex->ChunkIndex != CurrentChunk )
		{
			// Fill in up to the granularity
			CurrentFileSize = FillToGranularity( hFile, CurrentFileSize, Granularity );
			if( 0 == CurrentFileSize )
				goto Error;

			CurrentChunk = pindex->ChunkIndex;
		}

		// Write out the indexed file
		HANDLE hIndexFile = CreateFile( FullFilePath.at(i).c_str(), FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
//		HANDLE hIndexFile = CreateFileW( FullFilePath.at(i).str, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if( INVALID_HANDLE_VALUE == hIndexFile )
			goto Error;

		BYTE* pTempData = new BYTE[ (SIZE_T)pindex->FileSize ];
		if( !pTempData )
		{
			CloseHandle( hIndexFile );
			goto Error;
		}

		if( !ReadFile( hIndexFile, pTempData, (DWORD)pindex->FileSize, &dwRead, NULL ) )
		{
			CloseHandle( hIndexFile );
			SafeDeleteArray( pTempData );
			goto Error;
		}

		CloseHandle( hIndexFile );

		if( !WriteFile( hFile, pTempData, (DWORD)pindex->FileSize, &dwWritten, NULL ) )
		{
			CloseHandle( hIndexFile );
			SafeDeleteArray( pTempData );
			goto Error;
		}

		SafeDeleteArray( pTempData );

		CurrentFileSize += pindex->FileSize;
	}
	
	bRet = true;
Error:

	for( size_t i=0; i<TempFileIndices.size(); i++ )
	{
		pindex = TempFileIndices.at(i);
		SafeDelete( pindex );
	}

	for( size_t i=0; i<TempHeaderList.size(); i++ )
	{
		pchunkheader = TempHeaderList.at(i);
		SafeDelete( pchunkheader );
	}

	CloseHandle( hFile );
	return bRet;
}

/**
 * Loads the index of a packed file and optionally creates mapped pointers using
 * MapViewOfFile for each of the different chunks in the file.  The chunks must be
 * aligned to the proper granularity (64k) or MapViewOfFile will fail.
 */
bool CPackedFile::LoadPackedFile( const string& filename, bool bUseMemoryMappedIO, UINT MaxChunksMapped  )
{
	bool bRet = false;

	// Open the file
	m_hFile = CreateFile( filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
//	m_hFile = CreateFileW( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( INVALID_HANDLE_VALUE == m_hFile )
		return bRet;

	// read the header
	DWORD dwRead;
	if( !ReadFile( m_hFile, &m_FileHeader, sizeof(PACKED_FILE_HEADER), &dwRead, NULL ) )
		goto Error;

	// Make sure the granularity is the same
	SYSTEM_INFO SystemInfo;
	GetSystemInfo( &SystemInfo );
	if( m_FileHeader.Granularity != SystemInfo.dwAllocationGranularity )
		goto Error;

	m_ChunksMapped = 0;
	if( 0 == MaxChunksMapped )
		m_MaxChunksMapped = (UINT)m_FileHeader.NumChunks;
	else
		m_MaxChunksMapped = MaxChunksMapped;

	// Create the chunk and index data
	m_pChunks = new CHUNK_HEADER[ (SIZE_T)m_FileHeader.NumChunks ];
	if( !m_pChunks )
		goto Error;
	m_pFileIndices = new FILE_INDEX[ (SIZE_T)m_FileHeader.NumFiles ];
	if( !m_pFileIndices )
		goto Error;

	// Load the chunk and index data
	if( !ReadFile( m_hFile, m_pChunks, sizeof(CHUNK_HEADER)*(DWORD)m_FileHeader.NumChunks, &dwRead, NULL ) )
		goto Error;
	if( !ReadFile( m_hFile, m_pFileIndices, sizeof(FILE_INDEX)*((DWORD)m_FileHeader.NumFiles), &dwRead, NULL ) )
		goto Error;

	if( bUseMemoryMappedIO )
	{
		m_pMappedChunks = new MAPPED_CHUNK[ (SIZE_T)m_FileHeader.NumChunks ];
		if( !m_pMappedChunks )
			goto Error;

		// Memory map the rest
		m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );
		if( INVALID_HANDLE_VALUE == m_hFile )
			goto Error;

		for( UINT64 i=0; i<m_FileHeader.NumChunks; i++ )
		{
			m_pMappedChunks[i].bInUse = FALSE;
			m_pMappedChunks[i].pMappingPointer = NULL;
			m_pMappedChunks[i].UseCounter = 0;
		}
	}

	bRet = true;
Error:
	return bRet;
}

//--------------------------------------------------------------------------------------
void CPackedFile::UnloadPackedFile()
{
	if( m_pMappedChunks )
	{
		for( UINT i=0; i<m_FileHeader.NumChunks; i++ )
		{
			if( m_pMappedChunks[i].bInUse )
			{
				UnmapViewOfFile( m_pMappedChunks[i].pMappingPointer );
			}
		}
	}

	SafeDeleteArray( m_pMappedChunks );

	if( m_hFileMapping )
		CloseHandle( m_hFileMapping );
	m_hFileMapping = 0;

	if( m_hFile )
		CloseHandle( m_hFile );
	m_hFile = 0;

	SafeDeleteArray( m_pChunks );
	SafeDeleteArray( m_pFileIndices );
}

//--------------------------------------------------------------------------------------
void CPackedFile::EnsureChunkMapped( UINT64 iChunk )
{
	if( !m_pMappedChunks[iChunk].bInUse )
	{
		if( m_ChunksMapped == m_MaxChunksMapped )
		{
			// We need to free a chunk
			UINT lruValue = m_CurrentUseCounter;
			UINT64 lruChunk = (UINT)-1;
			for( UINT64 i=0; i<m_FileHeader.NumChunks; i++ )
			{
				if( m_pMappedChunks[i].bInUse )
				{
					if( lruChunk == (UINT)-1 || m_pMappedChunks[i].UseCounter < lruValue )
					{
						lruValue = m_pMappedChunks[i].UseCounter;
						lruChunk = i;
					}
				}
			}

			UnmapViewOfFile( m_pMappedChunks[lruChunk].pMappingPointer );
			m_pMappedChunks[lruChunk].pMappingPointer = NULL;
			m_pMappedChunks[lruChunk].bInUse = FALSE;
			m_ChunksMapped --;
		}

		// Map this chunk
		DWORD dwOffsetHigh = (DWORD)( (m_pChunks[iChunk].ChunkOffset & 0xFFFFFFFF00000000) >> 32 );
		DWORD dwOffsetLow =  (DWORD)( (m_pChunks[iChunk].ChunkOffset & 0x00000000FFFFFFFF)	   );
		m_pMappedChunks[iChunk].bInUse = TRUE;
		m_pMappedChunks[iChunk].pMappingPointer = MapViewOfFile( m_hFileMapping, FILE_MAP_READ, dwOffsetHigh, dwOffsetLow, (DWORD)m_pChunks[iChunk].ChunkSize );
		m_ChunksMapped ++;
	}

	// Set our use counter for the LRU check
	m_pMappedChunks[iChunk].UseCounter = m_CurrentUseCounter;
	m_CurrentUseCounter ++;
}

//--------------------------------------------------------------------------------------
/*
bool CPackedFile::GetPackedFileInfo( char* szFile, UINT* pDataBytes )
{
	WCHAR str[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, szFile, -1, str, MAX_PATH );

	return GetPackedFileInfo( str, pDataBytes );
}
*/

//--------------------------------------------------------------------------------------
// Finds information about a resource using the index
//--------------------------------------------------------------------------------------
bool CPackedFile::GetPackedFileInfo( const string& filename, UINT* pDataBytes )
{
	// Look the file up in the index
	int iFoundIndex = -1;
	for( UINT i=0; i<m_FileHeader.NumFiles; i++ )
	{
//		if( 0 == wcscmp( szFile, m_pFileIndices[i].szFileName ) )
		if( 0 == strcmp( filename.c_str(), m_pFileIndices[i].szFileName ) )
		{
			iFoundIndex = i;
			break;
		}
	}

	if( -1 == iFoundIndex )
		return false;

	*pDataBytes = (UINT)m_pFileIndices[iFoundIndex].FileSize;
	
	return true;
}

//--------------------------------------------------------------------------------------
/*
bool CPackedFile::GetPackedFile( char* szFile, BYTE** ppData, UINT* pDataBytes )
{
	WCHAR str[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, szFile, -1, str, MAX_PATH );

	return GetPackedFile( str, ppData, pDataBytes );
}
*/


bool CPackedFile::GetPackedFile( const string& filename, vector<unsigned char>& vecDest )
{
	UINT filesize = 0;
	bool res = GetPackedFileInfo( filename, &filesize );
	if( !res )
		return false;

	vecDest.resize( filesize );
	GetPackedFile( filename, 

	return true;
}


/**
 * Finds the location of a resource in a packed file and returns its contents in 
 * *ppData.
 */
bool CPackedFile::GetPackedFile( const string& filename, BYTE** ppData, UINT* pDataBytes )
{
	// Look the file up in the index
	int iFoundIndex = -1;
	for( UINT i=0; i<m_FileHeader.NumFiles; i++ )
	{
//		if( 0 == wcscmp( szFile, m_pFileIndices[i].szFileName ) )
		if( 0 == strcmp( filename.c_str(), m_pFileIndices[i].szFileName ) )
		{
			iFoundIndex = i;
			break;
		}
	}

	if( -1 == iFoundIndex )
		return false;

	*pDataBytes = (UINT)m_pFileIndices[iFoundIndex].FileSize;

	if( m_pMappedChunks )
	{
		// Memory mapped io
		EnsureChunkMapped( m_pFileIndices[iFoundIndex].ChunkIndex );
		*ppData = (BYTE*)m_pMappedChunks[ m_pFileIndices[iFoundIndex].ChunkIndex ].pMappingPointer + m_pFileIndices[iFoundIndex].OffsetIntoChunk;
	}
	else
	{
		// Load it in the old fashioned way
		LARGE_INTEGER liMove;
		liMove.QuadPart = m_pChunks[ m_pFileIndices[iFoundIndex].ChunkIndex ].ChunkOffset;
		liMove.QuadPart += m_pFileIndices[iFoundIndex].OffsetIntoChunk;
		if( !SetFilePointerEx( m_hFile, liMove, NULL, FILE_BEGIN ) )
			return false;

		DWORD dwRead;
		if( !ReadFile( m_hFile, *ppData, *pDataBytes, &dwRead, NULL ) )
			return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------
bool CPackedFile::UsingMemoryMappedIO()
{
	return ( NULL != m_pMappedChunks );
}