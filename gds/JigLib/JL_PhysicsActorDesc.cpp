#include "JL_PhysicsActorDesc.hpp"
#include "JL_ShapeDesc.hpp"
#include "JL_ShapeDescFactory.hpp"

void CJL_PhysicsActorDesc::CalculateMassProperties()
{
	memset( &m_matLocalInertia, 0, sizeof(Matrix33) );

	for( size_t i=0; i<vecpShapeDesc.size(); i++ )
	{
//		m_matLocalInertia += vecpShapeDesc[i]->GetLocalInertia();
	}
}

void CJL_PhysicsActorDesc::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_matLocalInertia;
	ar & vPosition & matOrient;
	ar & vVelocity & vAngularVelocity;
	ar & fMass;
	ar & ActorFlag;
	ar & bAllowFreezing;
	ar & iCollisionGroup;

	CJL_ShapeDescFactory shapedesc_factory;

	ar.Polymorphic( vecpShapeDesc, shapedesc_factory );
}
