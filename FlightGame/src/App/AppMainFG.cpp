
#include "ApplicationBaseFG.h"

#include <windows.h>
#include <mmsystem.h>

#include <vld.h>


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
//	CApplicationBase game_app;
	g_pAppBase = new CApplicationBaseFG;

	g_pAppBase->Run();

	delete g_pAppBase;

	return 0;
}
