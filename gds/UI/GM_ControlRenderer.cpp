
#include "GM_ControlRenderer.h"
#include "GM_Control.h"
#include "GM_Dialog.h"
#include "GM_DialogManager.h"
#include "GM_ControlRendererManager.h"

#include "3DCommon/GraphicsElementManager.h"
#include "3DCommon/GraphicsEffectManager.h"

using namespace std;


CGM_ControlRenderer::CGM_ControlRenderer()
:
m_pGroupElement(NULL)
{
//	m_TextLayoutH = TL_CENTER;
//	m_TextLayoutV = TL_CENTER;
}


CGM_ControlRenderer::~CGM_ControlRenderer()
{
//	int i;
//	for( i=0; i<NUM_EFFECTTRIGGEREVENTS; i++ )
//		SafeDeleteVector( m_vecpEffectDesc[i] );
}


void CGM_ControlRenderer::GetGraphicsElements( vector<CGraphicsElement *>& rvecpDestElement )
{
	size_t i;
	for( i=0; i<m_vecpElementToRegisterToParentDialog.size(); i++ )
		rvecpDestElement.push_back( m_vecpElementToRegisterToParentDialog[i] );

//	rvecpDestElement.push_back( m_pElementGroupToRegisterToParentDialog );
}

/*
void CGM_ControlRenderer::CreateGraphicsElementGroup()
{
	m_pGraphicsElementGroup = m_pGraphicsElementManager->CreateGroup( m_vecpGraphicsElement );
}
*/


void CGM_ControlRenderer::SetControlRendererManager( CGM_ControlRendererManager *pRendererMgr )
{
	m_pRendererManager = pRendererMgr;

	m_pGraphicsEffectManager = m_pRendererManager->GetGraphicsEffectManager();
	m_pGraphicsElementManager = m_pGraphicsEffectManager->GetGraphicsElementManager();
}


void CGM_ControlRenderer::GroupGraphicsElements()
{
	CGM_Dialog *pDialog = GetDialog();

	if( pDialog )
	{
		if( m_pGroupElement )
			return; // already created a group element
//			m_pGraphicsElementManager->RemoveElement( m_pGroupElement );

		// this is a control renderer of a dialog
		// - collect all the graphics elements managed by the control renderers
		//   of controls on this dialog
		vector<CGM_Control *>& rvecpControl = pDialog->GetControls();

		// but first, call GroupGraphicsElements() of controls on the current dialog
		// - should be already done. called from CGM_Dialog::UpdateGraphicsProperties() -> CGM_Control::UpdateGraphicsProperties()
		size_t i, num_controls = rvecpControl.size();

		// collect graphics elements of controls on the dialog
		vector<CGraphicsElement *> vecpGraphicsElementsToGroup;
		for( i=0; i<num_controls; i++ )
		{
			vector<CGraphicsElement *> vecpGraphicsElements;

			CGM_ControlRenderer *pRenderer = rvecpControl[i]->GetRenderer();
			if( pRenderer )
				pRenderer->GetGraphicsElements( vecpGraphicsElements );

			// set top left positions in local owner dialog coord
//			for( size_t j=0; j<vecpGraphicsElements.size(); j++ )
//				vecpGraphicsElements[j]->SetLocalTopLeftPos( rvecpControl[i]->GetLocalRect().GetTopLeftCorner() );

			// add to dest buffer
			vecpGraphicsElementsToGroup.insert( vecpGraphicsElementsToGroup.end(), vecpGraphicsElements.begin(), vecpGraphicsElements.end() );
		}

		// collect graphics elements of this dialog renderer
		this->GetGraphicsElements( vecpGraphicsElementsToGroup );

		const SPoint local_origin = pDialog->GetBoundingBox().GetTopLeftCorner();

		// create a group
		// - top-left corner of the dialog box is used as the local origin of the group
		m_pGroupElement = m_pGraphicsElementManager->CreateGroup( vecpGraphicsElementsToGroup, local_origin );

		OnGroupElementCreated();
	}
}

/*
void CGM_ControlRenderer::AddEffect( int effect_trigger_event, CElementEffectDesc *pEffectDescBase )
{
	m_vecpEffectDesc[effect_trigger_event].push_back( pEffectDescBase );
}
*/

void CGM_ControlRenderer::SetLocalLayerOffset( int local_layer_index, CGraphicsElement *pElement )
{
	m_vecLocalLayerInfo.push_back( CLocalLayerInfo( local_layer_index, pElement ) );
}


void CGM_ControlRenderer::UpdateGraphicsLayerSettings()
{
	size_t i, num_elements = m_vecLocalLayerInfo.size();
	for( i=0; i<num_elements; i++ )
	{
		int global_layer_index
			= m_pRendererManager->CalcUILayerIndex( this ) + m_vecLocalLayerInfo[i].layer_index;

		m_vecLocalLayerInfo[i].pElement->SetLayer( global_layer_index );
	}
}


void CGM_ControlRenderer::ChangeScale( float scale )
{
	size_t i, num_elements = m_vecLocalLayerInfo.size();
	for( i=0; i<num_elements; i++ )
	{
		m_vecLocalLayerInfo[i].pElement->ChangeScale( scale );
	}
}


void CGM_ControlRenderer::RegisterGraphicsElement( int local_layer_index, CGraphicsElement *pElement )
{
	RegisterGraphicsElementToParentDialog( pElement );

	SetLocalLayerOffset( local_layer_index, pElement );
}






/*
class CGM_TextElement
{
public:

	int m_TextLayoutH;
	int m_TextLayoutV;
	Point m_vTextOffset;
	std::string m_Text;
	CGE_TextElement *m_pElement;

public:

	CGM_TextElement( int font_id )
		:
	m_TextLayoutH(),
	m_TextLayoutV(),
	m_vTextOffset()

	void SetTextOffset();
};
*/
