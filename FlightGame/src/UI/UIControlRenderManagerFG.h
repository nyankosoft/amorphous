#ifndef  __UIControlRenderManagerFG_H__
#define  __UIControlRenderManagerFG_H__


#include "UI/ui_fwd.h"


class CGM_FontDesc;
class CFontBase;


class CUIControlRenderManagerFG : public CGM_ControlRenderMgr_Default//CGM_ControlRenderManager
{

protected:

//	void BlendFontColor( CGM_Control* pControl );
    
//	void DrawDialogTitle( CGM_Dialog& dialog );

	/// basic render param registration
	/// - register the bounding box of the control to render param
	/// - set index to the render param to the control
	/// \return reference to the render param for the control
//	CControlRenderParam& InitRenderParamBase( CGM_Control& ctrl );

public:

	CUIControlRenderManagerFG();
	virtual ~CUIControlRenderManagerFG();

//	void SetCaptionParameters( CGM_CaptionParameters& param ) { m_CaptionParam = param; }
//	void AddTextureFromFile( int index, const string& filename );

//	virtual void InitRenderParamStatic( CGM_Static& static_ctrl );
//	virtual void InitRenderParamButton( CGM_Button& button );
//	virtual void InitRenderParamCheckBox( CGM_CheckBox& checkbox );
//	virtual void InitRenderParamRadioButton( CGM_RadioButton& radio_button );
//	virtual void InitRenderParamDialogCloseButton( CGM_DialogCloseButton& button );
//	virtual void InitRenderParamSlider( CGM_Slider& slider );
//	virtual void InitRenderParamListBox( CGM_ListBox& listbox );
//	virtual void InitRenderParamScrollBar( CGM_ScrollBar& scrollbar ) {}
//	virtual void InitRenderParamSubDialogButton( CGM_SubDialogButton& subdlg_button );
//	virtual void InitRenderParamDialog( CGM_Dialog& dialog );

	virtual void RenderStatic( CGM_Static& static_ctrl );
	virtual void RenderButton( CGM_Button& button );
	virtual void RenderCheckBox( CGM_CheckBox& checkbox );
	virtual void RenderRadioButton( CGM_RadioButton& radio_button );
	virtual void RenderDialogCloseButton( CGM_DialogCloseButton& button );
	virtual void RenderSlider( CGM_Slider& slider );
	virtual void RenderListBox( CGM_ListBox& listbox );
	virtual void RenderScrollBar( CGM_ScrollBar& scrollbar ) {}
	virtual void RenderSubDialogButton( CGM_SubDialogButton& subdlg_button );
	virtual void RenderDialog( CGM_Dialog& dialog );

//	virtual void RenderCaption( const std::string& strCaption );

//	virtual void ChangeScale( float factor );
};


extern void SetDefaultTextureStageStates();
extern void SetDefaultAlphaBlend();
extern void SetAdditiveAlphaBlend();


#endif		/*  __UIControlRenderManagerFG_H__  */
