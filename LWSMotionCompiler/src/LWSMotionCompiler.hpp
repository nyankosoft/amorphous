#ifndef  __LWSMOtionCompiler_HPP__
#define  __LWSMOtionCompiler_HPP__


#include <list>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include "3DMath/Matrix34.hpp"
#include "3DMath/Quaternion.hpp"
#include "MotionSynthesis/MotionDatabaseBuilder.hpp"

//#include "Graphics/MeshModel/3DMeshModelBuilder.hpp"
//#include "Graphics/MeshModel/General3DMesh.hpp"

#include "LightWave/fwd.hpp"

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
	boost::shared_ptr<CPositionEnvelope> m_pPositionEnvelope;
	boost::shared_ptr<CRotaitonEnvelope> m_pRotationEnvelope;

public:

	std::vector< boost::shared_ptr<CEnvelopeContainer> > m_vecpChild;
};


class CLWSMotionDatabaseCompiler : public msynth::CMotionDatabaseCompiler
//class CLWSMotionCompiler
{
//	boost::filesystem::path m_SceneFilepath;

//	std::string m_OutputFilepath;

	boost::shared_ptr<CLightWaveSceneLoader> m_pScene;

	boost::shared_ptr<msynth::CSkeleton> m_pSkeleton;

//	std::vector<CLWSMotionPrimitiveDesc> m_vecMotionPrimitiveDesc;

private:

	boost::shared_ptr<CLWS_Bone> CreateSkeleton();

	void CreateMotionPrimitive();

	void CreateMotionPrimitives( boost::shared_ptr<CLWS_Bone> pRootBone );

	void CollectKeyFrameTimes( CLWS_Bone& bone, std::vector<float>& vecKeyframeTime );

	void CreateKeyframe( boost::shared_ptr<CLWS_Bone> pBone, float fTime, const Matrix34& parent_transform, msynth::CTransformNode& dest_node );

	void CreateMotionPrimitive( msynth::CMotionPrimitiveDescGroup& desc_group, msynth::CMotionPrimitiveDesc& desc, std::vector<msynth::CKeyframe>& vecSrcKeyframe );

public:

	CLWSMotionDatabaseCompiler();

	~CLWSMotionDatabaseCompiler();

	void CreateMotionPrimitives( msynth::CMotionPrimitiveDescGroup& desc_group );

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

class CLWSMotionPrimitiveCompilerCreator : public msynth::CMotionPrimitiveCompilerCreator
{
public:

	const char *Extension() const { return "lws"; }

	boost::shared_ptr<msynth::CMotionDatabaseCompiler> Create() const
	{
		boost::shared_ptr<CLWSMotionDatabaseCompiler> pCompiler( new CLWSMotionDatabaseCompiler );
		return pCompiler;
	}
};


#endif		/*  __LWSMOtionCompiler_HPP__  */
