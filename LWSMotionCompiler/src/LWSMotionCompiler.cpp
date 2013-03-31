#include "LWSMotionCompiler.hpp"
#include "Platform_Win32.hpp"
#include "gds/LightWave/LightWaveSceneLoader.hpp"
#include "gds/MotionSynthesis/MotionPrimitive.hpp"
#include "gds/Support/TextFileScanner.hpp"
#include "gds/Support/StringAux.hpp"
#include "gds/base.hpp"

using std::string;
using std::vector;
using namespace boost;
using namespace boost::filesystem;
using namespace msynth;


/*
Playing motions
- preloaded
  - short motions often played in loop
    - walk, run, etc.
  - called "Motion Primitive"
- streaming
  - longer motions

*/

/*
Runtime Motion Data Format
- Complete frame-based format
  - every key frame data has transformations for all the bones
	- easy to calclate the transformation at runtime
    - redundant
	- not practical if you want to save motions of multiple characters and other objects
	  because of too much redundancy and data size
- LWS Envelope-based format
  - Can make data less redundant.
    - Need some extra calculations at runtime to obtain keyframe data
      because the keyframe for a specified time needs to be searched every frame.
    - 

*/

/*
Possible optimization(s)
- Skip interpolation if possible
  - A bone that have the same transformation throughout the entire motion primitive
  -> When playing the motion primitive, always use the same pose and skip the interpolation

*/


/*
Motion Primitive Desc File

LWSFile   src_scene_file.lws
Output    dest_motion_database_file.mdb

NamePrefix

#                name             start_frame  end_frame   root_node
------------------------------------------------------------------------
MotionPrimitive  walk             0            0           
MotionPrimitive  run			  0            0           
MotionPrimitive  jump			  0            0           
MotionPrimitive  stand_to_crouch  0            0           
MotionPrimitive  crouch_to_prone  0            0           
MotionPrimitive  crouching		  0            0           
MotionPrimitive  standing		  0            0           

*/

/*
void DumpTransformNodeToTextFile( FILE *fp, const CTransformNode& node )
{
//	fprintf( fp, "%f.3 ", node. );

	for( int i=0; i<node.GetNumChildren(); i++ )
		DumpTransformNodeToTextFile( fp, node.GetChildNode(i) );
}


void DumpKeyframeToTextFile( FILE *fp, CKeyframe& kf )
{
	fprintf( fp, "%f.3 ", kf.GetTime() );
	DumpTransformNodeToTextFile( fp, kf.GetRootNode() );
}


void DumpMotionPrimitiveToTextFile( FILE *fp, CMotionPrimitive& motion )
{
	fprintf( fp, "name: %s\n", motion.GetName().c_str() );

	vector<CKeyframe>& keyframes = motion.GetKeyframeBuffer();
	const int num_keyframes = (int)keyframes.size();
	fprintf( fp, "%d keyframes:\n", num_keyframes );

	for( int i=0; i<num_keyframes; i++ )
	{
		DumpKeyframeToTextFile( fp, keyframes[i] );
	}
}*/


inline void Quantize( float& f, float q )
{
	f = f - fmodf( f, q );
}

inline void Quantize( Vector3& v, float q )
{
	Quantize( v.x, q );
	Quantize( v.y, q );
	Quantize( v.z, q );
}

inline void Quantize( Matrix33& mat, float q )
{
	for( int i=0; i<3; i++ )
	{
		for( int j=0; j<3; j++ )
			Quantize( mat(i,j), q );
	}
}


CLWSMotionDatabaseCompiler::CLWSMotionDatabaseCompiler()
{
}


CLWSMotionDatabaseCompiler::~CLWSMotionDatabaseCompiler()
{
}


