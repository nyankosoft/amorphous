#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"
//#include "Support/fnop.hpp"
#include "XML/xmlch2x.hpp"
#include "XML/XercesString.hpp"
#include "XML/XMLDocumentLoader.hpp"

#include "MotionDatabaseBuilder.hpp"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace msynth;


static std::vector< boost::shared_ptr<CMotionPrimitiveCompilerCreator> > sg_vecpExtAndMotionPrimitiveCompiler;


void msynth::RegisterMotionPrimitiveCompilerCreator( shared_ptr<CMotionPrimitiveCompilerCreator> pCreator )
{
	if( !pCreator )
		return;

	sg_vecpExtAndMotionPrimitiveCompiler.push_back( pCreator );
}


shared_ptr<CMotionDatabaseCompiler> CreateMotionPrimitiveCompiler( const std::string& filepath )
{
	// The extension returned by boost::filesystem::path::extension() includes "."
	// before the extension.
	string ext = filesystem::path(filepath).extension();

	BOOST_FOREACH( shared_ptr<CMotionPrimitiveCompilerCreator>& pCreator, sg_vecpExtAndMotionPrimitiveCompiler )
	{
		if( string(".") + pCreator->Extension() == ext )
			return pCreator->Create();
	}

	LOG_PRINT_ERROR( "No motion compiler for the motion file: " + filepath );
	return shared_ptr<CMotionDatabaseCompiler>();
/*
	sg_vecpExtAndMotionPrimitiveCompiler;

	if( ext == "bvh" )
	{
		return new CBVHMotionDatabaseCompiler;
	}
	else if( ext == "lws" )
	{
		return shared_ptr<CMotionDatabaseCompiler>();
//		return new CBVHMotionDatabaseCompiler;
	}
	else
		return shared_ptr<CMotionDatabaseCompiler>();
*/
}



void msynth::AlignLastKeyframe( std::vector<CKeyframe>& vecKeyframe )
{
	if( vecKeyframe.size() == 0 )
		return;

	CKeyframe& last_keyframe = vecKeyframe.back();

	Vector3 vLastRootPos = last_keyframe.GetRootPose().vPosition;
	vLastRootPos.y = 0;

	Scalar heading = - Vec3GetAngleBetween( Vector3(0,0,1), vLastRootPos );

	if( vLastRootPos.x < 0 )
		heading *= -1.0f;

	Matrix33 rotation_y = Matrix33RotationY( heading );

	BOOST_FOREACH( CKeyframe& keyframe, vecKeyframe )
	{
		keyframe.SetRootPose(
			Matrix34(
				rotation_y * keyframe.GetRootPose().vPosition,
				rotation_y * keyframe.GetRootPose().matOrient
			)
		);
	}


	// modify the last keyframe so that it faces the forward direction (0,0,1)

	Matrix34 last_root_pose = last_keyframe.GetRootPose();
	Vector3 vLastFwd = last_root_pose.matOrient.GetColumn(2);
	vLastFwd.y = 0;
	heading = - Vec3GetAngleBetween( Vector3(0,0,1), vLastFwd );

	if( 0.001 < heading )
	{
		if( vLastFwd.x < 0 )
			heading *= -1.0f;

		rotation_y = Matrix33RotationY( heading );

		last_root_pose.matOrient = rotation_y * last_root_pose.matOrient;
		last_keyframe.SetRootPose( last_root_pose );
	}
}



//==============================================================================
// CMotionDatabaseCompiler
//==============================================================================

int CMotionDatabaseCompiler::GetAnnotationIndex( const std::string& annotation_name )
{
	return get_str_index( annotation_name, *m_vecpAnnotationName );
}



//==============================================================================
// CMotionDatabaseBuilder
//==============================================================================

//bool CMotionDatabaseBuilder::CreateAnnotationTable( xercesc::DOMNode *pAnnotTableNode )
bool CMotionDatabaseBuilder::CreateAnnotationTable( CXMLNodeReader& annot_table_node )
{
	vector<CXMLNodeReader> vecChild = annot_table_node.GetImmediateChildren( "Annotation" );
	for( size_t i=0; i<vecChild.size(); i++ )
	{
		CXMLNodeReader& annot_node = vecChild[i];
		string annot_name = annot_node.GetTextContent();

		LOG_PRINT( " - Found a new annotation: " + annot_name );

		m_vecAnnotationName.push_back( annot_name );
	}
/*
	for( DOMNode *pNode = pAnnotTableNode->getFirstChild();
		 pNode;
		 pNode = pNode->getNextSibling() )
	{
		const string element_name = to_string( pNode->getNodeName() );

		if( element_name == "Annotation" )
		{
			string annot_name = to_string( pNode->getTextContent() );

			g_Log.Print( "found a new annotation: " + annot_name );

			m_vecAnnotationName.push_back( annot_name );
		}
	}
*/
	return true;
}


