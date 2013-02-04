#ifndef __ThirdPersonMotionController_HPP__
#define __ThirdPersonMotionController_HPP__


#include <boost/shared_ptr.hpp>
#include "../3DMath/Matrix34.hpp"
#include "../Input/fwd.hpp"


namespace amorphous
{


class SkeletalCharacter;


class CThirdPersonMotionController
{
	boost::shared_ptr<SkeletalCharacter> m_pCharacter;

	Matrix34 m_CameraPose;

	float m_fVeritcalCameraSpaceInput; ///< camera space input (usually y-axis of an analog gamepad)

	float m_fHorizontalCameraSpaceInput; ///< camera space input (usually x-axis of an analog gamepad)

public:

	CThirdPersonMotionController();
	~CThirdPersonMotionController(){}

	void SetSkeletalCharacter( boost::shared_ptr<SkeletalCharacter> pCharacter ) { m_pCharacter = pCharacter; }

	void Update();

	void SetCameraPose( const Matrix34& pose ) { m_CameraPose = pose; }

	void SetVerticalMotionInput( float f ) { m_fVeritcalCameraSpaceInput = f; }
	void SetHorizontalMotionInput( float f ) { m_fHorizontalCameraSpaceInput = f; }

	void HandleInput( int action_code, const SInputData& input );
};

} // namespace amorphous



#endif /* __ThirdPersonMotionController_HPP__ */
