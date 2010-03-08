#include "GlobalInputHandler.hpp"

#include "App/GameWindowManager_Win32.hpp"
#include "App/GameApplicationBase.hpp"
#include "Task/GameTaskManager.hpp"
#include "Task/GameTask.hpp"

#include "Graphics/GraphicsresourceManager.hpp"
#include "GameCommon/ImageCapture.hpp"
#include "GameCommon/ScreenShotManager.hpp"

#include "Support/DebugOutput.hpp"


CGlobalInputHandler::CGlobalInputHandler()
{
}


CGlobalInputHandler::~CGlobalInputHandler()
{
}


void CGlobalInputHandler::ProcessInput(SInputData& input)
{
	CImageCapture image_capture;
	DWORD *pdwImageData = NULL;
	CScreenShotManager screenshot;

	switch( input.iGICode )
	{
	case GIC_ESC:
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
			g_bAppExitRequested = true;
		}*/
//		else if( input.iType == ITYPE_KEY_RELEASED ) {}
		break;

	case GIC_F1:
		if( input.iType == ITYPE_KEY_PRESSED )
			DebugOutput.NextDebugItem();
		break;

	case GIC_F2:
		if( input.iType == ITYPE_KEY_PRESSED )
            DebugOutput.ToggleDisplay();
		break;

	case GIC_F4:
		break;

	case GIC_F5:
		if( input.iType == ITYPE_KEY_PRESSED )
			GraphicsResourceManager().Refresh();
		break;

	case GIC_F7:	// quick load
		break;

	case GIC_F9:
		break;

	case GIC_F10:	// change screen size (to a smaller one)
		break;

	case GIC_F11:	// change screen size (to a larger one)
		break;

	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			int screen_width  = GameWindowManager().GetScreenWidth();
			int screen_height = GameWindowManager().GetScreenHeight();
			image_capture.SetImageSize( screen_width, screen_height );
			image_capture.EnterCaptureMode();

			if( g_pGameAppBase
			 && g_pGameAppBase->GetTaskManager()
			 && g_pGameAppBase->GetTaskManager()->GetCurrentTask() )
			{
				g_pGameAppBase->GetTaskManager()->GetCurrentTask()->Render();
			}

			image_capture.ExitCaptureMode();
			pdwImageData = new DWORD [screen_width * screen_height];
			image_capture.GetCapturedImage( pdwImageData );
			image_capture.Release();
			screenshot.OutputImageDataToFile( pdwImageData, screen_width, screen_height );
			delete [] pdwImageData;
		}
		break;

	default:
		break;	// no action is assigned to this input
	}
}
