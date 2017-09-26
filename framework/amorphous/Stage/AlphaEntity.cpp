#include "AlphaEntity.hpp"

#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/CopyEntityDesc.hpp"
#include "amorphous/Stage/GameMessage.hpp"
#include "amorphous/Support/Log/StateLog.hpp"
#include "amorphous/Support/memory_helpers.hpp"
#include "amorphous/Support/MTRand.hpp"


namespace amorphous
{

using namespace std;


AlphaEntity::AlphaEntity()
{
	// set material index 0 as the alpha material by default
	m_AlphaMaterialIndex = 0;

	RaiseEntityFlags( BETYPE_USE_ZSORT );

//	m_Attrib |= DESTROY_IF_PARENT_IS_GONE;
}


AlphaEntity::~AlphaEntity()
{
}


/*
void AlphaEntity::Init( CCopyEntityDesc& desc )
{
//	AlphaEntityDesc *pAircraftDesc
//		= dynamic_cast<AlphaEntityDesc *> (&desc);

//	if( pAircraftDesc )
//		Init( *pAircraftDesc );
}

void AlphaEntity::Init( AlphaEntityDesc& desc )
{
	desc.
}*/


void AlphaEntity::Update( float dt )
{
	CCopyEntity *pParent = m_pParent;
	if( !pParent )
		return;

	SetWorldPose( pParent->GetWorldPose() );
}

/*
void AlphaEntity::UpdatePhysics( float dt )
{
}
*/

void AlphaEntity::Draw()
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


void AlphaEntity::HandleMessage( GameMessage& msg )
{
/*	switch( msg.iEffect )
	{
	default:
		break;
	}*/
}


void AlphaEntity::TerminateDerived()
{
//	shared_ptr<CCopyEntity> pSelf = this->Self().lock();
//	m_pPool->release( pSelf ); // pSelf is CCopyEntity type pointer!!!
}


} // namespace amorphous