void CMotionDatabaseBuilder::ProcessRootNodeHorizontalElementOptions( xercesc::DOMNode *RootJointNode, CMotionPrimitiveDesc& desc )
{
	for( DOMNode *pNode = RootJointNode->getFirstChild();
		 pNode;
		 pNode = pNode->getNextSibling() )
	{
		const string element_name = to_string( pNode->getNodeName() );

		if( element_name == "ResetPos" )
		{
			desc.m_bResetHorizontalRootPos = to_bool( pNode->getTextContent(), "yes", "no" );

//			g_Log.Print( "reset horizontal root position: " + true_or_false );
		}
		else if( element_name == "NormalizeOrientation" )
		{
			string orient_opt = to_string( pNode->getTextContent() );

			desc.m_NormalizeOrientation = orient_opt;

//			if( orient_opt == "AverageMotionDirection" ) ;
//			else if( orient_opt == "LocalPositiveZDirection" ) ;
//			else if( orient_opt == "None" ) ;
		}
	}
}


void CMotionDatabaseBuilder::CreateMotionPrimitiveDesc( xercesc::DOMNode *pMotionNode )
{
	// found <MotionPrimitive> element

	const xercesc::DOMNamedNodeMap *pAttrib = pMotionNode->getAttributes();

	if( !pAttrib )
	{
		LOG_PRINT_ERROR( " - No attribute for input bvh file" );
		return; // no attribute - <File> element must have filepath attribute
	}

	const XercesString attrib_name("name");
	xercesc::DOMNode *pNameNode = pAttrib->getNamedItem( attrib_name.begin() );

	if( !pNameNode )
	{
		LOG_PRINT_ERROR( " - No name for motion primitive" );
		return;
	}

	// get the current desc group
	CMotionPrimitiveDescGroup& desc_group = m_vecDescGroup.back();

	CMotionPrimitiveDesc desc;

	string motion_primitive_name
		= xercesc::XMLString::transcode( XercesString(pNameNode->getNodeValue()) );

	// save the name of the motion primitive
	desc.m_Name = motion_primitive_name;

	for( DOMNode *pNode = pMotionNode->getFirstChild();
		 pNode;
		 pNode = pNode->getNextSibling() )
	{
		const string element_name
			= xercesc::XMLString::transcode( XercesString(pNode->getNodeName()) );

		if( element_name == "Frame" )
		{
			const xercesc::DOMNamedNodeMap *pFrameAttrib
				= pNode->getAttributes();

			if( pFrameAttrib )
			{
				xercesc::DOMNode *pStart, *pEnd;

				pStart = pFrameAttrib->getNamedItem(XercesString("start"));
				if( pStart )
					desc.m_StartFrame = to_int(pStart->getNodeValue());

				pEnd = pFrameAttrib->getNamedItem(XercesString("end"));
				if( pEnd )
					desc.m_EndFrame = to_int(pEnd->getNodeValue());
			}
		}
		else if( element_name == "Loop" )
		{
			const xercesc::DOMNamedNodeMap *pLoopAttrib
				= pNode->getAttributes();

			if( pLoopAttrib )
			{
				xercesc::DOMNode *pLoop = pLoopAttrib->getNamedItem(XercesString("loop"));
				if( pLoop )
					desc.m_bIsLoopMotion = to_bool(pLoop->getNodeValue());
			}
		}
		else if( element_name == "AnnotationList" )
		{
			for( DOMNode *pAnnotNode = pNode->getFirstChild();
				 pAnnotNode;
				 pAnnotNode = pAnnotNode->getNextSibling() )
			{
				if( to_string(pAnnotNode->getNodeName()) == "Annotation" )
				{
					desc.m_vecAnnotation.push_back( to_string(pAnnotNode->getTextContent()) );
				}
			}
		}
		else if( element_name == "RootJoint" )
		{
			for( DOMNode *pRJNode = pNode->getFirstChild();
				 pRJNode;
				 pRJNode = pRJNode->getNextSibling() )
			{
				if( to_string(pRJNode->getNodeName()) == "HorizontalElement" )
				{
					ProcessRootNodeHorizontalElementOptions( pRJNode, desc );
				}
			}
		}
	}

	desc_group.m_Desc.push_back( desc );
}


