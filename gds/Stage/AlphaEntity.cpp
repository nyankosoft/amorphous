#include "AlphaEntity.h"

#include "Stage/Stage.h"
#include "Stage/CopyEntityDesc.h"
#include "Stage/GameMessage.h"
#include "Support/Log/StateLog.h"
#include "Support/memory_helpers.h"
#include "GameCommon/MTRand.h"

using namespace std;


CAlphaEntity::CAlphaEntity()
:
m_AlphaMaterialIndex(0)
{
//	m_Attrib |= DESTROY_IF_PARENT_IS_GONE;
}


CAlphaEntity::~CAlphaEntity()
{
}


/*
void CAlphaEntity::Init( CCopyEntityDesc& desc )
{
//	CAlphaEntityDesc *pAircraftDesc
//		= dynamic_cast<CAlphaEntityDesc *> (&desc);

//	if( pAircraftDesc )
//		Init( *pAircraftDesc );
}

void CAlphaEntity::Init( CAlphaEntityDesc& desc )
{
	desc.
}*/


void CAlphaEntity::Update( float dt )
{
	if( !pParent )
		return;

	SetWorldPose( pParent->GetWorldPose() );
}

/*
void CAlphaEntity::UpdatePhysics( float dt )
{
}
*/

void CAlphaEntity::Draw()
{
	if( !pParent )
		return;

/*	SetWorldPose( pParent->GetWorldPose() );
	CMeshObjectBase *pMesh = MeshObjectHandle.GetMeshObject();
	if( pMesh )
	{
		pMesh->DrawSubset( m_AlphaMaterialIndex );
	}
*/

/*	pParent->pBaseEntity->DrawMeshObject(
		pParent->GetWorldPose(),
		MeshObjectHandle.GetMeshObject(),
		pBaseEntity->m_MeshProperty->m_ShaderTechniqueTable,
		0
		);*/
}


void CAlphaEntity::HandleMessage( SGameMessage& msg )
{
/*	switch( msg.iEffect )
	{
	default:
		break;
	}*/
}


void CAlphaEntity::TerminateDerived()
{
}
