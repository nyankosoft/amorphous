#ifndef  __LWSMOtionCompiler_HPP__
#define  __LWSMOtionCompiler_HPP__


#include <list>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include "3DMath/Matrix34.hpp"
#include "3DMath/Quaternion.hpp"
#include "MotionSynthesis/fwd.hpp"

//#include "Graphics/MeshModel/3DMeshModelBuilder.hpp"
//#include "Graphics/MeshModel/General3DMesh.hpp"

#include "LightWave/fwd.hpp"


class CLightWaveSceneLoader;


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


class CLWSMotionCompiler
{
	boost::filesystem::path m_SceneFilepath;

	std::string m_OutputFilepath;

	boost::shared_ptr<CLightWaveSceneLoader> m_pScene;

	std::vector<CMotionPrimitiveDesc> m_vecMotionPrmitiveDesc;

private:

	boost::shared_ptr<CLWS_Bone> CreateSkeleton();

	void CreateMotionPrimitive();

	void CreateMotionPrimitives( boost::shared_ptr<CLWS_Bone> pRootBone );

	void CollectKeyFrameTimes( CLWS_Bone& bone, std::vector<float>& vecKeyframeTime );

	void CreateKeyframe( boost::shared_ptr<CLWS_Bone> pBone, float fTime, msynth::CTransformNode& dest_node );

public:

	CLWSMotionCompiler();

	~CLWSMotionCompiler();

//	void LoadMeshModel();

	/// build mesh from the layers that has the same name as m_strTargetLayerName
//	bool BuildMeshModel();

	bool LoadLWSceneFile( const std::string& filepath );

	Result::Name LoadDescFile( const std::string& filepath );

	Result::Name BuildFromDescFile( const std::string& filepath );
};



#endif		/*  __LWSMOtionCompiler_HPP__  */
