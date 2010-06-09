#include "MotionDatabaseBuilder.hpp"
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"
#include "XML/XMLDocumentLoader.hpp"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace msynth;


inline bool to_bool( const std::string& src, const char *true_str = "true", const char *false_str = "false" )
{
	if( src == true_str )
		return true;
	else if( src == false_str )
		return false;
	else
		return false;
}


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

	return true;
}


void CopyTransformNodesOfBone(
	const vector<int>& src_locator,
	const vector<CKeyframe>& src_keyframes,
//	CSkeleton& src_skeleton,
//	pSrcSkeleton->GetRootBone(),
	const vector<int>& dest_locator,
	vector<CKeyframe>& dest_keyframes
//	CSkeleton& dest_skeleton,
//	pDestSkeleton->GetRootBone()
)
{
	int num_keyframes = (int)src_keyframes.size();
	for( int i=0; i<num_keyframes; i++ )
	{
		Transform pose = src_keyframes[i].GetTransform( src_locator );
		dest_keyframes[i].SetTransform( pose, dest_locator );
	}
}


Result::Name CMotionDatabaseBuilder::MapMotionPrimitivesToAnotherSkeleton()
{
	CMotionMapTarget& tgt = m_MotionMapTarget;

	path mdb_path = path(m_SourceXMLFilename).parent_path() / tgt.m_DestSkeletonMDB;
	CMotionDatabase dest_skeleton_src_mdb( mdb_path.string() );
	shared_ptr<CMotionPrimitive> pMotion = dest_skeleton_src_mdb.GetMotionPrimitive( tgt.m_DestSkeletonMotion );
	if( !pMotion )
		return Result::UNKNOWN_ERROR;

	shared_ptr<CSkeleton> pDestSkeleton = pMotion->GetSkeleton();

	if( m_vecMotionPrimitive.empty() )
		return Result::UNKNOWN_ERROR;

	shared_ptr<CSkeleton> pSrcSkeleton = m_vecMotionPrimitive.front().GetSkeleton();

/*	for( map<string,string>::iterator itr = bone_maps.begin(); itr != bone_maps.end(); itr++ )
	{
		vector<int> src_locator;
		bool found = pSrcSkeleton->CreateLocator( itr->first, src_locator );

		vector<int> dest_locator;
		found = pDestSkeleton->CreateLocator( itr->second, dest_locator );
	}*/

	const size_t num_motion_primitives = m_vecMotionPrimitive.size();

	const map<string,string>& bone_maps = tgt.m_BoneMaps;
	for( size_t i=0; i<num_motion_primitives; i++ )
	{
		CMotionPrimitive& src_motion = m_vecMotionPrimitive[i];
		vector<CKeyframe>& src_keyframes = src_motion.GetKeyframeBuffer();

		shared_ptr<CMotionPrimitive> pDestMotion( new CMotionPrimitive );
		vector<CKeyframe>& dest_keyframes = pDestMotion->GetKeyframeBuffer();
		dest_keyframes.resize( src_keyframes.size() );

		pDestMotion->SetName( src_motion.GetName() );
		pDestMotion->SetSkeleton( *pDestSkeleton );
		pDestMotion->CreateEmptyKeyframes( (uint)src_keyframes.size() );

		// copy keyframe times
		const int num_keyframes = (int)src_keyframes.size();
		for( int kf=0; kf<num_keyframes; kf++ )
			dest_keyframes[kf].SetTime( src_keyframes[kf].GetTime() );

		// convert transform node tree
		map<string,string>::const_iterator itr;
		for( itr = bone_maps.begin();
			itr != bone_maps.end();
			itr++ )
		{
			vector<int> src_locator;
			bool src_found = pSrcSkeleton->CreateLocator( itr->first, src_locator );

			vector<int> dest_locator;
			bool dest_found = pDestSkeleton->CreateLocator( itr->second, dest_locator );

			if( !src_found || !dest_found )
				continue;

			CopyTransformNodesOfBone(
				src_locator,
				src_keyframes,
//				*pSrcSkeleton,
//				pSrcSkeleton->GetRootBone(),
				dest_locator,
				dest_keyframes
//				*pDestSkeleton,
//				pDestSkeleton->GetRootBone()
				);
		}

		// overwrite the motion primitive
		m_vecMotionPrimitive[i] = *pDestMotion;
	}

	return Result::SUCCESS;

/*
	// create node locator for the src bone
	// create the node locator for the dest bone
	for( each src bone )
	{
		// find the dest bone

		for( each keyframe )
		{
			// copy transform nodes

			Transform pose;
			found = src_keyframe.GetTransform( src_locator, pose );
			if( !found )
				continue;

			dest_keyframe.SetTransform( dest_locator, pose );
		}
	}
*/
}


