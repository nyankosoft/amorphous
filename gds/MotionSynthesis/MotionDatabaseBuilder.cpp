#include <boost/foreach.hpp>
#include "BVH/BVHPlayer.h"
#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/fnop.h"
#include "XML/xmlch2x.h"
#include "XML/XercesString.h"
#include "XML/XMLDocumentLoader.h"
namespace xer = xercesc_2_8;
//#define xer xercesc_2_8;

#include "MotionDatabaseBuilder.h"

using namespace msynth;


inline float to_float( const std::string& input_str )
{
	return (float)atof( input_str.c_str() );
}


inline int get_str_index( const std::string& input_str, const std::vector<std::string>& reference_str )
{
	const size_t num_elements = reference_str.size();
	for( size_t i=0; i<num_elements; i++ )
	{
		if( input_str == reference_str[i] )
			return (int)i;
	}

	return -1;
}


int CMotionDatabaseBuilder::GetAnnotationIndex( const std::string& annotation_name )
{
	return get_str_index( annotation_name, m_vecAnnotationName );
}


static void AlignLastKeyframe( vector<CKeyframe>& vecKeyframe )
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


void CMotionDatabaseBuilder::CreateMotionPrimitive( const CMotionPrimitiveDesc& desc,
												    const CMotionPrimitiveDescGroup& desc_group,
													CBVHPlayer& bvh_player )
{
	vector<CKeyframe> vecKeyframe;
	vecKeyframe.reserve( 64 );

	// add a new motion primitive and get the reference to it
	m_vecMotionPrimitive.push_back( CMotionPrimitive( desc.m_Name ) );
	CMotionPrimitive& motion = m_vecMotionPrimitive.back();

	motion.SetSkeleton( desc_group.m_Skeleton );

	motion.SetLoopedMotion( desc.m_bIsLoopMotion );

	vecKeyframe.resize(0);

	int i = 0;
	int frame = 0;
	int start = desc.m_StartFrame;
	int end   = desc.m_EndFrame;
	float time_per_frame = bvh_player.GetFrameTime();

	// create keyframes
	for( frame = start, i=0;
		 frame <= end;
		 frame++,       i++ )
	{
		float frame_time = (float)frame * time_per_frame;

		bvh_player.SetWorldTransformation( frame_time );

		// extract world transforms

		CBVHBone *pRootBone = bvh_player.GetRootBone();

		// create keyframe from frametime and transformation hierarchy
		vecKeyframe.push_back( CKeyframe( (float)i * time_per_frame, CTransformNode( *pRootBone ) ) );
	}

	// modify root position
	if( 0 < vecKeyframe.size() && desc.m_bResetHorizontalRootPos )
	{
		Matrix34 root_pose = vecKeyframe[0].GetRootPose();
		Vector3 vBasePosH = root_pose.vPosition;
		vBasePosH.y = 0;

		BOOST_FOREACH( CKeyframe& keyframe, vecKeyframe )
		{
			root_pose = keyframe.GetRootPose();
			root_pose.vPosition = root_pose.vPosition - vBasePosH;
			keyframe.SetRootPose( root_pose );
		}
	}

	if( desc.m_NormalizeOrientation == "AlignLastKeyframe" )
		AlignLastKeyframe( vecKeyframe );

	if( 0.0001 < abs(1.0 - desc_group.m_fScalingFactor) )
	{
		BOOST_FOREACH( CKeyframe& keyframe, vecKeyframe )
		{
			keyframe.Scale( desc_group.m_fScalingFactor );
		}
	}

	motion.SetKeyframes( vecKeyframe );

	// set annotations
	motion.m_vecAnnotation.resize( m_vecAnnotationName.size(), 0 );

	BOOST_FOREACH( const std::string& annot, desc.m_vecAnnotation )
	{
		int index = GetAnnotationIndex( annot );
		if( 0 <= index )
			motion.m_vecAnnotation[index] = 1;
	}
}


