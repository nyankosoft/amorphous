
#include "3DCommon/GraphicsElementManager.h"
#include "FG_StdControlRendererManager.h"
#include "FG_StdControlRenderer.h"
#include "UI/ui.h"


CFG_StdControlRendererManager::CFG_StdControlRendererManager()
{
	m_pGraphicsElementManager->LoadFont( ID_FONT_MAIN, "Arial", CFontBase::FONTTYPE_NORMAL, 8, 16, 0, 0 );

	m_pGraphicsElementManager->LoadTexture( ID_TEX_ROUNDFRAME, "Texture/rf.dds" );
}


/*
CGM_ControlRenderer *CFG_StdControlRendererManager::InitRenderer( CFG_StdControlRenderer *pControlRenderer )
{
}
*/


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateStaticRenderer( CGM_Static *pStatic )
{
	return new CFG_StdStaticRenderer();
}


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateButtonRenderer( CGM_Button *pButton )
{
//	pButtonRenderer = new CFG_StdButtonRenderer( &button );

	return new CFG_StdButtonRenderer();
}


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateCheckBoxRenderer( CGM_CheckBox *pCheckBox )
{
	return new CFG_StdCheckBoxRenderer();
}


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateRadioButtonRenderer( CGM_RadioButton *pRadioButton )
{
	return new CFG_StdRadioButtonRenderer();
}


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateDialogCloseButtonRenderer( CGM_DialogCloseButton *pDlgCloseButton )
{
	return new CFG_StdButtonRenderer(); /// use the std button renderer
}


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateSubDialogButtonRenderer( CGM_SubDialogButton *pSubDlgButton )
{
	return new CFG_StdButtonRenderer(); /// use the std button renderer
}


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateSliderRenderer( CGM_Slider *pSlider )
{
	return new CFG_StdSliderRenderer();
}


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateListBoxRenderer( CGM_ListBox *pListBox )
{
	return new CFG_StdListBoxRenderer();
}


/*
CGM_ControlRenderer *CFG_StdControlRendererManager::CreateScrollBarRenderer( CGM_ScrollBar& scrollbar )
{
}



CGM_ControlRenderer *CFG_StdControlRendererManager::CreateDialogRenderer( CGM_Dialog& dialog )
{
}
*/


CGM_ControlRenderer *CFG_StdControlRendererManager::CreateDialogRenderer( CGM_Dialog *pDialog )
{
	return new CFG_StdDialogRenderer();
}

