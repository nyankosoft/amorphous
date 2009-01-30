//--------------------------------------------------------------------------------------
// File: ResourceReuseCache.hpp
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef RESOURCE_REUSE_CACHE_H
#define RESOURCE_REUSE_CACHE_H

//#include "DXUTmisc.h"
// Standard Windows includes
#include <windows.h>
#include <assert.h>
#include <shlobj.h>
#include <math.h>      
#include <limits.h>      
#include <stdio.h>

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

// Direct3D9 includes
#include <d3d9.h>
#include <d3dx9.h>

// Direct3D10 includes
#include <dxgi.h>
//#include <d3d10.h>
//#include <d3dx10.h>

// HRESULT translation for Direct3D10 and other APIs 
#include <dxerr.h>
#include "ddraw.h"

#include <vector>
#include <string>


#include "Support/Serialization/Serialization.hpp"
using namespace GameLib1::Serialization;


//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define E_TRYAGAIN MAKE_HRESULT(1,FACILITY_WIN32,123456)

enum LOADER_DEVICE_TYPE
{
	LDT_D3D10 = 0x0,
	LDT_D3D9,
};

//--------------------------------------------------------------------------------------
// structures
//--------------------------------------------------------------------------------------
typedef struct _DDSURFACEDESC2_32BIT
{
	DWORD			   dwSize;				 // size of the DDSURFACEDESC structure
	DWORD			   dwFlags;				// determines what fields are valid
	DWORD			   dwHeight;			   // height of surface to be created
	DWORD			   dwWidth;				// width of input surface
	union
	{
		LONG			lPitch;				 // distance to start of next line (return value only)
		DWORD		   dwLinearSize;		   // Formless late-allocated optimized surface size
	} DUMMYUNIONNAMEN(1);
	union
	{
		DWORD		   dwBackBufferCount;	  // number of back buffers requested
		DWORD		   dwDepth;				// the depth if this is a volume texture 
	} DUMMYUNIONNAMEN(5);
	union
	{
		DWORD		   dwMipMapCount;		  // number of mip-map levels requestde
												// dwZBufferBitDepth removed, use ddpfPixelFormat one instead
		DWORD		   dwRefreshRate;		  // refresh rate (used when display mode is described)
		DWORD		   dwSrcVBHandle;		  // The source used in VB::Optimize
	} DUMMYUNIONNAMEN(2);
	DWORD			   dwAlphaBitDepth;		// depth of alpha buffer requested
	DWORD			   dwReserved;			 // reserved
	DWORD			   lpSurface32;			// this ptr isn't 64bit safe in the ddraw.h header
	union
	{
		DDCOLORKEY	  ddckCKDestOverlay;	  // color key for destination overlay use
		DWORD		   dwEmptyFaceColor;	   // Physical color for empty cubemap faces
	} DUMMYUNIONNAMEN(3);
	DDCOLORKEY		  ddckCKDestBlt;		  // color key for destination blt use
	DDCOLORKEY		  ddckCKSrcOverlay;	   // color key for source overlay use
	DDCOLORKEY		  ddckCKSrcBlt;		   // color key for source blt use
	union
	{
		DDPIXELFORMAT   ddpfPixelFormat;		// pixel format description of the surface
		DWORD		   dwFVF;				  // vertex format description of vertex buffers
	} DUMMYUNIONNAMEN(4);
	DDSCAPS2			ddsCaps;				// direct draw surface capabilities
	DWORD			   dwTextureStage;		 // stage in multitexture cascade
} DDSURFACEDESC2_32BIT;

struct LOADER_DEVICE
{
	LOADER_DEVICE_TYPE Type;
	union
	{
		LPDIRECT3DDEVICE9 pDev9;
//		ID3D10Device*	 pDev10;
	};

	LOADER_DEVICE() {}
//	LOADER_DEVICE( ID3D10Device* pDevice ) { pDev10 = pDevice; Type = LDT_D3D10; }
	LOADER_DEVICE( IDirect3DDevice9* pDevice ) { pDev9 = pDevice; Type = LDT_D3D9; }
};

struct DEVICE_TEXTURE
{
	UINT Width;
	UINT Height;
	UINT MipLevels;
	UINT Format;
	union
	{
//		ID3D10ShaderResourceView* pRV10;
		IDirect3DTexture9*		pTexture9;
		UINT64					Align64bit;

	};
	union
	{
		IDirect3DTexture9*		pStagingTexture9;
		UINT64					Align64bit2;
	};
	
	UINT64 EstimatedSize;
	BOOL bInUse;
};

struct DEVICE_VERTEX_BUFFER
{
	UINT iSizeBytes;

	union
	{
//		ID3D10Buffer*			pVB10;
		IDirect3DVertexBuffer9*  pVB9;
		UINT64				   Align64bit;
	};
	
	BOOL bInUse;
};

struct DEVICE_INDEX_BUFFER
{
	UINT iSizeBytes;
	UINT ibFormat;

	union
	{
//		ID3D10Buffer*			pIB10;
		IDirect3DIndexBuffer9*   pIB9;
		UINT64				   Align64bit;
	};
	
	BOOL bInUse;
};


inline IArchive& operator & ( IArchive& ar, DEVICE_TEXTURE& tex )
{
	ar & tex.Width & tex.Height & tex.MipLevels & tex.Format;
	return ar;
}

inline IArchive& operator & ( IArchive& ar, DEVICE_VERTEX_BUFFER& vb )
{
	ar & vb.iSizeBytes;
	return ar;
}

