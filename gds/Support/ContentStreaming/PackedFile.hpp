//--------------------------------------------------------------------------------------
// File: PackedFile.hpp
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef PACKD_FILE_H
#define PACKD_FILE_H

#include <vector>
#include <string>
#include <windows.h>

//--------------------------------------------------------------------------------------
// Packed file structures
//--------------------------------------------------------------------------------------
struct PACKED_FILE_HEADER
{
	UINT64 FileSize;
	UINT64 ChunkSize;
	UINT64 NumChunks;
	UINT64 NumFiles;
	UINT64 Granularity;
};

struct CHUNK_HEADER
{
	UINT64 ChunkOffset;
	UINT64 ChunkSize;
};

struct FILE_INDEX
{
//	enum { MAX_PATH = 260; }

//	WCHAR szFileName[MAX_PATH];
	char szFileName[MAX_PATH];
	UINT64 FileSize;
	UINT64 ChunkIndex;
	UINT64 OffsetIntoChunk;
};

struct LEVEL_ITEM
{
	WCHAR szName[MAX_PATH];
//	D3DXVECTOR3 vScale;
//	D3DXVECTOR3 vCenterOffset;
//	CDXUTSDKMesh Mesh;
};

struct MAPPED_CHUNK
{
	void*	pMappingPointer;
	UINT	UseCounter;
	BOOL	bInUse;
};

//--------------------------------------------------------------------------------------
// CPackedFile class
//--------------------------------------------------------------------------------------
class CPackedFile
{
private:
	PACKED_FILE_HEADER		m_FileHeader;
	CHUNK_HEADER*			m_pChunks;
	FILE_INDEX*				m_pFileIndices;
	MAPPED_CHUNK*			m_pMappedChunks;

	HANDLE					m_hFile;
	HANDLE					m_hFileMapping;
	UINT					m_ChunksMapped;
	UINT					m_MaxChunksMapped;
	UINT					m_CurrentUseCounter;

public:
	CPackedFile();
	~CPackedFile();

	bool CreatePackedFile( //ID3D10Device* pDev10,
		                   //IDirect3DDevice9* pDev9,
						   const std::string& packfilename,//WCHAR* szFileName,
						   std::vector<std::string> src_filename,
						   std::vector<std::string> dest_filename );

//						   LEVEL_ITEM* pLevelItems,
//						   UINT NumLevelItems );

	bool LoadPackedFile( const std::string& filename, bool bUseMemoryMappedIO, UINT MaxChunksMapped );

	void UnloadPackedFile();
	void EnsureChunkMapped( UINT64 iChunk );

//	bool GetPackedFileInfo( char* szFile, UINT* pDataBytes );
//	bool GetPackedFileInfo( WCHAR* szFile, UINT* pDataBytes );
	bool GetPackedFileInfo( const std::string& filename, UINT* pDataBytes );

//	bool GetPackedFile( char* szFile, BYTE** ppData, UINT* pDataBytes );
//	bool GetPackedFile( WCHAR* szFile, BYTE** ppData, UINT* pDataBytes );
	bool GetPackedFile( const std::string& filename, BYTE** ppData, UINT* pDataBytes );

	bool UsingMemoryMappedIO();
};


#endif /* PACKD_FILE_H */
