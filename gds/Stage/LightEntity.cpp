#include "LightEntity.hpp"

#include "Stage/BE_Light.hpp"
#include "Stage/Stage.hpp"
#include "Stage/EntitySet.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"
#include "Stage/Trace.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/memory_helpers.hpp"

using namespace std;
using namespace boost;


class CLightInitializer : public CLightVisitor
{
	CLightDesc *m_pDesc;

	void InitHSLightAttribute( CHemisphericLightAttribute& hs_attrib )
	{
		hs_attrib.UpperDiffuseColor = m_pDesc->aColor[0];
		hs_attrib.LowerDiffuseColor = m_pDesc->aColor[1];
	}

public:

	CLightInitializer( CLightDesc &desc ) : m_pDesc(&desc) {}

	void VisitLight( CLight& light )
	{
		light.DiffuseColor = m_pDesc->aColor[0].GetRGBColor();
		light.fIntensity   = m_pDesc->fIntensity;
	}

//	void VisitAmbientLight( CAmbientLight& ambient_light ) {}

	void VisitPointLight( CPointLight& point_light )
	{
		VisitLight( point_light );
		point_light.vPosition        = m_pDesc->vPosition;
		for( int i=0; i<numof(point_light.fAttenuation); i++ )
			point_light.fAttenuation[i]  = m_pDesc->afAttenuation[i];
	}

	void VisitDirectionalLight( CDirectionalLight& directional_light )
	{
		VisitLight( directional_light );
		directional_light.vDirection = m_pDesc->vPosition;
	}

	void VisitHemisphericPointLight( CHemisphericPointLight& hs_point_light )
	{
		VisitPointLight( hs_point_light );
		InitHSLightAttribute( hs_point_light.Attribute );
	}

	void VisitHemisphericDirectionalLight( CHemisphericDirectionalLight& hs_directional_light )
	{
		VisitDirectionalLight( hs_directional_light );
		InitHSLightAttribute( hs_directional_light.Attribute );
	}

//	void VisitTriPointLight( CTriPointLight& tri_point_light ) {}
//	void VisitTriDirectionalLight( CTriDirectionalLight& tri_directional_light ) {}
};


CLightEntity::CLightEntity()
:
m_pLightHolder(NULL),
m_pLightBaseEntity(NULL)
{
//	m_fRadius = 0.0f;
//	m_aabb.Nullify();

	m_LightEntityLink.pOwner = this;

	EntityFlag |= BETYPE_USE_ZSORT;

	EntityFlag |= BETYPE_COPY_PARENT_POSE;

//	m_Attrib |= DESTROY_IF_PARENT_IS_GONE;
}


CLightEntity::~CLightEntity()
{
}


void CLightEntity::Init( CCopyEntityDesc& desc )
{
	// link the entity to the light entity list on the entity node
//	GetStage()->GetEntitySet()->LinkLightEntity( this );
/*
	shared_prealloc_pool<CPointLight> m_PointLightPool;
	shared_prealloc_pool<CDirectionalLight> m_DirectionalLightPool;
	shared_prealloc_pool<CHemisphericPointLight> m_HSPointLightPool;
	shared_prealloc_pool<CHemisphericLight> m_HSDirectionalLightPool;
//	shared_prealloc_pool<CTriPointLight> m_TriPointLightPool;
//	shared_prealloc_pool<CTriDirctionalLight> m_TriDirectionalLightPool;
*/
	CBE_Light *pBaseEntity = dynamic_cast<CBE_Light *>(this->pBaseEntity);

	m_pLightBaseEntity = pBaseEntity;

	CLightEntityDesc *pLightDesc = dynamic_cast<CLightEntityDesc *>(&desc);

	if( !pLightDesc )
		return;

	if( pLightDesc->pLight )
	{
		LOG_PRINT_ERROR( " Init by a shared_ptr light object - Not supported yet." );
//		m_pLight = pLightDesc->pLight;
	}
//	else if( pLightDesc->pLightRawPtr )
//	{
		// copy light
//	}
	else
	{
		// fill in the light desc

		const Vector3& vLightPos = pLightDesc->WorldPose.vPosition;
		const Vector3& vLightDir = pLightDesc->WorldPose.matOrient(2);
		const float& fLightIntensity = pLightDesc->fIntensity;

		CLightDesc light_desc;
		light_desc.LightType = pLightDesc->LightType;
		for( int i=0; i<numof(light_desc.aColor); i++ )
			light_desc.aColor[i] = pLightDesc->aColor[i];
		light_desc.vPosition = pLightDesc->WorldPose.vPosition;
		light_desc.vDirection = pLightDesc->WorldPose.matOrient(2);
		light_desc.fIntensity = pLightDesc->fIntensity;

		for( int i=0; i<numof(light_desc.afAttenuation); i++ )
			light_desc.afAttenuation[i] = pLightDesc->afAttenuation[i];

		// get pooled light object from the base entity

		m_pLightHolder = pBaseEntity->GetPooledLight( light_desc.LightType );

		CLightInitializer initializer( light_desc );
		if( m_pLightHolder )
			m_pLightHolder->pLight->Accept( initializer );
	}

	float r = 1.0f;
	switch( pLightDesc->LightType )
	{
	case CLight::POINT:
	case CLight::HEMISPHERIC_POINT:
		r = 100.0f;
		break;
	case CLight::AMBIENT:
	case CLight::DIRECTIONAL:
	case CLight::HEMISPHERIC_DIRECTIONAL:
		r = FLT_MAX;
		break;
	default:
		break;
	}
	this->fRadius = r;
	this->local_aabb.vMin = -Vector3(r,r,r);
	this->local_aabb.vMax =  Vector3(r,r,r);

	// link to the tree
	GetStage()->GetEntitySet()->LinkLightEntity( this );
}



