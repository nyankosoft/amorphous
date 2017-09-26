#include "LightEntity.hpp"

#include "amorphous/Stage/BE_Light.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/EntitySet.hpp"
#include "amorphous/Stage/CopyEntityDesc.hpp"
#include "amorphous/Stage/GameMessage.hpp"
#include "amorphous/Stage/Trace.hpp"
#include "amorphous/Support/Log/StateLog.hpp"
#include "amorphous/Support/memory_helpers.hpp"


namespace amorphous
{



class CLightInitializer : public LightVisitor
{
	CLightDesc *m_pDesc;

	void InitHSLightAttribute( HemisphericLightAttribute& hs_attrib )
	{
		hs_attrib.UpperDiffuseColor = m_pDesc->aColor[0];
		hs_attrib.LowerDiffuseColor = m_pDesc->aColor[1];
	}

public:

	CLightInitializer( CLightDesc &desc ) : m_pDesc(&desc) {}

	void VisitLight( Light& light )
	{
		light.DiffuseColor = m_pDesc->aColor[0].GetRGBColor();
		light.fIntensity   = m_pDesc->fIntensity;
	}

//	void VisitAmbientLight( CAmbientLight& ambient_light ) {}

	void VisitPointLight( PointLight& point_light )
	{
		VisitLight( point_light );
		point_light.vPosition        = m_pDesc->vPosition;
		for( int i=0; i<numof(point_light.fAttenuation); i++ )
			point_light.fAttenuation[i]  = m_pDesc->afAttenuation[i];
	}

	void VisitDirectionalLight( DirectionalLight& directional_light )
	{
		VisitLight( directional_light );
		directional_light.vDirection = m_pDesc->vDirection;
	}

	void VisitHemisphericPointLight( HemisphericPointLight& hs_point_light )
	{
		VisitPointLight( hs_point_light );
		InitHSLightAttribute( hs_point_light.Attribute );
	}

