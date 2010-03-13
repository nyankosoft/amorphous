#include "GameWindowManager.hpp"
#include "GameWindowManager_Win32.hpp"
#include "GameWindowManager_Win32_GL.hpp"
#include "GameWindowManager_Win32_D3D.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Graphics/2DPrimitive/2DRectSet.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Graphics/GraphicsResourceCacheManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"

// D3D
#include "Graphics/Direct3D/2DPrimitive/2DPrimitiveRenderer_D3D.hpp"
#include "Graphics/Direct3D/Mesh/D3DXMeshObjectBase.hpp"
#include "Graphics/Direct3D/Shader/D3DFixedFunctionPipelineManager.hpp"
#include "Graphics/Direct3D/Mesh/D3DCustomMeshRenderer.hpp"

// OpenGL
#include "Graphics/OpenGL/2DPrimitive/2DPrimitiveRenderer_GL.hpp"
#include "Graphics/OpenGL/Mesh/GLBasicMeshImpl.hpp"
#include "Graphics/OpenGL/Shader/GLFixedFunctionPipelineManager.hpp"

using namespace std;
using namespace boost;


CGameWindowManager *g_pGameWindowManager = NULL;


Result::Name SelectGraphicsLibrary( const std::string& graphics_library_name )
{
	if( graphics_library_name == "Direct3D" )
	{
		g_pGameWindowManager_Win32 = &GameWindowManager_Win32_D3D();
		PrimitiveRendererPtr() = &PrimitiveRenderer_D3D();
		GraphicsResourceFactory().Init( new CD3DGraphicsResourceFactoryImpl() );
		Ref2DPrimitiveFactory().Init( new C2DPrimitiveFactoryImpl_D3D );
		MeshImplFactory() = shared_ptr<CMeshImplFactory>( new CD3DMeshImplFactory );
		CFixedFunctionPipelineManagerHolder::Get()->Init( &D3DFixedFunctionPipelineManager() );
		CCustomMeshRenderer::ms_pInstance = &(CD3DCustomMeshRenderer::ms_Instance);
	}
	else if( graphics_library_name == "OpenGL" )
	{
		g_pGameWindowManager_Win32 = &GameWindowManager_Win32_GL();
		PrimitiveRendererPtr() = &PrimitiveRenderer_GL();
		GraphicsResourceFactory().Init( new CGLGraphicsResourceFactoryImpl() );
		Ref2DPrimitiveFactory().Init( new C2DPrimitiveFactoryImpl_GL );
		MeshImplFactory() = shared_ptr<CMeshImplFactory>( new CGLMeshImplFactory );
		CFixedFunctionPipelineManagerHolder::Get()->Init( &GLFixedFunctionPipelineManager() );
//		CCustomMeshRenderer::ms_pInstance = &(CGLCustomMeshRenderer::ms_Instance);
	}
	else
	{
		LOG_PRINT_ERROR( "Unsupported graphics library: " + graphics_library_name );
		g_pGameWindowManager_Win32 = NULL;

		return Result::INVALID_ARGS;
	}

	CGraphicsDeviceHolder::Get()->SelectGraphicsDevice( graphics_library_name );

//	PrimitiveRenderer().Init();

	g_pGameWindowManager = g_pGameWindowManager_Win32;

	return Result::SUCCESS;
}
