
#include "3DCommon/GraphicsElementManager.h"
#include "GM_StdControlRendererManager.h"
#include "GM_StdControlRenderer.h"
#include "ui.h"


CGM_StdControlRendererManager::CGM_StdControlRendererManager()
{
	m_pGraphicsElementManager->LoadFont( 0, "Arial", CFontBase::FONTTYPE_NORMAL, 20, 32, 0, 0 );
}


/*
CGM_ControlRenderer *CGM_StdControlRendererManager::InitRenderer( CGM_StdControlRenderer *pControlRenderer )
{
}
*/


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateStaticRenderer( CGM_Static *pStatic )
{
	return new CGM_StdStaticRenderer();
}


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateButtonRenderer( CGM_Button *pButton )
{
//	pButtonRenderer = new CGM_StdButtonRenderer( &button );

	return new CGM_StdButtonRenderer();
}


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateCheckBoxRenderer( CGM_CheckBox *pCheckBox )
{
	return new CGM_StdCheckBoxRenderer();
}


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateRadioButtonRenderer( CGM_RadioButton *pRadioButton )
{
	return new CGM_StdRadioButtonRenderer();
}


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateDialogCloseButtonRenderer( CGM_DialogCloseButton *pDlgCloseButton )
{
	return new CGM_StdButtonRenderer(); /// use the std button renderer
}


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateSubDialogButtonRenderer( CGM_SubDialogButton *pSubDlgButton )
{
	return new CGM_StdButtonRenderer(); /// use the std button renderer
}


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateSliderRenderer( CGM_Slider *pSlider )
{
	return new CGM_StdSliderRenderer();
}


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateListBoxRenderer( CGM_ListBox *pListBox )
{
	return new CGM_StdListBoxRenderer();
}


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateScrollBarRenderer( CGM_ScrollBar *pScrollbar )
{
	return NULL;//new CGM_StdScrollBarRenderer();
}


/*
CGM_ControlRenderer *CGM_StdControlRendererManager::CreateDialogRenderer( CGM_Dialog& dialog )
{
}
*/


CGM_ControlRenderer *CGM_StdControlRendererManager::CreateDialogRenderer( CGM_Dialog *pDialog )
{
	return new CGM_StdDialogRenderer();
}