	void VisitHemisphericDirectionalLight( HemisphericDirectionalLight& hs_directional_light )
	{
		VisitDirectionalLight( hs_directional_light );
		InitHSLightAttribute( hs_directional_light.Attribute );
	}

//	void VisitTriPointLight( TriPointLight& tri_point_light ) {}
//	void VisitTriDirectionalLight( TriDirectionalLight& tri_directional_light ) {}
};


void LightEntityHandle::SetUpperColor( const SFloatRGBAColor& color )
{
	shared_ptr<LightEntity> pLight = Get();
	if( pLight )
		pLight->SetColor( 0, color );
}


void LightEntityHandle::SetLowerColor( const SFloatRGBAColor& color )
{
	shared_ptr<LightEntity> pLight = Get();
	if( pLight )
		pLight->SetColor( 2, color );
}


void LightEntityHandle::SetAttenuationFactors( float a0, float a1, float a2 )
{
	shared_ptr<LightEntity> pLight = Get();
	if( pLight )
		pLight->SetAttenuationFactors( a0, a1, a2 );
}



LightEntity::LightEntity()
:
m_pLightHolder(NULL),
m_pLightBaseEntity(NULL)
{
//	m_fRadius = 0.0f;
//	m_aabb.Nullify();

	m_LightEntityLink.pOwner = this;

	RaiseEntityFlags( BETYPE_USE_ZSORT );

	RaiseEntityFlags( BETYPE_COPY_PARENT_POSE );

//	m_Attrib |= DESTROY_IF_PARENT_IS_GONE;
}


LightEntity::~LightEntity()
{
}


void LightEntity::Unlink()
{
	CCopyEntity::Unlink();

	UnlinkFromLightEntityList();
}


void LightEntity::LinkDerivedEntity()
{
	GetStage()->GetEntitySet()->LinkLightEntity( this );
}


void LightEntity::Init( CCopyEntityDesc& desc )
{
	// link the entity to the light entity list on the entity node
//	GetStage()->GetEntitySet()->LinkLightEntity( this );
/*
	shared_prealloc_pool<PointLight> m_PointLightPool;
	shared_prealloc_pool<DirectionalLight> m_DirectionalLightPool;
	shared_prealloc_pool<HemisphericPointLight> m_HSPointLightPool;
	shared_prealloc_pool<HemisphericLight> m_HSDirectionalLightPool;
//	shared_prealloc_pool<TriPointLight> m_TriPointLightPool;
//	shared_prealloc_pool<TriDirctionalLight> m_TriDirectionalLightPool;
*/
	CBE_Light *pBaseEntity = dynamic_cast<CBE_Light *>(this->pBaseEntity);

	m_pLightBaseEntity = pBaseEntity;

	LightEntityDesc *pLightDesc = dynamic_cast<LightEntityDesc *>(&desc);

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

		CLightDesc light_desc;

		const LightEntityDesc& default_desc = pBaseEntity->GetDefaultDesc();
		light_desc.LightType = pLightDesc->LightType;

		// colors
		for( int i=0; i<numof(light_desc.aColor); i++ )
		{
			if( fabs(pLightDesc->aColor[i].alpha - CBE_Light::ms_InvalidColor.alpha) < 0.001f )
				light_desc.aColor[i] = default_desc.aColor[i];
			else
				light_desc.aColor[i] = pLightDesc->aColor[i];
		}

		// direction (for directional lights)
		if( pLightDesc->WorldPose.matOrient.GetColumn(2) == CBE_Light::ms_vInvalidDirection )
			light_desc.vDirection = default_desc.WorldPose.matOrient(2);
		else
			light_desc.vDirection = pLightDesc->WorldPose.matOrient(2);

		// intensity
		if( fabs(pLightDesc->fIntensity - CBE_Light::ms_fInvalidIntensity) < 0.001f )
			light_desc.fIntensity = default_desc.fIntensity;
		else
			light_desc.fIntensity = pLightDesc->fIntensity;

		// attenuation factors (for point lights)
		for( int i=0; i<numof(light_desc.afAttenuation); i++ )
		{
			if( fabs(pLightDesc->afAttenuation[i] - CBE_Light::ms_fInvalidAttenuation) < 0.001f )
				light_desc.afAttenuation[i] = default_desc.afAttenuation[i];
			else
				light_desc.afAttenuation[i] = pLightDesc->afAttenuation[i];
		}

		light_desc.vPosition = pLightDesc->WorldPose.vPosition;

		//if( 
		// m_LightGroup = pLightDesc->LightGroup;

		// get pooled light object from the base entity

		m_pLightHolder = pBaseEntity->GetPooledLight( light_desc.LightType );

		CLightInitializer initializer( light_desc );
		if( m_pLightHolder )
			m_pLightHolder->pLight->Accept( initializer );
	}

	float r = 1.0f;
	switch( pLightDesc->LightType )
	{
	case Light::POINT:
	case Light::HEMISPHERIC_POINT:
		r = 100.0f;
		break;
	case Light::AMBIENT:
	case Light::DIRECTIONAL:
	case Light::HEMISPHERIC_DIRECTIONAL:
		r = FLT_MAX;
		break;
	default:
		break;
	}
	this->fRadius = r;
	this->local_aabb.vMin = -Vector3(r,r,r);
	this->local_aabb.vMax =  Vector3(r,r,r);

	// Manually update the world aabb at initialization
	// - needed because world aabb update is done before calling CopyEntity::Init()
	//   in EntityManager::CreateEntity()
	this->world_aabb.TransformCoord( this->local_aabb, this->GetWorldPosition() );

	// link to the tree
	GetStage()->GetEntitySet()->LinkLightEntity( this );
}



static float s_DirLightCheckDist = 100.0f;

