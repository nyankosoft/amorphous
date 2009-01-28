
#ifndef  __GM_AUXILIARY_H__
#define  __GM_AUXILIARY_H__

#include <string>

#include "Graphics/Rect.h"
using namespace Graphics;

#include "fwd.h"


extern CGM_Dialog *Create2ChoiceDialog( CGM_DialogManager* pDlgMgr, bool root_dialog,
									    int dlg_id, const std::string& dlg_title, const SRect& dlg_rect,
									    int btn1_id, const std::string& btn1_title, const SRect& btn1_rect,
									    int btn2_id, const std::string& btn2_title, const SRect& btn2_rect,
									    int static_id = 0, const std::string& static_title = "", const SRect& static_rect = SRect(0,0,0,0) );

extern CGM_Dialog *Create2ChoiceDialog( CGM_DialogManager* pDlgMgr, bool root_dialog,
									    const std::string& dlg_title, const SRect& dlg_rect,
									    int btn1_id, const std::string& btn1_title, const SRect& btn1_rect,
									    int btn2_id, const std::string& btn2_title, const SRect& btn2_rect,
									    int static_id = 0, const std::string& static_title = "", const SRect& static_rect = SRect(0,0,0,0) );


extern CGM_Dialog *Create2ChoiceDialog( CGM_DialogManagerSharedPtr pDlgMgr, bool root_dialog,
									    int dlg_id, const std::string& dlg_title, const SRect& dlg_rect,
									    int btn1_id, const std::string& btn1_title, const SRect& btn1_rect,
									    int btn2_id, const std::string& btn2_title, const SRect& btn2_rect,
									    int static_id = 0, const std::string& static_title = "", const SRect& static_rect = SRect(0,0,0,0) );

extern CGM_Dialog *Create2ChoiceDialog( CGM_DialogManagerSharedPtr pDlgMgr, bool root_dialog,
									    const std::string& dlg_title, const SRect& dlg_rect,
									    int btn1_id, const std::string& btn1_title, const SRect& btn1_rect,
									    int btn2_id, const std::string& btn2_title, const SRect& btn2_rect,
									    int static_id = 0, const std::string& static_title = "", const SRect& static_rect = SRect(0,0,0,0) );


#endif		/*  __GM_AUXILIARY_H__  */
