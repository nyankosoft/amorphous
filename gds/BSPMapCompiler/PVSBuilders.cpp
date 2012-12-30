#include "MapCompiler.h"


namespace amorphous
{

//===================================================================================
// CPortalVisibilityBuilder::Methods()                    - CPortalVisibilityBuilder
//===================================================================================

CPortalVisibilityBuilder::CPortalVisibilityBuilder(vector<CCell>* paCell,
												   vector<CPortalFace> *paPortal,
												   CPortalVisibilityBuilder* pPtrlVisBuilder)
{
	m_paCell = paCell;
	m_paPortal = paPortal;
	m_psPortalVisibility = new short [ m_paPortal->size() ];
	m_pPrevVisBuilder = pPtrlVisBuilder;
}

CPortalVisibilityBuilder::~CPortalVisibilityBuilder()
{

	if(m_psPortalVisibility)
	{
		delete m_psPortalVisibility;
		m_psPortalVisibility = NULL;
	}
}

//===================================================================================
// CPositionalVisibilityBuilder::Methods()              - CPositionalVisibilityBuilder
//===================================================================================

short* CPositionalVisibilityBuilder::SetVisibility(int iCellIndex, int iPortalIndex2)
{
	CCell& rCell = m_paCell->at(iCellIndex);
	CPortalFace& rPortal = m_paPortal->at( rCell.GetPortalIndex(iPortalIndex2) );
	memset( m_psPortalVisibility, PVS_NOTVIS, sizeof(short) * m_paPortal->size() );
	SetPositionPortalVisibility(rCell, rPortal);

	if((int)rPortal.GetFrontCellIndex() == iCellIndex)
		m_psPortalVisibility[ rCell.GetPortalIndex(iPortalIndex2) ] = PVS_FRONT;
	else
		m_psPortalVisibility[ rCell.GetPortalIndex(iPortalIndex2) ] = PVS_BACK;

	return m_psPortalVisibility;
}

void CPositionalVisibilityBuilder::SetPositionPortalVisibility(CCell& rCell, CPortalFace& rPortal)
{
	SPlane srcplane = rPortal.GetPlane();
	if(m_paCell->at( rPortal.GetFrontCellIndex() )== rCell)
		srcplane.Flip();
	//Now, srcplane is facing outside the current cell

	int i, c;
	for(i=0; i<m_paPortal->size(); i++)
	{
		CPortalFace& rDestPortal = m_paPortal->at(i);
		SPlane& rDestPlane = rDestPortal.GetPlane();
		c = ClassifyFace( srcplane, rDestPortal );

		if( c == FCE_BACK || c == FCE_ONPLANE )
		{
			m_psPortalVisibility[i] = PVS_NOTVIS;
			continue;
		}

		c = ClassifyFace( rDestPlane, rPortal );
		switch(c)
		{
		case FCE_FRONT:  //We can see the  front side of rDestPortal. (rDestPlane is facing towards srcplane.)
			m_psPortalVisibility[i] = PVS_FRONT;
			break;
		case FCE_BACK:  //We can see the back side of rDestPortal. (rDestPlane is facing away from srcplane.)
			m_psPortalVisibility[i] = PVS_BACK;
			break;
		case FCE_SPLIT:  //We can see the both sides of rDestPortal
			m_psPortalVisibility[i] = PVS_BOTH;
			break;
		case FCE_ONPLANE:  //impossible situation
		default:
			m_psPortalVisibility[i] = PVS_NOTVIS;
		}

	}
}


//===================================================================================
// CTracingVisibilityBuilder::Methods()              - CTracingVisibilityBuilder
//===================================================================================

short* CTracingVisibilityBuilder::SetVisibility(int iCellIndex, int iPortalIndex2)
{
	CCell& rCell = m_paCell->at(iCellIndex);
	CPortalFace& rPortal = m_paPortal->at( rCell.GetPortalIndex(iPortalIndex2) );

	if( m_pPrevVisBuilder )
	{//A preceding visibility building is available 
		m_psPrevVisibility = m_pPrevVisBuilder->SetVisibility(iCellIndex, iPortalIndex2);
	}
	memset( m_psPortalVisibility, PVS_NOTVIS, sizeof(short) * m_paPortal->size() );

	SetTraversePortalVis_r(rCell);

/*	if(rPortal.GetFrontCellIndex() == iCellIndex)
		m_psPortalVisibility[ rCell.GetPortalIndex(iPortalIndex2) ] = PVS_FRONT;
	else
		m_psPortalVisibility[ rCell.GetPortalIndex(iPortalIndex2) ] = PVS_BACK;
*/
	return m_psPortalVisibility;
}

void CTracingVisibilityBuilder::SetTraversePortalVis_r(CCell& rCell)
{
	int i;
	for(i=0; i<rCell.GetNumPortals(); i++)
	{
		short sPortalIndex = rCell.GetPortalIndex( i );
		CPortalFace& rSrcPortal = m_paPortal->at( sPortalIndex );
		if( m_psPrevVisibility[ sPortalIndex ] == PVS_NOTVIS )
			continue;
		if( m_paCell->at( rSrcPortal.GetFrontCellIndex() ) == rCell ) 
		{//'rCell' is on the front side of the 'rSrcPortal'
			if( (m_psPrevVisibility[ sPortalIndex ] & PVS_FRONT)
				&& !(m_psPortalVisibility[ sPortalIndex ] & PVS_FRONT) )
				{
					m_psPortalVisibility[ sPortalIndex ] |= PVS_FRONT;
					CCell& rBackCell = m_paCell->at( rSrcPortal.GetBackCellIndex() );
					SetTraversePortalVis_r( rBackCell );	//表が見えてるから次は裏側セル
				}
		}
		else
		{//'rCell' is on the back side of the 'rSrcPortal'
			if( (m_psPrevVisibility[ sPortalIndex ] & PVS_BACK)
				&& !(m_psPortalVisibility[ sPortalIndex ] & PVS_BACK) )
				{
					m_psPortalVisibility[ sPortalIndex ] |= PVS_BACK;
					CCell& rFrontCell = m_paCell->at( rSrcPortal.GetFrontCellIndex() );
					SetTraversePortalVis_r( rFrontCell );	//裏が見えてるから次は表側セル
				}
		}
	}
}


//===================================================================================
// CClippingVisibilityBuilder::Methods()              - CClippingVisibilityBuilder
//===================================================================================

short* CClippingVisibilityBuilder::SetVisibility(int iCellIndex, int iPortalIndex2)
{
	m_psPrevVisibility = m_pPrevVisBuilder->SetVisibility(iCellIndex, iPortalIndex2);

	CCell& rCell = m_paCell->at(iCellIndex);
	short sPortalIndex = rCell.GetPortalIndex(iPortalIndex2);
	CPortalFace& rPortal = m_paPortal->at( sPortalIndex );

	memset( m_psPortalVisibility, PVS_NOTVIS, sizeof(short) * m_paPortal->size() );
	m_psPortalVisibility[ sPortalIndex ] = m_psPrevVisibility[ sPortalIndex ];

	short sCellIndex;
	CPortalFace firstportal = rPortal;
	if(rPortal.GetFrontCellIndex() == (short)iCellIndex)
		sCellIndex = rPortal.GetBackCellIndex();
	else
		sCellIndex = rPortal.GetFrontCellIndex();

	SetClippingPortalVis_r(sCellIndex, NULL, &firstportal);
	return m_psPortalVisibility;
}

void CClippingVisibilityBuilder::SetClippingPortalVis_r( short sCellIndex,
														CPortalFace* pSrcPortal,
														CPortalFace* pDestPortal )
{
	int i, checkpvs;
	short sNextCellIndex, sPortalIndex;
	CCell& rCell = m_paCell->at( sCellIndex );
	CPortalFace stacksrc;
	CPortalFace stackdest;
	if( pSrcPortal == NULL )	//子ポータルなので、このセルに含まれているポータルは全て見える。
	{
		for(i=0; i<rCell.GetNumPortals(); i++)
		{
			sPortalIndex = rCell.GetPortalIndex(i);
			CPortalFace& rThisPortal = m_paPortal->at( sPortalIndex );
			if( m_psPrevVisibility[ sPortalIndex ] == PVS_NOTVIS )
				continue;
			if(rThisPortal.GetFrontCellIndex() == sCellIndex)
			{
				checkpvs = PVS_FRONT;
				sNextCellIndex = rThisPortal.GetBackCellIndex();
			}
			else
			{
				checkpvs = PVS_BACK;
				sNextCellIndex = rThisPortal.GetFrontCellIndex();
			}
			//checkpvs shows which side of the rThisPortal is visible

			if( m_psPrevVisibility[ sPortalIndex ] & checkpvs 
				&& !(m_psPortalVisibility[ sPortalIndex ] & checkpvs) )
			{
				m_psPortalVisibility[ sPortalIndex ] |= checkpvs;
				stacksrc = *pDestPortal;
				stackdest = rThisPortal;
				SetClippingPortalVis_r( sNextCellIndex, &stacksrc, &stackdest );
			}
		}
	}
	else	//孫ポータルなので、親と子によって、クリップして判定される。
	{
		for(i=0; i<rCell.GetNumPortals(); i++)
		{
			sPortalIndex = rCell.GetPortalIndex(i);
			if( m_psPrevVisibility[ sPortalIndex ] == PVS_NOTVIS )
				continue;
			CPortalFace clippedportal = m_paPortal->at( sPortalIndex );
			if( clippedportal.GetFrontCellIndex() == sCellIndex )
			{
				checkpvs = PVS_FRONT;
				sNextCellIndex = clippedportal.GetBackCellIndex();
			}
			else
			{
				checkpvs = PVS_BACK;
				sNextCellIndex = clippedportal.GetFrontCellIndex();
			}

			if( m_psPrevVisibility[ sPortalIndex ] & checkpvs
				&& !(m_psPortalVisibility[ sPortalIndex ] & checkpvs) )
			{
				//対象のポータルをクリップしてみる。
				if( !VisibilityClipping(*pSrcPortal, *pDestPortal, clippedportal) )
					continue;
				m_psPortalVisibility[sPortalIndex] |= checkpvs;

				//今度はクリップしたポータルから、ソースを見る。
				stacksrc = *pSrcPortal;
				if( !VisibilityClipping(clippedportal, *pDestPortal, *pSrcPortal) )
					continue;

				stackdest = clippedportal;
				SetClippingPortalVis_r( sNextCellIndex, &stacksrc, &stackdest );
			}
		}
	}
}

//Calculate the visible area of rTgtPortal when it is seen through rSrcPortal and rDestPortal.
//rTgtPortal is actually clipped to a face of visible area.
bool CClippingVisibilityBuilder::VisibilityClipping(CPortalFace& rSrcPortal,
						CPortalFace& rDestPortal,
						CPortalFace& rTgtPortal)
{
	if( rTgtPortal.ClipVisibility(rSrcPortal, rDestPortal, CLIP_NORMAL)
		&& rTgtPortal.ClipVisibility(rDestPortal, rSrcPortal, CLIP_INVERT) )
		return true;
	else
		return false;
}


} // amorphous
