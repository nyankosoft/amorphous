//-----------------------------------------------------------------------------
// File: .cpp
//-----------------------------------------------------------------------------

#include "Support/FileOpenDialog_Win32.hpp"
#include "Support/CameraController_Win32.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Timer.hpp"

#include "Graphics/MeshModel/3DMeshModelBuilder.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/D3DXMeshObject.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "LightWave/3DMeshModelExportManager_LW.hpp"

#include <vld.h>



using namespace MeshModel;


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

CMeshObjectHandle g_MeshObject;

CPlatformDependentCameraController g_CameraController;

CShaderManager g_ShaderManager;



//-----------------------------------------------------------------------------
// Global functions
//-----------------------------------------------------------------------------

void UpdateCameraMatrix()
{
	D3DXMATRIX matWorld, matView, matProj;

	//
	// calc matrices
	//

	// world - always set identity matrix
	D3DXMatrixIdentity( &matWorld );

	// view - take from camera controller
	g_CameraController.GetCameraMatrix( matView );

	// projection - use the fixed values for now
	float fov    = D3DX_PI / 4;
	float aspect = 16.0f / 9.0f;
	float z_near = 0.1f;
	float z_far  = 500.0f;
	D3DXMatrixPerspectiveFovLH( &matProj, fov, aspect, z_near, z_far );

	//
	// set matrices to fixed function shader
	//
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_VIEW,       &matWorld );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_VIEW,       &matView );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );

	//
	// set matrices to programmable shader
	//
	CShaderManager &rShaderMgr = g_ShaderManager;
	LPD3DXEFFECT pEffect = rShaderMgr.GetEffect();
	if( pEffect )
	{
		rShaderMgr.SetWorldViewProjectionTransform( matWorld, matView, matProj );

		pEffect->CommitChanges();
	}

/*
	D3DXMatrixTranspose( &matWorldView, &matWorldView );
*/
}


bool InitHLSL()
{
	CShader::Get()->SetShaderManager( &g_ShaderManager );

//	return g_ShaderManager.LoadShaderFromFile( "MeshTest.fx" );
	return g_ShaderManager.LoadShaderFromFile( "../../_Data/Shader/basic.fx" );

}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;

    // Clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

    // Begin the scene
    pd3dDevice->BeginScene();

	CShaderManager *pShaderManager = CShader::Get()->GetCurrentShaderManager();

	LPD3DXEFFECT pEffect = pShaderManager->GetEffect();

	if( pEffect )
	{
		hr = pEffect->SetVector( "g_vEye", (D3DXVECTOR4 *)&g_CameraController.GetPosition() );

		if( FAILED(hr) )
			int iFailed = 1;

		hr = pEffect->SetTechnique( "QuickTest" );

		if( FAILED(hr) )
			int iFailed = 1;

		CD3DXMeshObjectBase *pMesh = g_MeshObject.GetMesh().get();

		// Rendering

		if( pMesh )
		{
			pMesh->Render( g_ShaderManager );
		}
	}
/*	else
	{
		if( g_pMeshObject )
			g_pMeshObject->Render();
//			g_pMeshObject->Render( CD3DXMeshModel::RENDER_USE_FIXED_FUNCTION_SHADER );
	}
*/
	// End the scene
	pd3dDevice->EndScene();

	// Present the backbuffer contents to the display
	pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static int s_iPrevMousePosX = 0, s_iPrevMousePosY = 0;
	int iMousePosX, iMousePosY, iMouseMoveX, iMouseMoveY;

    switch( msg )
    {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			s_iPrevMousePosX = LOWORD(lParam);
			s_iPrevMousePosY = HIWORD(lParam);
			break;

		case WM_MOUSEMOVE:
			if( !(wParam & MK_RBUTTON) && !(wParam & MK_LBUTTON) )
				break;
			
			iMousePosX = LOWORD(lParam);
			iMousePosY = HIWORD(lParam);
			iMouseMoveX = iMousePosX - s_iPrevMousePosX;
			iMouseMoveY = iMousePosY - s_iPrevMousePosY;

			if( wParam & MK_RBUTTON )
			{	// camera roration
				g_CameraController.AddYaw( iMouseMoveX / 450.0f );
				g_CameraController.AddPitch( - iMouseMoveY / 450.0f );
			}
			else
			{	// camera translation
				g_CameraController.Move( iMouseMoveX / 200.0f, iMouseMoveY / 200.0f, 0.0f );
			}

			s_iPrevMousePosX = iMousePosX;
			s_iPrevMousePosY = iMousePosY;
			break;

		case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


void ReleaseGraphicsResources()
{
	g_ShaderManager.Release();
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: entry point
//-----------------------------------------------------------------------------

INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR	 lpCmdLine, int nCmdShow )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "MeshCompiler", NULL };

    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "MeshCompiler", "Mesh Compiler",
                              WS_OVERLAPPEDWINDOW, 200, 200, 640, 480,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // Initialize Direct3D
	if( !DIRECT3D9.InitD3D(hWnd) )
		return 0;


	bool bShaderLoaded;
	// load the shader before the directory is changed
	if( !InitHLSL() )
	{
		bShaderLoaded = false;
//		return 0;
	}
	else
	{
		bShaderLoaded = true;
	}

    // Show the window
    ShowWindow( hWnd, SW_SHOWDEFAULT );
    UpdateWindow( hWnd );

	char acFilename[512];
	if( 0 < strlen(lpCmdLine) )
	{
		// a filename is specified as an argument
		// trim the double quotation characters at the beginning and the end of the string
		strcpy( acFilename, lpCmdLine+1 );
		acFilename[ strlen(acFilename)-1 ] = '\0';
	}
	else
	{
		if( !GetFilename(acFilename, NULL) )
			return 0;
	}

	LOG_PRINT( "Input model file: " + string(acFilename) );

	// load a LightWave model and export mesh file(s)
	C3DMeshModelExportManager_LW exporter;
	bool mesh_compiled = exporter.BuildMeshModels( acFilename );


	if( !bShaderLoaded || !mesh_compiled )
	{
		ReleaseGraphicsResources();
		return 0;
	}

	int i = MessageBox( NULL, "Review model file?", "Mesh Model compiled.", MB_OK );


	if( i == IDOK )
	{
		g_MeshObject.Load( acFilename );
	}
	else
		return 0;


	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	// set projection matrix
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, 3.141592f / 3.0f, 4.0f / 3.0f, 0.1f, 1200.0f);
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );


	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );

	while( msg.message != WM_QUIT )
	{
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
		{
			/**/
		}

		GlobalTimer().UpdateFrameTime();

		UpdateCameraMatrix();

		g_CameraController.UpdateCameraPose( GlobalTimer().GetFrameTime() );

        Render();
    }


//	SafeDelete( g_pMeshObject );

    // Clean up everything and exit the app
    UnregisterClass( "MeshCompiler", wc.hInstance );
    return 0;
}
