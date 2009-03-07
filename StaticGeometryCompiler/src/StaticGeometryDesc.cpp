#include "StaticGeometryDesc.h"
#include "Support/Log/DefaultLog.hpp"
#include "Support/fnop.hpp"
#include "XML.hpp"
#include "3DMath/AABTree.hpp"

//#include <dae.h>

using namespace boost;

/*
class CXMLDocument
{
public:
};

	CXMLDocument xml_doc( "" );
	if( !xml_doc.IsValid() )
		return false;

	xml_doc.GetRootNodeReader();
*/

#define LOG_ERR_RETURN_FALSE( msg ) { LOG_PRINT_ERROR( msg ); return false; }

//#define GET_CHILD_ELEMENT_TEXT_CONTENT( node_reader, text ) node_reader.GetChildElementTextContent( #text, text )


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

	CXMLDocumentLoader xml_doc_loader;
	shared_ptr<CXMLDocument> pXMLDocument = xml_doc_loader.Load( to_string(pFilterNode->getTextContent()) );
	if( !pXMLDocument )
		return false;

	DOMNode *pFilterRootNode = pXMLDocument->GetRootNodeReader().GetDOMNode();
//	DOMNode *pFilterRootNode = GetRootNode( pXMLDocument );
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



//=====================================================================
// CTextureSubdivisionOptions
//=====================================================================

bool CTextureSubdivisionOptions::Load( CXMLNodeReader& node_reader )
{
	if( node_reader.IsValid() )
		m_Enabled = true;
	else
	{
		m_Enabled = false;
		return false;
	}

	node_reader.GetChildElementTextContent( "OutputImageFormat", m_OutputImageFormat ); // image format
	node_reader.GetChildElementTextContent( "SplitSize",         m_SplitSize ); // split size
	node_reader.GetChildElementTextContent( "TargetSurfaceName", m_TargetSurfaceName ); 

	return true;
}



//=====================================================================
// CMeshTreeOptions
//=====================================================================

bool CMeshTreeOptions::Load( CXMLNodeReader& node_reader )
{
//	GET_CHILD_ELEMENT_TEXT_CONTENT( node_reader, MinimumCellVolume );
	node_reader.GetChildElementTextContent( "MinimumCellVolume"       , MinimumCellVolume );
	node_reader.GetChildElementTextContent( "NumMaxGeometriesPerCell" , NumMaxGeometriesPerCell );
	node_reader.GetChildElementTextContent( "MaxDepth"                , MaxDepth );

	string cond;
	node_reader.GetChildElementTextContent( "RecursionStopCondition"  , cond );
	if( cond == "OR" )       RecursionStopCondition = CAABTree<int>::COND_OR;
	else if( cond == "AND" ) RecursionStopCondition = CAABTree<int>::COND_AND;
	else LOG_PRINT_ERROR( "An invalid recursion condition: " + cond );

	return true;
}


//=====================================================================
// CGeometrySurfaceDesc
//=====================================================================

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

/*	CXMLNodeReader node_reader( pDescNode );
	m_Name = node_reader.GetAttributeText( "name" );

	CXMLNodeReader lightmap_node_reader = node_reader.GetChild( "Lightmap" );
*/
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

	CXMLDocumentLoader xml_doc_loader;
	shared_ptr<CXMLDocument> pXMLDocument = xml_doc_loader.Load( surfdesc_filepath );
	if( !pXMLDocument )
		return false;

	CXMLNodeReader root_node_reader = pXMLDocument->GetRootNodeReader();
	DOMNode *pRootNode = root_node_reader.GetDOMNode();
//	DOMNode *pRootNode = GetRootNode( pXMLDocument );
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


bool CStaticGeometryDesc::LoadShaderOptions( DOMNode *pShaderNode )
{
	if( !pShaderNode )
	{
		LOG_ERR_RETURN_FALSE( "A shader node is missing." );
		return false;
	}

	string shaderparam_filepath
		= GetTextContentOfImmediateChildNode( pShaderNode, "ShaderParamFile" );

	if( 0 < shaderparam_filepath.length() )
	{
		LoadShaderParamsFromFile( shaderparam_filepath );
	}

	return true;
}


