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



float CalculateHeading( const Matrix34& pose )
{
	Vector3 vRootPos = pose.vPosition;
	vRootPos.y = 0;

	Scalar heading = - Vec3GetAngleBetween( Vector3(0,0,1), vRootPos );

	if( vRootPos.x < 0 )
		heading *= -1.0f;

	return heading;
}


Matrix33 msynth::CalculateHorizontalOrientation( const Matrix34& pose )
{
	const float heading = CalculateHeading( pose );

	Matrix33 rotation_y = Matrix33RotationY( heading );

	return rotation_y;
}


void msynth::AlignLastKeyframe( std::vector<CKeyframe>& vecKeyframe )
{
	if( vecKeyframe.size() == 0 )
		return;

	CKeyframe& last_keyframe = vecKeyframe.back();

	float heading = CalculateHeading( last_keyframe.GetRootPose() );

	Matrix33 rotation_y = Matrix33RotationY( heading );
/*
	Vector3 vLastRootPos = last_keyframe.GetRootPose().vPosition;
	vLastRootPos.y = 0;

	Scalar heading = - Vec3GetAngleBetween( Vector3(0,0,1), vLastRootPos );

	if( vLastRootPos.x < 0 )
		heading *= -1.0f;

	Matrix33 rotation_y = Matrix33RotationY( heading );
*/
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


Result::Name CMotionDatabaseBuilder::MapMotionPrimitiveToAnotherSkeleton( boost::shared_ptr<CMotionPrimitive>& pSrcMotion,
																		  boost::shared_ptr<CSkeleton>& pDestSkeleton )
{
	CMotionMapTarget& tgt = m_MotionMapTarget;

//	shared_ptr<CSkeleton> pSrcSkeleton = m_vecpMotionPrimitive.front()->GetSkeleton();
	shared_ptr<CSkeleton> pSrcSkeleton = pSrcMotion->GetSkeleton();

//	const size_t num_motion_primitives = m_vecpMotionPrimitive.size();

	const map<string,string>& bone_maps = tgt.m_BoneMaps;

	CMotionPrimitive& src_motion = *pSrcMotion;//m_vecpMotionPrimitive[i];
	vector<CKeyframe>& src_keyframes = src_motion.GetKeyframeBuffer();

	shared_ptr<CMotionPrimitive> pDestMotion( new CMotionPrimitive );
	vector<CKeyframe>& dest_keyframes = pDestMotion->GetKeyframeBuffer();
	dest_keyframes.resize( src_keyframes.size() );

	(*pDestMotion) = src_motion;
//	pDestMotion->SetName( src_motion.GetName() );
	pDestMotion->SetSkeleton( *pDestSkeleton );
//	pDestMotion->SetLoopedMotion( src_motion.IsLoopedMotion() );
//	pDestMotion->SetStartsBoneName( src_motion.GetStartBoneName() );
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
		const std::string& src_name  = itr->first;
		const std::string& dest_name = itr->second;

		vector<int> src_locator;
		bool src_found = pSrcSkeleton->CreateLocator( src_name, src_locator );

		vector<int> dest_locator;
		bool dest_found = pDestSkeleton->CreateLocator( itr->second, dest_locator );

		if( !src_found || !dest_found )
		{
			const string msg = fmt_string( " Cannot map from '%s' to '%s'.", src_name.c_str(), dest_name.c_str() );
			if( !src_found && dest_found )
				LOG_PRINT_WARNING( msg + fmt_string( " The src bone '%s' was not found.", src_name.c_str() ) );
			else if( src_found && !dest_found )
				LOG_PRINT_WARNING( msg + fmt_string( " The dest bone '%s' was not found.", dest_name.c_str() ) );
			else
				LOG_PRINT_WARNING( msg + fmt_string( " Neither src nor dest bone was found." ) );

			continue;
		}

		CopyTransformNodesOfBone(
			src_locator,
			src_keyframes,
//			*pSrcSkeleton,
//			pSrcSkeleton->GetRootBone(),
			dest_locator,
			dest_keyframes
//			*pDestSkeleton,
//			pDestSkeleton->GetRootBone()
			);
	}

	// overwrite the motion primitive
	pSrcMotion = pDestMotion;
//	m_vecpMotionPrimitive[i] = pDestMotion;

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


Result::Name CMotionDatabaseBuilder::MapMotionPrimitivesToAnotherSkeleton()
{
	CMotionMapTarget& tgt = m_MotionMapTarget;

	path mdb_path = path(m_SourceXMLFilename).parent_path() / tgt.m_DestSkeletonMDB;
	CMotionDatabase dest_skeleton_src_mdb( mdb_path.string() );
	shared_ptr<CMotionPrimitive> pMotion = dest_skeleton_src_mdb.GetMotionPrimitive( tgt.m_DestSkeletonMotion );
	if( !pMotion )
		return Result::UNKNOWN_ERROR;

	shared_ptr<CSkeleton> pDestSkeleton = pMotion->GetSkeleton();

	Result::Name res = Result::SUCCESS;
	BOOST_FOREACH( CMotionPrimitiveDescGroup& desc_group, m_vecDescGroup )
	{
		BOOST_FOREACH( CMotionPrimitiveDesc& desc, desc_group.m_Desc )
		{
			if( !desc.m_pMotionPrimitive )
				continue;

			res = MapMotionPrimitiveToAnotherSkeleton( desc.m_pMotionPrimitive, pDestSkeleton );
		}
	}

	return Result::SUCCESS;

//	if( m_vecpMotionPrimitive.empty() )
//		return Result::UNKNOWN_ERROR;

//	if( !m_vecpMotionPrimitive.front() )
//		return Result::UNKNOWN_ERROR;

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


void CMotionDatabaseBuilder::ProcessGlobalModificationOptions( CXMLNodeReader& node )
{
	vector<CXMLNodeReader> children = node.GetImmediateChildren();
	for( size_t i=0; i<children.size(); i++ )
	{
		const string element_name = children[i].GetName();

		if( element_name == "Joint" )
		{
			CJointModification mod;
			string joint_name = children[i].GetAttributeText("name");
			if( joint_name.length() == 0 )
				continue;

			mod.m_Name = joint_name;
			float b=0,h=0,p=0;
			bool found_b=false, found_h=false, found_p=false;
			found_b = children[i].GetChildElementTextContent( "Pre/Rotation/Bank",    b );
			found_h = children[i].GetChildElementTextContent( "Pre/Rotation/Heading", h );
			found_p = children[i].GetChildElementTextContent( "Pre/Rotation/Pitch",   p );
			if( found_b || found_h || found_p )
			{
				mod.m_matPreRotation
					= Matrix33RotationX(deg_to_rad(p))
					* Matrix33RotationY(deg_to_rad(h))
					* Matrix33RotationZ(deg_to_rad(b));

				mod.m_ApplyPreRotation = true;
			}

			m_vecJointModification.push_back( mod );
		}
	}
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
		else if( element_name == "Modifications" )
		{
			vector<CXMLNodeReader> fixes = children[i].GetImmediateChildren( "Fix" );
			for( size_t j=0; j<fixes.size(); j++ )
			{
				CJointFixMod mod;
				mod.m_JointName = fixes[j].GetAttributeText( "joint_name" );
				CXMLNodeReader fix_t = fixes[j].GetChild("Translation");
				if( fix_t.IsValid() )
				{
					string tx,ty,tz;
					tx = (fix_t.GetAttributeText("x")); if( 0 < tx.length() ) { mod.m_TargetFlags |= CJointFixMod::TX; mod.m_Fixed.vPosition.x = to_float(tx); }
					ty = (fix_t.GetAttributeText("y")); if( 0 < ty.length() ) { mod.m_TargetFlags |= CJointFixMod::TY; mod.m_Fixed.vPosition.y = to_float(ty); }
					tz = (fix_t.GetAttributeText("z")); if( 0 < tz.length() ) { mod.m_TargetFlags |= CJointFixMod::TZ; mod.m_Fixed.vPosition.z = to_float(tz); }
				}
				desc.m_vecFixMod.push_back( mod );
			}
		}
		else if( element_name == "RootJoint" )
		{
			vector<CXMLNodeReader> root_joints = children[i].GetImmediateChildren();
			for( size_t j=0; j<root_joints.size(); j++ )
			{
				string node_name( root_joints[j].GetName() );
				if( node_name == "HorizontalElement" )
				{
					ProcessRootNodeHorizontalElementOptions( root_joints[j], desc );
				}
				else if( node_name == "HeightShift" )
				{
					root_joints[j].GetTextContent( desc.m_fRootNodeHeightShift );
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

	m_vecDescGroup.back().m_fTimeScalingFactor = to_float( input_file_node_reader.GetAttributeText( "time_scaling" ) );

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
		else if( node_name == "GlobalModifications" )
		{
			ProcessGlobalModificationOptions( file_nodes[i] );
		}
	}
}


void RecursivelyApplyPrePostTransforms( CTransformNode& node,
										const Transform& pre,
										const Transform& post )
{
	Transform local_transform
		= post * node.GetLocalTransform() * pre;

	node.SetLocalTransform( local_transform );

	const int num_children = node.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		RecursivelyApplyPrePostTransforms( node.ChildNode(i), pre, post );
	}

}


void CMotionDatabaseBuilder::ProcessCreatedMotionPrimitive( CMotionPrimitiveDesc& desc )
{
	shared_ptr<CMotionPrimitive>& pMotion = desc.m_pMotionPrimitive;

	if( !pMotion )
		return;

	std::vector<CKeyframe>& keyframes = pMotion->GetKeyframeBuffer();
	int num_keyframes = (int)keyframes.size();

	if( 0.001 <= fabs(desc.m_fRootNodeHeightShift) )
	{
		const float height_shift = desc.m_fRootNodeHeightShift;
//		ShiftRootNodeHeight( vecKeyframe, desc.m_fRootNodeHeightShift );
		for( int i=0; i<num_keyframes; i++ )
		{
			const CTransformNode& root_node = keyframes[i].GetRootNode();
			Transform local_transform = root_node.GetLocalTransform();
			local_transform.vTranslation.y += height_shift;

			keyframes[i].RootNode().SetLocalTransform( local_transform );
		}
	}

	// Apply joint modifications.
	// Applied to all the motion primitives
	const int num_joint_mods = (int)m_vecJointModification.size();
	for( int i=0; i<num_joint_mods; i++ )
	{
		CJointModification& joint_mod = m_vecJointModification[i];

		shared_ptr<CSkeleton> pSkeleton = pMotion->GetSkeleton();
		if( !pSkeleton )
			continue;

		if( joint_mod.m_ApplyPreRotation )//|| joint_mod.m_ApplyPostRotation )
		{
			const Transform pre(  Matrix34( Vector3(0,0,0), joint_mod.m_matPreRotation ) );
//			const Transform post( Matrix34( Vector3(0,0,0), joint_mod.m_matPostRotation ) );

			vector<int> locator;
			bool located = pSkeleton->CreateLocator( joint_mod.m_Name, locator );
			for( int j=0; j<num_keyframes; j++ )
			{
				CTransformNode *pNode = keyframes[j].GetTransformNode( locator );
				if( !pNode )
					continue;

				Transform local_transform = pNode->GetLocalTransform() * pre;

				pNode->SetLocalTransform( local_transform );

				const int num_children = pNode->GetNumChildren();
				for( int i=0; i<num_children; i++ )
				{
					RecursivelyApplyPrePostTransforms( pNode->ChildNode(i), pre, pre.ToMatrix34().GetInverseROT() );
				}
			}
		}
	}

	int num_desc_groups = (int)m_vecDescGroup.size();
	for( int i=0; i<num_desc_groups; i++ )
	{
		int num_descs = (int)m_vecDescGroup[i].m_Desc.size();
		for( int j=0; j<num_descs; j++ )
		{
			CMotionPrimitiveDesc& desc = m_vecDescGroup[i].m_Desc[j];
			if( !desc.m_pMotionPrimitive )
				continue;

			ApplyJointFixModification( desc.m_vecFixMod, *(desc.m_pMotionPrimitive) );
		}

	}
}


void CMotionDatabaseBuilder::CreateMotionPrimitives()
{
	m_vecpMotionPrimitive.reserve( 256 );

	BOOST_FOREACH( CMotionPrimitiveDescGroup& desc_group, m_vecDescGroup )
	{
		shared_ptr<CMotionDatabaseCompiler> pCompiler = CreateMotionPrimitiveCompiler( desc_group.m_Filename );

		if( !pCompiler )
		{
			continue;
		}

		pCompiler->m_pvecpMotionPrimitive = &m_vecpMotionPrimitive;
		pCompiler->m_vecpAnnotationName   = &m_vecAnnotationName;
		pCompiler->m_pMotionTable         = &m_MotionTable;

		pCompiler->CreateMotionPrimitives( desc_group );
	}
}


void CMotionDatabaseBuilder::ApplyJointModification( const CJointModification& mod )
{
	const int num_motions = (int)m_vecpMotionPrimitive.size();
	for( int i=0; i<num_motions; i++ )
	{
		CMotionPrimitive& motion = *m_vecpMotionPrimitive[i];

		shared_ptr<CSkeleton> pSkeleton = motion.GetSkeleton();
		if( !pSkeleton )
			continue;

		vector<int> locator;
		bool res = pSkeleton->CreateLocator( mod.m_Name, locator );

		if( mod.m_ApplyPreRotation )
		{
			vector<CKeyframe>& keyframes = motion.GetKeyframeBuffer();
			const int num_keyframes = (int)keyframes.size();
			for( int kf=0; kf<num_keyframes; kf++ )
			{
				CKeyframe& keyframe = keyframes[kf];
				CTransformNode *pNode = keyframe.GetTransformNode(locator);
				if( !pNode )
					break;

				Transform transform = pNode->GetTransform();
				Transform modified
					= transform
					* Transform(Quaternion(mod.m_matPreRotation),Vector3(0,0,0));

				pNode->SetTransform( modified );
			}
		}

	}
}


void CMotionDatabaseBuilder::ApplyJointFixModification( const std::vector<CJointFixMod>& mods, CMotionPrimitive& target_motion )
{
	if( mods.empty() )
		return;

	shared_ptr<CSkeleton> pSkeleton = target_motion.GetSkeleton();
	if( !pSkeleton )
		return;

	const int num_fix_mods = (int)mods.size();
	for( int i=0; i<num_fix_mods; i++ )
	{
		const CJointFixMod& mod = mods[i];
		vector<int> locator;
		bool res = pSkeleton->CreateLocator( mods[i].m_JointName, locator );

//		if( locator.empty() )
//			continue;

		vector<CKeyframe>& keyframes = target_motion.GetKeyframeBuffer();
		int num_keyframes = (int)keyframes.size();
		for( int j=0; j<num_keyframes; j++ )
		{
			CKeyframe& keyframe = keyframes[j];
			CTransformNode *pNode = keyframe.GetTransformNode( locator );

			Transform transform = pNode->GetLocalTransform();
			if( mod.m_TargetFlags & CJointFixMod::TX ) transform.vTranslation.x = mod.m_Fixed.vPosition.x;
			if( mod.m_TargetFlags & CJointFixMod::TY ) transform.vTranslation.y = mod.m_Fixed.vPosition.y;
			if( mod.m_TargetFlags & CJointFixMod::TZ ) transform.vTranslation.z = mod.m_Fixed.vPosition.z;
			pNode->SetLocalTransform( transform );
		}

	}
}


bool CMotionDatabaseBuilder::Build( const std::string& source_script_filename )
{
	m_SourceXMLFilename = source_script_filename;

	shared_ptr<CXMLDocument> pXMLDocument;

	bool file_exists = exists( source_script_filename );

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

	BOOST_FOREACH( CMotionPrimitiveDescGroup& desc_group, m_vecDescGroup )
	{
		BOOST_FOREACH( CMotionPrimitiveDesc& desc, desc_group.m_Desc )
		{
			ProcessCreatedMotionPrimitive( desc );
		}
	}

	if( !m_vecJointModification.empty() )
	{
		for( int i=0; i<(int)m_vecJointModification.size(); i++ )
		{
//			ApplyJointModification( m_vecJointModification[i] );
		}
	}

	// Copy motion primitives to a single array
	m_vecpMotionPrimitive.resize( 0 );
	m_vecpMotionPrimitive.reserve( 0xFF );
	BOOST_FOREACH( CMotionPrimitiveDescGroup& desc_group, m_vecDescGroup )
	{
		BOOST_FOREACH( CMotionPrimitiveDesc& desc, desc_group.m_Desc )
		{
			m_vecpMotionPrimitive.push_back( desc.m_pMotionPrimitive );
		}
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

	BOOST_FOREACH( shared_ptr<CMotionPrimitive>& pMotion, m_vecpMotionPrimitive )
	{
		if( !pMotion )
			continue;

		db.m_DB.AddData( pMotion->GetName(), *pMotion );
	}

	return true;
}