void CLWSMotionDatabaseCompiler::CreateMotionPrimitives( CMotionPrimitiveDescGroup& desc_group )
{
	bool scene_loaded = LoadLWSceneFile( desc_group.m_Filename );
	if( !scene_loaded )
	{
		LOG_PRINT_ERROR( " Failed to load an LWS scene file, " + desc_group.m_Filename );
		return;
	}

	shared_ptr<LWS_Bone> pRootBone = CreateSkeleton();

	if( m_pSkeleton )
	{
		path dir_path = path(desc_group.m_Filename).parent_path() / "skeletons";
		create_directories( dir_path );
		path filepath = dir_path / path(desc_group.m_Filename + ".txt").leaf();
		m_pSkeleton->DumpToTextFile( filepath.string() );
	}

	if( !pRootBone )
		return;// Result::UNKNOWN_ERROR;

	vector<float> vecfKeyframeTime;
	vecfKeyframeTime.reserve( 0xFF );
	CollectKeyFrameTimes( *(pRootBone.get()), vecfKeyframeTime );

	const int fps = m_pScene ? m_pScene->GetSceneInfo().m_FramesPerSecond : 30;

	const int num_keyframes = (int)vecfKeyframeTime.size();

	vector<CKeyframe> vecKeyframe;
	vecKeyframe.resize( num_keyframes );

	for( int i=0; i<num_keyframes; i++ )
	{
		float fTime = vecfKeyframeTime[i];//(float)i / (float)fps;

		CKeyframe& dest_keyframe = vecKeyframe[i];
		dest_keyframe.SetTime( fTime );
		CreateKeyframe( pRootBone, fTime, Matrix34Identity(), dest_keyframe.RootNode() );
	}

	const size_t num = desc_group.m_Desc.size();
	for( size_t i=0; i<num; i++ )
	{
		if( i == 1 )
			int second_motion = 1;

		CreateMotionPrimitive( desc_group, desc_group.m_Desc[i], vecKeyframe );

//		CreateMotionPrimitives( pRootBone );
	}
}


inline bool find_similar_time( float in, vector<float>& vecfRef )
{
	const size_t num = vecfRef.size();
	for( size_t i=0; i<num; i++ )
	{
		if( fabs( in - vecfRef[i] ) < 0.000001f )
			return true;
	}

	return false;
}


void CLWSMotionDatabaseCompiler::CollectKeyFrameTimes( LWS_Bone& bone, vector<float>& vecKeyframeTime )
{
//	shared_ptr<LWS_Bone> pRootBone = ???;

	int i,j;
	int num_channels = bone.GetNumChannels();
	for( i=0; i<num_channels; i++ )
	{
		const LWS_Channel& channel = bone.GetChannel(i);
		const int num_keys = (int)channel.vecKey.size();
		for( j=0; j<num_keys; j++ )
		{
			const LWS_Keyframe& keyframe = channel.vecKey[j];
			if( !find_similar_time( keyframe.fTime, vecKeyframeTime ) )
				vecKeyframeTime.push_back( keyframe.fTime );
		}
	}

	const size_t num_children = bone.ChildBone().size();
	for( size_t i=0; i<num_children; i++ )
	{
		CollectKeyFrameTimes( *(bone.ChildBone()[i].get()), vecKeyframeTime );
	}
}


