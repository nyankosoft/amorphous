
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

#include "UI.h"

using namespace std;
using namespace boost;


xercesc_2_8::DOMDocument *LoadXMLDocument( const std::string& xml_filepath )
{
	xercesc_2_8::DOMDocument* pXMLDocument = NULL;
	CXMLDocumentLoader loader;
	bool loaded = loader.Load( xml_filepath, &pXMLDocument );
	if( !loaded )
	{
		return NULL;
	}
	else
		return pXMLDocument;
}


//========================================================================================
// CGM_XMLParser
//========================================================================================


CGM_XMLParser::~CGM_XMLParser()
{
}


void CGM_XMLParser::LoadCommonDesc( CXMLNodeReader& reader, CGM_ControlDescBase *pControlDescBase )
{
	pControlDescBase->StringID = reader.GetAttributeText("id");
	reader.GetTextContentLTWH( "LTWH", pControlDescBase->Rect );
	reader.GetTextContentLTRB( "LTRB", pControlDescBase->Rect );
//	reader.GetTextContent( "Caption", pControlDescBase->strCaption );
}


void CGM_XMLParser::LoadStaticDesc( CXMLNodeReader& reader, CGM_StaticDesc *pStaticDesc )
{
	LoadCommonDesc( reader, pStaticDesc );
	reader.GetTextContent( "Text", pStaticDesc->strText );
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
	reader.GetTextContent( "Group", pRadioButtonDesc->iButtonGroup );
}


void CGM_XMLParser::LoadSliderDesc( CXMLNodeReader& reader, CGM_SliderDesc *pSliderDesc )
{
	LoadCommonDesc( reader, pSliderDesc );
//	reader.Get( "Title", pSliderDesc->Title );
	reader.GetTextContent( "MinVal",  pSliderDesc->iMin );
	reader.GetTextContent( "MaxVal",  pSliderDesc->iMax );
	reader.GetTextContent( "InitVal", pSliderDesc->iInitialValue );
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
		else if( control_type == "SubDialogButton" )
		{
			CGM_SubDialogButtonDesc *pSubDlgBtnDesc = new CGM_SubDialogButtonDesc;( control_reader[i] );
			LoadButtonDesc( control_reader[i], pSubDlgBtnDesc );
			pDesc = pSubDlgBtnDesc;
		}
		else if( control_type == "Slider" )
		{
//			CGM_SliderDesc *pSliderDesc = new CGM_SliderDesc();
//			LoadSliderDesc( control_reader[i], pSliderDesc );
//			pDesc = pSliderDesc;
		}
		else if( control_type == "ListBox" )
		{
//			CGM_ListBoxDesc *pListBoxDesc = new CGM_ListBoxDesc();
//			LoaListBoxDesc( control_reader[i], pListBoxDesc );
		}

		if( pDesc )
		{
			pDialog->AddControl( pDesc );
			SafeDelete( pDesc );
		}

	}

	return true;
}


bool CGM_XMLParser::LoadDialog( CXMLNodeReader& reader )
{
	xercesc_2_8::DOMNode *pDlgNode = reader.GetDOMNode();

	CGM_DialogDesc desc;
	LoadCommonDesc( reader, &desc );
	reader.GetTextContent( "Title", desc.strTitle );

	CGM_Dialog *pDialog = m_pDialogManager->AddDialog( desc );

	LoadControls( reader.GetChild( "Controls" ), pDialog );

	return true;
}


bool CGM_XMLParser::LoadFromXMLFile( const std::string& xml_filename )
{
	xercesc_2_8::DOMDocument *pXMLDocument = LoadXMLDocument( xml_filename );

	if( !pXMLDocument )
		return false;

	xercesc_2_8::DOMNode *pRoot = pXMLDocument->getFirstChild();

	// get elements for root dialogs
	vector<xercesc_2_8::DOMNode *> vecpRootDlgNodes = GetImmediateChildNodes( pRoot, "Dialog" );
	const size_t num = vecpRootDlgNodes.size();
	for( size_t i=0; i<num; i++ )
	{
		LoadDialog( CXMLNodeReader( vecpRootDlgNodes[i] ) );
	}

//	CXMLNodeReader root(  );
//	CXMLNodeReader root.get_child( "Root" );

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
