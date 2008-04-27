
#include "GM_DialogManager.h"

#include "GM_DialogDesc.h"
#include "GM_Dialog.h"

#include "GM_FontDesc.h"
#include "GM_CaptionRenderRoutine.h"

//#include "GameInput/InputHandler.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/font.h"
#include "3DCommon/TextureFont.h"

#include "Support/memory_helpers.h"
//#include "Support/.h"
//#include "Support/.h"

//#include <d3d9.h>
//#include <d3dx9.h>


CGM_DialogManager::CGM_DialogManager()
{
	m_pRootDialog = NULL;


	// caption-related settings
	m_bCaption = false;
	m_pControlForCaption = NULL;
}


CGM_DialogManager::~CGM_DialogManager()
{
	Release();
}


void CGM_DialogManager::Init()
{
}


void CGM_DialogManager::Release()
{
	size_t i;
	for( i=0; i<m_vecpDialog.size(); i++ )
	{
		SafeDelete( m_vecpDialog[i] );
	}
	m_vecpDialog.clear();

	m_pRootDialog = NULL;

	// delete fonts
	for( i=0; i<m_vecpFontCache.size(); i++ )
	{
		SafeDelete( m_vecpFontCache[i] );
	}
	m_vecpFontCache.clear();

	// delete textures
	for( i=0; i<m_vecpTextureCache.size(); i++ )
	{
		SAFE_RELEASE( m_vecpTextureCache[i].pTexture );
	}
	m_vecpTextureCache.clear();

}


CGM_Dialog *CGM_DialogManager::AddDialog( CGM_DialogDesc &rDialogDesc )
{
	CGM_Dialog *pDialog = new CGM_Dialog( this, rDialogDesc );

	if( !pDialog )
		return NULL;


	if( rDialogDesc.bRootDialog )
	{
		m_pRootDialog = pDialog;
		m_pRootDialog->Open();		// the root dialog must be always open
	}

	m_vecpDialog.push_back( pDialog );

	return pDialog;
}


bool CGM_DialogManager::AddFont( int index, CGM_FontDesc& rFontDesc )
{
	if( rFontDesc.width == 0 || rFontDesc.height == 0 || rFontDesc.strFontName.length() == 0 )
		return false;

	int i;
	for( i=m_vecpFontCache.size(); i<=index; i++ )
	{
		m_vecpFontCache.push_back( NULL );
	}

	SafeDelete( m_vecpFontCache[index] );

	// create a font object
	if( rFontDesc.type == CGM_FontDesc::FONT_NORMAL )
	{
		CFont *pFont = new CFont;
		pFont->InitFont( rFontDesc.strFontName.c_str(), rFontDesc.width, rFontDesc.height );
		m_vecpFontCache[index] = pFont;
	}
	else if( rFontDesc.type == CGM_FontDesc::FONT_TEXTURE )
	{
		CTextureFont *pTexFont = new CTextureFont;
		// TODO: support fonts with different texture segments
		pTexFont->InitFont( rFontDesc.strFontName.c_str(), 256, 256, 16, 8 );
		pTexFont->SetFontSize( rFontDesc.width, rFontDesc.height );
		m_vecpFontCache[index] = pTexFont;
	}
	else
		return false;


	return true;
}


void CGM_DialogManager::LoadTextureFromFile( int iIndex, const char *pFilename )
{
	int i;
	for( i=m_vecpTextureCache.size(); i<=iIndex; i++ )
	{
		m_vecpTextureCache.push_back( CGM_TextureNode() );
	}

	SAFE_RELEASE( m_vecpTextureCache[iIndex].pTexture );

	m_vecpTextureCache[iIndex].strFilename = pFilename;

	HRESULT hr;
	hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), pFilename, &(m_vecpTextureCache[iIndex].pTexture) );
	if( FAILED( hr ) )
        m_vecpTextureCache[iIndex].pTexture = NULL;

}


bool CGM_DialogManager::HandleInput( SInputData& input )
{
	if( !m_pRootDialog )
		return false;

	// currently, only the mouse input is supported
	return m_pRootDialog->HandleInput( input );
}


void CGM_DialogManager::Render( float fElapsedTime )
{
	if( m_pRootDialog )
		m_pRootDialog->Render( fElapsedTime );


	// display caption
	if( m_pControlForCaption )
	{
		if( m_CaptionParam.pCaptionRenderRoutine )
		{
			// render the caption using the routine defined by the user
			m_CaptionParam.pCaptionRenderRoutine->Render( fElapsedTime,
			                                              m_pControlForCaption->GetCaptionText(),
														  m_CaptionParam );
		}
		else
		{
			// render caption using the standard render routine

			// render background rect for caption text
//			m_CaptionParam.Rect.Draw( pDialog );

			// render caption text
			D3DXVECTOR2 vPos = D3DXVECTOR2( m_CaptionParam.fPosX, m_CaptionParam.fPosY );

			CFontBase *pFont = GetFont( m_CaptionParam.iFontIndex );
			if( pFont )
			{
				pFont->DrawText( m_pControlForCaption->GetCaptionText().c_str(),
									vPos,
									m_CaptionParam.FontColor.Get32BitARGBColor() );
			}
		}
	}
}