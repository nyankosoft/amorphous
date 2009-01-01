
#include "DynamicResourceManager.h"

#include "Graphics/Direct3D9.h"

#include "Support/memory_helpers.h"
#include "Support//TextFileScanner.h"
#include "Support/ContentStreaming/AsyncLoader.h"
#include "Support/ContentStreaming/ContentLoaders.h"


CSingleton<CDynamicResourceManager> CDynamicResourceManager::m_obj;

//--------------------------------------------------------------------------------------
// Async create texture (used as a callback function in original ContentStreaming sample)
//--------------------------------------------------------------------------------------
void /*CALLBACK */CDynamicResourceManager::CreateTextureFromFile_Async( const string& filename,
																		CPackedFile& packfile,
																		IDirect3DTexture9** ppTexture,
																		void* pContext )
{
    CAsyncLoader* pAsyncLoader = m_pAsyncLoader;//(CAsyncLoader*)pContext;
    if( pAsyncLoader )
    {
        CTextureLoader *pLoader = new CTextureLoader( filename.c_str(), &packfile );
		CTextureProcessor *pProcessor = new CTextureProcessor( DIRECT3D9.GetDevice(), ppTexture, m_pResourceReuseCache );

        pAsyncLoader->AddWorkItem( pLoader, pProcessor, NULL, (void**)ppTexture );
    }
}

/*
//--------------------------------------------------------------------------------------
// Async create texture (used as a callback function in original ContentStreaming sample)
//--------------------------------------------------------------------------------------
void CALLBACK CreateTextureFromFile9_Async( IDirect3DDevice9* pDev, char* szFileName, IDirect3DTexture9** ppTexture, void* pContext )
{
    CAsyncLoader* pAsyncLoader = (CAsyncLoader*)pContext;
    if( pAsyncLoader )
    {
        CTextureLoader *pLoader = new CTextureLoader( szFileName, &g_PackFile );
        CTextureProcessor *pProcessor = new CTextureProcessor( pDev, ppTexture, g_pResourceReuseCache );

        pAsyncLoader->AddWorkItem( pLoader, pProcessor, NULL, (void**)ppTexture );
    }
}



void CALLBACK CreateTextureFromFile_Async( CPackedFile& packfile, char* szFileName, CTextureEntity& dest_tex_entity, void* pContext )
{
    CAsyncLoader* pAsyncLoader = (CAsyncLoader*)pContext;

	stat = TexMgr.GetState( filename );
	if( stat == loaded )
	{
	}
	else if( stat == loading )
	{
	}
	else
	{
		// load
		if( pAsyncLoader )
		{
			CTextureLoader *pLoader = new CTextureLoader( szFileName, &packfile );
			CTextureProcessor *pProcessor = new CTextureProcessor( pDev, ppTexture, g_pResourceReuseCache );

			pAsyncLoader->AddWorkItem( pLoader, pProcessor, NULL, (void**)ppTexture );
		}
	}
}*/


CDynamicResourceManager::CDynamicResourceManager()
:
m_pAsyncLoader(NULL),
m_pResourceReuseCache(NULL)
{
	m_NumResourceToLoadPerFrame = 2;
}


CDynamicResourceManager::~CDynamicResourceManager()
{
	SafeDelete( m_pAsyncLoader );
	SafeDelete( m_pResourceReuseCache );
}


void CDynamicResourceManager::OnFrameRender()
{
    // Load in up to m_NumResourceToLoadPerFrame resources at the end of every frame
	m_pAsyncLoader->ProcessDeviceWorkItems( m_NumResourceToLoadPerFrame );
}


void CDynamicResourceManager::Init( const string& ResourceProfileFilename )
{
	m_pAsyncLoader = new CAsyncLoader( 1 );

	m_pResourceReuseCache = new CResourceReuseCache( DIRECT3D9.GetDevice() );

	LoadResourceProfile( ResourceProfileFilename );
}


void CDynamicResourceManager::LoadResourceProfile( const string& filename )
{
	CResourceProfile res_profile;
	if( filename.rfind(".txt") == filename.length() - 4 )
	{
		// load text & update the binary file
		CTextFileScanner scanner( filename );
		if( !scanner.IsReady() )
			return;

		while( !scanner.End() )
		{
			res_profile.LoadFromText( scanner );

			scanner.NextLine();
		}

		res_profile.SaveToFile( "res_profile.bin" );
	}

	// set profile to resource reuse cahce
	m_pResourceReuseCache->LoadProfileFromFile( "res_profile.bin" );

	// load binary
}
