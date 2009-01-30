#ifndef  __GM_STDCONTROLRENDERERMANAGER_H__
#define  __GM_STDCONTROLRENDERERMANAGER_H__


#include "GM_ControlRendererManager.hpp"


class CGM_StdControlRendererManager : public CGM_ControlRendererManager
{
//	std::string m_PrimaryFontName;

public:

	CGM_StdControlRendererManager();

	virtual ~CGM_StdControlRendererManager() {}

//	virtual void OnControlCreated( CGM_Control *pControl );

	virtual CGM_ControlRenderer *CreateStaticRenderer( CGM_Static *pStatic );
	virtual CGM_ControlRenderer *CreateButtonRenderer( CGM_Button *pButton );
	virtual CGM_ControlRenderer *CreateCheckBoxRenderer( CGM_CheckBox *pCheckBox );
	virtual CGM_ControlRenderer *CreateRadioButtonRenderer( CGM_RadioButton *pRadioButton );
	virtual CGM_ControlRenderer *CreateDialogCloseButtonRenderer( CGM_DialogCloseButton *pDlgCloseButton );
	virtual CGM_ControlRenderer *CreateSliderRenderer( CGM_Slider *pSlider );
	virtual CGM_ControlRenderer *CreateListBoxRenderer( CGM_ListBox *pListBox );
	virtual CGM_ControlRenderer *CreateScrollBarRenderer( CGM_ScrollBar *pScrollbar );
	virtual CGM_ControlRenderer *CreateSubDialogButtonRenderer( CGM_SubDialogButton *pSubDlgButton );
	virtual CGM_ControlRenderer *CreateDialogRenderer( CGM_Dialog *pDialog );
	virtual CGM_ControlRenderer *CreateCaptionRenderer();

	virtual int GetNumGraphicsLayersPerUILayer() { return 8; } ///< called by control renderers

//	void SetPrimaryFontName( const std::string& font_name ) { m_PrimaryFontName = font_name; }

//	virtual int CalcUILayerIndex( CGM_ControlRenderer *pControlRenderer );

//	virtual void ChangeScale( float factor ) {}
};


/*
	virtual void OnStaticCreated( CGM_Static *pStatic );
	virtual void OnButtonCreated( CGM_Button *pButton );
	virtual void OnCheckBoxCreated( CGM_CheckBox *pCheckBox );
	virtual void OnRadioButtonCreated( CGM_RadioButton *pRadioButton );
	virtual void OnDialogCloseButtonCreated( CGM_DialogCloseButton *pDlgCloseButton );
	virtual void OnSliderCreated( CGM_Slider *pSlider );
	virtual void OnListBoxCreated( CGM_ListBox *pListBox );
//	virtual void OnScrollBarCreated( CGM_ScrollBar *pScrollBar );
	virtual void OnSubDialogButtonCreated( CGM_SubDialogButton *pSubDlgButton );
	virtual void OnDialogCreated( CGM_Dialog *pDialog );
*/


#endif		/*  __GM_STDCONTROLRENDERERMANAGER_H__  */
