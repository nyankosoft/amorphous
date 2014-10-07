#ifndef  __GM_XMLParser_H__
#define  __GM_XMLParser_H__


#include <string>
#include <vector>
#include "amorphous/Graphics/Point.hpp"
#include "amorphous/Graphics/Rect.hpp"
#include "amorphous/XML/fwd.hpp"
#include "fwd.hpp"


namespace amorphous
{


/**
 * base class of UI components
 * - CGM_Control and CGM_Dialog are derived from this class
 */
class CGM_XMLParser
{
private:

	/// borrowed reference
	CGM_DialogManager *m_pDialogManager;

	/// borrowed reference
	std::vector<CGM_Dialog *> m_vecpRootDialog;

private:

	CGM_Dialog *LoadDialog( XMLNode& reader, bool root_dialog );

	CGM_Dialog *LoadDialogFromXMLFile( const std::string& xml_filename, bool root_dialog );

	CGM_Dialog *LoadSubDialog( XMLNode& reader );

	bool LoadControls( XMLNode& reader, CGM_Dialog *pDialog );

	void LoadCommonDesc( XMLNode& reader, CGM_ControlDescBase *pControlDescBase );
	void LoadStaticDesc( XMLNode& reader, CGM_StaticDesc *pStaticDesc );
	void LoadButtonDesc( XMLNode& reader, CGM_ButtonDesc *pButtonDesc );
	void LoadCheckBoxDesc( XMLNode& reader, CGM_CheckBoxDesc *pCheckBoxDesc );
	void LoadRadioButtonDesc( XMLNode& reader, CGM_RadioButtonDesc *pRadioButtonDesc );
	void LoadSliderDesc( XMLNode& reader, CGM_SliderDesc *pSliderDesc );
	void LoaListBoxDesc( XMLNode& reader, CGM_ListBoxDesc *pListBoxDesc );

public:

//	CGM_XMLParser();
	CGM_XMLParser( CGM_DialogManager *pDlgMgr ) : m_pDialogManager(pDlgMgr) {}

	virtual ~CGM_XMLParser();

	bool LoadFromXMLFile( const std::string& xml_filename );
};

} // namespace amorphous



#endif		/*  __GM_XMLParser_H__  */
