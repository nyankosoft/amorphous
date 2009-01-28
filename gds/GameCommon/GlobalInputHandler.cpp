#include "GlobalInputHandler.h"

#include "App/GameWindowManager_Win32.h"
#include "App/ApplicationBase.h"
#include "Task/GameTaskManager.h"
#include "Task/GameTask.h"

#include "Graphics/GraphicsresourceManager.h"
#include "GameCommon/ImageCapture.h"
#include "GameCommon/ScreenShotManager.h"

#include "Support/DebugOutput.h"


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

			if( g_pAppBase
			 && g_pAppBase->GetTaskManager()
			 && g_pAppBase->GetTaskManager()->GetCurrentTask() )
			{
				g_pAppBase->GetTaskManager()->GetCurrentTask()->Render();
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
