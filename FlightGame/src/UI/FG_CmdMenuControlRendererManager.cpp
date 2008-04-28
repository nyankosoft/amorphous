
#include "3DCommon/GraphicsElementManager.h"
#include "FG_CmdMenuControlRendererManager.h"
#include "FG_CmdMenuControlRenderer.h"
#include "UI/ui.h"


CFG_CmdMenuControlRendererManager::CFG_CmdMenuControlRendererManager()
{
	m_pGraphicsElementManager->LoadFont( ID_FONT_MAIN, "Texture\\HGGE_16x8_256.dds", CFontBase::FONTTYPE_TEXTURE, 8, 16, 0, 0 );

	m_pGraphicsElementManager->LoadTexture( ID_TEX_ROUNDFRAME, "Texture/rf.dds" );
}


/*
CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::InitRenderer( CFG_CmdMenuControlRenderer *pControlRenderer )
{
}
*/


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateStaticRenderer( CGM_Static *pStatic )
{
	return new CFG_CmdMenuStaticRenderer();
}


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateButtonRenderer( CGM_Button *pButton )
{
//	pButtonRenderer = new CFG_CmdMenuButtonRenderer( &button );

	return new CFG_CmdMenuButtonRenderer();
}


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateCheckBoxRenderer( CGM_CheckBox *pCheckBox )
{
	return new CFG_CmdMenuCheckBoxRenderer();
}


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateRadioButtonRenderer( CGM_RadioButton *pRadioButton )
{
	return new CFG_CmdMenuRadioButtonRenderer();
}


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateDialogCloseButtonRenderer( CGM_DialogCloseButton *pDlgCloseButton )
{
	return new CFG_CmdMenuButtonRenderer(); /// use the std button renderer
}


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateSubDialogButtonRenderer( CGM_SubDialogButton *pSubDlgButton )
{
	return new CFG_CmdMenuButtonRenderer(); /// use the std button renderer
}


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateSliderRenderer( CGM_Slider *pSlider )
{
	return new CFG_CmdMenuSliderRenderer();
}


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateListBoxRenderer( CGM_ListBox *pListBox )
{
	return new CFG_CmdMenuListBoxRenderer();
}


/*
CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateScrollBarRenderer( CGM_ScrollBar& scrollbar )
{
}



CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateDialogRenderer( CGM_Dialog& dialog )
{
}
*/


CGM_ControlRenderer *CFG_CmdMenuControlRendererManager::CreateDialogRenderer( CGM_Dialog *pDialog )
{
	return new CFG_CmdMenuDialogRenderer();
}

