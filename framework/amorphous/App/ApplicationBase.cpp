#include "ApplicationBase.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"


namespace amorphous
{


// Has to be defined by the user of the framework.
extern ApplicationBase *CreateApplicationInstance();


ApplicationBase *ApplicationBase::ms_pInstance = nullptr;
std::vector<std::string> ApplicationBase::ms_CommandLineArguments;


void ApplicationBase::StartApp()
{
	amorphous::SetCurrentThreadAsRenderThread();

	// Create the instance of application implemented by the user
	ApplicationBase::SetInstance( CreateApplicationInstance() );

	ApplicationBase::GetInstance()->Run();

	ApplicationBase::ReleaseInstance();
}


} // namespace amorphous
