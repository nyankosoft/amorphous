
#include "ApplicationBase.h"

#include <windows.h>
#include <mmsystem.h>

#include <vld.h>

/*
class CUserDefinedApp : public CApplicationBase
{
};
*/


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
//	CApplicationBase game_app;
	g_pAppBase = new /*CUserDefinedApp*/;

	g_pAppBase->Run();

	delete g_pAppBase;

	return 0;
}