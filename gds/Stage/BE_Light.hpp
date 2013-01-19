#ifndef __BE_Light_H__
#define __BE_Light_H__

#include "BaseEntity.hpp"
#include "CopyEntity.hpp"
#include "LightEntity.hpp"

#include "Graphics/FloatRGBAColor.hpp"
#include "Graphics/HemisphericLight.hpp"
#include "Support/Serialization/Serialization_Light.hpp"
#include "Support/StringAux.hpp"
#include "Support/prealloc_pool.hpp"


namespace amorphous
{


class CBE_Light : public CBaseEntity
{
protected:

	prealloc_pool<CLightHolder> m_DirectionalLightPool;
	prealloc_pool<CLightHolder> m_PointLightPool;
	prealloc_pool<CLightHolder> m_SpotlightPool;
	prealloc_pool<CLightHolder> m_HSDirectionalLightPool;
	prealloc_pool<CLightHolder> m_HSPointLightPool;
	prealloc_pool<CLightHolder> m_HSSpotlightPool;
//	prealloc_pool<CTriPointLight> m_TriPointLightPool;
//	prealloc_pool<CTriDirctionalLight> m_TriDirectionalLightPool;

	unsigned int m_TypeFlag;

	CLightEntityDesc m_DefaultDesc;

protected:

	float& FadeoutTime( CCopyEntity* pCopyEnt ) { return pCopyEnt->f4; }

	float& TimeLeft( CCopyEntity* pCopyEnt ) { return pCopyEnt->f5; }

public:

	enum eLightTypeFlag
	{
//		TYPE_STATIC		= (1 << 0),
//		TYPE_DYNAMIC	= (1 << 1),
		TYPE_TIMER		= (1 << 2),
		TYPE_FADEOUT	= (1 << 3),
		TYPE_GLARE		= (1 << 4),
	};

	/* examples for type flags

	// static lights on static geometry - loaded with the stage
	m_TypeFlag = TYPE_GLARE|TYPE_STATIC;

	// dynamic lights with no glare textures
	// - used with texture animations for explosions, muzzle flashes, etc.
	m_TypeFlag = TYPE_DYNAMIC;

	// glare effect only and don't illuminate anything. mainly used with items
	m_TypeFlag = TYPE_GLARE;

*/

	CBE_Light();

	virtual ~CBE_Light() {}

	virtual void Init();

	virtual void InitCopyEntity( CCopyEntity* pCopyEnt );

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual void Serialize( IArchive& ar, const unsigned int version );

	CLightHolder *GetPooledLight( Light::Type light_type );

	void ReleasePooledLight( CLightHolder *pLightHolder );

	const CLightEntityDesc& GetDefaultDesc() const { return m_DefaultDesc; }

	friend class CCoreBaseEntitiesLoader;

public:

	static const SFloatRGBAColor ms_InvalidColor;
	static const Vector3 ms_vInvalidDirection;
	static const int ms_InvalidLightGroup;
	static const float ms_fInvalidIntensity;
	static const float ms_fInvalidAttenuation;
};

} // namespace amorphous



#endif  /*  __BE_Light_H__  */
