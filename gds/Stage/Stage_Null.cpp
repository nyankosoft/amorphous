#include "bspmap.h"
#include "EntitySet.hpp"
#include "trace.hpp"
#include "ViewFrustumTest.hpp"

#include "Sound/GameSoundManager.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/StageEntryPoint.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "GameEvent/GameEventManager.hpp"

#include "Item/WeaponSystem.hpp"


namespace amorphous
{
//#include "GameCommon/WeaponDatabase.hpp"
//#include "GameCommon/AmmoDatabase.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/FileNameOperation.h"
#include "Support/Profile.hpp"


#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "ShaderTechniqueIDs.h"


#include "JigLib/JL_PhysicsManager.hpp"
#include "JigLib/JL_SurfaceMaterial.hpp"

#include "SurfaceMaterialManager.hpp"

#include "stage.h"

#include "PerformanceCheck.h"
SPerformanceCheck g_PerformanceCheck;

CStage::CStage()
{

	m_pScreenEffectManager = NULL;
	m_pStaticGeometry = NULL;
	m_pEntitySet = NULL;

	m_pPhysicsManager = NULL;

	m_pMaterialManager = NULL;

//	CBSPTreeForAABB::Initialize();

	m_pEventManager = NULL;
}


CStage::~CStage()
{
//	CGraphicsComponentCollector::Get()->DeleteComponent( this );

//	SafeDelete( m_pBSPMap );
//	SafeDelete( m_pEntitySet );
//	SafeDelete( m_pGlareEffect );
//	SafeDelete( m_pScreenEffectManager );
//	SafeDelete( m_pEventManager );
//	SafeDelete( m_pPhysicsManager );
}


bool CStage::LoadStageFromFile( const std::string& filename )
{
	return false;
}


bool CStage::EnterStage( string& strPlayerEntityName, string& strEntryPointName )
{
	return false;
}


// render stage from the view of an entity which has been specifed as the camera entity
void CStage::Render()
{
}


// render stage by directly providing the camera
void CStage::Render(CCamera& rCam)
{
}


void CStage::ClipTrace(STrace& tr)
{
}


//check if a bounding volume is in a valid position
//if it is in a solid position, turn 'tr.in_solid' to 'true' and return
void CStage::CheckPosition(STrace& tr)
{
}


void CStage::CheckCollision(CTrace& tr)
{
}


void CStage::GetVisibleEntities( CViewFrustumTest& vf_test )
{
}


char CStage::IsCurrentlyVisibleCell(short sCellIndex)
{
	return 0;
}


void CStage::Update( float dt )
{
}


void CStage::ReleasePhysicsActor( CJL_PhysicsActor* pPhysicsActor )
{
}

/*
void CStage::PlaySound3D( char* pcSoundName, Vector3& rvPosition )
{
}


void CStage::PlaySound3D( int iIndex, Vector3& rvPosition )
{
}


void CStage::PlaySound3D( CSoundHandle &rSoundHandle, Vector3& rvPosition )
{
}
*/

Vector3 CStage::GetGravityAccel() const
{
	return Vector3( 0, -9.8f, 0 );
}

/*
void CStage::SaveCurrentState(FILE* fp)
{
}

void CStage::LoadSavedData(FILE* fp)
{
}
*/

bool CStage::LoadMaterial()
{
	return false;
}


CSurfaceMaterial s_BlankMaterial;

CSurfaceMaterial& CStage::GetMaterial( int index )
{
	return s_BlankMaterial;
}


void CStage::LoadEvents()
{
}


//CGameEvent *CStage::GetEvent( int iIndex ) { return NULL; }


CJL_PhysicsVisualizer_D3D *CStage::GetPhysicsVisualizer()
{
	return NULL;
}

} // amorphous
