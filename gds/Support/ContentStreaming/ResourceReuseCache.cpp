//--------------------------------------------------------------------------------------
// File: ResourceReuseCache.cpp
//
// Illustrates streaming content using Direct3D 9/10
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//#include "DXUT.h"
#include "ResourceReuseCache.h"

#include "../SafeDelete.h"
#include "../TextFileScanner.h"

using namespace std;


//--------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------
#define ISBITMASK( r,g,b,a ) ( ddpf.dwRBitMask == r && ddpf.dwGBitMask == g && ddpf.dwBBitMask == b && ddpf.dwRGBAlphaBitMask == a )


inline UINT GetTextureFormat( const string& fmt )
{
	if( fmt == "R8G8B8" ) return D3DFMT_R8G8B8;
	else if( fmt == "X8R8G8B8" ) return D3DFMT_X8R8G8B8;
	else if( fmt == "A8R8G8B8" ) return D3DFMT_A8R8G8B8;
//	else if( fmt == "" ) return ;
//	else if( fmt == "" ) return ;
//	else if( fmt == "" ) return ;
	else return D3DFMT_A8R8G8B8;
}


void CResourceProfile::LoadFromText( CTextFileScanner& scanner )
{
	int num = 1, w = 1, h = 1, mip_levels = 1;//, fmt = D3DFMT_A8R8G8B8;
	char str[64], fmt[64];
	strcpy( fmt, "A8R8G8B8" );

	if( scanner.TryScanLine( "textures", num, w, h, mip_levels ) )
	{
		sscanf( scanner.GetCurrentLine().c_str(), "%s %d %d %d %d %s", str, &num, &w, &h, &mip_levels, fmt );

		DEVICE_TEXTURE tex;
		memset( &tex, 0, sizeof(DEVICE_TEXTURE) );
		tex.Width = w;
		tex.Height = h;
		tex.MipLevels = mip_levels;
		tex.Format = GetTextureFormat( fmt );

		vecTexture.resize( num, tex );
	}

//	scanner.TryScanLine( "vb",  );
//	scanner.TryScanLine( "ib",  );
}



//--------------------------------------------------------------------------------------
// Texture functions
//--------------------------------------------------------------------------------------
/*
int CResourceReuseCache::FindTexture( ID3D10ShaderResourceView* pRV10 )
{
	for( size_t i=0; i<m_TextureList.size(); i++ )
	{
		DEVICE_TEXTURE* texTest = m_TextureList.at(i);
		if( texTest->pRV10 == pRV10 )
			return i;
	}

	return -1;
}
*/

//--------------------------------------------------------------------------------------
int CResourceReuseCache::FindTexture( IDirect3DTexture9* pTex9 )
{
	for( size_t i=0; i<m_TextureList.size(); i++ )
	{
		DEVICE_TEXTURE* texTest = m_TextureList.at(i);
		if( texTest->pTexture9 == pTex9 )
			return i;
	}

	return -1;
}

//--------------------------------------------------------------------------------------
DXGI_FORMAT ConvertToDXGI_FORMAT( D3DFORMAT d3dformat )
{
	switch( d3dformat )
	{
	case D3DFMT_A32B32G32R32F:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case D3DFMT_A16B16G16R16:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	case D3DFMT_A16B16G16R16F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case D3DFMT_G32R32F:
		return DXGI_FORMAT_R32G32_FLOAT;

	case D3DFMT_R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case D3DFMT_G16R16:
	case D3DFMT_V16U16:
		return DXGI_FORMAT_R16G16_UNORM;

	case D3DFMT_G16R16F:
		return DXGI_FORMAT_R16G16_FLOAT;
	case D3DFMT_R32F:
		return DXGI_FORMAT_R32_FLOAT;

	case D3DFMT_R16F:
		return DXGI_FORMAT_R16_FLOAT;

	case D3DFMT_A8:
		return DXGI_FORMAT_A8_UNORM;
	case D3DFMT_P8:
	case D3DFMT_L8:
		return DXGI_FORMAT_R8_UNORM;

	case D3DFMT_DXT1:
		return DXGI_FORMAT_BC1_UNORM;
	case D3DFMT_DXT2:
		return DXGI_FORMAT_BC1_UNORM;
	case D3DFMT_DXT3:
		return DXGI_FORMAT_BC2_UNORM;
	case D3DFMT_DXT4:
		return DXGI_FORMAT_BC2_UNORM;
	case D3DFMT_DXT5:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	default:
		assert(FALSE); // unhandled format
		return DXGI_FORMAT_UNKNOWN;
	}
}

