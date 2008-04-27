
#include "ui.h"
#include "GM_Aux.h"


CGM_Dialog *Create2ChoiceDialog( CGM_DialogManager* pDlgMgr, bool root_dialog,
                                 int dlg_id, const string& dlg_title, const SRect& dlg_rect,
                                 int btn1_id, const string& btn1_title, const SRect& btn1_rect,
                                 int btn2_id, const string& btn2_title, const SRect& btn2_rect,
								 int static_id, const std::string& static_title, const SRect& static_rect )
{
	CGM_DialogDesc dlg_desc;
	dlg_desc.ID = dlg_id;
	dlg_desc.Rect = dlg_rect;
	dlg_desc.strTitle = dlg_title;
	dlg_desc.bRootDialog = root_dialog;

	CGM_Dialog *pDialog = pDlgMgr->AddDialog( dlg_desc );

	if( !pDialog )
		return NULL;

	CGM_DialogCloseButtonDesc btn_desc[2];

	btn_desc[0].ID		= btn1_id;
	btn_desc[0].strText	= btn1_title;
	btn_desc[0].Rect	= btn1_rect;

	btn_desc[1].ID		= btn2_id;
	btn_desc[1].strText = btn2_title;
	btn_desc[1].Rect    = btn2_rect;

	pDialog->AddControl( &btn_desc[0] );
	pDialog->AddControl( &btn_desc[1] );

	if( static_title != "" )
	{
		CGM_StaticDesc desc;
		desc.strText = static_title;
		desc.Rect = static_rect;
		pDialog->AddControl( &desc );
	}

	return pDialog;
}


CGM_Dialog *Create2ChoiceDialog( CGM_DialogManager* pDlgMgr, bool root_dialog,
                                 const string& dlg_title, const SRect& dlg_rect,
                                 int btn1_id, const string& btn1_title, const SRect& btn1_rect,
                                 int btn2_id, const string& btn2_title, const SRect& btn2_rect,
								 int static_id, const std::string& static_title, const SRect& static_rect )
{
	return Create2ChoiceDialog( pDlgMgr, root_dialog, 0, dlg_title, dlg_rect,
		btn1_id, btn1_title, btn1_rect,
		btn2_id, btn2_title, btn2_rect,
		static_id, static_title, static_rect );
}


CGM_Dialog *Create2ChoiceDialog( CGM_DialogManagerSharedPtr pDlgMgr, bool root_dialog,
                                 int dlg_id, const string& dlg_title, const SRect& dlg_rect,
                                 int btn1_id, const string& btn1_title, const SRect& btn1_rect,
                                 int btn2_id, const string& btn2_title, const SRect& btn2_rect,
								 int static_id, const std::string& static_title, const SRect& static_rect )
{
	return Create2ChoiceDialog( pDlgMgr.get(), root_dialog, dlg_id, dlg_title, dlg_rect,
		btn1_id, btn1_title, btn1_rect,
		btn2_id, btn2_title, btn2_rect,
		static_id, static_title, static_rect );
}


CGM_Dialog *Create2ChoiceDialog( CGM_DialogManagerSharedPtr pDlgMgr, bool root_dialog,
                                 const string& dlg_title, const SRect& dlg_rect,
                                 int btn1_id, const string& btn1_title, const SRect& btn1_rect,
                                 int btn2_id, const string& btn2_title, const SRect& btn2_rect,
								 int static_id, const std::string& static_title, const SRect& static_rect )
{
	return Create2ChoiceDialog( pDlgMgr.get(), root_dialog, 0, dlg_title, dlg_rect,
		btn1_id, btn1_title, btn1_rect,
		btn2_id, btn2_title, btn2_rect,
		static_id, static_title, static_rect );
}
