#include "CorrelationGraph.h"

#include "Graphics/Direct3D9.h"
#include "Support/memory_helpers.h"


void CCorrelationGraph::SetData(vector<float> *pvecfData, U32 dwColor)
{
	SGraphData new_graph;

	//set color
	new_graph.m_dwGraphColor = dwColor;

	//copy the data from the source buffer 'pvecfData'
	int i,j;
	for(i=0; i<pvecfData->size(); i++)
	{
		float fNewDataValue = pvecfData->at(i);

		if( new_graph.fMaxValue < fNewDataValue )
			new_graph.fMaxValue = fNewDataValue;
		if( fNewDataValue < new_graph.fMinValue )
			new_graph.fMinValue	= fNewDataValue;

		if( m_fMaxValue < fNewDataValue )
			m_fMaxValue	= fNewDataValue;
		if( fNewDataValue < m_fMinValue )
			m_fMinValue	= fNewDataValue;

		new_graph.m_vecfData.push_back( fNewDataValue );
	}

	m_vecGraphData.push_back( new_graph );

	int iNumData = new_graph.m_vecfData.size();

	if( m_iMaxNumData < iNumData )
	{
		m_iMaxNumData = iNumData;
		SafeDeleteArray( m_paVertex );
		m_paVertex = new TLVERTEX [iNumData];
		ZeroMemory(m_paVertex, sizeof(TLVERTEX) * iNumData);
		for(j=0; j<iNumData; j++)
			m_paVertex[j].rhw = 1.0f;
	}
}


void CCorrelationGraph::SetPosition(float sx, float ex, float sy, float ey)
{
	m_vMin = D3DXVECTOR2(sx, sy);
	m_vMax = D3DXVECTOR2(ex, ey);

	if (m_vecGraphData.size() < 2)
		return;

	float fScaleX = m_vecGraphData[0].fMaxValue - m_vecGraphData[0].fMinValue;
	float fScaleY = m_vecGraphData[1].fMaxValue - m_vecGraphData[1].fMinValue;
	float fHeight = ey - sy;
	float fWidth  = ex - sx;

	int i, iMaxNumData = m_iMaxNumData;
	for(i=0; i<iMaxNumData; i++)
	{
		// local position
		m_paVertex[i].vPosition.x
			= ( ((float)i / (float)iMaxNumData) * 0.9f + 0.05f ) * fWidth;

		// global position
		m_paVertex[i].vPosition.x += sx;
		
	}

	// set positions of the segment rects
	for(i=0; i<m_vecGraphSegment.size(); i++)
	{
		m_vecGraphSegment[i].UpdatePosition( iMaxNumData,
			sx + fWidth  * 0.05f, ex - fWidth  * 0.05f,
			sy + fHeight * 0.05f, ey - fHeight * 0.05f );
	}

	// set position of the background rectangle
	m_BackGroundRect.SetPosition( m_vMin, m_vMax );

	// set the VERTICAL length of the indicator
	m_avIndicator[0].vPosition.y = sy + fHeight * 0.1f;
	m_avIndicator[1].vPosition.y = ey - fHeight * 0.1f;
}


void CCorrelationGraph::Draw()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// draw rectangle as background
	m_BackGroundRect.Draw();

	// draw graph
	pd3dDev->SetVertexShader( NULL );
	pd3dDev->SetFVF( D3DFVF_TLVERTEX );

	int i, j, iMaxNumData = m_iMaxNumData;

	// draw segments
	int iNumGraphSegments = m_vecGraphSegment.size();
	for(i=0; i<iNumGraphSegments; i++)
		m_vecGraphSegment[i].Draw();

	// set graph data
	int iNumData;
	if( m_vecGraphData[0].m_vecfData.size() < m_vecGraphData[1].m_vecfData.size() )
		iNumData = m_vecGraphData[0].m_vecfData.size();
	else
		iNumData = m_vecGraphData[1].m_vecfData.size();

	float fScaleX = m_vecGraphData[1].fMaxValue - m_vecGraphData[1].fMinValue;
	float fScaleY = m_vecGraphData[1].fMaxValue - m_vecGraphData[1].fMinValue;
	float fGraphWidth  = m_vMax.x - m_vMin.x;
	float fGraphHeight = m_vMax.y - m_vMin.y;
	float fOffsetX = m_vecGraphData[0].fMinValue;
	float fOffsetY = m_vecGraphData[1].fMinValue;

	U32 dwColor = m_vecGraphData[i].m_dwGraphColor;
	for(j=0; j<iNumData; j++)
	{
		// local position
		m_paVertex[j].vPosition.x
			= ( ((m_vecGraphData[0].m_vecfData[j] - fOffsetX) / fScaleX) * 0.9f + 0.05f ) * fGraphWidth;
		m_paVertex[j].vPosition.y
			= ( ((m_vecGraphData[1].m_vecfData[j] - fOffsetY) / fScaleY) * 0.9f + 0.05f ) * fGraphHeight;

		// global position
		m_paVertex[j].vPosition.x += m_vMin.x;
		m_paVertex[j].vPosition.y = m_vMax.y - m_paVertex[j].vPosition.y;

		// set color
		m_paVertex[j].color = dwColor;
	}

	// draw graph data
	pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, iNumData - 1, m_paVertex, sizeof(TLVERTEX) );

	// draw indicator
	pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, m_avIndicator, sizeof(TLVERTEX) );

	// draw vertical and horizontal axes
	TLVERTEX avAxis[4];
	for(i=0; i<4; i++)
		avAxis[i].color = 0xFF808080;
	avAxis[0].vPosition.x = m_vMin.x;
	avAxis[1].vPosition.x = m_vMax.x;
	avAxis[0].vPosition.y = avAxis[1].vPosition.y = (m_vMax.y + m_vMin.y) / 2.0f;
	avAxis[2].vPosition.y = m_vMax.y;
	avAxis[3].vPosition.y = m_vMin.y;
	avAxis[2].vPosition.x = avAxis[3].vPosition.x = (m_vMax.x + m_vMin.x) / 2.0f;
	pd3dDev->DrawPrimitiveUP( D3DPT_LINELIST, 2, avAxis, sizeof(TLVERTEX) );


}