#include "StaticGeometryDesc.h"
#include "Support/Log/DefaultLog.h"
#include "Support/fnop.h"
#include "XML/XMLDocumentLoader.h"
#include "XML/XercesString.h"
#include "XML/xmlch2x.h"


#define LOG_ERR_RETURN_FALSE( msg ) { LOG_PRINT_ERROR( msg ); return false; }


DOMNode *GetRootNode( xercesc_2_8::DOMDocument *pXMLDocument )
{	
	xercesc_2_8::DOMElement *pElem = pXMLDocument->getDocumentElement();

	xercesc_2_8::DOMNodeIterator *iterator
		= pXMLDocument->createNodeIterator( pXMLDocument->getFirstChild(), xercesc_2_8::DOMNodeFilter::SHOW_TEXT, NULL, false );

	DOMNode *pRootNode = iterator->getRoot();

	if( pRootNode )
		return pRootNode;
	else
	{
		LOG_PRINT_WARNING( "- Cannot find a root node" );
		return NULL;
	}

	return NULL;
}


DOMNode *GetChildNode( DOMNode *pParentNode, const std::string& node_name )
{
	DOMNodeList *pNodeList = pParentNode->getChildNodes();
	const int num_nodes = (int)pNodeList->getLength();
	for( int i=0; i<num_nodes; i++ )
	{
		if( to_string(pNodeList->item(i)->getNodeName()) == node_name )
			return pNodeList->item(i);
	}

	return NULL; // not found
}


vector<DOMNode *> GetImmediateChildNodes( DOMNode *pParentNode,
										  const std::string& child_node_name )
{
	vector<DOMNode *> child_nodes;
	DOMNodeList *pNodeList = pParentNode->getChildNodes();
	const int num_nodes = (int)pNodeList->getLength();
	for( int i=0; i<num_nodes; i++ )
	{
		if( to_string(pNodeList->item(i)->getNodeName()) == child_node_name )
			child_nodes.push_back( pNodeList->item(i) );
	}

	return child_nodes;
}


std::string GetTextContentOfImmediateChildNode( DOMNode *pParentNode,
												const std::string& child_node_name )
{
	DOMNode *pNode = GetChildNode( pParentNode, child_node_name );

	if( !pNode )
		return string();

	return string( to_string( pNode->getTextContent() ) );
}


/**
 Returns an array of text contents of immediate child nodes named child_node_name
 - e.g.
 <Directory>
   <File>a.txt</File>
   <File>b.txt</File>
   <File>c.txt</File>
</Directory>

 - If the 1st arg, pParentNode, points to <Directory> and the 2nd arg,
   child_node_name, is "File", the function returns ["a.txt", "b.txt", "c.txt"]

*/
vector<string> GetTextContentsOfImmediateChildNodes( DOMNode *pParentNode,
													 const std::string& child_node_name )
{
	vector<DOMNode *> nodes = GetImmediateChildNodes( pParentNode, child_node_name );

	vector<string> text_contents;
	const size_t num_nodes = nodes.size();
	for( size_t i=0; i<num_nodes; i++ )
	{
		text_contents.push_back( to_string(nodes[i]->getTextContent()) );
	}

	return text_contents;
}

inline std::string GetAttributeText( DOMNode *pNode, const std::string& attrib_name )
{

	const DOMNamedNodeMap *pAttrib = pNode->getAttributes();

	if( !pAttrib )
	{
//		LOG_PRINT_WARNING( " - No attribute named " );
		return string();
	}

	DOMNode *pNameNode = pAttrib->getNamedItem( XercesString(attrib_name.c_str()) );

	return to_string(pNameNode->getNodeValue());
}


bool CGeometrySurfaceDesc::Load( DOMNode *pDescNode )
{
	m_Name = GetAttributeText( pDescNode, "name" );

	DOMNode *pLightmapNode = GetChildNode( pDescNode, "Lightmap" );

	string lightmap_enabled = GetAttributeText( pLightmapNode, "enabled" );

	if( lightmap_enabled == "true" )
	{
		m_UseLightmap = true;

		// load other lightmap parameters for each surface
	}
	else
		m_UseLightmap = false;

	DOMNode *pShaderNode = GetChildNode( pDescNode, "Shader" );
	if( pShaderNode )
	{
		m_ShaderFilepath  = GetTextContentOfImmediateChildNode( pShaderNode, "File" );

		m_ShaderTechnique = GetTextContentOfImmediateChildNode( pShaderNode, "Technique" );
	}

	return true;
}


