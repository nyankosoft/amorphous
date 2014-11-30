#include "2DPrimitivesTest.hpp"
#include "AsyncLoadingTest.hpp"
#include "BrickWallTextureGeneratorTest.hpp"
#include "CustomMeshTest.hpp"
#include "CubeMapTest.hpp"
#include "EnvMapTest.hpp"
#include "GenericShaderTest.hpp"
#include "GLSLTest.hpp"
#include "GraphicsElementEffectTest.hpp"
#include "GraphicsElementsTest.hpp"
#include "HLSLEffectTest.hpp"
#include "LensFlareTest.hpp"
#include "LightingTest.hpp"
#include "MeshSplitterTest.hpp"
#include "MultibyteFontTest.hpp"
#include "OBBTreeTest.hpp"
#include "PerlinNoiseTextureGeneratorTest.hpp"
#include "PlanarReflectionTest.hpp"
#include "PostProcessEffectTest.hpp"
#include "PrimitiveRendererTest.hpp"
#include "PrimitiveShapeMeshesTest.hpp"
#include "ResolutionChangeTest.hpp"
#include "ShadowMapTest.hpp"
#include "SimpleMotionBlurTest.hpp"
#include "SimpleOverlayEffectsTest.hpp"
#include "SkeletalMeshTest.hpp"
#include "TextureFontTest.hpp"
#include "TextureRenderTargetTest.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/App/GameWindowManager.hpp"

using std::string;


static const char *sg_demos[] =
{
	"2DPrimitivesTest",
	"AsyncLoadingTest",
	"BrickWallTextureGeneratorTest",
	"CustomMeshTest",
	"CubeMapTest",
	"EnvMapTest",
	"GenericShaderTest",
	"GLSLTest",
	"GraphicsElementEffectTest",
	"GraphicsElementsTest",
	"HLSLEffectTest",
	"LensFlareTest",
	"LightingTest",
	"MeshSplitterTest",
	"MultibyteFontTest",
	"OBBTreeTest",
	"PerlinNoiseTextureGeneratorTest",
	"PlanarReflectionTest",
	"PostProcessEffectTest",
	"PrimitiveRendererTest",
	"PrimitiveShapeMeshesTest",
	"ResolutionChangeTest",
	"ShadowMapTest",
	"SimpleMotionBlurTest",
	"SimpleOverlayEffectsTest",
	"SkeletalMeshTest",
	"TextureFontTest",
	"TextureRenderTargetTest"
};


unsigned int DemoSwitcher::GetNumDemos()
{
	return sizeof(sg_demos) / sizeof(sg_demos[0]);
}


const char **DemoSwitcher::GetDemoNames()
{
	return sg_demos;
}


