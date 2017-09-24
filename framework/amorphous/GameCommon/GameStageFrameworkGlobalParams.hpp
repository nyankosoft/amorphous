#ifndef __GSF_GLOBALPARAMS_H__
#define __GSF_GLOBALPARAMS_H__


#include <stdio.h>
#include <string>

#include "amorphous/Support/Serialization/Serialization.hpp"


namespace amorphous
{
using namespace serialization;


/**
  - Stores default values for parameters
    - physics sim timestep: 0.01
    - default shader filename: './Shader/Default.fx'
    - default sleep time [MS]: 3

  - Loaded from a text file during development phase

  - Loaded from a binary file in the release version



  -------------------------- template --------------------------

  DefaultShaderFilename             ./Shader/Default.fx
  DefaultPhysicsSimulationTimestep  0.01
  DefaultSleepTimeMS                2

  --------------------------------------------------------------

*/
class GameStageFrameworkGlobalParams : public IArchiveObjectBase
{

    float m_fDefaultPhysicsSimulationTimestep;

	std::string m_DefaultShaderFilename;

	int m_DefaultSleepTimeMS;

	std::string m_StartupDebugWindow;

public:

	GameStageFrameworkGlobalParams();

	bool LoadFromTextFile( const std::string& filename );

	void UpdateParams();

	void Serialize( IArchive& ar, const unsigned int version );
};

} // namespace amorphous



#endif  /*  __GSF_GLOBALPARAMS_H__  */
