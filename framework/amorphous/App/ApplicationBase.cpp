#include "ApplicationBase.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Input/InputHub.hpp"
#include "GameWindowManager_Generic.hpp"


namespace amorphous
{


// Has to be defined by the user of the framework.
extern ApplicationBase *CreateApplicationInstance();


ApplicationBase *ApplicationBase::ms_pInstance = nullptr;
std::vector<std::string> ApplicationBase::ms_CommandLineArguments;

std::map<unsigned int,int> sg_KeyMaps;


void ApplicationBase::StartApp()
{
	GetKeyMaps(sg_KeyMaps);

	amorphous::SetCurrentThreadAsRenderThread();

	// Create the instance of application implemented by the user
	ApplicationBase::SetInstance( CreateApplicationInstance() );

	ApplicationBase::GetInstance()->Run();

	ApplicationBase::ReleaseInstance();
}

void ApplicationBase::OnKeyDown(unsigned int keycode)
{
	InputData input;
	if( sg_KeyMaps.find(keycode) == sg_KeyMaps.end() )
		return;
	input.iGICode = sg_KeyMaps[keycode];
	input.iType = ITYPE_KEY_PRESSED;
	GetInputHub().UpdateInput(input);
}

void ApplicationBase::OnKeyUp(unsigned int keycode)
{
	InputData input;
	if( sg_KeyMaps.find(keycode) == sg_KeyMaps.end() )
		return;
	input.iGICode = sg_KeyMaps[keycode];
	input.iType = ITYPE_KEY_RELEASED;
	GetInputHub().UpdateInput(input);
}

void ApplicationBase::OnButtonDown(unsigned int button)
{

}

void ApplicationBase::OnButtonUp(unsigned int button)
{

}

} // namespace amorphous