// Recursively copies the bones and creates a tree which has CBone class objects as its nodes
/// \param pSrcBone [in] source from which skeletal structure is extracted
/// \param parent_space [in]
/// \param dest_bone [out] a destination buffer
void CopyBones( const shared_ptr<LWS_Bone> pSrcBone,
			    const Matrix34& parent_space,
			    CBone& dest_bone )
{
	const string& bone_name = pSrcBone->GetBoneName();
	Vector3 vRestPos  = pSrcBone->GetBoneRestPosition();
	float fRestLen    = pSrcBone->GetBoneRestLength();
	float afRestAngle[3] = 
	{
		pSrcBone->GetBoneRestAngle(0),
		pSrcBone->GetBoneRestAngle(1),
		pSrcBone->GetBoneRestAngle(2)
	};

	Quantize( vRestPos, 0.000001f );
	Quantize( fRestLen, 0.000001f );
	for( int j=0; j<3; j++ )
		Quantize( afRestAngle[j], 0.000001f );

	Matrix34 local_space;
	local_space.matOrient
		= Matrix33Identity()
		* Matrix33RotationY( afRestAngle[0] ) // heading
		* Matrix33RotationX( afRestAngle[1] ) // pitch
		* Matrix33RotationZ( afRestAngle[2] ) // bank
		* Matrix33Identity();

	Quantize( local_space.matOrient, 0.000001f );

	local_space.vPosition = vRestPos;


	Matrix34 world_pose = parent_space * local_space;

	// this makes a parent bone
	Vector3 vOffset = world_pose.vPosition - parent_space.vPosition;

	Vector3 vAnotherOffset
		= parent_space
		* Matrix34( Vector3(0,0,0), local_space.matOrient )
		* Vector3(0,0,1) * fRestLen;

	// prior to r1
//	dest_bone.SetOffset( vOffset );
//	dest_bone.SetOffset( Vector3(0,0,1) * fRestLen );

	if( false /* r1 */ )
	{
		dest_bone.SetOffset( pSrcBone->GetBoneRestPosition() );
		dest_bone.SetOrient( pSrcBone->GetBoneRestOrientation() );
	}
	else // r2
	{
		dest_bone.SetOffset( parent_space.matOrient * local_space.matOrient * pSrcBone->GetBoneRestPosition() );
		dest_bone.SetOrient( Matrix33Identity() );
	}

	dest_bone.SetName( pSrcBone->GetBoneName() );

	const size_t num_children = pSrcBone->ChildBone().size();
	dest_bone.Children().resize( num_children );
	for( size_t i=0; i<num_children; i++ )
	{
		shared_ptr<LWS_Bone> pSrcChildBone = pSrcBone->ChildBone()[i];
//		CBone dest_child_bone;
//		dest_bone.AddChildBone( dest_child_bone );

		// recursively copy the bones
		CopyBones( pSrcChildBone, parent_space * local_space, dest_bone.Child((int)i) );
	}
}

//void CLWSMotionDatabaseCompiler::CreateSkeleton()

/// Returns a valid root bone on success
/// Also, a valid skeleton is set to m_pSkeleton if everything works fine.
shared_ptr<LWS_Bone> CLWSMotionDatabaseCompiler::CreateSkeleton()
{
	if( !m_pScene )
		return shared_ptr<LWS_Bone>();

	// restore tree structure of the bones

	vector< shared_ptr<LWS_Bone> > vecpRootBones = m_pScene->GetRootBones();

	shared_ptr<LWS_Bone> pRootBone;
	const size_t num_root_bones = vecpRootBones.size();
	if( num_root_bones == 0 )
	{
		LOG_PRINT_ERROR( " No root bone was found." );
		return shared_ptr<LWS_Bone>();
	}
	if( num_root_bones == 1 )
	{
		pRootBone = vecpRootBones[0];
	}
	else //( 1 < num_root_bones )
	{
		// multiple root bones - add a root bone?
	}

	CBone root_bone;

	// Extract skeletal structure from the LWS data and create a skeletal structure
	// on root_bone
	CopyBones( pRootBone, Matrix34Identity(), root_bone );

	m_pSkeleton = shared_ptr<CSkeleton>( new CSkeleton );
	m_pSkeleton->SetBones( root_bone );

	return pRootBone;
}


