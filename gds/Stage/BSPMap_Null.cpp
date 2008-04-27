
#include "BSPMap.h"

#include "Stage/stage.h"
#include "Stage/EntitySet.h"
#include "Stage/trace.h"
#include "Stage/ScreenEffectManager.h"

#include "Support/memory_helpers.h"
#include "PerformanceCheck.h"

#include "DynamicLightManagerForStaticGeometry.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/TextureTool.h"

#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"

#include "ShaderTechniqueIDs.h"

#include "JigLib/JL_PhysicsManager.h"
#include "JigLib/JL_ShapeDesc_TriangleMesh.h"
#include "JigLib/JL_PhysicsActor.h"
#include <direct.h>


CBSPMap::CBSPMap( CStage *pStage )
: CStaticGeometryBase( pStage )
{
	assert( !"CBSPMap::CBSPMap() - this is a dummy implementation, and should not be instantiated."
}


CBSPMap::~CBSPMap()
{
}


void CBSPMap::ReleaseGraphicsResources()
{
}


bool CBSPMap::LoadFromFile(const char *pFilename, bool bLoadGraphicsOnly )
{
	return false;
}


//==========================================================================
//
//					'Collision Detection' Functions
//
//==========================================================================

short CBSPMap::CheckPosition(STrace& tr)
{
	return 0;
}


int CBSPMap::ClipTrace(STrace &tr)
{
	return 0;
}


//works as a 'CheckPosition() ' when tr.fFraction == 0
void CBSPMap::ClipTraceToInteriorModel_r(STrace& tr, short nodeindex)
{
}


//==========================================================================
//
//						'Rendering' Functions
//
//==========================================================================

bool CBSPMap::Render( const CCamera &rCam, const unsigned int EffectFlag )
{
	return false;
}



void CBSPMap::SetDynamicLightManager( CEntitySet* pEntitySet )
{
}
