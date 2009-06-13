#ifndef  __BaseEntity_Draw_HPP__
#define  __BaseEntity_Draw_HPP__


#include "EntityHandle.hpp"
#include "Graphics/MeshContainerRenderMethod.hpp"


extern void SetLightsToShader( CCopyEntity& entity,  CShaderManager& rShaderMgr );


class CEntityShaderLightParamsLoader : public CShaderParamsLoader
{
//	boost::shared_ptr<CCopyEntity> m_pEntity;
	CEntityHandle<> m_Entity;

public:

	CEntityShaderLightParamsLoader( boost::shared_ptr<CCopyEntity> pEntity = boost::shared_ptr<CCopyEntity>() )
		:
	m_Entity(pEntity)
	{}

//	void SetEntity( boost::shared_ptr<CCopyEntity> pEntity ) { m_Entity = CEntityHandle<>( pEntity ); }
	void SetEntity( boost::weak_ptr<CCopyEntity> pEntity ) { m_Entity = CEntityHandle<>( pEntity ); }

	void UpdateShaderParams( CShaderManager& rShaderMgr )
	{
		boost::shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
		if( pEntity )
			SetLightsToShader( *(pEntity.get()), rShaderMgr );
	}
};


#endif  /*  __BaseEntity_Draw_HPP__  */