void CLWSMotionDatabaseCompiler::CreateMotionPrimitive( CMotionPrimitiveDescGroup& desc_group,
													    CMotionPrimitiveDesc& desc,
													    vector<CKeyframe>& vecSrcKeyframe )
{
	// add a new motion primitive and get the reference to it
//	m_vecpMotionPrimitive->push_back( CMotionPrimitive( desc.m_Name ) );
	shared_ptr<CMotionPrimitive> pMotion( new CMotionPrimitive( desc.m_Name ) );
	m_pvecpMotionPrimitive->push_back( pMotion );
	desc.m_pMotionPrimitive = pMotion;

	CMotionPrimitive& motion = *(m_pvecpMotionPrimitive->back());

	if( !m_pSkeleton )
		return;

//	motion.SetSkeleton( desc_group.m_Skeleton );
	motion.SetSkeleton( *(m_pSkeleton.get()) );

	motion.SetLoopedMotion( desc.m_bIsLoopMotion );
	motion.SetStartsBoneName( desc.m_StartBoneName );

	vector<CKeyframe> vecKeyframe;
	vecKeyframe.resize(0);

	const int fps = m_pScene ? m_pScene->GetSceneInfo().m_FramesPerSecond : 30;

	int start_frame = desc.m_StartFrame;
	int end_frame   = desc.m_EndFrame;

	if( end_frame < start_frame )
		return;

	// Find start keyframe.
	// If vecSrcKeyframe does not have a keyframe at desc.m_StartFrame,
	// creating a new keyframe by interpolating keyframes in vecSrcKeyframe
	// before and after desc.m_StartFrame.
	// 
	float fStartTime = (float)start_frame / (float)fps;
	float fEndTime   = (float)end_frame   / (float)fps;
	const int num_src_keyframes = (int)vecSrcKeyframe.size();
/*	for( int i=0; i<num_src_keyframes; i++ )
	{
		if( vecSrcKeyframe[i].GetTime() < fStartTime )
			break;
	}

	CKeyframe start_keyframe;
	if( i == num_src_keyframes )
		start_keyframe = vecSrcKeyframe.back();
	else if( fabs( vecSrcKeyframe[i].GetTime() - fStartTime ) < 0.001 )
		start_keyframe = vecSrcKeyframe[i];
	else
	{
		// need to interpolate the keyframes before and after desc.m_StartFrame
		CMotionPrimitive motion;
		motion.AddKeyframe( 
		start_keyframe = ;
	}
*/
	for( int i=0; i<num_src_keyframes; i++ )
	{
		float fTime = vecSrcKeyframe[i].GetTime();
		if( fStartTime <= fTime && fTime <= fEndTime )
		{
			vecKeyframe.push_back( vecSrcKeyframe[i] );
		}
	}

//	clamp( start, 0, (int)vecSrcKeyframe.size() );
//	clamp( end,   0, (int)vecSrcKeyframe.size() );


//	vecKeyframe.assign( vecSrcKeyframe.begin() + start, vecSrcKeyframe.begin() + end);

	if( vecKeyframe.empty() )
		return;

	CMotionPrimitive m_SourceMotion;
	for( int i=0; i<num_src_keyframes; i++ )
		m_SourceMotion.InsertKeyframe( vecSrcKeyframe[i] );

	if( 0.001 < fabs( vecKeyframe.front().GetTime() - fStartTime ) )
	{
		// Need to insert an interpolated keyframe at the beginning
		vecKeyframe.insert( vecKeyframe.begin(), CKeyframe(fStartTime) );
		m_SourceMotion.GetInterpolatedKeyframe( vecKeyframe.front(), fStartTime );
	}

	if( 0.001 < fabs( vecKeyframe.front().GetTime() - fEndTime ) )
	{
		// Need to insert an interpolated keyframe at the end
		CKeyframe end_keyframe( fEndTime );
		Result::Name res = m_SourceMotion.GetInterpolatedKeyframe( end_keyframe, fEndTime );
		if( res == Result::SUCCESS )
			vecKeyframe.push_back( end_keyframe );
	}

	// modify the time of the copied keyframes so that the motion primitive start at time 0

	float fOrigStartTime = vecKeyframe.front().GetTime();
	const size_t num_keyframes = vecKeyframe.size();
	for( size_t i=0; i<num_keyframes; i++ )
		vecKeyframe[i].SetTime( vecKeyframe[i].GetTime() - fOrigStartTime );
//		vecKeyframe[i].SetTime( (float)i / (float)fps );
/*
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
*/
	motion.SetKeyframes( vecKeyframe );
}


