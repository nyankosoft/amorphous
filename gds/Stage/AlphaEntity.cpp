#include "AlphaEntity.hpp"

#include "Stage/Stage.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/MTRand.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


CAlphaEntity::CAlphaEntity()
{
	// set material index 0 as the alpha material by default
	m_AlphaMaterialIndex = 0;

	RaiseEntityFlags( BETYPE_USE_ZSORT );

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
	CCopyEntity *pParent = m_pParent;
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
	CCopyEntity *pParent = m_pParent;
	if( !pParent )
		return;

	// copy parent light info
	m_vecLight = pParent->m_vecLight;

	this->pBaseEntity->Draw3DModel( this );

/*	pParent->pBaseEntity->DrawMeshMaterial(
		pParent->GetWorldPose(),
		m_AlphaMaterialIndex,
		m_ShaderTechnique
		);
*/
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
//	shared_ptr<CCopyEntity> pSelf = this->Self().lock();
//	m_pPool->release( pSelf ); // pSelf is CCopyEntity type pointer!!!
}


} // namespace amorphous
