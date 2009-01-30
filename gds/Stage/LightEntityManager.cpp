w#include "LightEntityManager.hpp"
#include "EntitySet.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/memory_helpers.hpp"


int g_iNumDynamicLights = 0;


//Atten = 1/( att0 + att1 * d + att2 * d^2 )

/*inline*/ void CLightEntity::SetLightFrom(CCopyEntity& rEntity, int light_type )
{
	ZeroMemory( &m_Light, sizeof(D3DLIGHT9) );
	m_Light.Type         = D3DLIGHT_POINT;	// currently support point light only
/*	m_Light.Diffuse.r    = rEntity.v1.x;
	m_Light.Diffuse.g    = rEntity.v1.y;
	m_Light.Diffuse.b    = rEntity.v1.z;
	m_Light.Diffuse.a    = 1.0f;*/
	SetDiffuseColor( (float *)&rEntity.v1 );

	m_Light.Ambient.r    = 1.0f;	//rEntity.v1.x;
	m_Light.Ambient.g    = 1.0f;	//rEntity.v1.y;
	m_Light.Ambient.b    = 1.0f;	//rEntity.v1.z;
	m_Light.Ambient.a    = 1.0f;

	m_Light.Attenuation0 = 0.0f;	//rEntity.f1
	m_Light.Attenuation1 = 0.2f;	//rEntity.f2
	m_Light.Attenuation2 = 0.0f;	//rEntity.f3
	m_Light.Position     = (D3DVECTOR)rEntity.Position();

	if( light_type == CLE_LIGHT_STATIC )
		m_Light.Range    = m_fRadius = 100.0f; // rEntity.f4
	else
		m_Light.Range    = m_fRadius = 15.0f;

	m_vPosition = rEntity.Position();
	m_aabb.vMax = Vector3( m_fRadius, m_fRadius, m_fRadius );	//rEntity.v2
	m_aabb.vMin = Vector3(-m_fRadius,-m_fRadius,-m_fRadius );	//rEntity.v3

	if( light_type == CLE_LIGHT_STATIC )
	{
		m_LightType = CLE_LIGHT_STATIC;
	}
	else
	{
		m_LightType = CLE_LIGHT_DYNAMIC;
	}

//	SetColor( (float *)&rEntity.v1 );
	m_HemisphericLight.fAttenuation[0] = 0.50f;	//rEntity.f1
	m_HemisphericLight.fAttenuation[1] = 0.50f;	//rEntity.f2
	m_HemisphericLight.fAttenuation[2] = 0.25f;	//rEntity.f3
	m_HemisphericLight.vPosition = rEntity.Position();

	in_use = true;
}


void CLightEntity::SetLight( const CHemisphericPointLight& point_light, int light_type )
{
	ZeroMemory( &m_Light, sizeof(D3DLIGHT9) );
	m_Light.Type         = D3DLIGHT_POINT;

	SFloatRGBAColor color = point_light.Attribute.UpperColor;	// use the upper color for non-hemispheric light
	m_Light.Diffuse.r    = color.fRed;
	m_Light.Diffuse.g    = color.fGreen;
	m_Light.Diffuse.b    = color.fBlue;
	m_Light.Diffuse.a    = color.fAlpha;

	m_Light.Ambient.r    = 1.0f;
	m_Light.Ambient.g    = 1.0f;
	m_Light.Ambient.b    = 1.0f;
	m_Light.Ambient.a    = 1.0f;

	m_Light.Attenuation0 = point_light.fAttenuation[0];
	m_Light.Attenuation1 = point_light.fAttenuation[1];
	m_Light.Attenuation2 = point_light.fAttenuation[2];
	m_Light.Position     = (D3DVECTOR)point_light.vPosition;

	if( light_type == CLE_LIGHT_STATIC )
		m_Light.Range    = m_fRadius = 100.0f;
	else
		m_Light.Range    = m_fRadius = 15.0f;

	m_vPosition = point_light.vPosition;
	m_aabb.vMax = Vector3( m_fRadius, m_fRadius, m_fRadius );
	m_aabb.vMin = Vector3(-m_fRadius,-m_fRadius,-m_fRadius );

	if( light_type == CLE_LIGHT_STATIC )
		m_LightType = CLE_LIGHT_STATIC;
	else
		m_LightType = CLE_LIGHT_DYNAMIC;

	m_HemisphericLight = point_light;

	in_use = true;
}