void CLWSMotionDatabaseCompiler::CreateMotionPrimitive()
{/*
	shared_ptr<LWS_Bone> pRootBone;

	if( desc.root_node_name.length() == 0 )
		pMotionRootBone = pRootBone;
	else
		pMotionRootBone = FindBoneByName( pRootBone, root_node_name );

	if( !pMotionRootBone )
	{
		return;
	}

	int start = 0;
	int end = 0;

//	m_pScene->
*/
}


void CLWSMotionDatabaseCompiler::CreateKeyframe( shared_ptr<LWS_Bone> pBone, float fTime, const Matrix34& parent_transform, CTransformNode& dest_node )
{
	if( 3.0f < fTime )
		int break_here = 1;

	const size_t num_children = pBone->ChildBone().size();
	dest_node.SetNumChildren( (int)num_children );

	// debug
//	Matrix33 mat1 = pBone->GetOrientationAt( fTime ) * pBone->GetBoneRestOrientation(); // debug
//	Matrix33 mat2 = pBone->GetOrientationAt( fTime ) * Matrix33Transpose(pBone->GetBoneRestOrientation()); // debug

  if( CBone::get_htrans_rev() == 1 )
  {
	// rotation
//	Quaternion qRotaiton = Quaternion( pBone->GetOffsetOrientationAt( fTime ) );
//	Quaternion qRotaiton = Quaternion( parent_transform.matOrient * pBone->GetOffsetOrientationAt( fTime ) );
	Quaternion qRotaiton = Quaternion( pBone->GetOrientationAt( fTime ) );
//	Quaternion qRotaiton = Quaternion( pBone->GetOrientationAt( fTime ) * Matrix33Transpose(pBone->GetBoneRestOrientation()) );
	dest_node.SetRotation( qRotaiton );

	// translation
	bool ignore_bone_rest_pos_for_root_bone = true;
	Vector3 vTranslation = Vector3(0,0,0);
//	if( !pBone->GetParent() && ignore_bone_rest_pos_for_root_bone )
//	{
		// root bone and the client specified to ignore bone rest position
		vTranslation = pBone->GetPositionAt( fTime );
/*	}
	else
	{
		vTranslation = pBone->GetPositionAt( fTime ) - pBone->GetBoneRestPosition();
	}*/

	LWS_Item *pParent = pBone->GetParent();
	if( pParent && pParent->GetItemType() != LWS_Item::TYPE_BONE )
	{
		// the parent is not a bone
		// - Assume this is a root bone and add the parent translation
		vTranslation += pParent->GetPositionAt( fTime );
	}

	Quantize( vTranslation, 0.000001f );
	dest_node.SetTranslation( vTranslation );

	// do recursive calls
	Matrix34 next_transform = Matrix34Identity();
	next_transform.matOrient = parent_transform.matOrient * pBone->GetBoneRestOrientation();
	for( size_t i=0; i<num_children; i++ )
	{
		CreateKeyframe( pBone->ChildBone()[i], fTime, next_transform, dest_node.Child()[i] );
	}
  }
  else if( CBone::get_htrans_rev() == 2 )
  {
	// rotation
	const Matrix33 matDeltaRotation = pBone->GetOrientationFromRestOrientationAt( fTime );
	const Matrix33 matRestRotation  = pBone->GetBoneRestOrientation();
	const Matrix33 matWorldRotation = parent_transform.matOrient * matRestRotation;
	Matrix33 matDestRotation( Matrix33Identity() );

	LWS_Item *pParent = pBone->GetParent();
	if( pParent && pParent->GetItemType() != LWS_Item::TYPE_BONE ) // is root bone
		matDestRotation = matDeltaRotation;
	else
		matDestRotation = matWorldRotation * matDeltaRotation * Matrix33Transpose(matWorldRotation);

//	matDestRotation  = matDeltaRotation;
	Quaternion qRotaiton = Quaternion( matDestRotation );
//	Quaternion qRotaiton = Quaternion( pBone->GetOrientationAt( fTime ) * Matrix33Transpose(pBone->GetBoneRestOrientation()) );
	dest_node.SetRotation( qRotaiton );

	// translation
	Vector3 vTranslation = Vector3(0,0,0);//pBone->GetPositionAt( fTime );

//	LWS_Item *pParent = pBone->GetParent();
	if( pParent && pParent->GetItemType() != LWS_Item::TYPE_BONE )
	{
		// the parent is not a bone
		// - Assume this is a root bone and add the parent translation
		vTranslation += pParent->GetPositionAt( fTime );
	}

	dest_node.SetTranslation( vTranslation );

	// do recursive calls
	Matrix34 next_transform = Matrix34Identity();
	next_transform.matOrient = matWorldRotation;
	for( size_t i=0; i<num_children; i++ )
	{
		CreateKeyframe( pBone->ChildBone()[i], fTime, next_transform, dest_node.Child()[i] );
	}
  }
  else if( CBone::get_htrans_rev() == 3 )
  {
	// rotation
	const Matrix33 matDeltaRotation = pBone->GetOrientationAt( fTime );// * pBone->GetInvBoneRestOrientation();
	const Matrix33 matRestRotation  = pBone->GetBoneRestOrientation();
	const Matrix33 matWorldRotation = parent_transform.matOrient * matRestRotation;
	Matrix33 matDestRotation( matDeltaRotation );

	LWS_Item *pParent = pBone->GetParent();
//	if( pParent && pParent->GetItemType() != LWS_Item::TYPE_BONE ) // is root bone
//		matDestRotation = matDeltaRotation;
//	else
		matDestRotation = parent_transform.matOrient * matDeltaRotation * Matrix33Transpose(matWorldRotation);

	Quaternion qRotaiton = Quaternion( matDestRotation );
	dest_node.SetRotation( qRotaiton );

	// translation
	Vector3 vTranslation = Vector3(0,0,0);//pBone->GetPositionAt( fTime );

//	LWS_Item *pParent = pBone->GetParent();
	if( pParent && pParent->GetItemType() != LWS_Item::TYPE_BONE )
	{
		// the parent is not a bone
		// - Assume this is a root bone and add the parent translation
		vTranslation += pParent->GetPositionAt( fTime );
	}

	dest_node.SetTranslation( vTranslation );

	// do recursive calls
	Matrix34 next_transform = Matrix34Identity();
	next_transform.matOrient = matWorldRotation;
	for( size_t i=0; i<num_children; i++ )
	{
		CreateKeyframe( pBone->ChildBone()[i], fTime, next_transform, dest_node.Child()[i] );
	}
  }
}