bool LightEntity::ReachesEntity( CCopyEntity *pEntity )
{
	// check trace from light to entity
	STrace tr;
	tr.bvType = BVTYPE_DOT;
	Vector3 vGoal = pEntity->GetWorldPosition();
	tr.vGoal = vGoal;
//	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	tr.sTraceType = TRACETYPE_IGNORE_ALL_ENTITIES;
//	tr.pSourceEntity = pEntity;

	Vector3 vLightCenterPos, vLightToEntity, vLightRefPos;

	const Light::Type light_type = GetLightType();

	if( light_type == Light::POINT
	 || light_type == Light::HEMISPHERIC_POINT
	 || light_type == Light::TRI_POINT
	)
	{
		vLightCenterPos = this->GetWorldPosition();
		tr.vStart = vLightCenterPos;
	}
	else if( light_type == Light::DIRECTIONAL
	      || light_type == Light::HEMISPHERIC_DIRECTIONAL
	      || light_type == Light::TRI_DIRECTIONAL
	)
	{
		vLightRefPos = pEntity->GetWorldPosition() - this->GetDirection() * s_DirLightCheckDist;
		tr.vStart = vLightRefPos;
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
	if( pLightEntity->GetLightType() == Light::TYPE_POINT )
	{
		fMaxRangeSq = pLightEntity->GetRadius() + pEntity->fRadius;
		fMaxRangeSq = fMaxRangeSq * fMaxRangeSq;
		vLightToEntity = pEntity->GetWorldPosition() - vLightCenterPos;
		if( fMaxRangeSq < Vec3LengthSq(vLightToEntity) )
			continue;	// out of the light range
	}*/
}


void LightEntity::AddLightIfReachesEntity( CCopyEntity *pEntity )
{
	if( ReachesEntity( pEntity ) )
		pEntity->AddLight( EntityHandle<LightEntity>( m_pLightEntitySelf ) );
}


void LightEntity::TerminateDerived()
{
	m_LightEntityLink.Unlink();

	if( m_pLightHolder )
		m_pLightBaseEntity->ReleasePooledLight( m_pLightHolder );
}


/*
void LightEntity::Init( CCopyEntityDesc& desc )
{
//	LightEntityDesc *pAircraftDesc
//		= dynamic_cast<LightEntityDesc *> (&desc);

//	if( pAircraftDesc )
//		Init( *pAircraftDesc );
}

void LightEntity::Init( LightEntityDesc& desc )
{
	desc.
}*/

/*
void LightEntity::Update( float dt )
{
}


void LightEntity::UpdatePhysics( float dt )
{
}


void LightEntity::Draw()
{
}


void LightEntity::HandleMessage( GameMessage& msg )
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
			case Light::AMBIENT:
				m_pLight = shared_ptr<AmbientLight>( new AmbientLight( pLightDesc->aColor[0], fLightIntensity ) );
				break;
			case Light::DIRECTIONAL:
				m_pLight = shared_ptr<PointLight>( new DirectionalLight( vLightDir, pLightDesc->aColor[0], fLightIntensity, vLightPos );
				break;
			case Light::POINT:
				m_pLight = shared_ptr<PointLight>( new PointLight( vLightPos, pLightDesc->aColor[0], fLightIntensity );
				break;
			case Light::HEMISPHERIC_DIRECTIONAL:
				m_pLight
				= shared_ptr<HemisphericDirectionalLight>( new HemisphericDirectionalLight( vLightDir, pLightDesc->aColor[0], pLightDesc->aColor[1], fLightIntensity, vLightPos );
				break;
			case Light::HEMISPHERIC_POINT:
				m_pLight
				= shared_ptr<HemisphericPointLight>( new HemisphericPointLight( vLightPos, pLightDesc->aColor[0], pLightDesc->aColor[1], fLightIntensity );
				break;
			default:
				break;
			}
//			CLightFactory factory;
//			m_pLight = factory.CreateLight( light_desc );*/


} // namespace amorphous
