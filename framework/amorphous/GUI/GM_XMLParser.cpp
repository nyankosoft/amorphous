#include "GM_XMLParser.hpp"
#include "GM_Control.hpp"
#include "GM_ControlDescBase.hpp"
#include "GM_Dialog.hpp"
#include "GM_DialogManager.hpp"
#include "GM_ControlRenderer.hpp"
#include "GM_ControlRendererManager.hpp"
#include "XML/XMLDocumentBase.hpp"
#include "XML/XMLNode.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/lfs.hpp"

#include "GUI.hpp"


namespace amorphous
{

using std::string;
using std::vector;
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


void CGM_XMLParser::LoadCommonDesc( XMLNode& reader, CGM_ControlDescBase *pControlDescBase )
{
	pControlDescBase->StringID = reader.GetAttributeText("id");
	reader.GetChildElementTextContentLTWH( "LTWH", pControlDescBase->Rect );
	reader.GetChildElementTextContentLTRB( "LTRB", pControlDescBase->Rect );
//	reader.GetTextContent( "Caption", pControlDescBase->strCaption );
}


void CGM_XMLParser::LoadStaticDesc( XMLNode& reader, CGM_StaticDesc *pStaticDesc )
{
	LoadCommonDesc( reader, pStaticDesc );
	reader.GetChildElementTextContent( "Text",  pStaticDesc->strText );
	reader.GetChildElementTextContent( "Title", pStaticDesc->strText );
}


void CGM_XMLParser::LoadButtonDesc( XMLNode& reader, CGM_ButtonDesc *pButtonDesc )
{
	LoadStaticDesc( reader, pButtonDesc );
}


void CGM_XMLParser::LoadCheckBoxDesc( XMLNode& reader, CGM_CheckBoxDesc *pCheckBoxDesc )
{
	LoadButtonDesc( reader, pCheckBoxDesc );
//	reader.Get( "Checked", pStaticDesc->strText );
}


void CGM_XMLParser::LoadRadioButtonDesc( XMLNode& reader, CGM_RadioButtonDesc *pRadioButtonDesc )
{
	LoadCheckBoxDesc( reader, pRadioButtonDesc );
	reader.GetChildElementTextContent( "Group", pRadioButtonDesc->iButtonGroup );
}


void CGM_XMLParser::LoadSliderDesc( XMLNode& reader, CGM_SliderDesc *pSliderDesc )
{
	LoadCommonDesc( reader, pSliderDesc );
//	reader.Get( "Title", pSliderDesc->Title );
	reader.GetChildElementTextContent( "MinVal",  pSliderDesc->iMin );
	reader.GetChildElementTextContent( "MaxVal",  pSliderDesc->iMax );
	reader.GetChildElementTextContent( "InitVal", pSliderDesc->iInitialValue );
}


void CGM_XMLParser::LoaListBoxDesc( XMLNode& reader, CGM_ListBoxDesc *pListBoxDesc )
{
	LoadCommonDesc( reader, pListBoxDesc );
	reader.GetChildElementTextContent( "PageSize",        pListBoxDesc->PageSize );
	reader.GetChildElementTextContent( "ScrollbarWidth",  pListBoxDesc->nSBWidth );
}


CGM_Dialog *CGM_XMLParser::LoadSubDialog( XMLNode& reader )
{
	XMLNode dlgfile = reader.GetChild( "DialogFile" );
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


bool CGM_XMLParser::LoadControls( XMLNode& reader, CGM_Dialog *pDialog )
{
	vector<XMLNode> control_reader = reader.GetImmediateChildren();

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

CGM_Dialog *CGM_XMLParser::LoadDialog( XMLNode& reader, bool root_dialog )
{
//	xercesc::DOMNode *pDlgNode = reader.GetDOMNode();

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
//	CXMLDocumentLoader xml_doc_loader;
//	shared_ptr<CXMLDocument> pXMLDocument = xml_doc_loader.Load( xml_filename );
	shared_ptr<XMLDocumentBase> pXMLDocument = CreateXMLDocument( xml_filename );

	if( !pXMLDocument )
		return false;

//	XMLNode dlgnode = XMLNode( pXMLDocument->getFirstChild() );
	XMLNode dlgnode = pXMLDocument->GetRootNode();
	return LoadDialog( dlgnode, root_dialog );
}


bool CGM_XMLParser::LoadFromXMLFile( const std::string& xml_filename )
{
	shared_ptr<XMLDocumentBase> pXMLDocument = CreateXMLDocument( xml_filename );

	if( !pXMLDocument )
		return false;

	lfs::dir_stack dir_stk( lfs::get_parent_path(xml_filename) );

	XMLNode rootnode = pXMLDocument->GetRootNode();

	// get elements for root dialogs
	vector<XMLNode> vecRootDlgNodes = rootnode.GetImmediateChildren( "Dialog" );
	const size_t num = vecRootDlgNodes.size();
	m_vecpRootDialog.reserve( num );
	for( size_t i=0; i<num; i++ )
	{
		CGM_Dialog *pDlg = LoadDialog( vecRootDlgNodes[i], true );
		if( pDlg )
			m_vecpRootDialog.push_back( pDlg );
	}

	dir_stk.pop_and_chdir();

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


} // namespace amorphous
