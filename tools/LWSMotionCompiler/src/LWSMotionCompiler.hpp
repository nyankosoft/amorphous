#ifndef  __LWSMOtionCompiler_HPP__
#define  __LWSMOtionCompiler_HPP__


#include <list>
#include <string>
#include <vector>
#include <memory>
#include <boost/filesystem.hpp>
#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/3DMath/Quaternion.hpp"
#include "amorphous/MotionSynthesis/MotionDatabaseBuilder.hpp"

//#include "Graphics/MeshModel/3DMeshModelBuilder.hpp"
//#include "Graphics/MeshModel/General3DMesh.hpp"

#include "amorphous/LightWave/fwd.hpp"

using namespace amorphous;


class CEnvelope
{
	std::vector<float> m_vecfTime;
};

class CPositionEnvelope : public CEnvelope
{
	std::vector<Vector3> m_vecvPosition;
};

class CRotaitonEnvelope : public CEnvelope
{
	std::vector<Quaternion> m_vecqRotation;
};


class CEnvelopeContainer
{
	std::shared_ptr<CPositionEnvelope> m_pPositionEnvelope;
	std::shared_ptr<CRotaitonEnvelope> m_pRotationEnvelope;

public:

	std::vector< std::shared_ptr<CEnvelopeContainer> > m_vecpChild;
};


class CLWSMotionDatabaseCompiler : public msynth::MotionDatabaseCompiler
//class CLWSMotionCompiler
{
//	boost::filesystem::path m_SceneFilepath;

//	std::string m_OutputFilepath;

	std::shared_ptr<LightWaveSceneLoader> m_pScene;

	std::shared_ptr<msynth::Skeleton> m_pSkeleton;

//	std::vector<CLWSMotionPrimitiveDesc> m_vecMotionPrimitiveDesc;

private:

	std::shared_ptr<LWS_Bone> CreateSkeleton();

	void CreateMotionPrimitive();

	void CreateMotionPrimitives( std::shared_ptr<LWS_Bone> pRootBone );

	void CollectKeyFrameTimes( LWS_Bone& bone, std::vector<float>& vecKeyframeTime );

	void CreateKeyframe( std::shared_ptr<LWS_Bone> pBone, float fTime, const Matrix34& parent_transform, msynth::TransformNode& dest_node );

	void CreateMotionPrimitive( msynth::MotionPrimitiveDescGroup& desc_group, msynth::MotionPrimitiveDesc& desc, std::vector<msynth::Keyframe>& vecSrcKeyframe );

public:

	CLWSMotionDatabaseCompiler();

	~CLWSMotionDatabaseCompiler();

	void CreateMotionPrimitives( msynth::MotionPrimitiveDescGroup& desc_group );

//	void LoadMeshModel();

	/// build mesh from the layers that has the same name as m_strTargetLayerName
//	bool BuildMeshModel();

	bool LoadLWSceneFile( const std::string& filepath );

	Result::Name LoadDescFile( const std::string& filepath );

	Result::Name BuildFromDescFile( const std::string& filepath );
};
/*
class CLWSMotionDatabaseCompiler : public CMotionDatabaseCompiler
{
	std::vector<CMotionPrimitiveDescGroup> m_vecDescGroup;

private:

	bool IsValidMotionFile( const std::string& src_filepath );

	void CreateMotionPrimitive( const CMotionPrimitiveDesc& desc, const CMotionPrimitiveDescGroup& desc_group, CBVHPlayer& bvh_player );

	void CreateMotionPrimitives( CMotionPrimitiveDescGroup& desc_group );

public:

};
*/

class CLWSMotionPrimitiveCompilerCreator : public msynth::MotionPrimitiveCompilerCreator
{
public:

	const char *Extension() const { return "lws"; }

	std::shared_ptr<msynth::MotionDatabaseCompiler> Create() const
	{
		std::shared_ptr<CLWSMotionDatabaseCompiler> pCompiler( new CLWSMotionDatabaseCompiler );
		return pCompiler;
	}
};


#endif		/*  __LWSMOtionCompiler_HPP__  */
