
#include "ApplicationBaseFG.h"


extern CApplicationBase *CreateApplicationInstance() { return new CApplicationBaseFG(); }


/*
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
//	CApplicationBase game_app;
	g_pAppBase = new CApplicationBaseFG;

	g_pAppBase->Run();

	delete g_pAppBase;

	return 0;
}
*/