//--------------------------------------------------------------------------------------
D3DFORMAT ConvertToD3DFORMAT( DXGI_FORMAT dxgiformat )
{
	switch( dxgiformat )
	{

	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return D3DFMT_A32B32G32R32F;

	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return D3DFMT_A16B16G16R16;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return D3DFMT_A16B16G16R16F;
	case DXGI_FORMAT_R32G32_FLOAT:
		return D3DFMT_G32R32F;

	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return D3DFMT_A8R8G8B8;

	case DXGI_FORMAT_R16G16_UNORM:
		return D3DFMT_G16R16;

	case DXGI_FORMAT_R16G16_FLOAT:
		return D3DFMT_G16R16F;
	case DXGI_FORMAT_R32_FLOAT:
		return D3DFMT_R32F;

	case DXGI_FORMAT_R16_FLOAT:
		return D3DFMT_R16F;

	case DXGI_FORMAT_A8_UNORM:
		return D3DFMT_A8;
	case DXGI_FORMAT_R8_UNORM:
		return D3DFMT_L8;

	case DXGI_FORMAT_BC1_UNORM:
		return D3DFMT_DXT1;
	case DXGI_FORMAT_BC2_UNORM:
		return D3DFMT_DXT3;
	case DXGI_FORMAT_BC3_UNORM:
		return D3DFMT_DXT5;

	default:
		assert(FALSE); // unhandled format
		return D3DFMT_UNKNOWN;
	}
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::EnsureFreeTexture( UINT Width, UINT Height, UINT MipLevels, UINT Format )
{
	// see if we have a free one available
	for( size_t i=0; i<m_TextureList.size(); i++ )
	{
		DEVICE_TEXTURE* texTest = m_TextureList.at(i);
		if( !texTest->bInUse )
		{
			if( texTest->Width == Width &&
				texTest->Height == Height &&
				texTest->MipLevels == MipLevels &&
				texTest->Format == Format )
			{
/*				if( LDT_D3D10 == m_Device.Type )
				{
					ID3D10Resource* pRes;
					texTest->pRV10->GetResource( &pRes );	// add a ref for ourselves
					texTest->pRV10->AddRef();	// add a ref for ourselves
				}
				else*/ if( LDT_D3D9 == m_Device.Type )
				{
					texTest->pTexture9->AddRef();	// add a ref for ourselves
				}

				// Found one that matches all criteria
				texTest->bInUse = TRUE;
				return i;
			}
		}
	}

	// haven't found a free one
	// try to create a new one
	UINT64 newSize = GetEstimatedSize( Width, Height, MipLevels, Format );
	if( m_UsedTextureMemory + newSize < m_MaxTextureMemory && !m_bDontCreateResources )
	{
		HRESULT hr = S_OK;
		DEVICE_TEXTURE* tex = new DEVICE_TEXTURE;
		tex->Width = Width;
		tex->Height = Height;
		tex->MipLevels = MipLevels;
		tex->Format = Format;
		tex->EstimatedSize = newSize;
		tex->pTexture9 = NULL;

		if( !m_bSilent )
//			OutputDebugString( L"RESOURCE WARNING: Device needs to create new Texture\n" );
			assert( !"RESOURCE WARNING: Device needs to create new Texture\n" );

/*		if( LDT_D3D10 == m_Device.Type )
		{
			D3D10_TEXTURE2D_DESC desc;
			desc.Width = Width;
			desc.Height = Height;
			desc.MipLevels = MipLevels;
			desc.ArraySize = 1;
			desc.Format = ConvertToDXGI_FORMAT( (D3DFORMAT)Format );
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D10_USAGE_DEFAULT;
			desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			ID3D10Texture2D* pTex2D;
			hr = m_Device.pDev10->CreateTexture2D( &desc, NULL, &pTex2D );

			if( SUCCEEDED(hr) && pTex2D )
			{
				D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
				ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
				SRVDesc.Format = desc.Format;
				SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
				SRVDesc.Texture2D.MipLevels = desc.MipLevels;
				hr = m_Device.pDev10->CreateShaderResourceView( pTex2D, &SRVDesc, &tex->pRV10 );

				pTex2D->AddRef();	// add a ref for ourselves
				tex->pRV10->AddRef();	// add a ref for ourselves
			}
		}
		else*/ if( LDT_D3D9 == m_Device.Type )
		{
			hr = m_Device.pDev9->CreateTexture( Width, 
												Height, 
												MipLevels, 
												0, 
												(D3DFORMAT)Format, 
												D3DPOOL_DEFAULT, 
												&tex->pTexture9, 
												NULL );
			if( SUCCEEDED(hr) && tex->pTexture9 )
			{
				hr = m_Device.pDev9->CreateTexture( Width, 
													Height, 
													MipLevels, 
													0, 
													(D3DFORMAT)Format, 
													D3DPOOL_SYSTEMMEM, 
													&tex->pStagingTexture9, 
													NULL );

				if( SUCCEEDED(hr) && tex->pStagingTexture9 )
				{
					tex->pTexture9->AddRef();	// add a ref for ourselves
				}
				else
				{
					tex->pTexture9->Release();
				}
			}
		}

		if( FAILED(hr) )
		{
			SafeDelete( tex );
			if( !m_bSilent )
//				OutputDebugString( L"RESOURCE ERROR: Cannot Load Texture!\n" );
				assert( !"RESOURCE ERROR: Cannot Load Texture!\n" );
			return -1;
		}

		m_UsedTextureMemory += tex->EstimatedSize;
		tex->bInUse = TRUE;

		int index = m_TextureList.size();
		m_TextureList.push_back(tex);
		return index;
	}

	return -1;
}

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
UINT BitsPerPixel( D3DFORMAT fmt )
{
	switch( fmt )
	{
	case D3DFMT_A32B32G32R32F:
		return 128;

	case D3DFMT_A16B16G16R16:
	case D3DFMT_Q16W16V16U16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_G32R32F:
		return 64;

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_G16R16:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_Q8W8V8U8:
	case D3DFMT_V16U16:
	case D3DFMT_X8L8V8U8:
	case D3DFMT_A2W10V10U10:
	case D3DFMT_D32:
	case D3DFMT_D24S8:
	case D3DFMT_D24X8:
	case D3DFMT_D24X4S4:
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_D24FS8:
	case D3DFMT_INDEX32:
	case D3DFMT_G16R16F:
	case D3DFMT_R32F:
		return 32;

	case D3DFMT_R8G8B8:
		return 24;

	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_R5G6B5:
	case D3DFMT_L16:
	case D3DFMT_A8L8:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_V8U8:
	case D3DFMT_CxV8U8:
	case D3DFMT_L6V5U5:
	case D3DFMT_G8R8_G8B8:
	case D3DFMT_R8G8_B8G8:
	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_D15S1:
	case D3DFMT_D16:
	case D3DFMT_INDEX16:
	case D3DFMT_R16F:
	case D3DFMT_YUY2:
		return 16;

	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_A8P8:
	case D3DFMT_P8:
	case D3DFMT_L8:
	case D3DFMT_A4L4:
		return 8;

	case D3DFMT_DXT1:
		return 4;
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
		return  8;

	default:
		assert(FALSE); // unhandled format
		return 0;
	}
}

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
void GetSurfaceInfo( UINT width, UINT height, D3DFORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
{
	UINT numBytes = 0;
	UINT rowBytes = 0;
	UINT numRows = 0;

	// From the DXSDK docs:
	//
	//	 When computing DXTn compressed sizes for non-square textures, the 
	//	 following formula should be used at each mipmap level:
	//
	//		 max(1, width E4) x max(1, height E4) x 8(DXT1) or 16(DXT2-5)
	//
	//	 The pitch for DXTn formats is different from what was returned in 
	//	 Microsoft DirectX 7.0. It now refers the pitch of a row of blocks. 
	//	 For example, if you have a width of 16, then you will have a pitch 
	//	 of four blocks (4*8 for DXT1, 4*16 for DXT2-5.)"

	if( fmt == D3DFMT_DXT1 || fmt == D3DFMT_DXT2 || fmt == D3DFMT_DXT3 || fmt == D3DFMT_DXT4 || fmt == D3DFMT_DXT5 )
	{
		// Note: we support width and/or height being 0 in order to compute
		// offsets in functions like CBufferLockEntry::CopyBLEToPerfectSizedBuffer().
		int numBlocksWide = 0;
		if( width > 0 )
			numBlocksWide = max( 1, width / 4 );
		int numBlocksHigh = 0;
		if( height > 0 )
			numBlocksHigh = max( 1, height / 4 );
		//int numBlocks = numBlocksWide * numBlocksHigh;
		int numBytesPerBlock = ( fmt == D3DFMT_DXT1 ? 8 : 16 );
		rowBytes = numBlocksWide * numBytesPerBlock;
		numRows = numBlocksHigh;
	}
	else
	{
		UINT bpp = BitsPerPixel( fmt );
		rowBytes = ( width * bpp + 7) / 8; // round up to nearest byte
		numRows = height;
	}
	numBytes = rowBytes * numRows;
	if( pNumBytes != NULL )
		*pNumBytes = numBytes;
	if( pRowBytes != NULL )
		*pRowBytes = rowBytes;
	if( pNumRows != NULL )
		*pNumRows = numRows;
}

//--------------------------------------------------------------------------------------
D3DFORMAT GetD3D9Format( DDPIXELFORMAT ddpf )
{
	if( ddpf.dwFlags & DDPF_RGB )	//rgb codes
									// Only do the more common formats
	{
		if( 32 == ddpf.dwRGBBitCount )
		{
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000) )
				return D3DFMT_A8R8G8B8;
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
				return D3DFMT_X8R8G8B8;
			if( ISBITMASK(0x000000ff,0x00ff0000,0x0000ff00,0xff000000) )
				return D3DFMT_A8B8G8R8;
			if( ISBITMASK(0x000000ff,0x00ff0000,0x0000ff00,0x00000000) )
				return D3DFMT_X8B8G8R8;
			if( ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000) )
				return D3DFMT_R32F;
		}

		if( 24 == ddpf.dwRGBBitCount )
		{
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
				return D3DFMT_R8G8B8;
		}

		if( 16 == ddpf.dwRGBBitCount )
		{
			if( ISBITMASK(0x0000F800,0x000007E0,0x0000001F,0x00000000) )
				return D3DFMT_R5G6B5;
		}
	}
	else if( ddpf.dwFlags & DDPF_LUMINANCE )
	{
		if( 8 == ddpf.dwRGBBitCount )
		{
			return D3DFMT_L8;
		}
	}
	else if( ddpf.dwFlags & DDPF_ALPHA )
	{
		if( 8 == ddpf.dwRGBBitCount )
		{
			return D3DFMT_A8;
		}
	}
	else if( ddpf.dwFlags & DDPF_FOURCC )  //fourcc codes (dxtn)
	{
		if( MAKEFOURCC('D','X','T','1') == ddpf.dwFourCC )
			return D3DFMT_DXT1;
		if( MAKEFOURCC('D','X','T','2') == ddpf.dwFourCC )
			return D3DFMT_DXT2;
		if( MAKEFOURCC('D','X','T','3') == ddpf.dwFourCC )
			return D3DFMT_DXT3;
		if( MAKEFOURCC('D','X','T','4') == ddpf.dwFourCC )
			return D3DFMT_DXT4;
		if( MAKEFOURCC('D','X','T','5') == ddpf.dwFourCC )
			return D3DFMT_DXT5;
	}
	
	return D3DFMT_UNKNOWN;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::GetEstimatedSize( UINT Width, UINT Height, UINT MipLevels, UINT Format )
{
	UINT64 SizeTotal = 0;
	UINT Size = 0;

	while( Width > 0 && Height > 0 && MipLevels > 0 )
	{
		GetSurfaceInfo( Width, Height, (D3DFORMAT)Format, &Size, NULL, NULL );

		SizeTotal += Size;
		Width  = Width >> 1;
		Height = Height >> 1;
		MipLevels --;
	}

	return SizeTotal;
}

