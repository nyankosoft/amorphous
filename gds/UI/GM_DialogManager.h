#ifndef  __GM_DIALOGMANAGER_H__
#define  __GM_DIALOGMANAGER_H__


#include "ui_fwd.h"
#include "GM_CaptionParameters.h"

#include <vector>
#include <string>


//===========================================================================
// CGM_DialogManager
//===========================================================================

/**
 - Creates dialogs
   - But not controls (buttons, listboxes, etc.).
     Controls are created by dialogs
 */
class CGM_DialogManager
{
	/// owned reference of dialogs
	std::vector<CGM_Dialog *> m_vecpDialog;

	/// borrowed reference of the root dialog
	/// - root dialog is always open
	std::vector<CGM_Dialog *> m_vecpRootDialog;

	/// borrowed reference
	/// - used to calculate layer offset and control rendering order
	std::vector<CGM_Dialog *> m_vecpRootDialogStack;

	std::vector<int> m_vecRootDialogDepthOffset;

	/// The control which currently owns focus
	/// - borrowed reference
	CGM_Control* m_pControlFocus;

	/// The control currently pressed
	/// - borrowed reference
	CGM_Control* m_pControlPressed;

	/// borrowed reference to render manager
//	CGM_ControlRenderManager *m_pRendererManager;
	CGM_ControlRendererManagerSharedPtr m_pRendererManager;

	/// enable / disable caption
	bool m_bCaption;

	/// holds a control to be captioned
	/// - CGM_DialogManager is just holding this and not using
	/// any of its functions
	CGM_Control *m_pControlForCaption;

	/// parameters for displaying caption text
//	CGM_CaptionParameters m_CaptionParam;

	bool m_NeedToUpdateGraphicsProperties;

	CGM_GlobalSoundPlayerSharedPtr m_pDefaultSoundPlayer;

	CGM_ControlRendererSharedPtr m_pCaptionRenderer;

private:

	void Init( CGM_ControlRendererManagerSharedPtr pRendererManager );

	void UpdateGraphicsProperties();

	int GetRootDialogIndex( CGM_Dialog *pDialog );

	void OnDialogClosed( CGM_Dialog *pDialog );

public:

//	void RenderControl( CGM_Control* pControl );

	CGM_DialogManager();

	CGM_DialogManager( CGM_ControlRendererManagerSharedPtr pRendererManager );

	~CGM_DialogManager();

	void Release();

	bool LoadFromXML( const std::string& xml_filepath );

	CGM_Dialog *AddDialog( CGM_DialogDesc &rDialogDesc );

	/// creates a root dialog
	CGM_Dialog *AddRootDialog( int id, const SRect& bound_rect, const std::string& title, int style_flag = 0, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );
//	CGM_Dialog *AddRootDialog( const std::string& string_id, const SRect& bound_rect, const std::string& title, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );

	/// creates a non-root dialog
	CGM_Dialog *AddDialog( int id, const SRect& bound_rect, const std::string& title, int style_flag = 0, CGM_ControlRendererSharedPtr pRenderer = CGM_ControlRendererSharedPtr() );

	CGM_Dialog *GetDialog( int id );

	CGM_Dialog *GetDialog( const std::string& string_id );

	bool OpenRootDialog( int id );

	bool OpenRootDialog( const std::string& string_id );

	CGM_Control* GetControl( int id );

	CGM_Control* GetControl( const std::string& string_id );

	/// \return reference to the control which currently owns focus
	CGM_Control*& ControlFocus() { return m_pControlFocus; }

	/// \return reference to the control currently pressed
	CGM_Control*& ControlPressed() { return m_pControlPressed; }

	/// send input to the currently open root dialog(s)
	/// - returns true if input data is handled by one of the open root dialog(s)
	/// - input data is handled by only one dialog at a time
	bool HandleInput( CGM_InputData& input );

	bool IsCaptionEnabled() const { return m_bCaption; }

	void EnableCaption( bool enable ) { m_bCaption = enable; }

	/// set a control to be captioned
	inline void SetControlForCaption( CGM_Control* pControl );

	/// be careful if you are sharing control renderer manager with more than one dialog manager
	void Render();

	/// needs to be called every frame
	void Update( float dt );

	CGM_ControlRendererManager *GetControlRendererManager() { return m_pRendererManager.get(); }
	CGM_ControlRendererManagerSharedPtr GetControlRendererManagerSharedPtr() { return m_pRendererManager; };

	/// re-scale all the controls and dialogs
	/// Scaling is not applied to CGM_ControlRenderManager linked to the dialog manager
	/// because it may be shared by more than one dialog manager.
	/// The user is responsible for calling CGM_ControlRenderManager::ChangeScale()
	/// to re-scale visual components of UI.
	void ChangeScale( float factor );

	CGM_GlobalSoundPlayerSharedPtr GetDefaultSoundPlayer() { return m_pDefaultSoundPlayer; }

	void OnFocusedControlChanged( CGM_Control* pFocusedControl, CGM_Control* pPrevFocusedControl );

	inline void SetCaptionText( int id, const std::string& caption_text );
	inline void SetCaptionText( const std::string& string_id, const std::string& caption_text );

	friend class CGM_Dialog; /// need to call OnDialogClosed
};


//=============================== inline implementations ===============================


inline void CGM_DialogManager::SetControlForCaption( CGM_Control* pControl )
{
	m_pControlForCaption = pControl;

//	if( m_CaptionParam.pCaptionRenderRoutine )
//		m_CaptionParam.pCaptionRenderRoutine->CaptionControlChanged( pControl );
}


inline void CGM_DialogManager::SetCaptionText( int id, const std::string& caption_text )
{
	CGM_Control *pControl = GetControl(id);
	if( pControl )
		pControl->SetCaptionText( caption_text );
}


inline void CGM_DialogManager::SetCaptionText( const std::string& string_id, const std::string& caption_text )
{
	CGM_Control *pControl = GetControl(string_id);
	if( pControl )
		pControl->SetCaptionText( caption_text );
}


#endif		/*  __GM_DIALOGMANAGER_H__  */
