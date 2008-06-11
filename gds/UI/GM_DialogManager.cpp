
#include "GM_DialogManager.h"
#include "GM_DialogDesc.h"
#include "GM_Dialog.h"
#include "GM_ControlRenderer.h"
#include "GM_ControlRendererManager.h"
#include "GM_StdControlRendererManager.h"
#include "GM_SoundPlayer.h"
#include "GM_XMLParser.h"

#include "Support/memory_helpers.h"

#include <algorithm>
using namespace std;


CGM_DialogManager::CGM_DialogManager()
{
	CGM_ControlRendererManagerSharedPtr pRendererMgr( new CGM_StdControlRendererManager() );
	Init( pRendererMgr );

	m_NeedToUpdateGraphicsProperties = true;

	m_pDefaultSoundPlayer = CGM_GlobalSoundPlayerSharedPtr( new CGM_GlobalSoundPlayer() );
}


CGM_DialogManager::CGM_DialogManager( CGM_ControlRendererManagerSharedPtr pRendererManager )
{
	Init( pRendererManager );

	m_NeedToUpdateGraphicsProperties = true;

	m_pDefaultSoundPlayer = CGM_GlobalSoundPlayerSharedPtr( new CGM_GlobalSoundPlayer() );
}


CGM_DialogManager::~CGM_DialogManager()
{
	Release();
}


void CGM_DialogManager::Init( CGM_ControlRendererManagerSharedPtr pRendererManager )
{
//	m_vecpRootDialog = NULL;

	m_pControlFocus = NULL;
	m_pControlPressed = NULL;

	// caption-related settings
	m_bCaption = false;
	m_pControlForCaption = NULL;

	m_pRendererManager = pRendererManager;

	if( m_pRendererManager )
	{
		// init caption renderer
		m_pCaptionRenderer = CGM_ControlRendererSharedPtr( m_pRendererManager->CreateCaptionRenderer() );
		if( m_pCaptionRenderer )
		{
			m_pCaptionRenderer->SetControlRendererManager( GetControlRendererManager() );
			m_pCaptionRenderer->SetControl( NULL ); // caption renderer is not bound to any control
			m_pCaptionRenderer->Init();
		}
	}
}


void CGM_DialogManager::Release()
{
	SafeDeleteVector( m_vecpDialog );

	m_vecpRootDialog.clear();
}


bool CGM_DialogManager::LoadFromXML( const std::string& xml_filepath )
{
	CGM_XMLParser parser( this );
	return parser.LoadFromXMLFile( xml_filepath );
}


CGM_Dialog *CGM_DialogManager::AddDialog( CGM_DialogDesc &rDialogDesc )
{
	CGM_Dialog *pDialog = new CGM_Dialog( this, rDialogDesc );

	if( !pDialog )
		return NULL;

	CGM_ControlRendererSharedPtr pRenderer;

	if( rDialogDesc.pRenderer.get() )
		pRenderer = rDialogDesc.pRenderer;
	else
		pRenderer = CGM_ControlRendererSharedPtr( m_pRendererManager->CreateDialogRenderer( pDialog ) );


	if( rDialogDesc.bRootDialog )
	{
		m_vecpRootDialog.push_back( pDialog );
//		m_vecpRootDialog.back()->Open();		// the root dialog must be always open

		// add to the stack of root dialogs
		m_vecpRootDialogStack.push_back( pDialog );
	}
	// 11:47 2007-12-21 - changed: root dialogs also need to be opened manually

	// set sound player
	if( rDialogDesc.pSoundPlayer.get() )
		pDialog->SetSoundPlayer( rDialogDesc.pSoundPlayer );
	else
		pDialog->SetSoundPlayer( GetDefaultSoundPlayer()->GetDialogSoundPlayer() );

	// add the owned reference of dialog to the dialog list
	m_vecpDialog.push_back( pDialog );

	// set default depth offset (0)
	m_vecRootDialogDepthOffset.push_back( 0 );


	// init control renderer
	if( pRenderer.get() )
	{
		pDialog->SetRendererSharedPtr( pRenderer );
		pRenderer->SetControlRendererManager( m_pRendererManager.get() );
		pRenderer->SetControl( pDialog );
		pRenderer->Init();
	}

	m_NeedToUpdateGraphicsProperties = true;

	return pDialog;
}


CGM_Dialog *CGM_DialogManager::AddRootDialog( int id,
										      const SRect& bound_rect,
										      const std::string& title,
											  int style_flag,
										      CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_DialogDesc desc;

	desc.bRootDialog = true;
	desc.ID          = id;
	desc.strTitle    = title;
	desc.StyleFlag   = style_flag;
	desc.Rect        = bound_rect;

	return AddDialog( desc );
}


