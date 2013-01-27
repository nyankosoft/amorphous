#ifndef __MiscEntityRenderers_HPP__
#define __MiscEntityRenderers_HPP__


#include "EntityRenderer.hpp"


namespace amorphous
{


class CStandardEntityRenderer : public CEntityRenderer
{
	CEntityRenderManager *m_pEntityRenderManager;

public:

	CStandardEntityRenderer( CEntityRenderManager *pEntityRenderManager )
		:
	m_pEntityRenderManager(pEntityRenderManager)
	{}

	bool ShouldRenderEntity( CCopyEntity& entity )
	{
		if( !(entity.GetEntityFlags() & BETYPE_VISIBLE) )
			return false;

		if( entity.GetEntityFlags() & BETYPE_PLANAR_REFLECTOR )
		{
			if( m_pEntityRenderManager )
			{
				int id = m_pEntityRenderManager->GetCurrentlyRenderedPlanarReflectionSceneID();
				if( 0 <= id || id == entity.s1 )
					return false;
			}
		}

		return true;
	}

	void RenderEntity( CCopyEntity& entity )
	{
		if( entity.GetEntityFlags() & BETYPE_USE_ZSORT )
		{
			// 'pEntity' includes transparent polygons
			if( m_pEntityRenderManager )
				m_pEntityRenderManager->SendToZSortTable( &entity );
		}
		else
		{
/*			if( entity.Lighting() )
			{
				if( entity.sState & CESTATE_LIGHT_INFORMATION_INVALID )
				{
					// need to update light information - find lights that reaches to this entity
					entity.ClearLights();
					m_pEntitySet->UpdateLightInfo( &entity );
					entity.sState &= ~CESTATE_LIGHT_INFORMATION_INVALID;
				}

				// turn on lights that reach 'pCopyEnt'
				m_pEntitySet->EnableLightForEntity();
				m_pEntitySet->SetLightsForEntity( &entity );
			}
			else
			{	// turn off lights
				m_pEntitySet->DisableLightForEntity();
			}*/

			// render the entity
			entity.Draw();
		}
	}
};


class CShadowCasterEntityRenderer : public CEntityRenderer
{
public:

	bool ShouldRenderEntity( CCopyEntity& entity )
	{
		U32 entity_flags = entity.GetEntityFlags();
		if( entity_flags & BETYPE_VISIBLE
		 && entity_flags & BETYPE_SHADOW_CASTER )
		{
			return true;
		}
		else
			return false;
	}

	void RenderEntity( CCopyEntity& entity )
	{
		// render the entity as a shadow caster
		entity.pBaseEntity->RenderAsShadowCaster( &entity );
	}
};


class CShadowReceiverEntityRenderer : public CEntityRenderer
{
public:

	bool ShouldRenderEntity( CCopyEntity& entity )
	{
		U32 entity_flags = entity.GetEntityFlags();
		if( entity_flags & BETYPE_VISIBLE
//		 && entity_flags & BETYPE_SHADOW_RECEIVER )
		 && (entity_flags & BETYPE_SHADOW_CASTER || entity_flags & BETYPE_SHADOW_RECEIVER) )
		{
			return true;
		}
		else
			return false;
	}

	void RenderEntity( CCopyEntity& entity )
	{
		// render the entity as a shadow receiver
		// Note that the shadow casters are also rendered as non-shadowed geometries
		entity.pBaseEntity->RenderAsShadowReceiver( &entity );
	}
};


class CNonEnvMapTargetEntityRenderer : public CEntityRenderer
{
	U32 m_EnvMapTargetEntityID;

public:

	CNonEnvMapTargetEntityRenderer()
		:
	m_EnvMapTargetEntityID(0)
	{}

	CNonEnvMapTargetEntityRenderer( U32 target_entity_id )
		:
	m_EnvMapTargetEntityID(target_entity_id)
	{}

	bool ShouldRenderEntity( CCopyEntity& entity )
	{
		if( entity.GetID() == m_EnvMapTargetEntityID
		 && entity.GetEntityFlags() & BETYPE_ENVMAPTARGET )
		{
			// envmap target
			// - should no be rendered to the cube texture of its own envmap
			return false;
		}
		else
			return true;
	}

	void RenderEntity( CCopyEntity& entity )
	{
		entity.pBaseEntity->Draw( &entity );
	}
};


class CEntityDepthRenderer : public CEntityRenderer
{
	ShaderHandle m_DepthRenderShader;
public:

	bool ShouldRenderEntity( CCopyEntity& entity )
	{
		return ( 0 < (entity.GetEntityFlags() & BETYPE_VISIBLE) );
	}

	void RenderEntity( CCopyEntity& entity )
	{
//		ShaderManager *pShaderMgr = m_DepthRenderShader.GetShaderManager();
//		if( !pShaderMgr )
//			return;
//
//		boost::shared_ptr<BasicMesh> pMesh = entity.m_MeshHandle.GetMesh();
//		if( !pMesh )
//			return;
//
//		pShaderMgr->SetWorldTransform( entity.GetWorldPose() );
	}
};


} // namespace amorphous



#endif /* __MiscEntityRenderers_HPP__ */
