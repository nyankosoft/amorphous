#include "BSPMap.hpp"

#include "Stage/stage.h"
#include "Stage/EntitySet.hpp"
#include "Stage/trace.hpp"
#include "Stage/ScreenEffectManager.hpp"

#include "DynamicLightManagerForStaticGeometry.hpp"

#include "Graphics/Direct3D9.hpp"
#include "Graphics/TextureTool.hpp"

#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "ShaderTechniqueIDs.h"

#include <direct.h>


namespace amorphous
{


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


} // amorphous