CGraphicsTestBase *DemoSwitcher::CreateTestInstance( const std::string& demo_name )
{
	if( demo_name == "" )
		return NULL;
	else if( demo_name == "2DPrimitivesTest" )                return new C2DPrimitivesTest;
	else if( demo_name == "AsyncLoadingTest" )                return new CAsyncLoadingTest;
	else if( demo_name == "BrickWallTextureGeneratorTest" )   return new CBrickWallTextureGeneratorTest;
	else if( demo_name == "CustomMeshTest" )                  return new CCustomMeshTest;
	else if( demo_name == "CubeMapTest" )                     return new CCubeMapTest;
	else if( demo_name == "EnvMapTest" )                      return new CEnvMapTest;
	else if( demo_name == "GenericShaderTest" )               return new CGenericShaderTest;
	else if( demo_name == "GLSLTest" )                        return new CGLSLTest;
	else if( demo_name == "GraphicsElementEffectTest" )       return new CGraphicsElementEffectTest;
	else if( demo_name == "GraphicsElementsTest" )            return new CGraphicsElementsTest;
	else if( demo_name == "HLSLEffectTest" )                  return new CHLSLEffectTest;
	else if( demo_name == "LensFlareTest" )                   return new CLensFlareTest;
	else if( demo_name == "LightingTest" )                    return new CLightingTest;
	else if( demo_name == "MeshSplitterTest" )                return new CMeshSplitterTest;
	else if( demo_name == "MultibyteFontTest" )               return new CMultibyteFontTest;
	else if( demo_name == "OBBTreeTest" )                     return new OBBTreeTest;
	else if( demo_name == "PerlinNoiseTextureGeneratorTest" ) return new CPerlinNoiseTextureGeneratorTest;
	else if( demo_name == "PlanarReflectionTest" )            return new CPlanarReflectionTest;
	else if( demo_name == "PostProcessEffectTest" )           return new CPostProcessEffectTest;
	else if( demo_name == "PrimitiveRendererTest" )           return new CPrimitiveRendererTest;
	else if( demo_name == "PrimitiveShapeMeshesTest" )        return new CPrimitiveShapeMeshesTest;
	else if( demo_name == "ResolutionChangeTest" )            return new CResolutionChangeTest;
	else if( demo_name == "ShadowMapTest" )                   return new CShadowMapTest;
	else if( demo_name == "SimpleMotionBlurTest" )            return new CSimpleMotionBlurTest;
	else if( demo_name == "SimpleOverlayEffectsTest" )        return new CSimpleOverlayEffectsTest;
	else if( demo_name == "SkeletalMeshTest" )                return new CSkeletalMeshTest;
	else if( demo_name == "TextureFontTest" )                 return new CTextureFontTest;
	else if( demo_name == "TextureRenderTargetTest" )         return new CTextureRenderTargetTest;
	else
		return NULL;
}


CGraphicsTestBase *DemoSwitcher::CreateDemoInstance( unsigned int index )
{
	return CreateTestInstance( GetDemoNames()[index] );
}


bool DemoSwitcher::InitDemo( int index )
{
	if( index < 0 )
		return false;

	// Create the instance of the test class
	m_pDemo.reset( CreateDemoInstance(index) );
	if( !m_pDemo )
		return false;

	m_pDemo->SetCameraController( GetCameraController() );

	const std::string app_title = m_pDemo->GetAppTitle();
	const std::string app_class_name = app_title;

	static bool window_created = false;
	string window_title = app_title;
	window_title += " (" + GetGraphicsLibraryName() + ")";
	GetGameWindowManager().SetWindowTitleText( window_title );
//	if( !window_created )
//	{
//		int w = m_pDemo->GetWindowWidth();  // 1280;
//		int h = m_pDemo->GetWindowHeight(); //  720;
//		LoadParamFromFile( "config", "screen_resolution", w, h );
//		GameWindow::ScreenMode mode = GameWindow::WINDOWED;//m_pDemo->GetFullscreen() ? GameWindow::FULLSCREEN : GameWindow::WINDOWED;
//		GetGameWindowManager().CreateGameWindow( w, h, mode, window_title );
//		Camera().SetAspectRatio( (float)w / (float)h );
//
//		window_created = true;
//	}
//	else
//	{
//		GetGameWindowManager().SetWindowTitleText( window_title );
//	}

	m_pDemo->InitBase();

	m_pDemo->Init();

//	g_pInputHandler.reset( new CGraphicsTestInputHandler(m_pDemo) );

//	g_pCameraController.reset( new CameraController( sg_CameraControllerInputHandlerIndex ) );

	return true;
}


bool DemoSwitcher::InitDemo()
{
	// Load the demo name from file
	std::string demo_name;
	LoadParamFromFile( "params.txt", "demo", demo_name );
	for( unsigned int i=0; i<GetNumDemos(); i++ )
	{
		if( GetDemoNames()[i] == demo_name )
		{
			m_DemoIndex = i;
			break;
		}
	}

	return InitDemo( m_DemoIndex );
}


void DemoSwitcher::NextDemo()
{
	m_DemoIndex = (m_DemoIndex + 1) % GetNumDemos();

	InitDemo( m_DemoIndex );
}


void DemoSwitcher::PrevDemo()
{
	m_DemoIndex = (m_DemoIndex + GetNumDemos() - 1) % GetNumDemos();

	InitDemo( m_DemoIndex );
}