bool CStaticGeometryDesc::LoadShaderParamsFromFile( const std::string& shaderparam_filepath )
{
	static const int max_texture_stages = 8;

	CXMLDocumentLoader xml_doc_loader;
	shared_ptr<CXMLDocument> pXMLDocument = xml_doc_loader.Load( shaderparam_filepath );
	if( !pXMLDocument )
		return false;

	CXMLNodeReader root_node_reader = pXMLDocument->GetRootNodeReader();
	DOMNode *pRootNode = root_node_reader.GetDOMNode();
//	DOMNode *pRootNode = GetRootNode( pXMLDocument );
	if( !pRootNode )
		return false;

	vector<DOMNode *> vecpShader = GetImmediateChildNodes( pRootNode, "Shader" );
	for( size_t i=0; i<vecpShader.size(); i++ )
	{
		string shader_filepath = GetTextContentOfImmediateChildNode( vecpShader[i], "File" );

		if( shader_filepath.length() == 0 )
			continue;

		CShaderParameterGroup param_group;

		vector<DOMNode *> vecpTex = GetImmediateChildNodes( vecpShader[i], "Texture" );
		for( size_t j=0; j<vecpTex.size(); j++ )
		{
			int index = 0;
			string stage_str = GetAttributeText( vecpTex[j], "stage" );
			if( 0 < stage_str.length() )
			{
				int stage = to_int(stage_str);
				clamp( stage, 0, max_texture_stages );
				while( (int)param_group.m_Texture.size() <= stage )
					param_group.m_Texture.push_back( CShaderParameter<CTextureParam>() );
				index = stage;
			}
			else
			{
				// simply add another texture param
				index = (int)param_group.m_Texture.size();
				param_group.m_Texture.push_back( CShaderParameter<CTextureParam>() );
			}

			DOMNode *pTexFile = GetChildNode( vecpTex[j], "File" );

			param_group.m_Texture[index].Parameter().m_Desc.ResourcePath
				= to_string(pTexFile->getTextContent());
		}

		m_ShaderFileToParamGroup[shader_filepath] = param_group;
	}

	return true;
}


/*
void LoadPointLights( vector<DOMNode *>& vecpNode )
{
	vecpNode;
	for( size_t i=0; i<vecpDesc.size(); i++ )
	{
	}
}
*/


void CStaticGeometryDesc::LoadLightsFromColladaFile( const std::string& dae_filepath )
{/*
	DAE dae;
	domCOLLADA *pDOM = dae.open( dae_filepath );

	// The DAE::open() call above causes bad allocation exception. Why?
*/
}


void CStaticGeometryDesc::LoadLights( DOMNode *pLightsNode )
{
//	vector<shared_ptr<CLight>> vecpLight;

	xercesc::DOMNodeList *pNodeList = pLightsNode->getChildNodes();
	const int num_nodes = (int)pNodeList->getLength();
	for( int i=0; i<num_nodes; i++ )
	{
		xercesc::DOMNode *pNode = pNodeList->item(i);
		CXMLNodeReader node( pNode );

		string light_type = to_string(pNode->getNodeName());

		if( light_type == "AmbientLight" )
		{
			CAmbientLight amb_light;
			node.GetChildElementTextContentRGB( "Color", amb_light.DiffuseColor );
			m_vecpLight.push_back( shared_ptr<CLight>( new CAmbientLight(amb_light) ) );
		}
		else if( light_type == "DirectionalLight" )
		{
			CDirectionalLight dir_light;
			node.GetChildElementTextContentRGB( "Color",     dir_light.DiffuseColor );
			node.GetChildElementTextContent( "Direction", dir_light.vDirection );

			m_vecpLight.push_back( shared_ptr<CLight>( new CDirectionalLight(dir_light) ) );
		}
		else if( light_type == "PointLight" )
		{
			CPointLight pnt_light;
			node.GetChildElementTextContentRGB( "Color",    pnt_light.DiffuseColor );
			node.GetChildElementTextContent( "Position", pnt_light.vPosition );

			Vector3 vAttenu;
			if( node.GetChildElementTextContent( "Attenuation", vAttenu ) )
			{
				for( size_t j=0; j<3; j++ )
					pnt_light.fAttenuation[j] = vAttenu[j];
			}

			m_vecpLight.push_back( shared_ptr<CLight>( new CPointLight(pnt_light) ) );
		}
	}

/*	vector<DOMNode *> vecpNode
		= GetImmediateChildNodes( pLightsNode, "AmbientLight" );

	vecpNode
		= GetImmediateChildNodes( pLightsNode, "PointLight" );

	LoadPointLights( vecpNode, vecpLight );*/
}