bool LoadFilter( DOMNode *pFilterNode, CGeometryFilter::CTarget& rTarget )
{
	if( !pFilterNode )
		return false;

	rTarget.Layers =        GetTextContentsOfImmediateChildNodes( pFilterNode, "Layer" );
	rTarget.Surfaces =      GetTextContentsOfImmediateChildNodes( pFilterNode, "Surface" );
	rTarget.PolygonGroups = GetTextContentsOfImmediateChildNodes( pFilterNode, "PolygonGroup" );

	return true;
}


bool LoadGeometryFilter( DOMNode *pParentNode, CGeometryFilter& rDestFilter )
{
	if( !pParentNode )
		LOG_ERR_RETURN_FALSE( "An invalid node" );

	DOMNode *pFilterNode = GetChildNode( pParentNode, "GeometryFilterFile" );
	if( !pFilterNode )
		LOG_ERR_RETURN_FALSE( "A geometry filter node is missing." );

	CXMLDocumentLoader xml_document;
	xercesc_2_8::DOMDocument *pXMLDocument = NULL;
	if( !xml_document.Load( to_string(pFilterNode->getTextContent()), &pXMLDocument )
	 || !pXMLDocument )
		return false;

	DOMNode *pFilterRootNode = GetRootNode( pXMLDocument );
	if( !pFilterRootNode )
		return false;

	DOMNode *pIncludeFilter = GetChildNode( pFilterRootNode, "Include" );
	if( pIncludeFilter )
	{
		LoadFilter( pIncludeFilter, rDestFilter.Include );
	}

	DOMNode *pExcludeFilter = GetChildNode( pFilterRootNode, "Exclude" );
	if( pExcludeFilter )
	{
		LoadFilter( pExcludeFilter, rDestFilter.Exclude );
	}

	return true;
}


bool CStaticGeometryDesc::LoadSurfaceToDescMaps( DOMNode *pSurfaceToDescMapsNode )
{
	if( !pSurfaceToDescMapsNode )
		LOG_ERR_RETURN_FALSE( " - <SurfaceToDescMaps> node is missing." );

	vector<DOMNode *> surf_to_desc
		= GetImmediateChildNodes( pSurfaceToDescMapsNode, "Map" );

	// load surface desc -> surface mappings
	string surface_name, desc_name;
	for( size_t i=0; i<surf_to_desc.size(); i++ )
	{
		surface_name = GetAttributeText( surf_to_desc[i], "surface" );
		desc_name    = GetAttributeText( surf_to_desc[i], "desc" );
		m_SurfaceToDesc[surface_name] = desc_name;
	}

	return true;
}


bool CStaticGeometryDesc::LoadSurfaceDescs( DOMNode *pSurfaceNode )
{
	if( !pSurfaceNode )
		LOG_ERR_RETURN_FALSE( "A surface node is missing." );

	string surfdesc_filepath
		= GetTextContentOfImmediateChildNode( pSurfaceNode, "SurfaceDescFile" );

	CXMLDocumentLoader xml_document;
	xercesc_2_8::DOMDocument *pXMLDocument = NULL;
	if( !xml_document.Load( surfdesc_filepath, &pXMLDocument )
	 || !pXMLDocument )
		return false;

	DOMNode *pRootNode = GetRootNode( pXMLDocument );
	if( !pRootNode )
		return false;

	// load surface descs

	DOMNode *pDescsNode = GetChildNode( pRootNode, "Descs" );
	vector<DOMNode *> vecpDesc = GetImmediateChildNodes( pDescsNode, "Desc" );

	m_vecSurfaceDesc.resize( vecpDesc.size() );
	for( size_t i=0; i<vecpDesc.size(); i++ )
	{
		m_vecSurfaceDesc[i].Load( vecpDesc[i] ); 
	}

	return LoadSurfaceToDescMaps( GetChildNode( pRootNode, "SurfaceToDescMaps" ) );
}