void CLightEntity::SetLight( const CHemisphericDirectionalLight& dir_light, int light_type )
{
	ZeroMemory( &m_Light, sizeof(D3DLIGHT9) );
	m_Light.Type         = D3DLIGHT_DIRECTIONAL;

	SFloatRGBAColor color = dir_light.Attribute.UpperColor;	// use the upper color for non-hemispheric light
	m_Light.Diffuse.r    = color.fRed;
	m_Light.Diffuse.g    = color.fGreen;
	m_Light.Diffuse.b    = color.fBlue;
	m_Light.Diffuse.a    = color.fAlpha;

	m_Light.Ambient.r    = 1.0f;
	m_Light.Ambient.g    = 1.0f;
	m_Light.Ambient.b    = 1.0f;
	m_Light.Ambient.a    = 1.0f;

	m_vPosition = Vector3(0,0,0);

	float r = 1000000.0f;
	m_fRadius = r;
	m_aabb.vMax = Vector3( r, r, r );
	m_aabb.vMin = Vector3(-r,-r,-r );

	if( light_type == CLE_LIGHT_STATIC )
		m_LightType = CLE_LIGHT_STATIC;
	else
		m_LightType = CLE_LIGHT_DYNAMIC;

	m_HemisphericDirLight = dir_light;

	in_use = true;
}


CLightEntityManager::CLightEntityManager()
{
	m_pEntitySet = NULL;

	m_iNumStaticLights = 0;
	m_iNumDynamicLights = 0;

//	m_pDynamicLightList = NULL;

	// for shaders
	m_pShaderLightManager = NULL;
	m_pEffect = NULL;

	m_iNumPrevLights = 0;
}


CLightEntityManager::~CLightEntityManager()
{
	SafeDelete( m_pShaderLightManager );
}


void CLightEntityManager::Init( CEntitySet* pEntitySet )
{
	m_pEntitySet = pEntitySet;

	InitShaderLightManager();
}


void CLightEntityManager::InitShaderLightManager()
{
	SafeDelete( m_pShaderLightManager );
	m_pShaderLightManager = new CShaderLightManager;

	m_pShaderLightManager->Init();

	m_pShaderLightManager->SetDirectionalLightOffset( 0 );
	m_pShaderLightManager->SetNumDirectionalLights( 0 );

//	m_pShaderLightManager->SetPointLightOffset( CLEM_POINT_LIGHT_OFFSET );
	m_pShaderLightManager->SetPointLightOffset( 0 );
	m_pShaderLightManager->SetNumPointLights( 0 );

	// provide direct access to effect interface
	// need to be updated when the shader is released and reloaded
	m_pEffect = CShader::Get()->GetCurrentShaderManager()->GetEffect();
}


int CLightEntityManager::RegisterLight( CCopyEntity& rLightEntity, int light_type )
{

	if( light_type == CLE_LIGHT_STATIC )
	{	// add a static light
//		m_vecStaticLightEntity.push_back( new_light );
//		CLightEntity& rNewLight = m_vecStaticLightEntity.back();
//		rNewLight.SetIndex( m_vecStaticLightEntity.size() - 1 );

		if( CLEM_NUM_MAX_STATIC_LIGHTS <= m_iNumStaticLights )
			return CE_INVALID_LIGHT_INDEX;

		CLightEntity& rNewLight = m_aStaticLightEntity[m_iNumStaticLights];
		rNewLight.SetLightFrom( rLightEntity, light_type );
		rNewLight.SetIndex( m_iNumStaticLights );
		m_iNumStaticLights++;

		// link to the scene tree
		m_pEntitySet->LinkLightEntity( &rNewLight );

		return rNewLight.GetIndex();
	}
	else if( light_type == CLE_LIGHT_DYNAMIC )
	{	// add a dynamic light
		int i;	//, iNumDynamicLights = m_aDynamicLightEntity.size();
		for( i=0; i<CLEM_NUM_MAX_DYNAMIC_LIGHTS; i++ )
		{
			// find a vacant slot
			if( m_aDynamicLightEntity[i].IsInUse() )
				continue;

			// found a vacant slot - create a dynamic light by overwriting an already deleted light
			m_aDynamicLightEntity[i].SetLightFrom( rLightEntity, light_type );
			m_aDynamicLightEntity[i].SetIndex( CLEM_NUM_MAX_STATIC_LIGHTS + i );

			// attach to the link list
			m_aDynamicLightEntity[i].m_pPrevLight = &m_DynamicLightHead;
			m_aDynamicLightEntity[i].m_pNextLight = m_DynamicLightHead.m_pNextLight;
			if( m_DynamicLightHead.m_pNextLight )
			{
				m_DynamicLightHead.m_pNextLight->m_pPrevLight = &m_aDynamicLightEntity[i];
			}
//			else
//			{	 first light entity in the link list	}

			m_DynamicLightHead.m_pNextLight = &m_aDynamicLightEntity[i];

			m_iNumDynamicLights++;
			g_iNumDynamicLights = m_iNumDynamicLights;

			// link to the scene tree
			m_pEntitySet->LinkLightEntity( &m_aDynamicLightEntity[i] );
			
			return m_aDynamicLightEntity[i].GetIndex();

		}

		// cannot create any more dynamic light entity
		return CE_INVALID_LIGHT_INDEX;

		// couldn't find a vacant slot in the dynamic light array - add a new dynamic light entity
/*		CLightEntity new_light;
		m_aDynamicLightEntity.push_back( new_light );
		CLightEntity& rNewLight = m_aDynamicLightEntity.back();
		rNewLight.SetLightFrom( rLightEntity, light_type );

		rNewLight.SetIndex( CLEM_NUM_MAX_STATIC_LIGHTS + iNumDynamicLights );

		m_pEntitySet->LinkLightEntity( &rNewLight );

		return rNewLight.GetIndex();*/
	}

	return CE_INVALID_LIGHT_INDEX;
}