bool CStaticGeometryDesc::LoadLightingDesc( DOMNode *pLightingNode )
{
	if( !pLightingNode )
		return false;

//	DOMNode *pLightFileNode = GetChildNode( pNode, "LightFile" );

	string light_filepath
		= GetTextContentOfImmediateChildNode( pLightingNode, "LightFile" );

	if( 0 < light_filepath.length() )
	{
		// load lights from custom lights file
		CXMLDocumentLoader xml_doc_loader;
		shared_ptr<CXMLDocument> pXMLDocument = xml_doc_loader.Load( light_filepath );
		if( !pXMLDocument )
			return false;

		DOMNode *pRootNode = pXMLDocument->GetRootNodeReader().GetDOMNode();
//		DOMNode *pRootNode = GetRootNode( pXMLDocument );
		if( !pRootNode )
			return false;

		LoadLights( GetChildNode( pRootNode, "Lights" ) );
	}

/*	string dae_filepath
		= GetTextContentOfImmediateChildNode( pLightingNode, "DAEFile" );

	if( 0 < dae_filepath.length() )
	{
		LoadLightsFromColladaFile( dae_filepath );
	}*/

	return true;
}


bool CStaticGeometryDesc::LoadGraphicsDesc( DOMNode *pNode )
{
	if( !pNode )
		LOG_ERR_RETURN_FALSE( "A graphics desc node is missing." );

	if( !LoadGeometryFilter( pNode, m_GraphcisGeometryFilter ) )
		return false;

	LoadSurfaceDescs( GetChildNode( pNode, "Surface" ) );

	LoadLightingDesc( GetChildNode( pNode, "Lighting" ) );

	m_Lightmap.Load( GetChildNode( pNode, "Lightmap" ) );

//	CXMLNodeReader texdiv_options_reader( GetChildNode( pNode, "TextureSubdivision" ) );
	LoadTextureSubdivisionOptions( GetChildNode( pNode, "TextureSubdivision" ) );

//	CXMLNodeReader meshtree_options_reader(  );
	LoadMeshTreeOptions( GetChildNode( pNode, "MeshTreeOptions" ) );

	LoadShaderOptions( GetChildNode( pNode, "Shader" ) );

	return true;
}


bool CStaticGeometryDesc::LoadTextureSubdivisionOptions( DOMNode *pNode )
{
	DOMNode *pFileNode = GetChildNode( pNode, "TextureSubdivisionFile" );
	if( pFileNode )
	{
		CXMLDocumentLoader xml_doc_loader;
		shared_ptr<CXMLDocument> pXMLDocument
			= xml_doc_loader.Load( to_string(pFileNode->getTextContent()) );
		if( !pXMLDocument )
			return false;

		CXMLNodeReader node_reader = pXMLDocument->GetRootNodeReader();//( GetRootNode( pXMLDocument ) );
		m_TextureSubdivisionOptions.Load( node_reader ); // "TextureSubdivision"
	}

	return true;
}


bool CStaticGeometryDesc::LoadMeshTreeOptions( DOMNode *pNode )
{
	DOMNode *pFileNode = GetChildNode( pNode, "MeshTreeOptionsFile" );
	if( pFileNode )
	{
		CXMLDocumentLoader xml_doc_loader;
		shared_ptr<CXMLDocument> pXMLDocument
			= xml_doc_loader.Load( to_string(pFileNode->getTextContent()) );
		if( !pXMLDocument )
			return false;

		CXMLNodeReader node_reader = pXMLDocument->GetRootNodeReader();//( GetRootNode( pXMLDocument ) );
		m_MeshTreeOptions.Load( node_reader ); // "TextureSubdivision"
	}

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
	CXMLDocumentLoader xml_doc_loader;
	shared_ptr<CXMLDocument> pXMLDocument = xml_doc_loader.Load( xml_filepath );
	if( !pXMLDocument )
		return false;

	// set the working directory to the directory path of the xml file
	fnop::dir_stack dir_stk;
	dir_stk.setdir( fnop::get_path(xml_filepath) );

	CXMLNodeReader root_node_reader = pXMLDocument->GetRootNodeReader();
	xercesc::DOMNode *pRootNode = root_node_reader.GetDOMNode();
//	xercesc::DOMNode *pRootNode = GetRootNode( pXMLDocument );

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

	m_ProgramRootDirectoryPath = GetTextContentOfImmediateChildNode( pOutputNode, "ProgramRootDirectoryPath" );

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

//	xercesc::DOMXPathEvaluator evaluator;




//	const xercesc::DOMXPathNSResolver *pNSResolver = pXMLDocument->createNSResolver( pRootNode ); // error
	const xercesc::DOMXPathNSResolver *pNSResolver = pXMLDocument->createNSResolver( NULL ); // error

	XercesString xpath( "/root/Input/LightWave/File" );
//	const xercesc::DOMXPathExpression *pExpression = pXMLDocument->createExpression( xpath.begin(), NULL ); // error
	const xercesc::DOMXPathExpression *pExpression = pXMLDocument->createExpression( xpath.begin(), pNSResolver ); // error
	void *pResult;

	pExpression->evaluate( pRootNode, 0, pResult )
*/
}
