#include "MotionEditorApp.hpp"
#include "MotionPrimitiveViewer.h"
#include "gds/Graphics/Font/Font.hpp"
#include "gds/Graphics/GraphicsDevice.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/FileOpenDialog_Win32.hpp"
#include "gds/Support/ParamLoader.hpp"

#include "gds/MotionSynthesis/MotionDatabaseBuilder.hpp"
#include "gds/MotionSynthesis/BVHMotionDatabaseCompiler.hpp"

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;
using namespace boost::filesystem;


CApplicationBase *CreateApplicationInstance() { return new CMotionEditorApp; }


/*
void CMotionEditorAppInputHandler::ProcessInput( SInputData& input )
{
	m_pTest->ProcessInput( input );
}
*/


CMotionEditorApp::CMotionEditorApp()
{
//	m_UseCameraController = false;

//	Camera().SetPose( Matrix34( Vector3( 0,0,-20 ), Matrix33Identity() ) );

	Vector3 vInitCamPos( 1.5f, 1.0f, -2.0f );
	Matrix34 init_cam_pose( 
		vInitCamPos,
		CreateOrientFromFwdDir( Vec3GetNormalized(-vInitCamPos) )
		);

	GetCameraController()->SetPose( init_cam_pose );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 1.0f ) );
}


CMotionEditorApp::~CMotionEditorApp()
{
}


int CMotionEditorApp::Init()
{
	const std::string default_wd = lfs::get_cwd();

	string cmdline;
	if( 0 < ms_CommandLineArguments.size() )
		cmdline = ms_CommandLineArguments[0];

	shared_ptr<msynth::CMotionPrimitiveCompilerCreator> pCompilerCreator( new msynth::CBVHMotionPrimitiveCompilerCreator );
	msynth::RegisterMotionPrimitiveCompilerCreator( pCompilerCreator );

	msynth::CMotionDatabaseBuilder mdb;

	// Take a motion database filepath from one of the following (evaluated in the ascending order).
	// 1. command line string
	// 2. filepath written in a text file named "params.txt" in the format MotionDatabaseFile path/to/motion_database
	// 3. "Open File" dialog box
	string input_filepath;
	if( 0 < cmdline.length() )
		input_filepath = cmdline;
	else
		input_filepath = LoadParamFromFile<string>( "params.txt", "MotionDatabaseFile" );

	if( input_filepath.length() == 0 )
	{
		GetFilename( input_filepath );
//		if( input_filepath.length() == 0 )
//			input_filepath = "../resources/bvh/Mocappers/ordinary.xml";
	}

	if( input_filepath.length() == 0 )
		return 0;

	string mdb_filepath;
	bool built = true;
	if( path(input_filepath).extension() == ".xml" )
	{
	//	string output_mdb_filepath = "../resources/mdb.bin";
		built = mdb.Build( input_filepath );

		bool saved = true;//mdb.SaveMotionDatabaseToFile( output_mdb_filepath );
	
		mdb_filepath = mdb.GetOutputFilepath();
	}
	else
		mdb_filepath = input_filepath;

	// set the working directory to the default for the application
	// because it may have been changed by user operations on FileOpenDialog.
	lfs::set_wd( default_wd );

	if( built )//&& saved )
	{
		// successfully built and saved a motion database
		m_pMotionPrimitiveViewer
			= shared_ptr<CMotionPrimitiveViewer>( new CMotionPrimitiveViewer() );

		int ret = m_pMotionPrimitiveViewer->LoadMotionPrimitivesFromDatabase( mdb_filepath/*, tbl_name*/ );
		if( ret != 0 )
			return 0;
	}

	return 0;
}


void CMotionEditorApp::Update( float dt )
{
	if( m_pMotionPrimitiveViewer )
		m_pMotionPrimitiveViewer->Update( dt );
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
void CMotionEditorApp::Render()
{
	if( m_pMotionPrimitiveViewer )
		m_pMotionPrimitiveViewer->Render();
}

/*
void CMotionEditorApp::HandleInput( const SInputData& input )
{
	shared_ptr<CInputDeviceGroup> pDeviceGroup = InputDeviceHub().GetInputDeviceGroup(0);

	switch( input.iGICode )
	{
	case GIC_F6:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case GIC_F9:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			GraphicsResourceManager().Refresh();
//			m_pPostProcessEffectManager->InitHDRLightingFilter();
		}
		break;

//	case 'R':
//		m_pSynthTest->m_pSynthesizer->SetRootPose( Matrix34Identity() );
		break;

	case GIC_RIGHT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_CurrentFileIndex == -1 || m_vecMeshFilepath.empty() )
				break;

			m_CurrentFileIndex = (m_CurrentFileIndex + 1) % (int)m_vecMeshFilepath.size();

			LoadModel( m_vecMeshFilepath[m_CurrentFileIndex] );
		}
		break;

	case GIC_LEFT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			if( m_CurrentFileIndex == -1 || m_vecMeshFilepath.empty() )
				break;

			m_CurrentFileIndex = (m_CurrentFileIndex + (int)m_vecMeshFilepath.size() - 1) % (int)m_vecMeshFilepath.size();

			LoadModel( m_vecMeshFilepath[m_CurrentFileIndex] );
		}
		break;

	case 'M':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case 'N':
		break;

	case 'V':
		break;

	case GIC_MOUSE_BUTTON_R:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_CurrentMouseX = input.GetParamH16();
			m_CurrentMouseY = input.GetParamL16();
		}
		break;

	case GIC_MOUSE_AXIS_X:
	case GIC_MOUSE_AXIS_Y:
		if( pDeviceGroup->GetInputState( GIC_MOUSE_BUTTON_R ) == CInputState::PRESSED )
		{
			int new_x = input.GetParamH16();
			int new_y = input.GetParamL16();
			m_fHeading -= ( new_x - m_CurrentMouseX ) * 0.01f;;
			m_fPitch   -= ( new_y - m_CurrentMouseY ) * 0.01f;;
			m_CurrentMouseX = new_x;
			m_CurrentMouseY = new_y;
		}
		break;

	case GIC_PAGE_UP:
	case GIC_PAGE_DOWN:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	default:
		break;
	}
}*/
