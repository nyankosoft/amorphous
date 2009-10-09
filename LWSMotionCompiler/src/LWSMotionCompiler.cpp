#include "LWSMotionCompiler.hpp"
#include "Platform_Win32.hpp"
#include "gds/LightWave/LightWaveSceneLoader.hpp"
#include "gds/MotionSynthesis/MotionPrimitive.hpp"
#include "gds/Support/TextFileScanner.hpp"
#include "gds/Support/StringAux.hpp"
#include "gds/base.hpp"
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;
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


class CMotionPrimitiveDesc
{
public:

	std::string name;
	int start_frame;
	int end_frame;
	std::string root_node_name;
	bool is_loop_motion;

	CMotionPrimitiveDesc()
		:
	start_frame(0),
	end_frame(0),
	is_loop_motion(false)
	{}
};



CLWSMotionCompiler::CLWSMotionCompiler()
{
}


CLWSMotionCompiler::~CLWSMotionCompiler()
{
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


void CLWSMotionCompiler::CollectKeyFrameTimes( CLWS_Bone& bone, vector<float>& vecKeyframeTime )
{
//	shared_ptr<CLWS_Bone> pRootBone = ???;

	int i,j;
	int num_channels = bone.GetNumChannels();
	for( i=0; i<num_channels; i++ )
	{
		const CLWS_Channel& channel = bone.GetChannel(i);
		const int num_keys = (int)channel.vecKey.size();
		for( j=0; j<num_keys; j++ )
		{
			const CLWS_Keyframe& keyframe = channel.vecKey[j];
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


// Recursively copies the bones and creates a skeleton structure
// composed of CBone class objects
void CopyBones( shared_ptr<CLWS_Bone> pSrcBone, CBone& dest_bone )
{
	const size_t num_children = pSrcBone->ChildBone().size();
	for( size_t i=0; i<num_children; i++ )
	{
		shared_ptr<CLWS_Bone> pSrcChildBone = pSrcBone->ChildBone()[i];
		CBone dest_child_bone;

		const string& bone_name = pSrcChildBone->GetBoneName();
		const Vector3 vRestDir  = pSrcChildBone->GetBoneRestDirection();
		const Vector3 vRestPos  = pSrcChildBone->GetBoneRestPosition();
		const float fRestLen    = pSrcChildBone->GetBoneRestLength();

		dest_child_bone.SetOffset( vRestDir * fRestLen );

		dest_child_bone.SetName( pSrcChildBone->GetBoneName() );

		dest_bone.AddChildBone( dest_child_bone );

		// recursively copy the bones
		CopyBones( pSrcChildBone, dest_bone.Child((int)i) );
	}
}

//void CLWSMotionCompiler::CreateSkeleton()

/// Returns a valid root bone on success
shared_ptr<CLWS_Bone> CLWSMotionCompiler::CreateSkeleton()
{
	if( !m_pScene )
		return shared_ptr<CLWS_Bone>();

	// restore tree structure of the bones

	vector< shared_ptr<CLWS_Bone> > vecpRootBones = m_pScene->GetRootBones();

	shared_ptr<CLWS_Bone> pRootBone;
	const size_t num_root_bones = vecpRootBones.size();
	if( num_root_bones == 0 )
	{
		return shared_ptr<CLWS_Bone>();
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

	CopyBones( pRootBone, root_bone );

	shared_ptr<CSkeleton> m_pSkeleton = shared_ptr<CSkeleton>( new CSkeleton );
	m_pSkeleton->SetBones( root_bone );

	return pRootBone;
}


void CLWSMotionCompiler::CreateMotionPrimitive()
{/*
	shared_ptr<CLWS_Bone> pRootBone;

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


void CLWSMotionCompiler::CreateKeyframe( shared_ptr<CLWS_Bone> pBone, float fTime, CTransformNode& dest_node )
{
	const size_t num_children = pBone->ChildBone().size();
	dest_node.SetNumChildren( (int)num_children );

	// rotation
	dest_node.SetRotation( Quaternion( pBone->GetOrientationAt( fTime ) ) );
	
	// translation
	dest_node.SetTranslation( pBone->GetPositionAt( fTime ) );

	for( size_t i=0; i<num_children; i++ )
	{
		CreateKeyframe( pBone->ChildBone()[i], fTime, dest_node.Child()[i] );
	}
}


void CLWSMotionCompiler::CreateMotionPrimitives( boost::shared_ptr<CLWS_Bone> pRootBone )
{
	vector<float> vecfKeyframeTime;
	vecfKeyframeTime.reserve( 0xFF );
	CollectKeyFrameTimes( *(pRootBone.get()), vecfKeyframeTime );

	const int fps = 30;

	const int num_keyframes = (int)vecfKeyframeTime.size();
	for( int i=0; i<num_keyframes; i++ )
	{
		float fTime = (float)num_keyframes / (float)fps;

		CKeyframe dest_keyframe;
		CreateKeyframe( pRootBone, fTime, dest_keyframe.RootNode() );

		dest_keyframe;
	}

	int num_motion_primitives = 1;
	for( int i=0; i<num_motion_primitives; i++ )
	{
	}
}


bool CLWSMotionCompiler::LoadLWSceneFile( const std::string& filepath ) //, const CGeometryFilter& geometry_filter )
{
	string filepath_copy = filepath;

	m_SceneFilepath = filepath_copy;

	m_pScene = shared_ptr<CLightWaveSceneLoader>( new CLightWaveSceneLoader() );

	m_pScene->LoadFromFile( filepath );

	return true;
}


Result::Name CLWSMotionCompiler::LoadDescFile( const std::string& filepath )
{
	using namespace filesystem;

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

/*			int start_frame = 0, end_frame = 0;
			memset( tag_str,  0, sizeof(tag_str) );
			memset( name_str, 0, sizeof(name_str) );
			memset( root_str, 0, sizeof(root_str) );
			sscanf( current_line.c_str(),
				"%s %s %d %d %s",
				 tag_str, name_str, &start_frame, &end_frame, root_str );

			CMotionPrimitiveDesc desc;
			desc.name           = name_str;
			desc.start_frame    = start_frame;
			desc.end_frame      = end_frame;
			desc.root_node_name = root_str;*/

			CMotionPrimitiveDesc desc;
			desc.name           = strings[1];
			desc.start_frame    = to_int( strings[2] );
			desc.end_frame      = to_int( strings[3] );

			if( 5 <= strings.size() )
				desc.root_node_name = strings[4];

			continue;
		}
	}

	return Result::SUCCESS;
}


Result::Name CLWSMotionCompiler::BuildFromDescFile( const std::string& filepath )
{
	Result::Name res = LoadDescFile( filepath );
	if( res != Result::SUCCESS )
		return res;

	bool ret = LoadLWSceneFile( m_SceneFilepath.string() );

	shared_ptr<CLWS_Bone> pRootBone = CreateSkeleton();

	if( !pRootBone )
		return Result::UNKNOWN_ERROR;

	CreateMotionPrimitives( pRootBone );

	return Result::SUCCESS;
}
