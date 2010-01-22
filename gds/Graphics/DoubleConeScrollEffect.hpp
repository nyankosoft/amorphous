#ifndef  __DoubleConeScrollEffect_HPP__
#define  __DoubleConeScrollEffect_HPP__


#include <vector>
#include <boost/shared_ptr.hpp>
#include "fwd.hpp"
#include "3DMath/Matrix34.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"
#include "Support/Serialization/Serialization_BoostSmartPtr.hpp"
using namespace GameLib1::Serialization;


//namespace ???
//{


class CDoubleConeScrollEffectDesc
{
public:
	float m_fStreakLength;
	float m_fScrollSpeed;
};


class CDoubleConeScrollEffect
{
	std::string m_TextureFilepath;

//	CTextureHandle m_Texture;

	Matrix34 m_CameraPose;

	Vector3 m_vCameraVelocity;

	CMeshObjectHandle m_DoubleConeMesh;

	float m_fTexShiftV;

public:

	CDoubleConeScrollEffect();

	~CDoubleConeScrollEffect() {}

	void SetTextureFilepath( const std::string& tex_filepath ) { m_TextureFilepath = tex_filepath; }

	void Init();

	void Update( float dt );

	void Render();

	void SetCameraVelocity( const Vector3& vVel ) { m_vCameraVelocity = vVel; }

	void SetCameraPose( const Matrix34& cam_pose ) { m_CameraPose = cam_pose; }

/*
	const std::string& GetName() const { return m_Name; }
	void SetName( const std::string& name ) { m_Name = name; }
*/
};


//} // namespace ???


#endif  /*  __DoubleConeScrollEffect_HPP__  */
