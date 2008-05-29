#ifndef  __GM_XMLParser_H__
#define  __GM_XMLParser_H__


#include "Graphics/Point.h"
#include "Graphics/Rect.h"
using namespace Graphics;

#include <string>


#include "ui_fwd.h"


class CXMLNodeReader;


/**
 * base class of UI components
 * - CGM_Control and CGM_Dialog are derived from this class
 */
class CGM_XMLParser
{
private:

	/// borrowed reference
	CGM_DialogManager *m_pDialogManager;

private:

	bool LoadDialog( CXMLNodeReader& reader );

	bool LoadControls( CXMLNodeReader& reader, CGM_Dialog *pDialog );

	void LoadCommonDesc( CXMLNodeReader& reader, CGM_ControlDescBase *pControlDescBase );
	void LoadStaticDesc( CXMLNodeReader& reader, CGM_StaticDesc *pStaticDesc );
	void LoadButtonDesc( CXMLNodeReader& reader, CGM_ButtonDesc *pButtonDesc );
	void LoadCheckBoxDesc( CXMLNodeReader& reader, CGM_CheckBoxDesc *pCheckBoxDesc );
	void LoadRadioButtonDesc( CXMLNodeReader& reader, CGM_RadioButtonDesc *pRadioButtonDesc );
	void LoadSliderDesc( CXMLNodeReader& reader, CGM_SliderDesc *pSliderDesc );

public:

//	CGM_XMLParser();
	CGM_XMLParser( CGM_DialogManager *pDlgMgr ) : m_pDialogManager(pDlgMgr) {}

	virtual ~CGM_XMLParser();

	bool LoadFromXMLFile( const std::string& xml_filename );
};


#endif		/*  __GM_XMLParser_H__  */