//--------------------------------------------------------------------------------------
// Vertex Buffer functions
//--------------------------------------------------------------------------------------
/*
int CResourceReuseCache::FindVB( ID3D10Buffer* pVB )
{
	for( int i=0; i<m_VBList.size(); i++ )
	{
		DEVICE_VERTEX_BUFFER* vb = m_VBList.at(i);
		if( vb->pVB10 == pVB )
			return i;
	}

	return -1;
}
*/

//--------------------------------------------------------------------------------------
int CResourceReuseCache::FindVB( IDirect3DVertexBuffer9* pVB )
{
	for( size_t i=0; i<m_VBList.size(); i++ )
	{
		DEVICE_VERTEX_BUFFER* vb = m_VBList.at(i);
		if( vb->pVB9 == pVB )
			return i;
	}

	return -1;
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::EnsureFreeVB( UINT iSizeBytes )
{
	// Find the closest match
	int closestindex = -1;
	UINT smallestSize = UINT_MAX;
	for( size_t i=0; i<m_VBList.size(); i++ )
	{
		DEVICE_VERTEX_BUFFER* vb = m_VBList.at(i);
		if( !vb->bInUse )
		{
			if( vb->iSizeBytes == iSizeBytes )
			{
				smallestSize = vb->iSizeBytes;
				closestindex = i;
				break;
			}
		}
	}

	// if we found a closest match, return it
	if( -1 != closestindex )
	{
		DEVICE_VERTEX_BUFFER* vb = m_VBList.at(closestindex);

/*		if( LDT_D3D10 == m_Device.Type )
			vb->pVB10->AddRef();	//add a ref for ourselves
		else*/ if( LDT_D3D9 == m_Device.Type )
			vb->pVB9->AddRef();	//add a ref for ourselves

		vb->bInUse = TRUE;
		return closestindex;
	}

	// haven't found a free one
	// try to create a new one
	UINT64 newSize = iSizeBytes;
	if( m_UsedBufferMemory + newSize < m_MaxBufferMemory && !m_bDontCreateResources  )
	{
		HRESULT hr = S_OK;
		DEVICE_VERTEX_BUFFER* vb = new DEVICE_VERTEX_BUFFER;
		vb->iSizeBytes = iSizeBytes;
//		vb->pVB10 = NULL;
		vb->pVB9 = NULL;

		if( !m_bSilent )
//			OutputDebugString( L"RESOURCE WARNING: Device needs to create new Vertex Buffer\n" );
			assert( !"RESOURCE WARNING: Device needs to create new Vertex Buffer\n" );

/*		if( LDT_D3D10 == m_Device.Type )
		{
			D3D10_BUFFER_DESC BufferDesc;
			BufferDesc.ByteWidth = iSizeBytes;
			BufferDesc.Usage = D3D10_USAGE_DEFAULT;
			BufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			BufferDesc.CPUAccessFlags = 0;
			BufferDesc.MiscFlags = 0;
			hr = m_Device.pDev10->CreateBuffer( &BufferDesc, NULL, &vb->pVB10 );
			if( SUCCEEDED(hr) && vb->pVB10 )
				vb->pVB10->AddRef(); // add a ref for us
		}
		else*/ if( LDT_D3D9 == m_Device.Type )
		{
			hr = m_Device.pDev9->CreateVertexBuffer( iSizeBytes,
													 D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
													 0,
													 D3DPOOL_DEFAULT,
													 &vb->pVB9,
													 NULL );
			if( SUCCEEDED(hr) && vb->pVB9 )
				vb->pVB9->AddRef();	// add a ref for ourselves
		}

		if( FAILED(hr) )
		{
			SafeDelete( vb );
			if( !m_bSilent )
//				OutputDebugString( L"RESOURCE ERROR: Cannot Load Vertex Buffer!\n" );
				assert( !"RESOURCE ERROR: Cannot Load Vertex Buffer!\n" );
			return -1;
		}

		m_UsedBufferMemory += vb->iSizeBytes;
		vb->bInUse = TRUE;
		int index = m_VBList.size();
		m_VBList.push_back(vb);
		return index;
	}

	return -1;
}

//--------------------------------------------------------------------------------------
// Index Buffer
//--------------------------------------------------------------------------------------
/*
int CResourceReuseCache::FindIB( ID3D10Buffer* pIB )
{
	for( int i=0; i<m_IBList.size(); i++ )
	{
		DEVICE_INDEX_BUFFER* IB = m_IBList.at(i);
		if( IB->pIB10 == pIB )
			return i;
	}

	return -1;
}
*/

//--------------------------------------------------------------------------------------
int CResourceReuseCache::FindIB( IDirect3DIndexBuffer9* pIB )
{
	for( size_t i=0; i<m_IBList.size(); i++ )
	{
		DEVICE_INDEX_BUFFER* IB = m_IBList.at(i);
		if( IB->pIB9 == pIB )
			return i;
	}

	return -1;
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::EnsureFreeIB( UINT iSizeBytes, UINT ibFormat )
{
	// Find the closest match
	int closestindex = -1;
	UINT smallestSize = UINT_MAX;
	for( int i=0; i<m_IBList.size(); i++ )
	{
		DEVICE_INDEX_BUFFER* IB = m_IBList.at(i);
		if( !IB->bInUse )
		{
			if( 0 == ibFormat )
				ibFormat = IB->ibFormat;
			if( IB->iSizeBytes == iSizeBytes && IB->ibFormat == ibFormat )
			{
				smallestSize = IB->iSizeBytes;
				closestindex = i;
				break;
			}
		}
	}

	// if we found a closest match, return it
	if( -1 != closestindex )
	{
		DEVICE_INDEX_BUFFER* IB = m_IBList.at(closestindex);

/*		if( LDT_D3D10 == m_Device.Type )
			IB->pIB10->AddRef();	//add a ref for ourselves
		else*/ if( LDT_D3D9 == m_Device.Type )
			IB->pIB9->AddRef();	//add a ref for ourselves

		IB->bInUse = TRUE;
		return closestindex;
	}

	// We haven't found a free one, so create a new one
	UINT64 newSize = iSizeBytes;
	if( m_UsedBufferMemory + newSize < m_MaxBufferMemory && !m_bDontCreateResources  )
	{
		HRESULT hr = S_OK;
		DEVICE_INDEX_BUFFER* IB = new DEVICE_INDEX_BUFFER;
		IB->iSizeBytes = iSizeBytes;
		IB->ibFormat = ibFormat;
//		IB->pIB10 = NULL;
		IB->pIB9 = NULL;

		if( !m_bSilent )
//			OutputDebugString( L"RESOURCE WARNING: Device needs to create new Index Buffer\n" );
			assert( !"RESOURCE WARNING: Device needs to create new Index Buffer\n" );

/*		if( LDT_D3D10 == m_Device.Type )
		{
			D3D10_BUFFER_DESC BufferDesc;
			BufferDesc.ByteWidth = iSizeBytes;
			BufferDesc.Usage = D3D10_USAGE_DEFAULT;
			BufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
			BufferDesc.CPUAccessFlags = 0;
			BufferDesc.MiscFlags = 0;
			hr = m_Device.pDev10->CreateBuffer( &BufferDesc, NULL, &IB->pIB10 );
			if( SUCCEEDED(hr) && IB->pIB10 )
				IB->pIB10->AddRef(); // add a ref for us
		}
		else*/ if( LDT_D3D9 == m_Device.Type )
		{
			hr = m_Device.pDev9->CreateIndexBuffer( iSizeBytes,
													 D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
													 (D3DFORMAT)ibFormat,
													 D3DPOOL_DEFAULT,
													 &IB->pIB9,
													 NULL );
			if( SUCCEEDED(hr) && IB->pIB9 )
				IB->pIB9->AddRef();	// add a ref for ourselves
		}

		if( FAILED(hr) )
		{
			SafeDelete( IB );
			if( !m_bSilent )
//				OutputDebugString( L"RESOURCE ERROR: Cannot Load Index Buffer!\n" );
				assert( !"RESOURCE ERROR: Cannot Load Index Buffer!\n" );
			return -1;
		}

		m_UsedBufferMemory += IB->iSizeBytes;
		IB->bInUse = TRUE;
		int index = m_IBList.size();
		m_IBList.push_back(IB);
		return index;
	}

	return -1;
}

//--------------------------------------------------------------------------------------
// publics
//--------------------------------------------------------------------------------------
/*
CResourceReuseCache::CResourceReuseCache( ID3D10Device* pDev ) :
m_MaxTextureMemory(1024*1024*32),
m_UsedTextureMemory(0),
m_MaxBufferMemory(1024*1024*32),
m_UsedBufferMemory(0),
m_Device(pDev),
m_bSilent(FALSE)
{
}
*/

CResourceReuseCache::CResourceReuseCache( LPDIRECT3DDEVICE9 pDev ) :
m_MaxTextureMemory(1024*1024*32),
m_UsedTextureMemory(0),
m_MaxBufferMemory(1024*1024*32),
m_UsedBufferMemory(0),
m_Device(pDev),
m_bSilent(FALSE)
{
	m_bDontCreateResources = false;
}

//--------------------------------------------------------------------------------------
CResourceReuseCache::~CResourceReuseCache()
{
	OnDestroy();
}

//--------------------------------------------------------------------------------------
//BOOL CResourceReuseCache::SaveResourceProfileToFile( WCHAR* szFileName )
BOOL CResourceReuseCache::SaveResourceProfileToFile( const string& filename )
{
	BOOL bRet = FALSE;

	HANDLE hFile;
	hFile = CreateFile( filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if( INVALID_HANDLE_VALUE == hFile )
		return bRet;

	DWORD dwWritten;
	int NumTextures = (int)m_TextureList.size();
	int NumVBs = (int)m_VBList.size();
	int NumIBs = (int)m_IBList.size();

	if( !WriteFile( hFile, &NumTextures, sizeof(int), &dwWritten, NULL ) )
		goto Error;
	if( !WriteFile( hFile, &NumVBs, sizeof(int), &dwWritten, NULL ) )
		goto Error;
	if( !WriteFile( hFile, &NumIBs, sizeof(int), &dwWritten, NULL ) )
		goto Error;
  
	for( int i=0; i<NumTextures; i++ )
	{
		DEVICE_TEXTURE* pData = m_TextureList.at(i);
		if( !WriteFile( hFile, pData, sizeof(DEVICE_TEXTURE), &dwWritten, NULL ) )
			goto Error;
	}

	for( int i=0; i<NumVBs; i++ )
	{
		DEVICE_VERTEX_BUFFER* pData = m_VBList.at(i);
		if( !WriteFile( hFile, pData, sizeof(DEVICE_VERTEX_BUFFER), &dwWritten, NULL ) )
			goto Error;
	}

	for( int i=0; i<NumIBs; i++ )
	{
		DEVICE_INDEX_BUFFER* pData = m_IBList.at(i);
		if( !WriteFile( hFile, pData, sizeof(DEVICE_INDEX_BUFFER), &dwWritten, NULL ) )
			goto Error;
	}

	bRet = TRUE;
Error:
	CloseHandle( hFile );
	return bRet;
}

//--------------------------------------------------------------------------------------
BOOL CResourceReuseCache::LoadResourceProfileFromFile( const string& filename )
{
	BOOL bRet = FALSE;

	HANDLE hFile;
	hFile = CreateFile( filename.c_str(), FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( INVALID_HANDLE_VALUE == hFile )
		return bRet;

	DWORD dwRead;
	int NumTextures;
	int NumVBs;;
	int NumIBs;;

	vector<DEVICE_TEXTURE*>			TextureList;
	vector<DEVICE_VERTEX_BUFFER*>	VBList;
	vector<DEVICE_INDEX_BUFFER*>	IBList;

	if( !ReadFile( hFile, &NumTextures, sizeof(int), &dwRead, NULL ) )
		goto Error;
	if( !ReadFile( hFile, &NumVBs, sizeof(int), &dwRead, NULL ) )
		goto Error;
	if( !ReadFile( hFile, &NumIBs, sizeof(int), &dwRead, NULL ) )
		goto Error;
  
	for( int i=0; i<NumTextures; i++ )
	{
		DEVICE_TEXTURE* pData = new DEVICE_TEXTURE;
		if( !ReadFile( hFile, pData, sizeof(DEVICE_TEXTURE), &dwRead, NULL ) )
			goto Error;

		TextureList.push_back( pData );
	}

	for( int i=0; i<NumVBs; i++ )
	{
		DEVICE_VERTEX_BUFFER* pData = new DEVICE_VERTEX_BUFFER;
		if( !ReadFile( hFile, pData, sizeof(DEVICE_VERTEX_BUFFER), &dwRead, NULL ) )
			goto Error;

		VBList.push_back( pData );
	}

	for( int i=0; i<NumIBs; i++ )
	{
		DEVICE_INDEX_BUFFER* pData = new DEVICE_INDEX_BUFFER;
		if( !ReadFile( hFile, pData, sizeof(DEVICE_INDEX_BUFFER), &dwRead, NULL ) )
			goto Error;

		IBList.push_back( pData );
	}

	// Pre-warm the textures
	m_bSilent = TRUE;
	for( int i=0; i<NumTextures; i++ )
	{
		DEVICE_TEXTURE* pData = TextureList.at(i);
		EnsureFreeTexture( pData->Width, pData->Height, pData->MipLevels, pData->Format );
		SafeDelete( pData );
	}
	for( int i=0; i<NumVBs; i++ )
	{
		DEVICE_VERTEX_BUFFER* pData = VBList.at(i);
		EnsureFreeVB( pData->iSizeBytes );
		SafeDelete( pData );
	}
	for( int i=0; i<NumIBs; i++ )
	{
		DEVICE_INDEX_BUFFER* pData = IBList.at(i);
		EnsureFreeIB( pData->iSizeBytes, pData->ibFormat );
		SafeDelete( pData );
	}
	m_bSilent = FALSE;

	// Set them all ready for use
	for( size_t i=0; i<m_TextureList.size(); i++ )
	{
		DEVICE_TEXTURE* pData = m_TextureList.at(i);
		pData->bInUse = FALSE;

		// Remove a ref that EnsureFreeIB put on
		if( LDT_D3D9 == m_Device.Type )
			pData->pTexture9->Release();
//		if( LDT_D3D10 == m_Device.Type )
//			pData->pRV10->Release();
	}
	for( size_t i=0; i<m_VBList.size(); i++ )
	{
		DEVICE_VERTEX_BUFFER* pData = m_VBList.at(i);
		pData->bInUse = FALSE;

		// Remove a ref that EnsureFreeIB put on
		if( LDT_D3D9 == m_Device.Type )
			pData->pVB9->Release();
//		if( LDT_D3D10 == m_Device.Type )
//			pData->pVB10->Release();
	}
	for( size_t i=0; i<m_IBList.size(); i++ )
	{
		DEVICE_INDEX_BUFFER* pData = m_IBList.at(i);
		pData->bInUse = FALSE;

		// Remove a ref that EnsureFreeIB put on
		if( LDT_D3D9 == m_Device.Type )
			pData->pIB9->Release();
//		if( LDT_D3D10 == m_Device.Type )
//			pData->pIB10->Release();
	}

	bRet = TRUE;
Error:
	CloseHandle( hFile );
	return bRet;
}

//--------------------------------------------------------------------------------------
void CResourceReuseCache::SetMaxTextureMemory( UINT64 MaxMemory )
{
	m_MaxTextureMemory = MaxMemory;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::GetMaxTextureMemory()
{
	return m_MaxTextureMemory;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::GetUsedTextureMemory()
{
	return m_UsedTextureMemory;
}

//--------------------------------------------------------------------------------------
void CResourceReuseCache::SetMaxBufferMemory( UINT64 MaxMemory )
{
	m_MaxBufferMemory = MaxMemory;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::GetMaxBufferMemory()
{
	return m_MaxBufferMemory;
}

//--------------------------------------------------------------------------------------
UINT64 CResourceReuseCache::GetUsedBufferMemory()
{
	return m_UsedBufferMemory;
}

void CResourceReuseCache::SetDontCreateResources( BOOL bDontCreateResources )
{
	m_bDontCreateResources = bDontCreateResources;
}

//--------------------------------------------------------------------------------------
// Texture functions
//--------------------------------------------------------------------------------------
/*
ID3D10ShaderResourceView* CResourceReuseCache::GetFreeTexture10( UINT Width, UINT Height, UINT MipLevels, UINT Format )
{
	int iTex = EnsureFreeTexture( Width, Height, MipLevels, Format );
	if( -1 == iTex )
		return NULL;
	else
		return m_TextureList.at(iTex)->pRV10;
}
*/

//--------------------------------------------------------------------------------------
IDirect3DTexture9* CResourceReuseCache::GetFreeTexture9( UINT Width, UINT Height, UINT MipLevels, UINT Format, IDirect3DTexture9** ppStagingTexture9 )
{
	int iTex = EnsureFreeTexture( Width, Height, MipLevels, Format );
	if( -1 == iTex )
		return NULL;
	else
	{
		*ppStagingTexture9 = m_TextureList.at(iTex)->pStagingTexture9;
		return m_TextureList.at(iTex)->pTexture9;
	}
}

//--------------------------------------------------------------------------------------
/*
ID3D10ShaderResourceView* CResourceReuseCache::LoadDeviceTexture10( DDSURFACEDESC2_32BIT* pSurfDesc9, void* pTexData, UINT TexDataBytes )
{
	if( !pSurfDesc9 )
		return NULL;

	// Get info about the texture
	UINT iWidth = pSurfDesc9->dwWidth;
	UINT iHeight = pSurfDesc9->dwHeight;
	UINT iMipCount = pSurfDesc9->dwMipMapCount;
	if( 0 == iMipCount )
		iMipCount = 1;
	D3DFORMAT fmt = GetD3D9Format( pSurfDesc9->ddpfPixelFormat );

	// find a free texture
	int index = EnsureFreeTexture( iWidth, iHeight, iMipCount, fmt );
	if( -1 == index )
		return NULL;

	DEVICE_TEXTURE* tex = m_TextureList.at(index);
	ID3D10ShaderResourceView* pRV10 = tex->pRV10;
	ID3D10Resource* pTexture10;
	pRV10->GetResource( &pTexture10 );

	// Update subresource
	UINT NumBytes, RowBytes, NumRows;
	BYTE* pSrcBits = (BYTE*)pTexData;

	for( UINT i=0; i<iMipCount; i++ )
	{
		GetSurfaceInfo( iWidth, iHeight, fmt, &NumBytes, &RowBytes, &NumRows );
		m_Device.pDev10->UpdateSubresource( pTexture10, D3D10CalcSubresource( i, 0, iMipCount ), NULL, pSrcBits, RowBytes, 0 );
		pSrcBits += NumBytes;

		iWidth = iWidth >> 1;
		iHeight = iHeight >> 1;
		if( iWidth == 0 )
			iWidth = 1;
		if( iHeight == 0 )
			iHeight = 1;
	}

	SAFE_RELEASE( pTexture10 );
	return tex->pRV10;
}
*/

//--------------------------------------------------------------------------------------
IDirect3DTexture9* CResourceReuseCache::LoadDeviceTexture9( DDSURFACEDESC2_32BIT* pSurfDesc9, void* pTexData, UINT TexDataBytes )
{
	if( !pSurfDesc9 )
		return NULL;

	// Get info about the texture
	UINT iWidth = pSurfDesc9->dwWidth;
	UINT iHeight = pSurfDesc9->dwHeight;
	UINT iMipCount = pSurfDesc9->dwMipMapCount;
	if( 0 == iMipCount )
		iMipCount = 1;
	D3DFORMAT fmt = GetD3D9Format( pSurfDesc9->ddpfPixelFormat );

	// find a free texture
	int index = EnsureFreeTexture( iWidth, iHeight, iMipCount, fmt );
	if( -1 == index )
		return NULL;

	DEVICE_TEXTURE* tex = m_TextureList.at(index);
	IDirect3DTexture9* pTexture = tex->pTexture9;

	D3DLOCKED_RECT LockedRect;

	// Lock, fill, unlock
	UINT RowBytes, NumRows;
	BYTE* pSrcBits = (BYTE*)pTexData;

	for( UINT i=0; i<iMipCount; i++ )
	{
		GetSurfaceInfo( iWidth, iHeight, fmt, NULL, &RowBytes, &NumRows );

		if( SUCCEEDED( pTexture->LockRect( i, &LockedRect, NULL, 0 ) ) )
		{
			BYTE* pDestBits = (BYTE*)LockedRect.pBits;

			// Copy stride line by line
			for( UINT h=0; h<NumRows; h++ )
			{
				CopyMemory( pDestBits, pSrcBits, RowBytes ); 
				pDestBits += LockedRect.Pitch;
				pSrcBits += RowBytes;
			}

			pTexture->UnlockRect( i );
		}

		iWidth = iWidth >> 1;
		iHeight = iHeight >> 1;
		if( iWidth == 0 )
			iWidth = 1;
		if( iHeight == 0 )
			iHeight = 1;
	}

	return tex->pTexture9;
}

//--------------------------------------------------------------------------------------
/*
void CResourceReuseCache::UnuseDeviceTexture10( ID3D10ShaderResourceView* pRV )
{
	int index = FindTexture( pRV );
	if( index >= 0 )
	{
		DEVICE_TEXTURE* tex = m_TextureList.at(index);
		tex->bInUse = FALSE;
	}
}
*/

//--------------------------------------------------------------------------------------
void CResourceReuseCache::UnuseDeviceTexture9( IDirect3DTexture9* pTexture )
{
	int index = FindTexture( pTexture );
	if( index >= 0 )
	{
		DEVICE_TEXTURE* tex = m_TextureList.at(index);
		tex->bInUse = FALSE;
	}
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::GetNumTextures()
{
	return (int)m_TextureList.size();
}

//--------------------------------------------------------------------------------------
DEVICE_TEXTURE* CResourceReuseCache::GetTexture( int i )
{
	return m_TextureList.at(i);
}

//--------------------------------------------------------------------------------------
// Vertex Buffer functions
//--------------------------------------------------------------------------------------
/*
ID3D10Buffer* CResourceReuseCache::GetFreeVB10( UINT sizeBytes )
{
	int iVB = EnsureFreeVB( sizeBytes );
	if( -1 == iVB )
		return NULL;
	else
		return m_VBList.at(iVB)->pVB10;
}
*/

//--------------------------------------------------------------------------------------
IDirect3DVertexBuffer9* CResourceReuseCache::GetFreeVB9( UINT sizeBytes )
{
	int iVB = EnsureFreeVB( sizeBytes );
	if( -1 == iVB )
		return NULL;
	else
		return m_VBList.at(iVB)->pVB9;
}

//--------------------------------------------------------------------------------------
/*
void CResourceReuseCache::UnuseDeviceVB10( ID3D10Buffer* pVB )
{
	int index = FindVB( pVB );
	if( index >= 0 )
	{
		DEVICE_VERTEX_BUFFER* vb = m_VBList.at(index);
		vb->bInUse = FALSE;
	}
}*/

//--------------------------------------------------------------------------------------
void CResourceReuseCache::UnuseDeviceVB9( IDirect3DVertexBuffer9* pVB )
{
	int index = FindVB( pVB );
	if( index >= 0 )
	{
		DEVICE_VERTEX_BUFFER* vb = m_VBList.at(index);
		vb->bInUse = FALSE;
	}
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::GetNumVBs()
{
	return (int)m_VBList.size();
}

//--------------------------------------------------------------------------------------
DEVICE_VERTEX_BUFFER* CResourceReuseCache::GetVB( int i )
{
	return m_VBList.at(i);
}

//--------------------------------------------------------------------------------------
// Index Buffer functions
//--------------------------------------------------------------------------------------
/*
ID3D10Buffer* CResourceReuseCache::GetFreeIB10( UINT sizeBytes, UINT ibFormat )
{
	int iIB = EnsureFreeIB( sizeBytes, ibFormat );
	if( -1 == iIB )
		return NULL;
	else
		return m_IBList.GetAt(iIB)->pIB10;
}
*/

//--------------------------------------------------------------------------------------
IDirect3DIndexBuffer9* CResourceReuseCache::GetFreeIB9( UINT sizeBytes, UINT ibFormat )
{
	int iIB = EnsureFreeIB( sizeBytes, ibFormat );
	if( -1 == iIB )
		return NULL;
	else
		return m_IBList.at(iIB)->pIB9;
}

//--------------------------------------------------------------------------------------
/*
void CResourceReuseCache::UnuseDeviceIB10( ID3D10Buffer* pIB )
{
	int index = FindIB( pIB );
	if( index >= 0 )
	{
		DEVICE_INDEX_BUFFER* IB = m_IBList.at(index);
		IB->bInUse = FALSE;
	}
}
*/

//--------------------------------------------------------------------------------------
void CResourceReuseCache::UnuseDeviceIB9( IDirect3DIndexBuffer9* pIB )
{
	int index = FindIB( pIB );
	if( index >= 0 )
	{
		DEVICE_INDEX_BUFFER* IB = m_IBList.at(index);
		IB->bInUse = FALSE;
	}
}

//--------------------------------------------------------------------------------------
int CResourceReuseCache::GetNumIBs()
{
	return m_IBList.size();
}

//--------------------------------------------------------------------------------------
DEVICE_INDEX_BUFFER* CResourceReuseCache::GetIB( int i )
{
	return m_IBList.at(i);
}

//--------------------------------------------------------------------------------------
void CResourceReuseCache::OnDestroy()
{
	int TexListSize = (int)m_TextureList.size();
	int VBListSize = (int)m_VBList.size();
	int IBListSize = (int)m_IBList.size();

	if( LDT_D3D9 == m_Device.Type )
	{
		for( int i=0; i<TexListSize; i++ )
		{
			DEVICE_TEXTURE* tex = m_TextureList.at(i);
			UINT iCount = tex->pTexture9->Release();
			while( iCount )
				iCount = tex->pTexture9->Release();
			iCount = tex->pStagingTexture9->Release();
			while( iCount )
				iCount = tex->pStagingTexture9->Release();
			SafeDelete( tex );
		}
		for( int i=0; i<VBListSize; i++ )
		{
			DEVICE_VERTEX_BUFFER* vb = m_VBList.at(i);
			UINT iCount = vb->pVB9->Release();
			while( iCount )
				iCount = vb->pVB9->Release();
			SafeDelete( vb );
		}
		for( int i=0; i<IBListSize; i++ )
		{
			DEVICE_INDEX_BUFFER* IB = m_IBList.at(i);
			UINT iCount = IB->pIB9->Release();
			while( iCount )
				iCount = IB->pIB9->Release();
			SafeDelete( IB );
		}
	}
/*	else if( LDT_D3D10 == m_Device.Type )
	{
		for( int i=0; i<TexListSize; i++ )
		{
			DEVICE_TEXTURE* tex = m_TextureList.at(i);
			ID3D10Resource* pRes = NULL;
			tex->pRV10->GetResource( &pRes );
			UINT iCount = tex->pRV10->Release();
			while( iCount )
				iCount = tex->pRV10->Release();
			pRes->Release();
			iCount = pRes->Release();
			while( iCount )
				iCount = pRes->Release();
			SafeDelete( tex );
		}
		for( int i=0; i<VBListSize; i++ )
		{
			DEVICE_VERTEX_BUFFER* vb = m_VBList.at(i);
			UINT iCount = vb->pVB10->Release();
			while( iCount )
				iCount = vb->pVB10->Release();
			SafeDelete( vb );
		}
		for( int i=0; i<IBListSize; i++ )
		{
			DEVICE_INDEX_BUFFER* IB = m_IBList.at(i);
			UINT iCount = IB->pIB10->Release();
			while( iCount )
				iCount = IB->pIB10->Release();
			SafeDelete( IB );
		}
	}*/

//	m_TextureList.RemoveAll();
//	m_VBList.RemoveAll();
//	m_IBList.RemoveAll();
	m_TextureList.resize(0);
	m_VBList.resize(0);
	m_IBList.resize(0);

	m_UsedTextureMemory = 0;
	m_UsedBufferMemory = 0;
}


bool CResourceReuseCache::LoadProfileFromFile( const std::string& filename )
{
	CResourceProfile res_profile;
	res_profile.LoadFromFile( filename );

	DWORD dwRead;
	int NumTextures;
	int NumVBs;;
	int NumIBs;;

	vector<DEVICE_TEXTURE*>			TextureList;
	vector<DEVICE_VERTEX_BUFFER*>	VBList;
	vector<DEVICE_INDEX_BUFFER*>	IBList;
  
	int num_textures = res_profile.vecTexture.size();
	for( int i=0; i<num_textures; i++ )
	{
		DEVICE_TEXTURE* pData = new DEVICE_TEXTURE;
		*pData = res_profile.vecTexture[i];

		TextureList.push_back( pData );
	}

	int num_vbs = res_profile.vecVB.size();
	for( int i=0; i<num_vbs; i++ )
	{
		DEVICE_VERTEX_BUFFER* pData = new DEVICE_VERTEX_BUFFER;
		*pData = res_profile.vecVB[i];

		VBList.push_back( pData );
	}

	int num_ibs = res_profile.vecIB.size();
    for( int i=0; i<num_ibs; i++ )
	{
		DEVICE_INDEX_BUFFER* pData = new DEVICE_INDEX_BUFFER;
		*pData = res_profile.vecIB[i];

		IBList.push_back( pData );
	}

	// Pre-warm the textures
	m_bSilent = TRUE;
	for( int i=0; i<num_textures; i++ )
	{
		DEVICE_TEXTURE* pData = TextureList.at(i);
		EnsureFreeTexture( pData->Width, pData->Height, pData->MipLevels, pData->Format );
		SafeDelete( pData );
	}
	for( int i=0; i<num_vbs; i++ )
	{
		DEVICE_VERTEX_BUFFER* pData = VBList.at(i);
		EnsureFreeVB( pData->iSizeBytes );
		SafeDelete( pData );
	}
	for( int i=0; i<num_ibs; i++ )
	{
		DEVICE_INDEX_BUFFER* pData = IBList.at(i);
		EnsureFreeIB( pData->iSizeBytes, pData->ibFormat );
		SafeDelete( pData );
	}
	m_bSilent = FALSE;

	// Set them all ready for use
	for( size_t i=0; i<m_TextureList.size(); i++ )
	{
		DEVICE_TEXTURE* pData = m_TextureList.at(i);
		pData->bInUse = FALSE;

		// Remove a ref that EnsureFreeIB put on
		if( LDT_D3D9 == m_Device.Type )
			pData->pTexture9->Release();
//		if( LDT_D3D10 == m_Device.Type )
//			pData->pRV10->Release();
	}
	for( size_t i=0; i<m_VBList.size(); i++ )
	{
		DEVICE_VERTEX_BUFFER* pData = m_VBList.at(i);
		pData->bInUse = FALSE;

		// Remove a ref that EnsureFreeIB put on
		if( LDT_D3D9 == m_Device.Type )
			pData->pVB9->Release();
//		if( LDT_D3D10 == m_Device.Type )
//			pData->pVB10->Release();
	}
	for( size_t i=0; i<m_IBList.size(); i++ )
	{
		DEVICE_INDEX_BUFFER* pData = m_IBList.at(i);
		pData->bInUse = FALSE;

		// Remove a ref that EnsureFreeIB put on
		if( LDT_D3D9 == m_Device.Type )
			pData->pIB9->Release();
//		if( LDT_D3D10 == m_Device.Type )
//			pData->pIB10->Release();
	}

	return true;
}