bool CMotionDatabaseBuilder::SetMotionMapTargets( CXMLNodeReader& mapping )
{
	string bone_mapping_file;
	mapping.GetChildElementTextContent( "BoneMapping", bone_mapping_file );

	if( bone_mapping_file.length() == 0 )
	{
		LOG_PRINT_ERROR( " Can't find a 'BoneMapping' node or a valid mapping file" );
		return false;
	}

	path mapping_file_path = path(m_SourceXMLFilename).parent_path() / bone_mapping_file;
	shared_ptr<CXMLDocument> pDoc = CreateXMLDocument( mapping_file_path.string() );
	if( !pDoc )
		return false;

	CMotionMapTarget& tgt = m_MotionMapTarget;
	map<string,string>& bone_maps = tgt.m_BoneMaps;
	CXMLNodeReader maps_root_node = pDoc->GetRootNodeReader();
	vector<CXMLNodeReader> children = maps_root_node.GetImmediateChildren( "Map" );
	for( size_t i=0; i<children.size(); i++ )
	{
		string from = children[i].GetAttributeText( "from" );
		string to   = children[i].GetAttributeText( "to" );
		if( from == "" || to == "" )
			continue;

		bone_maps[from] = to;
	}

	mapping.GetChildElementTextContent( "DestSkeleton/MDB", tgt.m_DestSkeletonMDB );
	mapping.GetChildElementTextContent( "DestSkeleton/MotionPrimitive", tgt.m_DestSkeletonMotion );

	return true;
}


void CMotionDatabaseBuilder::ProcessRootNodeHorizontalElementOptions( CXMLNodeReader& root_joint_node, CMotionPrimitiveDesc& desc )
{
	vector<CXMLNodeReader> children = root_joint_node.GetImmediateChildren();
	for( size_t i=0; i<children.size(); i++ )
	{
		const string element_name = children[i].GetName();

		if( element_name == "ResetPos" )
		{
			desc.m_bResetHorizontalRootPos = to_bool( children[i].GetTextContent(), "yes", "no" );

//			LOG_PRINT( "reset horizontal root position: " + true_or_false );
		}
		else if( element_name == "NormalizeOrientation" )
		{
			string orient_opt = children[i].GetTextContent();

			desc.m_NormalizeOrientation = orient_opt;

//			if( orient_opt == "AverageMotionDirection" ) ;
//			else if( orient_opt == "LocalPositiveZDirection" ) ;
//			else if( orient_opt == "None" ) ;
		}
	}
}


void CMotionDatabaseBuilder::CreateMotionPrimitiveDesc( CXMLNodeReader& node_reader )
{
	// found <MotionPrimitive> element

	string motion_primitive_name = node_reader.GetAttributeText( "name" );
	if( motion_primitive_name.length() == 0 )
	{
		LOG_PRINT_ERROR( " - No name for motion primitive" );
		return;
	}

	// get the current desc group
	CMotionPrimitiveDescGroup& desc_group = m_vecDescGroup.back();

	CMotionPrimitiveDesc desc;

	// save the name of the motion primitive
	desc.m_Name = motion_primitive_name;

	vector<CXMLNodeReader> children = node_reader.GetImmediateChildren();
	for( size_t i=0; i<children.size(); i++ )
	{
		const string element_name = children[i].GetName();

		if( element_name == "Frame" )
		{
			desc.m_StartFrame = to_int( children[i].GetAttributeText( "start" ) );
			desc.m_EndFrame   = to_int( children[i].GetAttributeText( "end" ) );
		}
		else if( element_name == "Loop" )
		{
			desc.m_bIsLoopMotion = to_bool( children[i].GetAttributeText( "loop" ), "true", "false" );
		}
		else if( element_name == "StartBoneName" )
		{
			desc.m_StartBoneName = children[i].GetTextContent();
		}
		else if( element_name == "AnnotationList" )
		{
			vector<CXMLNodeReader> annotations = children[i].GetImmediateChildren();
			for( size_t j=0; j<annotations.size(); j++ )
			{
				if( annotations[j].GetName() == "Annotation" )
					desc.m_vecAnnotation.push_back( annotations[j].GetTextContent() );
			}
		}
		else if( element_name == "RootJoint" )
		{
			vector<CXMLNodeReader> root_joints = children[i].GetImmediateChildren();
			for( size_t j=0; j<root_joints.size(); j++ )
			{
				if( root_joints[j].GetName() == "HorizontalElement" )
				{
					ProcessRootNodeHorizontalElementOptions( root_joints[j], desc );
				}
			}
		}
	}

	desc_group.m_Desc.push_back( desc );
}


