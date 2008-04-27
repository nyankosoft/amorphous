
#include "GM_Static.h"

#include "GM_ControlDescBase.h"
#include "GM_Dialog.h"
#include "GM_ControlRenderer.h"


//========================================================================================
// CGM_Static
//========================================================================================

CGM_Static::CGM_Static( CGM_Dialog *pDialog, CGM_StaticDesc* pDesc )
: CGM_Control( pDialog, pDesc )
{
	m_strText = pDesc->strText;
}

/*
void CGM_Static::Render()
{
//	m_pRenderManager->RenderStatic( *this );
}
*/

void CGM_Static::SetText( const std::string& strText )
{
	m_strText = strText;

	if( m_pRenderer.get() )
		m_pRenderer->OnTextChanged();
}
