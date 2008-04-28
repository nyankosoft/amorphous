#ifndef  __FG_CmdMenuControlRendererManager_H__
#define  __FG_CmdMenuControlRendererManager_H__


#include "UI/GM_StdControlRendererManager.h"


class CFG_CmdMenuControlRendererManager : public CGM_StdControlRendererManager
{
public:

	CFG_CmdMenuControlRendererManager();
	virtual ~CFG_CmdMenuControlRendererManager() {}

//	virtual void OnControlCreated( CGM_Control *pControl );

	virtual CGM_ControlRenderer *CreateStaticRenderer( CGM_Static *pStatic );
	virtual CGM_ControlRenderer *CreateButtonRenderer( CGM_Button *pButton );
	virtual CGM_ControlRenderer *CreateCheckBoxRenderer( CGM_CheckBox *pCheckBox );
	virtual CGM_ControlRenderer *CreateRadioButtonRenderer( CGM_RadioButton *pRadioButton );
	virtual CGM_ControlRenderer *CreateDialogCloseButtonRenderer( CGM_DialogCloseButton *pDlgCloseButton );
	virtual CGM_ControlRenderer *CreateSliderRenderer( CGM_Slider *pSlider );
	virtual CGM_ControlRenderer *CreateListBoxRenderer( CGM_ListBox *pListBox );
//	virtual CGM_ControlRenderer *CreateScrollBarRenderer( CGM_ScrollBar *pScrollBar );
	virtual CGM_ControlRenderer *CreateSubDialogButtonRenderer( CGM_SubDialogButton *pSubDlgButton );
	virtual CGM_ControlRenderer *CreateDialogRenderer( CGM_Dialog *pDialog );

	virtual int GetNumGraphicsLayersPerUILayer() { return 8; } ///< called by control renderers

//	virtual int CalcUILayerIndex( CGM_ControlRenderer *pControlRenderer );

//	virtual void ChangeScale( float factor ) {}

	enum eTextureID
	{
		ID_TEX_ROUNDFRAME = 0,
		ID_TEX_BACKGROUNDTESTIMAGE,
		NUM_TEXTURES
	};

	enum eFontID
	{
		ID_FONT_MAIN = 0,
		NUM_FONTS
	};

};


#endif		/*  __FG_CmdMenuControlRendererManager_H__  */