bool CStaticGeometryDesc::LoadGraphicsDesc( DOMNode *pNode )
{
	if( !pNode )
		LOG_ERR_RETURN_FALSE( "A graphics desc node is missing." );

	if( !LoadGeometryFilter( pNode, m_GraphcisGeometryFilter ) )
		return false;

	LoadSurfaceDescs( GetChildNode( pNode, "Surface" ) );

	return true;
}


bool CStaticGeometryDesc::LoadCollisionDesc( DOMNode *pNode )
{
	if( !pNode )
		LOG_ERR_RETURN_FALSE( "A collision desc node is missing." );

	if( !LoadGeometryFilter( pNode, m_CollisionGeometryFilter ) )
		return false;

	return true;
}


bool CStaticGeometryDesc::LoadFromXML( const std::string& xml_filepath )
{
	xercesc_2_8::DOMDocument *pXMLDocument = NULL;

	CXMLDocumentLoader xml_doc_loader;
	bool bSuccess = xml_doc_loader.Load( xml_filepath, &pXMLDocument );
	if( !bSuccess )
		return false;

	// set the working directory to the directory path of the xml file
	fnop::dir_stack dir_stk;
	dir_stk.setdir( fnop::get_path(xml_filepath) );

	xercesc_2_8::DOMNode *pRootNode = GetRootNode( pXMLDocument );

	// get input file

	DOMNode *pInputNode = GetChildNode( pRootNode, "Input" );
	if( !pInputNode )
		LOG_ERR_RETURN_FALSE( "'Input' node not found." );

	// get child nodes

	DOMNode *pLWNode = GetChildNode( pInputNode, "LightWave" );
	if( !pLWNode )
		LOG_ERR_RETURN_FALSE( "'LightWave' node not found." );

	DOMNode *pInputFileNode = GetChildNode( pLWNode, "File" );
	if( pInputFileNode )
	{
		m_InputFilepath = to_string( pInputFileNode->getTextContent() );
		LOG_PRINT( "input model filepath:" + m_InputFilepath );
	}
	else
		LOG_ERR_RETURN_FALSE( "Cannot find an input model filepath" );


	// get output file

	DOMNode *pOutputNode = GetChildNode( pRootNode, "Output" );
	if( !pOutputNode )
		LOG_ERR_RETURN_FALSE( "'Output' node was not found." );

	DOMNode *pOutputFileNode = GetChildNode( pOutputNode, "File" );
	if( pOutputFileNode )
	{
		m_OutputFilepath = to_string( pOutputFileNode->getTextContent() );
		LOG_PRINT( "output filepath:" + m_OutputFilepath );
	}
	else
		LOG_ERR_RETURN_FALSE( "Cannot find an output filepath" );

	if( !LoadGraphicsDesc( GetChildNode( pRootNode, "Graphics" ) ) )
		return false;

	if( !LoadCollisionDesc( GetChildNode( pRootNode, "Collision" ) ) )
		return false;

	dir_stk.prevdir();

	return true;


/*
	http://xerces.apache.org/xerces-c/faq-other.html

	Does Xerces-C++ support XPath?	
	No. The Xerces-C++ 2.8.0 only has partial XPath implementation
	for the purposes of handling Schema identity constraints.
	For full XPath support, you can refer Apache Xalan C++
	or other Open Source Projects like Pathan.
*/
/*

//	xercesc_2_8::DOMXPathEvaluator evaluator;




//	const xercesc_2_8::DOMXPathNSResolver *pNSResolver = pXMLDocument->createNSResolver( pRootNode ); // error
	const xercesc_2_8::DOMXPathNSResolver *pNSResolver = pXMLDocument->createNSResolver( NULL ); // error

	XercesString xpath( "/root/Input/LightWave/File" );
//	const xercesc_2_8::DOMXPathExpression *pExpression = pXMLDocument->createExpression( xpath.begin(), NULL ); // error
	const xercesc_2_8::DOMXPathExpression *pExpression = pXMLDocument->createExpression( xpath.begin(), pNSResolver ); // error
	void *pResult;

	pExpression->evaluate( pRootNode, 0, pResult )
*/
}