void CMotionDatabaseBuilder::CreateMotionPrimitiveDescGroup( xercesc::DOMNode *pBVHFileNode )
{
	// found a <File> element
	// - element that contains a bvh file

	const xercesc::DOMNamedNodeMap *pAttrib = pBVHFileNode->getAttributes();

	if( !pAttrib )
	{
		LOG_PRINT_ERROR( " - No attribute for input bvh file" );
		return; // no attribute - <File> element must have filepath attribute
	}

	xercesc::DOMNode *pNode = pAttrib->getNamedItem( XercesString("filepath") );

	if( !pNode )
	{
		LOG_PRINT_ERROR( " - No bvh filepath found" );
		return;
	}

	// add a new desc group
	m_vecDescGroup.push_back( CMotionPrimitiveDescGroup() );

	string src_filename = to_string(pNode->getNodeValue());

	path src_filepath = path(m_SourceXMLFilename).parent_path() / src_filename;

	m_vecDescGroup.back().m_Filename = src_filepath.string();

	bool res = IsValidMotionFile( src_filepath.string() );
	if( res )
	{
		// copy the skeleton structure of original bvh file
		LOG_PRINT( " - A valid motion file: " + src_filepath.string() );
//		m_vecDescGroup.back().m_Skeleton.CopyFromBVHSkeleton( *bvh_player.GetRootBone() );
	}
	else
	{
		LOG_PRINT_ERROR( " - A source motion file is not valid: " + src_filepath.string() );
		return;
	}

	// load scaling factor
	xercesc::DOMNode *pScalingNode = pAttrib->getNamedItem(  XercesString("scaling") );
	if( pScalingNode )
		m_vecDescGroup.back().m_fScalingFactor = to_float( to_string(pScalingNode->getNodeValue()) );

	for( DOMNode *pNode = pBVHFileNode->getFirstChild();
		 pNode;
		 pNode = pNode->getNextSibling() )
	{
		const XercesString node_name(pNode->getNodeName());

		if( node_name != XercesString("MotionPrimitive") )
			continue;

		CreateMotionPrimitiveDesc( pNode );

//		const XMLCh *pName = pNode->getNodeName();
	}
}


void CMotionDatabaseBuilder::CreateMotionTableEntry( xercesc::DOMNode *pMotionEntryNode, CHumanoidMotionEntry& entry )
{
	for( DOMNode *pNode = pMotionEntryNode->getFirstChild();
		 pNode;
		 pNode = pNode->getNextSibling() )
	{
		const string node_name = to_string(pNode->getNodeName());
		if( node_name == "MotionPrimitive" )
		{
			const xercesc::DOMNamedNodeMap *pAttrib = pNode->getAttributes();

			if( !pAttrib )
			{
				LOG_PRINT_ERROR( " - No attribute for motion primitive in HumanoidMotionTable" );
				return;
			}

			xercesc::DOMNode *pAttribNode = pAttrib->getNamedItem( XercesString("name") );

			if( !pAttribNode )
			{
				LOG_PRINT_ERROR( "" );
				return;
			}

			const string motion_primitive_name = to_string(pAttribNode->getNodeValue());

			entry.m_vecMotionPrimitiveName.push_back( motion_primitive_name );
		}
	}
}


void CMotionDatabaseBuilder::CreateMotionTable( xercesc::DOMNode *pMotionTableNode )
{
	const xercesc::DOMNamedNodeMap *pTableAttrib = pMotionTableNode->getAttributes();

	xercesc::DOMNode *pNameAttrib = pTableAttrib->getNamedItem(XercesString("name"));

	if( !pNameAttrib )
	{
		LOG_PRINT_ERROR( " - No name attribute for HumanoidMotionTable" );
	}

	m_MotionTable.m_Name = to_string(pNameAttrib->getNodeValue());

	for( DOMNode *pNode = pMotionTableNode->getFirstChild();
		 pNode;
		 pNode = pNode->getNextSibling() )
	{
		const string node_name = to_string(pNode->getNodeName());
		if( node_name == "Motion" )
		{
			const xercesc::DOMNamedNodeMap *pAttrib = pNode->getAttributes();

			if( !pAttrib )
			{
				LOG_PRINT_ERROR( " - No attribute for motion in HumanoidMotionTable" );
				return;
			}

			xercesc::DOMNode *pAttribNode = pAttrib->getNamedItem( XercesString("type") );

			if( !pAttribNode )
			{
				LOG_PRINT_ERROR( "" );
				return;
			}

			const string motion_type = to_string(pAttribNode->getNodeValue());

			m_MotionTable.m_vecEntry.push_back( CHumanoidMotionEntry() );
			CHumanoidMotionEntry& entry = m_MotionTable.m_vecEntry.back();
			entry.m_MotionType = motion_type;

			CreateMotionTableEntry( pNode, entry );
		}
	}

	return;
}