void CLightEntityManager::GetNewLightEntity( int light_type, int& index, CLightEntity*& pLightEntity )
{

	if( light_type == CLE_LIGHT_STATIC )
	{
		// add a static light to the scene
		if( CLEM_NUM_MAX_STATIC_LIGHTS <= m_iNumStaticLights )
		{
			index = CE_INVALID_LIGHT_INDEX;
			return;
		}

		CLightEntity& rNewLight = m_aStaticLightEntity[m_iNumStaticLights];
		rNewLight.SetIndex( m_iNumStaticLights );

		m_iNumStaticLights++;

		// set the output values of this functions
		index = rNewLight.GetIndex();
		pLightEntity = &rNewLight;
	}
	else if( light_type == CLE_LIGHT_DYNAMIC )
	{
		// add a dynamic light

		// find a vacant slot
		for( int i=0; i<CLEM_NUM_MAX_DYNAMIC_LIGHTS; i++ )
		{
			if( m_aDynamicLightEntity[i].IsInUse() )
				continue;	// this one's already taken - try the next one

			// found a vacant slot - create a dynamic light by overwriting an already deleted light
			m_aDynamicLightEntity[i].SetIndex( CLEM_NUM_MAX_STATIC_LIGHTS + i );

			// attach to the link list
			m_aDynamicLightEntity[i].m_pPrevLight = &m_DynamicLightHead;
			m_aDynamicLightEntity[i].m_pNextLight = m_DynamicLightHead.m_pNextLight;
			if( m_DynamicLightHead.m_pNextLight )
			{
				m_DynamicLightHead.m_pNextLight->m_pPrevLight = &m_aDynamicLightEntity[i];
			}
//			else
//			{	 first light entity in the link list	}

			m_DynamicLightHead.m_pNextLight = &m_aDynamicLightEntity[i];

			m_iNumDynamicLights++;
			g_iNumDynamicLights = m_iNumDynamicLights;
			
			// set the output values of this function
			index = m_aDynamicLightEntity[i].GetIndex();
			pLightEntity = &m_aDynamicLightEntity[i];

			return;
		}

		// cannot create any more dynamic light entity
		index = CE_INVALID_LIGHT_INDEX;
	}
}


int CLightEntityManager::RegisterHemisphericPointLight( CHemisphericPointLight& light, int light_type )
{
	int index;
	CLightEntity* pLightEntity = NULL;
	GetNewLightEntity( light_type, index, pLightEntity );

	if( index == CE_INVALID_LIGHT_INDEX )
		return CE_INVALID_LIGHT_INDEX;

	// copy light properties to the newly acquired light entity
	pLightEntity->SetLight( light, light_type );

	// link to the scene tree
	m_pEntitySet->LinkLightEntity( pLightEntity );

	return index;
}

int CLightEntityManager::RegisterHemisphericDirLight( CHemisphericDirectionalLight& dir_light, int light_type )
{
	int index;
	CLightEntity* pLightEntity = NULL;
	GetNewLightEntity( light_type, index, pLightEntity );

	if( index == CE_INVALID_LIGHT_INDEX )
		return CE_INVALID_LIGHT_INDEX;


	// copy light properties to the newly acquired light entity
	pLightEntity->SetLight( dir_light, light_type );


	///////////////// testing for error - OK /////////////////
//	return index;

	///////////////// check inside - error in normal execution mode (Ctrl + F5) /////////////////
	// link to the scene tree
	m_pEntitySet->LinkLightEntity( pLightEntity );

//	MsgBoxFmt( "light entity(index: %d) has been linked to the entity tree", pLightEntity->GetIndex() );

	return index;
}


void CLightEntityManager::UpdateLightPosition( int light_index, Vector3& rvNewPosition )
{
	if( light_index < CLEM_NUM_MAX_STATIC_LIGHTS )
		return;	// don't change the positions of static lights

	CLightEntity& rLightEntity = m_aDynamicLightEntity[light_index - CLEM_NUM_MAX_STATIC_LIGHTS];

	rLightEntity.SetPosition( rvNewPosition );

	rLightEntity.UnlinkFromEntityTree();

	m_pEntitySet->LinkLightEntity(&rLightEntity);
}


void CLightEntityManager::RelinkLightEntities()
{
	int i;
	for( i=0; i<m_iNumStaticLights; i++ )
	{
		m_pEntitySet->LinkLightEntity( &m_aStaticLightEntity[i] );
	}

	for( i=0; i<m_iNumDynamicLights; i++ )
	{
		m_pEntitySet->LinkLightEntity( &m_aDynamicLightEntity[i] );
	}
}


void CLightEntityManager::ReleaseGraphicsResources()
{
}


void CLightEntityManager::LoadGraphicsResources()
{
	InitShaderLightManager();
}