inline IArchive& operator & ( IArchive& ar, DEVICE_INDEX_BUFFER& ib )
{
	ar & ib.iSizeBytes & ib.ibFormat;
	return ar;
}

class CTextFileScanner;

class CResourceProfile : public IArchiveObjectBase
{
public:
	std::vector<DEVICE_TEXTURE> vecTexture;
	std::vector<DEVICE_VERTEX_BUFFER> vecVB;
	std::vector<DEVICE_INDEX_BUFFER> vecIB;

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & vecTexture & vecVB & vecIB;
	}

	void LoadFromText( CTextFileScanner& scanner );
};


//--------------------------------------------------------------------------------------
// CResourceReuseCache class
//--------------------------------------------------------------------------------------
class CResourceReuseCache
{
private:
	LOADER_DEVICE							m_Device;
	std::vector<DEVICE_TEXTURE*>			m_TextureList;
	std::vector<DEVICE_VERTEX_BUFFER*>	m_VBList;
	std::vector<DEVICE_INDEX_BUFFER*>	m_IBList;
	UINT64									m_MaxTextureMemory;
	UINT64									m_UsedTextureMemory;
	UINT64									m_MaxBufferMemory;
	UINT64									m_UsedBufferMemory;
	BOOL									m_bSilent;
	BOOL									m_bDontCreateResources;

//	int FindTexture( ID3D10ShaderResourceView* pRV10 );
	int FindTexture( IDirect3DTexture9* pTex9 );
	int EnsureFreeTexture( UINT Width, UINT Height, UINT MipLevels, UINT Format );
	UINT64 GetEstimatedSize( UINT Width, UINT Height, UINT MipLevels, UINT Format );

//	int FindVB( ID3D10Buffer* pVB );
	int FindVB( IDirect3DVertexBuffer9* pVB );
	int EnsureFreeVB( UINT iSizeBytes );

//	int FindIB( ID3D10Buffer* pIB );
	int FindIB( IDirect3DIndexBuffer9* pIB );
	int EnsureFreeIB( UINT iSizeBytes, UINT ibFormat );

public:
//	CResourceReuseCache( ID3D10Device* pDev );
	CResourceReuseCache( LPDIRECT3DDEVICE9 pDev );
	~CResourceReuseCache();

	// profile handling
//	BOOL SaveResourceProfileToFile( WCHAR* szFileName );
//	BOOL LoadResourceProfileFromFile( WCHAR* szFileName );
	BOOL SaveResourceProfileToFile( const std::string& filename );
	BOOL LoadResourceProfileFromFile( const std::string& filename );

	// memory handling
	void SetMaxTextureMemory( UINT64 MaxMemory );
	UINT64 GetMaxTextureMemory();
	UINT64 GetUsedTextureMemory();
	void SetMaxBufferMemory( UINT64 MaxMemory );
	UINT64 GetMaxBufferMemory();
	UINT64 GetUsedBufferMemory();
	void SetDontCreateResources( BOOL bDontCreateResources );

	// texture functions
//	ID3D10ShaderResourceView* GetFreeTexture10( UINT Width, UINT Height, UINT MipLevels, UINT Format );
	IDirect3DTexture9* GetFreeTexture9( UINT Width, UINT Height, UINT MipLevels, UINT Format, IDirect3DTexture9** ppStagingTexture9 );
//	ID3D10ShaderResourceView* LoadDeviceTexture10( DDSURFACEDESC2_32BIT* pSurfDesc9, void* pTexData, UINT TexDataBytes );
	IDirect3DTexture9* LoadDeviceTexture9( DDSURFACEDESC2_32BIT* pSurfDesc9, void* pTexData, UINT TexDataBytes );
//	void UnuseDeviceTexture10( ID3D10ShaderResourceView* pRV );
	void UnuseDeviceTexture9( IDirect3DTexture9* pTexture );
	int GetNumTextures();
	DEVICE_TEXTURE* GetTexture( int i );

	// vertex buffer functions
//	ID3D10Buffer* GetFreeVB10( UINT sizeBytes );
	IDirect3DVertexBuffer9* GetFreeVB9( UINT sizeBytes );
//	ID3D10Buffer* LoadDeviceVB10( void* pData, UINT sizeBytes );
	IDirect3DVertexBuffer9* LoadDeviceVB9( void* pData, UINT sizeBytes );
//	void UnuseDeviceVB10( ID3D10Buffer* pVB );
	void UnuseDeviceVB9( IDirect3DVertexBuffer9* pVB );
	int GetNumVBs();
	DEVICE_VERTEX_BUFFER* GetVB( int i );

	// index buffer functions
//	ID3D10Buffer* GetFreeIB10( UINT sizeBytes, UINT ibFormat );
	IDirect3DIndexBuffer9* GetFreeIB9( UINT sizeBytes, UINT ibFormat );
//	ID3D10Buffer* LoadDeviceIB10( void* pData, UINT sizeBytes, UINT ibFormat );
	IDirect3DIndexBuffer9* LoadDeviceIB9( void* pData, UINT sizeBytes, UINT ibFormat );
//	void UnuseDeviceIB10( ID3D10Buffer* pVB );
	void UnuseDeviceIB9( IDirect3DIndexBuffer9* pVB );
	int GetNumIBs();
	DEVICE_INDEX_BUFFER* GetIB( int i );

	void OnDestroy();

	bool LoadProfileFromFile( const std::string& filename );
};

D3DFORMAT GetD3D9Format( DDPIXELFORMAT ddpf );
void GetSurfaceInfo( UINT width, UINT height, D3DFORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows );

#endif