void CMotionDatabaseBuilder::ProcessXMLFile( CXMLNodeReader& root_node )
//void CMotionDatabaseBuilder::ProcessXMLFile( xercesc::DOMNode *pRootNode )
{
	xercesc::DOMNode *pRootNode = root_node.GetDOMNode();

	string output_filename;
	root_node.GetChildElementTextContent( "Output", output_filename );
	m_OutputFilepath = path( path(m_SourceXMLFilename).parent_path() / output_filename ).string();

	if( m_OutputFilepath.length() == 0 )
	{
		LOG_PRINT_WARNING( "Output filepath was not found in source XML file. Call SaveMotionDatabaseToFile() to save motion database to disk."  );
	}

	for( DOMNode *pFileNode = pRootNode->getFirstChild();
		 pFileNode;
		 pFileNode = pFileNode->getNextSibling() )
	{
		const string node_name = to_string(pFileNode->getNodeName());

		if( node_name == "File" )
		{
			CreateMotionPrimitiveDescGroup( pFileNode );
		}
		else if( node_name == "HumanoidMotionTable" )
		{
			CreateMotionTable( pFileNode );
		}
		else if( node_name == "AnnotationTable" )
		{
			CXMLNodeReader node(pFileNode);
			CreateAnnotationTable( node );
		}


//		const XMLCh *pName = pNode->getNodeName();
	}
}


void CMotionDatabaseBuilder::CreateMotionPrimitives()
{
	m_vecMotionPrimitive.reserve( 256 );

	BOOST_FOREACH( CMotionPrimitiveDescGroup& desc_group, m_vecDescGroup )
	{
		shared_ptr<CMotionDatabaseCompiler> pCompiler = CreateMotionPrimitiveCompiler( desc_group.m_Filename );

		if( !pCompiler )
		{
			continue;
		}

		pCompiler->m_vecpMotionPrimitive = &m_vecMotionPrimitive;
		pCompiler->m_vecpAnnotationName  = &m_vecAnnotationName;
		pCompiler->m_pMotionTable        = &m_MotionTable;

		pCompiler->CreateMotionPrimitives( desc_group );
	}
}


bool CMotionDatabaseBuilder::Build( const std::string& source_script_filename )
{
	m_SourceXMLFilename = source_script_filename;

	shared_ptr<CXMLDocument> pXMLDocument;

	// (S)

	CXMLDocumentLoader xml_doc_loader;
	pXMLDocument = xml_doc_loader.Load( source_script_filename );
	if( !pXMLDocument )
		return false;

	CXMLNodeReader root_node_reader = pXMLDocument->GetRootNodeReader();

	xercesc::DOMNode *pRootNode = root_node_reader.GetDOMNode();

	// (E)

	// set the working directory to the directory path of source_script_filename
	// - Do this after (S) - (E), or the exception ocurrs
//	fnop::dir_stack dirstk( fnop::get_path(source_script_filename) );

	XercesString xer_root_name = XercesString("root");
	const XMLCh* pCurrentNodeName = pRootNode->getNodeName();
	if( XercesString(pRootNode->getNodeName()) == xer_root_name )
	{
		ProcessXMLFile( root_node_reader );
	}

	// create motion primitives from descs
	CreateMotionPrimitives();

	// restore the original working directory (pop)
//	dirstk.prevdir();

	if( 0 < m_OutputFilepath.length() )
		return SaveMotionDatabaseToFile( m_OutputFilepath );

	return true;
}


bool CMotionDatabaseBuilder::SaveMotionDatabaseToFile( const std::string& db_filename )
{
	CMotionDatabase db;
	bool success = db.m_DB.Open( db_filename, CBinaryDatabase<string>::DB_MODE_NEW );

	if( !success )
		return false;

	/// annotation table
	if( 0 < m_vecAnnotationName.size() )
		db.m_DB.AddData( "__AnnotationTable__", m_vecAnnotationName );

	/// humanoid motion table
	if( 0 < m_MotionTable.m_Name.length() )
		db.m_DB.AddData( m_MotionTable.m_Name, m_MotionTable );

	BOOST_FOREACH( CMotionPrimitive& motion, m_vecMotionPrimitive )
	{
		db.m_DB.AddData( motion.GetName(), motion );
	}

	return true;
}
