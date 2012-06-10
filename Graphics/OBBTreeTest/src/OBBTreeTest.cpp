#include "OBBTreeTest.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/3DMath/OBBTree.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/BoundingVolumeTreeRenderers.hpp"
//#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/Graphics/MeshModel/3DMeshModelArchive.hpp"

using std::string;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new COBBTreeTest();
}


extern const std::string GetAppTitle()
{
	return string("OBB Tree Test");
}



COBBTreeTest::COBBTreeTest()
:
m_DrawLevel(0),
m_NumDrawLevels(8)
{
}


COBBTreeTest::~COBBTreeTest()
{
}


int COBBTreeTest::Init()
{
	if( CameraController() )
		CameraController()->SetPosition( Vector3(0,1,-3) );

	m_pFont = CreateDefaultBuiltinFont();

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/OBBTreeTest.fx" );

	// load skybox mesh
//	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );

	// load the terrain mesh
//	CMeshResourceDesc mesh_desc;
//	mesh_desc.ResourcePath = "./models/terrain06.msh";
//	mesh_desc.MeshType     = CMeshType::BASIC;
//	m_Mesh.Load( mesh_desc );

	string model_pathname = "models/Chevelle.msh";
	LoadParamFromFile( "params.txt", "model", model_pathname );

	bool model_loaded = m_Mesh.Load( model_pathname );

	C3DMeshModelArchive mesh_archive;
	mesh_archive.LoadFromFile( model_pathname );

	InitOBBTree( mesh_archive );

	if( m_pOBBTree )
		m_pOBBTree->DumpToTextFile( model_pathname + "-obbtree.txt" );

	return 0;
}


void COBBTreeTest::InitOBBTree( C3DMeshModelArchive& mesh_archive )
{
	m_pOBBTree.reset( new COBBTree );

	int level = 8;
	LoadParamFromFile( "params.txt", "obb_tree_level", level );

	bool created = m_pOBBTree->Create(
		mesh_archive.GetVertexSet().vecPosition,
		mesh_archive.GetVertexIndex(),
		level
		);

	if( !created )
		LOG_PRINT_ERROR( " Failed to create the OBB tree." );
}


void COBBTreeTest::Update( float dt )
{
}


void COBBTreeTest::Render()
{
	Matrix44 matWorld = Matrix44Identity();
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();

	CShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

//	RenderAsSkybox( m_SkyboxMesh, g_CameraController.GetPosition() );

	shader_mgr.SetWorldTransform( matWorld );
/*
	pShaderMgr->SetTechnique( m_MeshTechnique );
*/
//	pShaderMgr->SetTechnique( m_DefaultTechnique );

	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );
	GraphicsDevice().Disable( RenderStateType::LIGHTING );
	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );

	shared_ptr<CBasicMesh> pMesh = m_Mesh.GetMesh();
	if( pMesh )
		pMesh->Render( shader_mgr );

	if( m_pOBBTree )
		RenderOBBTree( *m_pOBBTree, m_DrawLevel );

//	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );
//	rect.Draw();
}


void COBBTreeTest::HandleInput( const SInputData& input )
{
	switch( input.iGICode )
	{
	case GIC_UP:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_DrawLevel = (m_DrawLevel+1) % m_NumDrawLevels;
		break;
	case GIC_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_DrawLevel = (m_DrawLevel-1+m_NumDrawLevels) % m_NumDrawLevels;
		break;
	default:
		break;
	}
}


/*
void COBBTreeTest::UpdateViewTransform( const Matrix44& matView )
{
}


void COBBTreeTest::UpdateProjectionTransform( const Matrix44& matProj )
{
}
*/
