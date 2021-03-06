#include "BE_StaticLiquid.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/ShaderVariableLoader.hpp"
#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "EntityRenderManager.hpp"
#include "amorphous/3DMath/MathMisc.hpp"
#include "amorphous/Graphics/MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{

using namespace std;

/*
class CStaticLiquidEntity : public CCopyEntity;
{
	MeshResourceDesc m_MeshDesc;

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
		m_MeshProperty.m_pMeshRenderMethod.reset( new MeshContainerRenderMethod() );

//	if( m_MeshProperty.m_pMeshRenderMethod->m_vecMeshRenderMethod.empty() )
//		m_MeshProperty.m_pMeshRenderMethod->m_vecMeshRenderMethod.push_back( SubsetRenderMethod() );

//	SubsetRenderMethod& render_method
//		= m_MeshProperty.m_pMeshRenderMethod->m_vecMeshRenderMethod.front();

	SubsetRenderMethod& render_method = m_MeshProperty.m_pMeshRenderMethod->PrimaryMeshRenderMethod();

	GenericShaderDesc shader_desc;
	shader_desc.PlanarReflection = PlanarReflectionOption::PERTURBED;

	ShaderResourceDesc& desc = render_method.m_ShaderDesc;
	desc.pShaderGenerator.reset( new GenericShaderGenerator( shader_desc ) );

//	bool shader_loaded = shader.Load( desc );

	bool loaded = m_MeshProperty.m_pMeshRenderMethod->LoadRenderMethodResources();

	std::shared_ptr<ShaderParamsLoader> pReflectionTextureParam( new ShaderVariableLoader<TextureParam>() );
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

	shared_ptr<EntityRenderManager> pEntityRenderManger
		= m_pStage->GetEntitySet()->GetRenderManager();

	EntityHandle<> handle( pCopyEnt->Self() );
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
	shared_ptr<EntityRenderManager> pEntityRenderManger
		= m_pStage->GetEntitySet()->GetRenderManager();

	EntityHandle<> handle( pCopyEnt->Self() );
	pEntityRenderManger->RemovePlanarReflector( handle, true );
}


bool CBE_StaticLiquid::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	return false;
}


void CBE_StaticLiquid::Serialize( IArchive& ar, const unsigned int version )
{
	BaseEntity::Serialize( ar, version );
}


} // namespace amorphous