void CLWSMotionDatabaseCompiler::CreateMotionPrimitives( boost::shared_ptr<LWS_Bone> pRootBone )
{
//	int num_motion_primitives = (int)m_vecMotionPrimitiveDesc.size();
//	for( int i=0; i<num_motion_primitives; i++ )
//	{
//	}
}


bool CLWSMotionDatabaseCompiler::LoadLWSceneFile( const std::string& filepath ) //, const CGeometryFilter& geometry_filter )
{
	string filepath_copy = filepath;

//	m_SceneFilepath = filepath_copy;

	m_pScene = shared_ptr<LightWaveSceneLoader>( new LightWaveSceneLoader() );

	bool res = m_pScene->LoadFromFile( filepath );

	return res;
}


Result::Name CLWSMotionDatabaseCompiler::LoadDescFile( const std::string& filepath )
{
/*	using namespace filesystem;

	CTextFileScanner scanner( filepath );
	if( !scanner.IsReady() )
		return Result::UNKNOWN_ERROR;

	string orig_lws_filepath;
	string motion_primitive_name_prifix;
//	char tag_str[256], name_str[256], root_str[256];
	for( ; !scanner.End(); scanner.NextLine() )
	{
		if( scanner.TryScanLine( "LWSFile", orig_lws_filepath ) )
		{
			m_SceneFilepath = path(filepath).parent_path() / path(orig_lws_filepath);
			continue;
		}

		else if( scanner.TryScanLine( "Output", m_OutputFilepath ) )
			continue;

		else if( scanner.TryScanLine( "NamePrefix", motion_primitive_name_prifix ) )
			continue;

		else if( scanner.GetTagString() == "MotionPrimitive" )
		{
			string current_line = scanner.GetCurrentLine();

			vector<string> strings;
			SeparateStrings( strings, current_line.c_str(), " \t\n" );

			if( strings.size() < 4 )
				continue;

			CLWSMotionPrimitiveDesc desc;
			desc.name           = strings[1];
			desc.start_frame    = to_int( strings[2] );
			desc.end_frame      = to_int( strings[3] );

			if( 5 <= strings.size() )
				desc.root_node_name = strings[4];

			m_vecMotionPrimitiveDesc.push_back( desc );

			continue;
		}
	}
*/
	return Result::SUCCESS;
}


