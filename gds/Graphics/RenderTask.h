#ifndef  __RenderTask_H__
#define  __RenderTask_H__


#include "Direct3D9.h"
#include "Support/Log/DefaultLog.h"


class CRenderTask
{
protected:

	int m_TypeFlags;

protected:

	enum eTypeFlags
	{
		DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE = ( 1 << 0 ),
		RENDER_TO_BACKBUFFER                = ( 1 << 1 ), ///< Present() is called at the end of Render(). Mutually exclusive with 'RENDER_TO_TEXTURE'
		RENDER_TO_TEXTURE                   = ( 1 << 2 ), ///< Mutually exclusive with 'RENDER_TO_BACKBUFFER'
	};

public:

	CRenderTask() : m_TypeFlags(0) {}

	virtual ~CRenderTask() {}

	inline void RenderBase();

	virtual void Render() = 0;

};


inline void CRenderTask::RenderBase()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	HRESULT hr;

	if( m_TypeFlags & DO_NOT_CALL_BEGINSCENE_AND_ENDSCENE )
	{
		Render();
	}
	else
	{
		// call BeginScene() and EndScene() pair before and after the rendering

//		hr = pd3dDev->BeginScene();

//		if( FAILED(hr) )
//			LOG_PRINT_ERROR( "IDirect3DDevice9::BeginScene() failed." );

		Render();

//		pd3dDev->EndScene();
	}

	if( m_TypeFlags & RENDER_TO_BACKBUFFER )
	{
		// present the backbuffer contents to the display
//		pd3dDev->Present( NULL, NULL, NULL, NULL );
	}
}


#endif		/*  __RenderTask_H__  */