void CMotionDatabaseBuilder::CreateMotionPrimitives()
{
	m_vecMotionPrimitive.reserve( 256 );

	BOOST_FOREACH( CMotionPrimitiveDescGroup& desc_group, m_vecDescGroup )
	{
		CBVHPlayer bvh_player;
		bool loaded = bvh_player.LoadBVHFile( desc_group.m_Filename );
		if( !loaded )
		{
			LOG_PRINT_ERROR( " - Failed to load the bvh file: " + desc_group.m_Filename );
			return;
		}

		// copy skeleton structure
		desc_group.m_Skeleton.CopyFromBVHSkeleton( *bvh_player.GetRootBone() );

		desc_group.m_Skeleton.Scale( desc_group.m_fScalingFactor );

		BOOST_FOREACH( CMotionPrimitiveDesc& desc, desc_group.m_Desc )
		{
			CreateMotionPrimitive( desc, desc_group, bvh_player );
		}
	}

}


bool CMotionDatabaseBuilder::CreateAnnotationTable( xercesc_2_8::DOMNode *pAnnotTableNode )
{
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

	return true;
}


void CMotionDatabaseBuilder::ProcessRootNodeHorizontalElementOptions( xercesc_2_8::DOMNode *RootJointNode, CMotionPrimitiveDesc& desc )
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


