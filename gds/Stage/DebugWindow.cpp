
#include "DebugWindow.h"

#include <d3d9.h>
#include <d3dx9.h>

#include "GameCommon/Timer.h"

#include "3DCommon/2DRect.h"
#include "3DCommon/font.h"
#include "3DCommon/TextureFont.h"

#include "Stage/BE_PlayerShip.h"
#include "Stage/PlayerInfo.h"
#include "Stage/PerformanceCheck.h"
#include "Stage/LightEntityManager.h"

#include "Support/Vec3_StringAux.h"
#include "Support/Profile.h"
#include "Support/Log/StateLog.h"


CFont g_Font;
//CTextureFont g_TexFont;
int g_iShowDebugInfo = 0;

/*
void DrawBasicInfo();
void RenderProfileResult();


void DrawDebugInfo()
{
	switch(g_iShowDebugInfo)
	{
	case 0:
//		DrawBasicInfo();
		break;
	case 1:
		RenderProfileResult();
		break;
	default:
		return;
	}
}

void DrawBasicInfo()
{
}

void RenderProfileResult()
{
}
*/