Result::Name CLWSMotionDatabaseCompiler::BuildFromDescFile( const std::string& filepath )
{/*
	Result::Name res = LoadDescFile( filepath );
	if( res != Result::SUCCESS )
		return res;

	bool ret = LoadLWSceneFile( m_SceneFilepath.string() );

	shared_ptr<LWS_Bone> pRootBone = CreateSkeleton();

	if( !pRootBone )
		return Result::UNKNOWN_ERROR;

	CreateMotionPrimitives( pRootBone );
*/
	return Result::SUCCESS;
}


/*
// Recursively copies the bones and creates a skeleton structure
// composed of CBone class objects
void CopyBones( shared_ptr<LWS_Bone> pSrcBone,
			    const Matrix34& parent_space,
			    CBone& dest_bone )
{
	const size_t num_children = pSrcBone->ChildBone().size();
	for( size_t i=0; i<num_children; i++ )
	{
		shared_ptr<LWS_Bone> pSrcChildBone = pSrcBone->ChildBone()[i];
		CBone dest_child_bone;

		string& bone_name = pSrcChildBone->GetBoneName();
		Vector3 vRestDir  = pSrcChildBone->GetBoneRestDirection();
		Vector3 vRestPos  = pSrcChildBone->GetBoneRestPosition();
		float fRestLen    = pSrcChildBone->GetBoneRestLength();

		Vec3Normalize( vRestDir, vRestDir );

		Quantize( vRestDir, 0.000001f );
		Quantize( vRestPos, 0.000001f );
		Quantize( fRestLen, 0.000001f );
		Quantize( fRestLen, 0.000001f );
		float afRestAngle[3] = 
		{
			pSrcChildBone->GetBoneRestAngle(0),
			pSrcChildBone->GetBoneRestAngle(1),
			pSrcChildBone->GetBoneRestAngle(2)
		};
		for( int j=0; j<3; j++ )
			Quantize( afRestAngle[j], 0.000001f );

		Matrix34 local_space;
		local_space.matOrient
			= Matrix33Identity()
			* Matrix33RotationX( afRestAngle[1] )
			* Matrix33RotationY( afRestAngle[0] )
			* Matrix33RotationZ( afRestAngle[2] )
			* Matrix33Identity();

		local_space.vPosition = vRestPos;


		Matrix34 world_pose = parent_space * local_space;

		Vector3 vOffset = world_pose.vPosition - parent_space.vPosition;

		dest_child_bone.SetOffset( vRestDir * fRestLen );

		dest_child_bone.SetName( pSrcChildBone->GetBoneName() );

		dest_bone.AddChildBone( dest_child_bone );

		// recursively copy the bones
		CopyBones( pSrcChildBone, parent_space * local_space, dest_bone.Child((int)i) );
	}
}
*/