CGM_Dialog *CGM_DialogManager::AddDialog( int id,
										  const SRect& bound_rect,
										  const std::string& title,
										  int style_flag,
										  CGM_ControlRendererSharedPtr pRenderer )
{
	CGM_DialogDesc desc;

	desc.bRootDialog = false;
	desc.ID          = id;
	desc.strTitle    = title;
	desc.StyleFlag   = style_flag;
	desc.Rect        = bound_rect;

	return AddDialog( desc );
}


CGM_Dialog *CGM_DialogManager::GetDialog( int id )
{
	size_t i, num_dlgs = m_vecpDialog.size();
	for( i=0; i<num_dlgs; i++ )
	{
		if( m_vecpDialog[i]->GetID() == id )
			return m_vecpDialog[i];
	}

	return NULL;
}


CGM_Dialog *CGM_DialogManager::GetDialog( const std::string& string_id )
{
	size_t i, num_dlgs = m_vecpDialog.size();
	for( i=0; i<num_dlgs; i++ )
	{
		if( m_vecpDialog[i]->GetStringID() == string_id )
			return m_vecpDialog[i];
	}

	return NULL;
}


int CGM_DialogManager::GetRootDialogIndex( CGM_Dialog *pDialog )
{
	int i, num_dlgs = (int)m_vecpDialog.size();
	for( i=0; i<num_dlgs; i++ )
	{
		if( m_vecpDialog[i] == pDialog )
			return i;
	}

	return -1;
}


void CGM_DialogManager::OnDialogClosed( CGM_Dialog *pDialog )
{
	if( pDialog->IsRoot() )
	{
		// put the closed root dialog to the bottom of the stack
		// so that all the other dialogs are rendered over it
		vector<CGM_Dialog *>::iterator itrRootDlg;
		for( itrRootDlg = m_vecpRootDialogStack.begin();
			 itrRootDlg != m_vecpRootDialogStack.end();
			 itrRootDlg++ )
		{
			if( (*itrRootDlg)->GetID() == pDialog->GetID() )
			{
				m_vecpRootDialogStack.erase( itrRootDlg );
				m_vecpRootDialogStack.insert( m_vecpRootDialogStack.begin(), pDialog );
				break;
			}
		}

		if( !ControlFocus() )
		{
			// No control is focused right now
			// - Let's see if there is any control that should be having the focus
			CGM_Dialog *pPrevDlg = m_vecpRootDialogStack.back();
			if( pPrevDlg->IsOpen() ) // make sure this is not a closed root dialog itself (Consider the case when there is only one dialog).
			{
				pPrevDlg->SetFocusOnLastFocusedControl();
			}
		}
	}
}

bool CGM_DialogManager::OpenRootDialog( int id )
{
	CGM_Dialog *pDialog = GetDialog( id );

	if( pDialog && pDialog->IsRoot() )
	{
		if( ControlFocus() )
		{
			// update the depth offset for the root dialog currently being opened
			m_vecRootDialogDepthOffset[ GetRootDialogIndex(pDialog) ]
			= ControlFocus()->GetOwnerDialog()->GetMaxDepth();
		}

		// remove the dialog from the stack and push it on top
		vector<CGM_Dialog *>::iterator itrDlgOnStack;
		for( itrDlgOnStack = m_vecpRootDialogStack.begin();
			 itrDlgOnStack != m_vecpRootDialogStack.end();
			 itrDlgOnStack++ )
		{
			if( (*itrDlgOnStack)->GetID() == id )
			{
				m_vecpRootDialogStack.erase( itrDlgOnStack );
				m_vecpRootDialogStack.push_back( pDialog );
				break;
			}
		}

		// update graphics properties since dialogs access
		// graphics elements when opened
		m_NeedToUpdateGraphicsProperties = true;
		UpdateGraphicsProperties();

		pDialog->Open();

		// set focus
		// - notice the difference with CGM_SubDialogButton,
		//   which sets focus after the button is released
		//   in CGM_SubDialogButton::OnReleased()
		pDialog->SetFocusOnLastFocusedControl();

		return true;
	}
	else
		return false;
}


bool CGM_DialogManager::OpenRootDialog( const std::string& string_id )
{
	CGM_Dialog *pDlg = GetDialog(string_id);
	if( pDlg )
		return OpenRootDialog( pDlg->GetID() );
	else
		return false;
}


