#include "GraphicsElementEffectDemo.hpp"
#include "amorphous/Graphics/Font/TextureFont.hpp"
#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Graphics/GraphicsEffectManager.hpp"
#include "amorphous/Graphics/PyModule_GraphicsElement.hpp"

#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/FileOpenDialog_Win32.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/lfs.hpp"
using namespace lfs;

#include <fstream>
#include <iostream>

using namespace std;


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

	// create some graphics elements for test
//	g_pGraphicsTest = new CGraphicsElementTest;
//	g_pGraphicsTest = new CAnimGraphicsTest;	// uses both C++ & python script
//	g_pGraphicsTest = new CPyScriptGraphicsTest;


void AddModule( const string& module_name, PyMethodDef method[] )
{
	PyObject *pModule0 = PyImport_AddModule( module_name.c_str() );
	PyObject *pModule  = Py_InitModule( module_name.c_str(), method );
}

bool RunPyScript( const std::string& filename )
{
	using namespace std;

	ifstream pythonFile;

	pythonFile.open( filename.c_str() );

	if ( !pythonFile.is_open() ) 
	{
		cout << "Cannot open Python script file, \"" << filename << "\"!" << endl;
		return false;
	}

	// Get the length of the file
	pythonFile.seekg( 0, ios::end );
	int nLength = pythonFile.tellg();
	pythonFile.seekg( 0, ios::beg );

	// Allocate  a char buffer for the read.
	char *buffer = new char[nLength];
	memset( buffer, 0, nLength );

	// read data as a block:
	pythonFile.read( buffer, nLength );

	PyObject *pMainModule     = PyImport_AddModule( "__main__" );
	PyObject *pMainDictionary = PyModule_GetDict( pMainModule );

	AddPythonModule_Graphics();
	AddPythonModule_Animation();

	PyRun_String( buffer, Py_file_input, pMainDictionary, pMainDictionary );

	delete [] buffer;

	return true;

}


void GraphicsElementEffectDemo::CreateGraphicsElements()
{
	shared_ptr<GraphicsElementManager> pGraphicsElementManager
		 = m_pAnimGraphicsManager->GetGraphicsElementManager();

	// load resources
	int font_id = pGraphicsElementManager->LoadTextureFont( "resources/TexFontTest2.dds", 8, 16 );

	m_pElements[0] = pGraphicsElementManager->CreateFillRect( RectLTWH( 10, 10, 100, 100 ), SFloatRGBAColor(1,1,1,1) );
	m_pElements[1] = pGraphicsElementManager->CreateFrameRect( RectLTWH( 160, 10, 100, 100 ), SFloatRGBAColor(1,1,1,1), 2.0f );
	m_pElements[2] = pGraphicsElementManager->CreateText( font_id, "text created from a cpp source file", 10, 120, SFloatRGBAColor(1,1,1,1) );

//	m_pElements[3] = pGraphicsElementManager->CreateFrameRect( RectLTWH( 310, 10, 100, 100 ), 0x00000000, 2.0f );
//	m_pElements[4] = pGraphicsElementManager->CreateFrameRect( RectLTWH( 460, 10, 100, 100 ), 0x00000000, 2.0f );
//	m_pElements[5] = pGraphicsElementManager->CreateFrameRect( RectLTWH( 610, 10, 100, 100 ), 0x00000000, 2.0f );

	m_pElements[3] = pGraphicsElementManager->CreateFrameRect( SRect( 310, 10, 410, 110 ), SFloatRGBAColor(0,0,0,0), 2.0f );
	m_pElements[4] = pGraphicsElementManager->CreateFrameRect( SRect( 460, 10, 560, 110 ), SFloatRGBAColor(0,0,0,0), 2.0f );
	m_pElements[5] = pGraphicsElementManager->CreateFrameRect( SRect( 610, 10, 710, 110 ), SFloatRGBAColor(0,0,0,0), 2.0f );
}


void GraphicsElementEffectDemo::CreateGraphicsEffects()
{
	// set the origin of the animation time
	m_pAnimGraphicsManager->SetTimeOffset();

	m_pAnimGraphicsManager->SetColor( m_pElements[0], 3.0f, 0, 0xFFFF0000 );

	m_pAnimGraphicsManager->ChangeColorTo( m_pElements[3], 1.00f, 1.75f, 0, 0xFFFF0000, 0 );
	m_pAnimGraphicsManager->ChangeColorTo( m_pElements[4], 2.00f, 2.75f, 0, 0xFF00FF00, 0 );
	m_pAnimGraphicsManager->ChangeColorTo( m_pElements[5], 3.00f, 3.75f, 0, 0xFF0000FF, 0 );

	m_pAnimGraphicsManager->TranslateTo( m_pElements[3], 1.00f, 1.75f, Vector2( 310, 110 ), 0, 0 );
	m_pAnimGraphicsManager->TranslateTo( m_pElements[4], 2.00f, 2.75f, Vector2( 460, 110 ), 0, 0 );
	m_pAnimGraphicsManager->TranslateTo( m_pElements[5], 3.00f, 3.75f, Vector2( 610, 110 ), 0, 0 );
}


void GraphicsElementEffectDemo::Render()
{
	shared_ptr<GraphicsElementManager> pGraphicsElementManager
		 = m_pAnimGraphicsManager->GetGraphicsElementManager();

	if( pGraphicsElementManager )
		pGraphicsElementManager->Render();
}


void GraphicsElementEffectDemo::Update( float dt )
{
	if( m_pAnimGraphicsManager )
		m_pAnimGraphicsManager->UpdateEffects( dt );
}


int GraphicsElementEffectDemo::Init()
{
	m_pAnimGraphicsManager.reset( new GraphicsElementAnimationManager() );

	shared_ptr<GraphicsElementManager> pGraphicsElementManager
		 = m_pAnimGraphicsManager->GetGraphicsElementManager();

	// retrieve and save the pointer to GraphicsElementManager
	// the pointer is a borrowed reference and must not be deleted
	// it is deleted by CAnimatedGraphicsManager
//	m_pGraphicsElementManager = m_pAnimGraphicsManager->GetGraphicsElementManager();

	CreateGraphicsElements();

//	pGraphicsElementManager->SetScale( g_WindowSize.x / 800.0f );

	CreateGraphicsEffects();

///	SetAnimatedGraphicsManager( m_pAnimGraphicsManager );

	return 0;
}


/*
void GraphicsElementEffectDemo::Init()
{
	string script_filename;
	if( 0 < strlen(lpCmdLine) )
	{
//		g_Log.Print( "%s", lpCmdLine );
		g_Log.Print( "current working directory: %s", get_cwd().c_str() );

		wchar_t buf[1024];
		GetCurrentDirectory( 1023, buf );
		MessageBox( NULL, buf, L"msg", MB_OK );
//		script_filename = "script/primitives.py";
//		script_filename = "script/texts.py";
		script_filename = "script/imageslides.py";
	}
	else
	{
		string arg_str;
		GetFilename( arg_str );
		script_filename = arg_str;
		g_Log.Print( "selected from fopen dlg: %s", script_filename.c_str() );
	}


	// run a python script
	Py_Initialize();
	RunPyScript( script_filename );
	Py_Finalize();
}
*/
