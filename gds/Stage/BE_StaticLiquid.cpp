#include "BE_StaticLiquid.hpp"
#include "Graphics/Shader/GenericShaderGenerator.hpp"
#include "Graphics/Shader/ShaderVariableLoader.hpp"
#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "3DMath/MathMisc.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{

using namespace boost;

/*
class CStaticLiquidEntity : public CCopyEntity;
{
	CMeshResourceDesc m_MeshDesc;

	Vector3 m_vLiquidFlowVelocity;

public:

	CStaticLiquidEntity()
		:
	m_vLiquidFlowVelocity( Vector3(0,0,0) )
	{}

	~CStaticLiquidEntity(){}

	void InitStaticLiquidEntity();
};


void CStaticLiquidEntity::InitStaticLiquidEntity()
{
	if( m_MeshHandle.ResourcePath.length() == 0 )
	{
		// No mesh file is specified.
		// - Create liquid surface as rectangular area that matches the top face of AABB.
		m_MeshDesc.pGenerator.reset( CPlaneMeshGenerator(  ) );
	}
	entity.m_MeshHandle.Load( entity.m_MeshDesc );
}


void InitStaticLiquidEntity( CStaticLiquidEntity& entity )
{
	entity.InitStaticLiquidEntity();
}
*/


CBE_StaticLiquid::CBE_StaticLiquid()
{
}


void CBE_StaticLiquid::Init()
{
	// Initialize m_MeshProperty.m_pMeshRenderMethod
	Init3DModel();

	// Each entity of static volume liquid may have its own mesh.
	// - Meshes of CBE_StaticLiquid cannot be stored in this->m_MeshProperty.

	if( !m_MeshProperty.m_pMeshRenderMethod )
		m_MeshProperty.m_pMeshRenderMethod.reset( new CMeshContainerRenderMethod() );

//	if( m_MeshProperty.m_pMeshRenderMethod->m_vecMeshRenderMethod.empty() )
//		m_MeshProperty.m_pMeshRenderMethod->m_vecMeshRenderMethod.push_back( CSubsetRenderMethod() );

//	CSubsetRenderMethod& render_method
//		= m_MeshProperty.m_pMeshRenderMethod->m_vecMeshRenderMethod.front();

	CSubsetRenderMethod& render_method = m_MeshProperty.m_pMeshRenderMethod->PrimaryMeshRenderMethod();

	CGenericShaderDesc shader_desc;
	shader_desc.PlanarReflection = CPlanarReflectionOption::PERTURBED;

	CShaderResourceDesc& desc = render_method.m_ShaderDesc;
	desc.pShaderGenerator.reset( new CGenericShaderGenerator( shader_desc ) );

//	bool shader_loaded = shader.Load( desc );

	bool loaded = m_MeshProperty.m_pMeshRenderMethod->LoadRenderMethodResources();

	shared_ptr<CShaderParamsLoader> pReflectionTextureParam( new CShaderVariableLoader<CTextureParam>() );
	m_MeshProperty.m_pMeshRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pReflectionTextureParam );
}


void CBE_StaticLiquid::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	// load shader to render liquid surface
	// - The shader load liquid surface is per-base entity property
//	CStaticVolumeLiquidEntity *pSLEntity = dynamic_case<CStaticVolumeLiquidEntity *>(pCopyEnt);
//	if( !pSLEntity )
//		return;

//	InitStaticLiquidEntity( *pSLEntity );

//	std::string model;

//	C3DMeshModelArchive ar;
//	bool loaded = ar.LoadFromFile( model );
//	if( !loaded )
//		return;

//	int water_surface_subset_index = 0;
//	for( size_t i=0; i<ar.GetTriangleSet().size(); i++ )
//	{
//	}

	shared_ptr<CEntityRenderManager> pEntityRenderManger
		= m_pStage->GetEntitySet()->GetRenderManager();

	CEntityHandle<> handle( pCopyEnt->Self() );
	const Plane plane( Vector3(0,1,0), 0 );
	pEntityRenderManger->AddPlanarReflector( handle, plane );
}


void CBE_StaticLiquid::Act(CCopyEntity* pCopyEnt)
{
}


/// Renders the surface of the liquid
void CBE_StaticLiquid::Draw(CCopyEntity* pCopyEnt)
{
	TextureHandle reflected_scene_texture;
}

void CBE_StaticLiquid::OnEntityDestroyed(CCopyEntity* pCopyEnt)
{
	shared_ptr<CEntityRenderManager> pEntityRenderManger
		= m_pStage->GetEntitySet()->GetRenderManager();

	CEntityHandle<> handle( pCopyEnt->Self() );
	pEntityRenderManger->RemovePlanarReflector( handle, true );
}


bool CBE_StaticLiquid::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	return false;
}


void CBE_StaticLiquid::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );
}


} // namespace amorphous