CGM_Control* CGM_DialogManager::GetControl( int id )
{
	size_t i, num_dlgs = m_vecpDialog.size();
	for( i=0; i<num_dlgs; i++ )
	{
		CGM_Control* pControl = m_vecpDialog[i]->GetControl( id );
		if( pControl )
			return pControl;
	}

	return NULL;
}


CGM_Control* CGM_DialogManager::GetControl( const std::string& string_id )
{
	size_t i, num_dlgs = m_vecpDialog.size();
	for( i=0; i<num_dlgs; i++ )
	{
		CGM_Control* pControl = m_vecpDialog[i]->GetControl( string_id );
		if( pControl )
			return pControl;
	}

	return NULL;
}


bool CGM_DialogManager::HandleInput( CGM_InputData& input )
{
	if( m_vecpRootDialog.size() == 0 )
		return false;

	bool input_handled = false;

	// find dialog which has a control with the focus
	if( ControlFocus() )
		input_handled = ControlFocus()->GetOwnerDialog()->HandleInput( input );

	if( input_handled )
		return true;

	size_t i, num_root_dialogs = m_vecpRootDialog.size();
	for( i=0; i<num_root_dialogs; i++ )
	{
		if( !m_vecpRootDialog[i]->IsOpen() )
			continue;

		input_handled = m_vecpRootDialog[i]->HandleInput( input );

		if( input_handled )
			return true;
	}

	return false;
}


void CGM_DialogManager::UpdateGraphicsProperties()
{
	if( !m_NeedToUpdateGraphicsProperties )
		return;

	m_NeedToUpdateGraphicsProperties = false;

	size_t i, num_root_dialogs = m_vecpRootDialog.size();
	if( num_root_dialogs == 0 )
		return;

	// set depth to controls
	// - used to calc graphics layer of each control
	for( i=0; i<num_root_dialogs; i++ )
		m_vecpRootDialog[i]->SetDepth( m_vecRootDialogDepthOffset[i] );

	// set the depth to each controls
	// - start from the root dialog at the bottom of the stack
	// - depth offset is given to root dialogs to control their rendering orders
	int depth_offset = 0;
	for( i=0; i<num_root_dialogs; i++ )
	{
		// calc depth without offset
		m_vecpRootDialogStack[i]->SetDepth( 0 );
		int orig_depth = m_vecpRootDialogStack[i]->GetMaxDepth();

		// set depth with offset
		m_vecpRootDialogStack[i]->SetDepth( depth_offset );

		// update depth offset
		// - need to add 1 to orig_depth to prevent overlaps of graphics layers
		depth_offset += ( orig_depth + 1 );
	}

	// - place graphics elements to graphics layers to
	//   set the rendering orders
	// - group elements
	for( i=0; i<num_root_dialogs; i++ )
		m_vecpRootDialog[i]->UpdateGraphicsProperties();

//	if( m_pRendererManager.get() )
//		m_pRendererManager->UpdateGraphicsLayers();
}


void CGM_DialogManager::Render()
{
	if( m_NeedToUpdateGraphicsProperties )
		UpdateGraphicsProperties();

	if( m_pRendererManager.get() )
		m_pRendererManager->Render();
}


void CGM_DialogManager::Update( float dt )
{
	if( m_pRendererManager.get() )
		m_pRendererManager->Update( dt );
}


void CGM_DialogManager::ChangeScale( float factor )
{
	size_t i, num_root_dialogs = m_vecpRootDialog.size();
	for( i=0; i<num_root_dialogs; i++ )
		m_vecpRootDialog[i]->ChangeScale( factor );
}


void CGM_DialogManager::OnFocusedControlChanged( CGM_Control* pFocusedControl, CGM_Control* pPrevFocusedControl )
{
	if( m_pCaptionRenderer )
		m_pCaptionRenderer->OnFocusedControlChanged( pFocusedControl, pPrevFocusedControl );
}



/*
//	// display caption
//	if( IsCaptionEnabled() && m_pControlForCaption )
//	{
////		m_pRendererManager->RenderCaption( m_pControlForCaption->GetCaptionText() );
//	}
//		if( m_CaptionParam.pCaptionRenderRoutine )
//		{
//			// render the caption using the routine defined by the user
//			m_CaptionParam.pCaptionRenderRoutine->Render( fElapsedTime,
//			                                              m_pControlForCaption->GetCaptionText(),
//														  m_CaptionParam );
//		}*/
/*		else
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
									m_CaptionParam.FontColor.GetARGB32() );
			}
		}*/