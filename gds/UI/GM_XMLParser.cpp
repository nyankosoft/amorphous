#include "GM_XMLParser.h"
#include "GM_Control.h"
#include "GM_ControlDescBase.h"
#include "GM_Dialog.h"
#include "GM_DialogManager.h"
#include "GM_ControlRenderer.h"
#include "GM_ControlRendererManager.h"

#include "XML/XercesString.h"
#include "XML/xmlch2x.h"
#include "XML/XMLDocumentLoader.h"
#include "XML/XMLNodeReader.h"
#include "Support/SafeDelete.h"
#include "Support/fnop.h"

#include "UI.h"

using namespace std;
using namespace boost;

/*
xercesc::DOMDocument *LoadXMLDocument( const std::string& xml_filepath )
{
	xercesc::DOMDocument* pXMLDocument = NULL;
	CXMLDocumentLoader loader;
	bool loaded = loader.Load( xml_filepath, &pXMLDocument );
	if( !loaded )
	{
		return NULL;
	}
	else
		return pXMLDocument;
}
*/

//========================================================================================
// CGM_XMLParser
//========================================================================================


CGM_XMLParser::~CGM_XMLParser()
{
}


void CGM_XMLParser::LoadCommonDesc( CXMLNodeReader& reader, CGM_ControlDescBase *pControlDescBase )
{
	pControlDescBase->StringID = reader.GetAttributeText("id");
	reader.GetChildElementTextContentLTWH( "LTWH", pControlDescBase->Rect );
	reader.GetChildElementTextContentLTRB( "LTRB", pControlDescBase->Rect );
//	reader.GetTextContent( "Caption", pControlDescBase->strCaption );
}


void CGM_XMLParser::LoadStaticDesc( CXMLNodeReader& reader, CGM_StaticDesc *pStaticDesc )
{
	LoadCommonDesc( reader, pStaticDesc );
	reader.GetChildElementTextContent( "Text",  pStaticDesc->strText );
	reader.GetChildElementTextContent( "Title", pStaticDesc->strText );
}


void CGM_XMLParser::LoadButtonDesc( CXMLNodeReader& reader, CGM_ButtonDesc *pButtonDesc )
{
	LoadStaticDesc( reader, pButtonDesc );
}


void CGM_XMLParser::LoadCheckBoxDesc( CXMLNodeReader& reader, CGM_CheckBoxDesc *pCheckBoxDesc )
{
	LoadButtonDesc( reader, pCheckBoxDesc );
//	reader.Get( "Checked", pStaticDesc->strText );
}


void CGM_XMLParser::LoadRadioButtonDesc( CXMLNodeReader& reader, CGM_RadioButtonDesc *pRadioButtonDesc )
{
	LoadCheckBoxDesc( reader, pRadioButtonDesc );
	reader.GetChildElementTextContent( "Group", pRadioButtonDesc->iButtonGroup );
}


void CGM_XMLParser::LoadSliderDesc( CXMLNodeReader& reader, CGM_SliderDesc *pSliderDesc )
{
	LoadCommonDesc( reader, pSliderDesc );
//	reader.Get( "Title", pSliderDesc->Title );
	reader.GetChildElementTextContent( "MinVal",  pSliderDesc->iMin );
	reader.GetChildElementTextContent( "MaxVal",  pSliderDesc->iMax );
	reader.GetChildElementTextContent( "InitVal", pSliderDesc->iInitialValue );
}


void CGM_XMLParser::LoaListBoxDesc( CXMLNodeReader& reader, CGM_ListBoxDesc *pListBoxDesc )
{
	LoadCommonDesc( reader, pListBoxDesc );
	reader.GetChildElementTextContent( "PageSize",        pListBoxDesc->PageSize );
	reader.GetChildElementTextContent( "ScrollbarWidth",  pListBoxDesc->nSBWidth );
}


CGM_Dialog *CGM_XMLParser::LoadSubDialog( CXMLNodeReader& reader )
{
	CXMLNodeReader dlgfile = reader.GetChild( "DialogFile" );
	if( dlgfile.IsValid() )
	{
		// load the subdialog from another xml file
		return LoadDialogFromXMLFile( dlgfile.GetTextContent(), false );
	}
	else
	{
		return LoadDialog( reader.GetChild( "Dialog" ), false );
	}
}