void CMotionDatabaseBuilder::CreateMotionPrimitiveDesc( xercesc_2_8::DOMNode *pMotionNode )
{
	// found <MotionPrimitive> element

	const xercesc_2_8::DOMNamedNodeMap *pAttrib = pMotionNode->getAttributes();

	if( !pAttrib )
	{
		LOG_PRINT_ERROR( " - No attribute for input bvh file" );
		return; // no attribute - <File> element must have filepath attribute
	}

	const XercesString attrib_name("name");
	xercesc_2_8::DOMNode *pNameNode = pAttrib->getNamedItem( attrib_name.begin() );

	if( !pNameNode )
	{
		LOG_PRINT_ERROR( " - No name for motion primitive" );
		return;
	}

	// get the current desc group
	CMotionPrimitiveDescGroup& desc_group = m_vecDescGroup.back();

	CMotionPrimitiveDesc desc;

	string motion_primitive_name
		= xercesc_2_8::XMLString::transcode( XercesString(pNameNode->getNodeValue()) );

	// save the name of the motion primitive
	desc.m_Name = motion_primitive_name;

	for( DOMNode *pNode = pMotionNode->getFirstChild();
		 pNode;
		 pNode = pNode->getNextSibling() )
	{
		const string element_name
			= xercesc_2_8::XMLString::transcode( XercesString(pNode->getNodeName()) );

		if( element_name == "Frame" )
		{
			const xercesc_2_8::DOMNamedNodeMap *pFrameAttrib
				= pNode->getAttributes();

			if( pFrameAttrib )
			{
				xercesc_2_8::DOMNode *pStart, *pEnd;

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
			const xercesc_2_8::DOMNamedNodeMap *pLoopAttrib
				= pNode->getAttributes();

			if( pLoopAttrib )
			{
				xercesc_2_8::DOMNode *pLoop = pLoopAttrib->getNamedItem(XercesString("loop"));
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


void CMotionDatabaseBuilder::CreateMotionPrimitiveDescGroup( xercesc_2_8::DOMNode *pBVHFileNode )
{
	// found a <File> element
	// - element that contains a bvh file

	const xercesc_2_8::DOMNamedNodeMap *pAttrib = pBVHFileNode->getAttributes();

	if( !pAttrib )
	{
		LOG_PRINT_ERROR( " - No attribute for input bvh file" );
		return; // no attribute - <File> element must have filepath attribute
	}

	xercesc_2_8::DOMNode *pNode = pAttrib->getNamedItem( XercesString("filepath") );

	if( !pNode )
	{
		LOG_PRINT_ERROR( " - No bvh filepath found" );
		return;
	}

	// add a new desc group
	m_vecDescGroup.push_back( CMotionPrimitiveDescGroup() );

	string bvh_filepath = to_string(pNode->getNodeValue());

	m_vecDescGroup.back().m_Filename = bvh_filepath;

	CBVHPlayer bvh_player;
	bool success = bvh_player.LoadBVHFile( bvh_filepath );
	if( success )
	{
		// copy the skeleton structure of original bvh file
		m_vecDescGroup.back().m_Skeleton.CopyFromBVHSkeleton( *bvh_player.GetRootBone() );
	}
	else
	{
		LOG_PRINT_ERROR( " - Failed to load a bvh file: " + bvh_filepath );
		return;
	}

	// load scaling factor
	xercesc_2_8::DOMNode *pScalingNode = pAttrib->getNamedItem(  XercesString("scaling") );
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


void CMotionDatabaseBuilder::CreateMotionTableEntry( xercesc_2_8::DOMNode *pMotionEntryNode, CHumanoidMotionEntry& entry )
{
	for( DOMNode *pNode = pMotionEntryNode->getFirstChild();
		 pNode;
		 pNode = pNode->getNextSibling() )
	{
		const string node_name = to_string(pNode->getNodeName());
		if( node_name == "MotionPrimitive" )
		{
			const xercesc_2_8::DOMNamedNodeMap *pAttrib = pNode->getAttributes();

			if( !pAttrib )
			{
				LOG_PRINT_ERROR( " - No attribute for motion primitive in HumanoidMotionTable" );
				return;
			}

			xercesc_2_8::DOMNode *pAttribNode = pAttrib->getNamedItem( XercesString("name") );

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


void CMotionDatabaseBuilder::CreateMotionTable( xercesc_2_8::DOMNode *pMotionTableNode )
{
	const xercesc_2_8::DOMNamedNodeMap *pTableAttrib = pMotionTableNode->getAttributes();

	xercesc_2_8::DOMNode *pNameAttrib = pTableAttrib->getNamedItem(XercesString("name"));

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
			const xercesc_2_8::DOMNamedNodeMap *pAttrib = pNode->getAttributes();

			if( !pAttrib )
			{
				LOG_PRINT_ERROR( " - No attribute for motion in HumanoidMotionTable" );
				return;
			}

			xercesc_2_8::DOMNode *pAttribNode = pAttrib->getNamedItem( XercesString("type") );

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


void CMotionDatabaseBuilder::ProcessXMLFile( xercesc_2_8::DOMNode *pRootNode )
{
	for( DOMNode *pFileNode = pRootNode->getFirstChild();
		 pFileNode;
		 pFileNode = pFileNode->getNextSibling() )
	{
		const XercesString node_name = XercesString(pFileNode->getNodeName());

		if( node_name == XercesString("File") )
		{
			CreateMotionPrimitiveDescGroup( pFileNode );
		}
		else if( node_name == XercesString("HumanoidMotionTable") )
		{
			CreateMotionTable( pFileNode );
		}
		else if( node_name == XercesString("AnnotationTable") )
		{
			CreateAnnotationTable( pFileNode );
		}


//		const XMLCh *pName = pNode->getNodeName();
	}
}



bool CMotionDatabaseBuilder::Build( const std::string& source_script_filename )
{
	xercesc_2_8::DOMDocument *pXMLDocument = NULL;

	// (S)

	CXMLDocumentLoader xml_doc_loader;
	xml_doc_loader.Load( source_script_filename, &pXMLDocument );

	xer::DOMElement *pElem = pXMLDocument->getDocumentElement();

	xercesc_2_8::DOMNodeIterator *iterator
		= pXMLDocument->createNodeIterator( pXMLDocument->getFirstChild(), xer::DOMNodeFilter::SHOW_TEXT, NULL, false );

	xercesc_2_8::DOMNode *pRootNode = iterator->getRoot();

	// (E)

	// set the working directory to the directory path of source_script_filename
	// - Do this after (S) - (E), or the exception ocurrs
	fnop::dir_stack dirstk( fnop::get_path(source_script_filename) );

	XercesString xer_root_name = XercesString("root");
	const XMLCh* pCurrentNodeName = pRootNode->getNodeName();
	if( XercesString(pRootNode->getNodeName()) == xer_root_name )
	{
		ProcessXMLFile( pRootNode );
	}

	// create motion primitives from descs
	CreateMotionPrimitives();

	// restore the original working directory (pop)
	dirstk.prevdir();

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

	/// humanod motion table
	if( 0 < m_MotionTable.m_Name.length() )
		db.m_DB.AddData( m_MotionTable.m_Name, m_MotionTable );

	BOOST_FOREACH( CMotionPrimitive& motion, m_vecMotionPrimitive )
	{
		db.m_DB.AddData( motion.GetName(), motion );
	}

	return true;
}
