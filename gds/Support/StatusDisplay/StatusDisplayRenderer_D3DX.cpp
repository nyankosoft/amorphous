
#include "StatusDisplayRenderer_D3DX.h"
#include "StatusDisplay.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/Font.h"
#include "3DCommon/2DRect.h"

#include "Support/memory_helpers.h"


CStatusDisplayRenderer_D3DX::CStatusDisplayRenderer_D3DX()
: m_pFont(NULL)
{
}

CStatusDisplayRenderer_D3DX::~CStatusDisplayRenderer_D3DX()
{
	SafeDelete( m_pFont );
}


void CStatusDisplayRenderer_D3DX::InitFont( const char *pcFontName, int width, int height )
{
	SafeDelete( m_pFont );
	m_pFont = new CFont;

	m_pFont->InitFont( pcFontName, width, height );
}


void CStatusDisplayRenderer_D3DX::Render( CStatusDisplay* pStatusDisplay )
{
	if( !m_pFont )
		return;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

    // clear the backbuffer
    pd3dDev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(32,32,32), 1.0f, 0 );

    // begin the scene
    pd3dDev->BeginScene();

	pd3dDev->SetVertexShader( NULL );


	SRect& rRect = pStatusDisplay->GetStatusRect();

	C2DRect rect_frame, rect_status;

	rect_frame.SetColor( 0xFF808080 );
	rect_status.SetColor( 0xFFA0A0A0 );

	vector<CStatusDisplay::STask>& rvecTask = pStatusDisplay->GetTask();

	SPoint pt = pStatusDisplay->GetPosition();
	D3DXVECTOR2 vPos = D3DXVECTOR2( pt.x, pt.y );

	int i, iNumTasks = rvecTask.size();
	for( i=0; i<iNumTasks; i++ )
	{
		m_pFont->DrawText( rvecTask[i].strName.c_str(), vPos, 0xFFFFFFFF );

		vPos.y += m_pFont->GetFontHeight();

		rect_frame.SetPosition( D3DXVECTOR2(rRect.left,rRect.top)     + vPos,
			                    D3DXVECTOR2(rRect.right,rRect.bottom) + vPos );

		rect_frame.Draw();
		
		if( rvecTask[i].fStaus == 1.0f )
			rect_status.SetColor( 0xFFA0FFA0 );	// completed
		else
			rect_status.SetColor( 0xFFFFA0A0 );

		rect_status.SetPosition( D3DXVECTOR2(rRect.left,rRect.top)     + vPos,
			                     D3DXVECTOR2(rRect.left + rRect.GetWidth() * rvecTask[i].fStaus,rRect.bottom) + vPos );

		rect_status.Draw();

		vPos.y += rRect.GetHeight() * 1.2f;
	}


    // End the scene
    pd3dDev->EndScene();

    // Present the backbuffer contents to the display
    pd3dDev->Present( NULL, NULL, NULL, NULL );
}