bool CGM_XMLParser::LoadControls( CXMLNodeReader& reader, CGM_Dialog *pDialog )
{
	vector<CXMLNodeReader> control_reader = reader.GetImmediateChildren();

	const size_t num = control_reader.size();
	for( size_t i=0; i<num; i++ )
	{
		string control_type = control_reader[i].GetName();
		
		CGM_ControlDesc *pDesc = NULL;

		if( control_type == "Static" )
		{
			CGM_StaticDesc *pStaticDesc = new CGM_StaticDesc();
			LoadStaticDesc( control_reader[i], pStaticDesc );
			pDesc = pStaticDesc;
		}
		else if( control_type == "Button" )
		{
			CGM_ButtonDesc *pButtonDesc = new CGM_ButtonDesc();
			LoadButtonDesc( control_reader[i], pButtonDesc );
			pDesc = pButtonDesc;
		}
		else if( control_type == "CheckBox" )
		{
			CGM_CheckBoxDesc *pCheckBoxDesc = new CGM_CheckBoxDesc();
			LoadCheckBoxDesc( control_reader[i], pCheckBoxDesc );
			pDesc = pCheckBoxDesc;
		}
		else if( control_type == "RadioButton" )
		{
			CGM_RadioButtonDesc *pRadioButtonDesc = new CGM_RadioButtonDesc();
			LoadRadioButtonDesc( control_reader[i], pRadioButtonDesc );
			pDesc = pRadioButtonDesc;
		}
		else if( control_type == "DialogCloseButton" )
		{
			CGM_DialogCloseButtonDesc *pDlgCloseBtnDesc = new CGM_DialogCloseButtonDesc;( control_reader[i] );
			LoadButtonDesc( control_reader[i], pDlgCloseBtnDesc );
			pDesc = pDlgCloseBtnDesc;
		}
		else if( control_type == "Slider" )
		{
			CGM_SliderDesc *pSliderDesc = new CGM_SliderDesc();
			LoadSliderDesc( control_reader[i], pSliderDesc );
			pDesc = pSliderDesc;
		}
		else if( control_type == "ListBox" )
		{
			CGM_ListBoxDesc *pListBoxDesc = new CGM_ListBoxDesc();
			LoaListBoxDesc( control_reader[i], pListBoxDesc );
			pDesc = pListBoxDesc;
		}
		else if( control_type == "SubDialogButton" )
		{
			CGM_SubDialogButtonDesc *pSubDlgBtnDesc = new CGM_SubDialogButtonDesc;( control_reader[i] );
			LoadButtonDesc( control_reader[i], pSubDlgBtnDesc );
			pSubDlgBtnDesc->pSubDialog = LoadSubDialog( control_reader[i].GetChild( "SubDialog" ) );
			pDesc = pSubDlgBtnDesc;
		}

		if( pDesc )
		{
			pDialog->AddControl( pDesc );
			SafeDelete( pDesc );
		}

	}

	return true;
}

CGM_Dialog *CGM_XMLParser::LoadDialog( CXMLNodeReader& reader, bool root_dialog )
{
	xercesc::DOMNode *pDlgNode = reader.GetDOMNode();

	CGM_DialogDesc desc;
	desc.bRootDialog = root_dialog;
	LoadCommonDesc( reader, &desc );
	reader.GetChildElementTextContent( "Title", desc.strTitle );

	CGM_Dialog *pDialog = m_pDialogManager->AddDialog( desc );

	LoadControls( reader.GetChild( "Controls" ), pDialog );

	return pDialog;
}


CGM_Dialog *CGM_XMLParser::LoadDialogFromXMLFile( const std::string& xml_filename, bool root_dialog )
{
	CXMLDocumentLoader xml_doc_loader;
	shared_ptr<CXMLDocument> pXMLDocument = xml_doc_loader.Load( xml_filename );

	if( !pXMLDocument )
		return false;

//	CXMLNodeReader dlgnode = CXMLNodeReader( pXMLDocument->getFirstChild() );
	CXMLNodeReader dlgnode = pXMLDocument->GetRootNodeReader();
	return LoadDialog( dlgnode, root_dialog );
}


bool CGM_XMLParser::LoadFromXMLFile( const std::string& xml_filename )
{
	CXMLDocumentLoader xml_loader;
	shared_ptr<CXMLDocument> pXMLDocument = xml_loader.Load( xml_filename );
//	xercesc::DOMDocument *pXMLDocument = LoadXMLDocument( xml_filename );

	if( !pXMLDocument )
		return false;

	fnop::dir_stack dir_stk( fnop::get_path(xml_filename) );

	CXMLNodeReader rootnode = pXMLDocument->GetRootNodeReader();

	// get elements for root dialogs
	vector<CXMLNodeReader> vecRootDlgNodes = rootnode.GetImmediateChildren( "Dialog" );
	const size_t num = vecRootDlgNodes.size();
	m_vecpRootDialog.reserve( num );
	for( size_t i=0; i<num; i++ )
	{
		CGM_Dialog *pDlg = LoadDialog( vecRootDlgNodes[i], true );
		if( pDlg )
			m_vecpRootDialog.push_back( pDlg );
	}

	dir_stk.prevdir();

	return true;
}


/*
void CGM_XMLParser::OnFocusOut()
{
}


void CGM_XMLParser::GetState()
{
}
*/
