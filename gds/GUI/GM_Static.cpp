
#include "GM_Static.hpp"

#include "GM_ControlDescBase.hpp"
#include "GM_Dialog.hpp"
#include "GM_ControlRenderer.hpp"


namespace amorphous
{


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
		m_pRenderer->OnTextChanged( *this );
}


} // namespace amorphous