static float s_DirLightCheckDist = 100.0f;

bool CLightEntity::ReachesEntity( CCopyEntity *pEntity )
{
	// check trace from light to entity
	STrace tr;
	tr.bvType = BVTYPE_DOT;
	tr.pvGoal = &pEntity->Position();
//	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	tr.sTraceType = TRACETYPE_IGNORE_ALL_ENTITIES;
//	tr.pSourceEntity = pEntity;

	Vector3 vLightCenterPos, vLightToEntity, vLightRefPos;

	const CLight::Type light_type = GetLightType();

	if( light_type == CLight::POINT
	 || light_type == CLight::HEMISPHERIC_POINT
	 || light_type == CLight::TRI_POINT
	)
	{
		vLightCenterPos = this->Position();
		tr.pvStart = &vLightCenterPos;
	}
	else if( light_type == CLight::DIRECTIONAL
	      || light_type == CLight::HEMISPHERIC_DIRECTIONAL
	      || light_type == CLight::TRI_DIRECTIONAL
	)
	{
		vLightRefPos = pEntity->Position() - this->GetDirection() * s_DirLightCheckDist;
		tr.pvStart = &vLightRefPos;
	}
	else
	{
		bool unsupported_light = true;
	}


	if( !IsSensible(vLightCenterPos) )
		int iError = 1;

	GetStage()->ClipTrace( tr );
	if( tr.fFraction < 1.0f )
		return false;	// static geometry obstacle between light and entity

	return true;

	// Shouldn't this be done in tree node traversal
/*
	if( pLightEntity->GetLightType() == CLight::TYPE_POINT )
	{
		fMaxRangeSq = pLightEntity->GetRadius() + pEntity->fRadius;
		fMaxRangeSq = fMaxRangeSq * fMaxRangeSq;
		vLightToEntity = pEntity->Position() - vLightCenterPos;
		if( fMaxRangeSq < Vec3LengthSq(vLightToEntity) )
			continue;	// out of the light range
	}*/
}


void CLightEntity::AddLightIfReachesEntity( CCopyEntity *pEntity )
{
	if( ReachesEntity( pEntity ) )
		pEntity->AddLight( CEntityHandle<CLightEntity>( m_pLightEntitySelf ) );
}


void CLightEntity::TerminateDerived()
{
	m_LightEntityLink.Unlink();

	if( m_pLightHolder )
		m_pLightBaseEntity->ReleasePooledLight( m_pLightHolder );
}


/*
void CLightEntity::Init( CCopyEntityDesc& desc )
{
//	CLightEntityDesc *pAircraftDesc
//		= dynamic_cast<CLightEntityDesc *> (&desc);

//	if( pAircraftDesc )
//		Init( *pAircraftDesc );
}

void CLightEntity::Init( CLightEntityDesc& desc )
{
	desc.
}*/

/*
void CLightEntity::Update( float dt )
{
}


void CLightEntity::UpdatePhysics( float dt )
{
}


void CLightEntity::Draw()
{
}


void CLightEntity::HandleMessage( SGameMessage& msg )
{
//	switch( msg.iEffect )
//	{
//	default:
//		break;
//	}
}
*/

//			CLightFactory factory;
//			m_pLight = factory.CreateLight( light_desc );
/*			switch( pLightDesc->LightType )
			{
			case CLight::AMBIENT:
				m_pLight = shared_ptr<CAmbientLight>( new CAmbientLight( pLightDesc->aColor[0], fLightIntensity ) );
				break;
			case CLight::DIRECTIONAL:
				m_pLight = shared_ptr<CPointLight>( new CDirectionalLight( vLightDir, pLightDesc->aColor[0], fLightIntensity, vLightPos );
				break;
			case CLight::POINT:
				m_pLight = shared_ptr<CPointLight>( new CPointLight( vLightPos, pLightDesc->aColor[0], fLightIntensity );
				break;
			case CLight::HEMISPHERIC_DIRECTIONAL:
				m_pLight
				= shared_ptr<CHemisphericDirectionalLight>( new CHemisphericDirectionalLight( vLightDir, pLightDesc->aColor[0], pLightDesc->aColor[1], fLightIntensity, vLightPos );
				break;
			case CLight::HEMISPHERIC_POINT:
				m_pLight
				= shared_ptr<CHemisphericPointLight>( new CHemisphericPointLight( vLightPos, pLightDesc->aColor[0], pLightDesc->aColor[1], fLightIntensity );
				break;
			default:
				break;
			}
//			CLightFactory factory;
//			m_pLight = factory.CreateLight( light_desc );*/