void CMotionDatabaseBuilder::CreateMotionPrimitiveDescGroup( CXMLNodeReader& input_file_node_reader )
{
	// found a <File> element
	// - element that contains a bvh file

	const std::string input_filepath = input_file_node_reader.GetAttributeText( "filepath" );
	if( input_filepath.length() == 0 )
	{
		LOG_PRINT_ERROR( " - No input filepath found" );
		return;
	}

	// add a new desc group
	m_vecDescGroup.push_back( CMotionPrimitiveDescGroup() );

	string src_filename = input_filepath;

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
	m_vecDescGroup.back().m_fScalingFactor = to_float( input_file_node_reader.GetAttributeText( "scaling" ) );

	vector<CXMLNodeReader> children = input_file_node_reader.GetImmediateChildren();
	for( size_t i=0; i<children.size(); i++ )
	{
		if( children[i].GetName() != "MotionPrimitive" )
			continue;

		CreateMotionPrimitiveDesc( children[i] );
	}
}


void CMotionDatabaseBuilder::CreateMotionTableEntry( xercesc::DOMNode *pMotionEntryNode, CHumanoidMotionEntry& entry )
{
	CXMLNodeReader node( pMotionEntryNode );
	vector<CXMLNodeReader> children = node.GetImmediateChildren( "MotionPrimitive" );
	for( size_t i=0; i<children.size(); i++ )
	{
		const string motion_primitive_name = children[i].GetAttributeText( "name" );
		if( motion_primitive_name.length() == 0 )
		{
			LOG_PRINT_ERROR( "A motion primitive must have a name." );
			continue;
		}

		entry.m_vecMotionPrimitiveName.push_back( motion_primitive_name );
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
{
	string output_filename;
	root_node.GetChildElementTextContent( "Output", output_filename );
	m_OutputFilepath = path( path(m_SourceXMLFilename).parent_path() / output_filename ).string();

	if( m_OutputFilepath.length() == 0 )
	{
		LOG_PRINT_WARNING( "Output filepath was not found in source XML file. Call SaveMotionDatabaseToFile() to save motion database to disk."  );
	}

	vector<CXMLNodeReader> file_nodes = root_node.GetImmediateChildren();
	for( size_t i=0; i<file_nodes.size(); i++ )
	{
		const string node_name = file_nodes[i].GetName();

		if( node_name == "File" )
		{
			CreateMotionPrimitiveDescGroup( file_nodes[i] );
		}
		else if( node_name == "HumanoidMotionTable" )
		{
			CreateMotionTable( file_nodes[i].GetDOMNode() );
		}
		else if( node_name == "AnnotationTable" )
		{
			CreateAnnotationTable( file_nodes[i] );
		}
		else if( node_name == "MotionMappingDestination" )
		{
			SetMotionMapTargets( file_nodes[i] );
		}
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

	CXMLNodeReader root_node = pXMLDocument->GetRootNodeReader();

	// (E)

	// set the working directory to the directory path of source_script_filename
	// - Do this after (S) - (E), or the exception ocurrs
//	fnop::dir_stack dirstk( fnop::get_path(source_script_filename) );

	if( root_node.GetName() == "root" )
	{
		ProcessXMLFile( root_node );
	}

	// create motion primitives from descs
	CreateMotionPrimitives();

	if( m_MotionMapTarget.IsValid() )
	{
		Result::Name res = MapMotionPrimitivesToAnotherSkeleton();
	}

	// restore the original working directory (pop)
//	dirstk.prevdir();

	if( 0 < m_OutputFilepath.length() )
		return SaveMotionDatabaseToFile( m_OutputFilepath );

	return true;
}


bool CMotionDatabaseBuilder::SaveMotionDatabaseToFile( const std::string& db_filename )
{
	if( db_filename.length() == 0 )
		return false;

	path parent_path( path(db_filename).parent_path() );
	if( !exists(parent_path) )
		create_directories(parent_path);

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
