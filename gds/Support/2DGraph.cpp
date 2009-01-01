//===============================================================================
// 2DGraph.cpp
//   - draw graphs using Direct3D
//===============================================================================

#include "2DGraph.h"
#include "Graphics/Direct3D9.h"
#include "Graphics/Font.h"

#include "Support/memory_helpers.h"

#include <stdlib.h>

using namespace std;


//================================================================================
// CGraphSegment
//================================================================================

C2DGraph::C2DGraph()
{
	m_paVertex = NULL;
	m_fMaxValue = -99999;
	m_fMinValue =  99999;
	m_iMaxNumData = 0;
	m_vMin = Vector2(0,0);
	m_vMax = Vector2(0,0);

	m_BackGroundRect.SetColor( 0xAA000000 );
	m_BackGroundRect.SetZDeppth( 0.1f );

	m_iIndicatorPosition = 0;
	ZeroMemory(m_avIndicator, sizeof(TLVERTEX) * 2);
	m_avIndicator[0].color = 0xBB202020;	// light gray
	m_avIndicator[1].color = 0xBB202020;
	m_avIndicator[0].rhw = 1.0f;
	m_avIndicator[1].rhw = 1.0f;

	// create the font object that is shared by all the graph segments in this graph
	m_pGraphSegmentIDFont = new CFont;
	m_pGraphSegmentIDFont->InitFont( "ÇlÇr ÉSÉVÉbÉN", 8, 12 );
}

C2DGraph::~C2DGraph()
{
	SafeDeleteArray( m_paVertex );
	m_pGraphSegmentIDFont->Release();
	delete m_pGraphSegmentIDFont;
}

C2DGraph::C2DGraph(const C2DGraph& r2DGraph)
{
	m_vecGraphData.assign( r2DGraph.m_vecGraphData.begin(), r2DGraph.m_vecGraphData.end() );

	m_fMaxValue = r2DGraph.m_fMaxValue;
	m_fMinValue = r2DGraph.m_fMinValue;

	m_iMaxNumData = r2DGraph.m_iMaxNumData;

	m_vMin = r2DGraph.m_vMin;
	m_vMax = r2DGraph.m_vMax;

	// SafeDeleteArray( m_paVertex );	//Do not "SafeDelete" this pointer. (Remember 'm_paVertex' hasn't been initialized yet)
	m_paVertex = new TLVERTEX [r2DGraph.m_iMaxNumData];
	for(int i=0; i<r2DGraph.m_iMaxNumData; i++)
		m_paVertex[i] = r2DGraph.m_paVertex[i];

	m_BackGroundRect = r2DGraph.m_BackGroundRect;

	m_avIndicator[0] = r2DGraph.m_avIndicator[0];
	m_avIndicator[1] = r2DGraph.m_avIndicator[1];
	m_iIndicatorPosition = r2DGraph.m_iIndicatorPosition;

	m_vecGraphSegment.assign( r2DGraph.m_vecGraphSegment.begin(), r2DGraph.m_vecGraphSegment.end() );

	// SafeDelete( m_pGraphSegmentIDFont );	// m_pGraphSegmentIDFont has an unknown value
	m_pGraphSegmentIDFont = new CFont;
	m_pGraphSegmentIDFont->InitFont( "ÇlÇr ÉSÉVÉbÉN", 8, 12 );
}

C2DGraph C2DGraph::operator=(C2DGraph r2DGraph)
{
	m_vecGraphData.assign( r2DGraph.m_vecGraphData.begin(), r2DGraph.m_vecGraphData.end() );

	m_fMaxValue = r2DGraph.m_fMaxValue;
	m_fMinValue = r2DGraph.m_fMinValue;

	m_iMaxNumData = r2DGraph.m_iMaxNumData;

	m_vMin = r2DGraph.m_vMin;
	m_vMax = r2DGraph.m_vMax;

	SafeDeleteArray( m_paVertex );
	m_paVertex = new TLVERTEX [r2DGraph.m_iMaxNumData];
	for(int i=0; i<r2DGraph.m_iMaxNumData; i++)
		m_paVertex[i] = r2DGraph.m_paVertex[i];

	m_BackGroundRect = r2DGraph.m_BackGroundRect;

	m_avIndicator[0] = r2DGraph.m_avIndicator[0];
	m_avIndicator[1] = r2DGraph.m_avIndicator[1];
	m_iIndicatorPosition = r2DGraph.m_iIndicatorPosition;

	m_vecGraphSegment.assign( r2DGraph.m_vecGraphSegment.begin(), r2DGraph.m_vecGraphSegment.end() );

	SafeDelete( m_pGraphSegmentIDFont );
	m_pGraphSegmentIDFont = new CFont;
	m_pGraphSegmentIDFont->InitFont( "ÇlÇr ÉSÉVÉbÉN", 8, 12 );

	return *this;
}

void C2DGraph::SetData(vector<int> *pveciData, U32 dwColor)
{
	vector<float> vecfTemp;

	for(int i=0; i<pveciData->size(); i++)
		vecfTemp.push_back( (float)pveciData->at(i) );

	SetData( &vecfTemp );
}

void C2DGraph::SetData(vector<float> *pvecfData, U32 dwColor)
{
	SGraphData new_graph;

	// set color
	new_graph.m_dwGraphColor = dwColor;

	// copy the data from the source buffer 'pvecfData'
	int i,j;
	for(i=0; i<pvecfData->size(); i++)
	{
		float fNewDataValue = pvecfData->at(i);

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

void C2DGraph::SetData(vector<Vector3> *pvecvData)
{
	int i, j;
	int iNumData = pvecvData->size();
	vector<float> vecfTempData;
	U32 adwColor[3] = { 0xCCFF0000, 0xCC00FF00, 0xCC0000FF };	// rgb for xyz
	for(i=0; i<3; i++)
	{
		vecfTempData.clear();
		for(j=0; j<iNumData; j++)
		{
			vecfTempData.push_back( (pvecvData->at(j))[i] );
		}
		SetData( &vecfTempData, adwColor[i] );
	}
}

void C2DGraph::SetPosition(float sx, float ex, float sy, float ey)
{
	m_vMin = Vector2(sx, sy);
	m_vMax = Vector2(ex, ey);

	float fScale = m_fMaxValue - m_fMinValue;
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


void C2DGraph::ChangeBackgroundRectColor(U32 dwColor, int iVertexNum)
{
	this->m_BackGroundRect.SetColor( dwColor, iVertexNum );
}


void C2DGraph::SetIndicatorPosition(int iIndex)
{
	if(iIndex < 0 || m_iMaxNumData <= iIndex)
		return;

	m_iIndicatorPosition = iIndex;

	// update the HORIZONTAL position of the indicator in the graph
	int iMaxNumData = m_iMaxNumData;
	float fWidth = m_vMax.x - m_vMin.x;
	m_avIndicator[0].vPosition.x
		= ( ((float)m_iIndicatorPosition / (float)iMaxNumData) * 0.9f + 0.05f ) * fWidth;
	m_avIndicator[0].vPosition.x += m_vMin.x;
	m_avIndicator[1].vPosition.x = m_avIndicator[0].vPosition.x;

}

void C2DGraph::Draw()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// draw rectangle as background
	m_BackGroundRect.Draw();

	// draw graph
	pd3dDev->SetVertexShader( NULL );
	pd3dDev->SetFVF( D3DFVF_TLVERTEX );

	float fDataScale = m_fMaxValue - m_fMinValue;
	float fHeight = m_vMax.y - m_vMin.y;
	float fMaxY = m_vMax.y;
	float fOffsetY = m_fMinValue;

	int i, j, iMaxNumData = m_iMaxNumData;
	int iNumGraphs = m_vecGraphData.size();

	// draw segments
	int iNumGraphSegments = m_vecGraphSegment.size();
	for(i=0; i<iNumGraphSegments; i++)
		m_vecGraphSegment[i].Draw();

	for(i=0; i<iNumGraphs; i++)
	{
		// set graph data
		int iNumData = m_vecGraphData[i].m_vecfData.size();
		U32 dwColor = m_vecGraphData[i].m_dwGraphColor;
		for(j=0; j<iNumData; j++)
		{
			// local position
			m_paVertex[j].vPosition.y
				= ( ((m_vecGraphData[i].m_vecfData[j] - fOffsetY) / fDataScale) * 0.8f + 0.1f ) * fHeight;

			// global position
			m_paVertex[j].vPosition.y = fMaxY - m_paVertex[j].vPosition.y;

			// set color
			m_paVertex[j].color = dwColor;
		}

		// draw graph data
		pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, iNumData - 1, m_paVertex, sizeof(TLVERTEX) );

		// draw indicator
		pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, m_avIndicator, sizeof(TLVERTEX) );
	}

}


void C2DGraph::AddSegment(int iSegmentID, int iStart, int iEnd, U32 dwColor)
{
	if( iStart < 0 || m_iMaxNumData <= iStart ||
		iEnd < 0   || m_iMaxNumData <= iEnd )
		return;

	CGraphSegment segment;
	segment.SetSegment( iSegmentID, iStart, iEnd, dwColor, m_pGraphSegmentIDFont);

	this->m_vecGraphSegment.push_back( segment );
}


void C2DGraph::SetSegmentID( int iSegmentNum, int iNewSegmentID )
{
	if( iSegmentNum < 0 || this->m_vecGraphSegment.size() <= iSegmentNum )
		return;

	// set new segment ID
	m_vecGraphSegment[iSegmentNum].SetSegmentID( iNewSegmentID );
}



//================================================================================
// CGraphSegment
//================================================================================

void CGraphSegment::UpdatePosition(int iNumData, float sx, float ex, float sy, float ey)
{
	// (sx,sy) - (ex,ey): boundary of the parent graph
	// iNumData: number of data in the parent graph

	Vector2 vMin, vMax;
	float fWidth = ex - sx;
	vMin.x = sx + fWidth * (float)m_iStart / (float)iNumData;
	vMin.y = sy;
	vMax.x = sx + fWidth * (float)m_iEnd   / (float)iNumData + 0.4f;
	vMax.y = ey;
	m_SegmentRect.SetPosition( vMin, vMax );

	// keep the digits of 'Segment ID' within the segment rect
	float fHOffset, fSegmentWidth = vMax.x - vMin.x;
	if( 10 <= m_iSegmentID && fSegmentWidth < 16 )
		fHOffset = 4.0f;
	else
		fHOffset = 0;
	// decide the location to display the 'Segment ID'
	// font size: w8 x h12
//	m_vIDPosition = Vector2(	// display at the bottom
//		(vMax.x + vMin.x)/2.0f - 4.0f - fHOffset , vMax.y - 12.0f );
	m_vIDPosition = Vector2(
		(vMax.x + vMin.x)/2.0f - 4.0f - fHOffset , vMin.y );
}


void CGraphSegment::SetSegment(int iSegmentID, int iStart, int iEnd, U32 dwColor, CFont* pFont)
{
	m_iSegmentID = iSegmentID;
	m_iStart     = iStart;
	m_iEnd       = iEnd;
//	m_SegmentRect.SetColor( dwColor );
	m_SegmentRect.SetZDeppth(0.05f);
	m_SegmentRect.SetColor( D3DCOLOR_ARGB(255, 240, 240, 240), 0 );
	m_SegmentRect.SetColor( D3DCOLOR_ARGB(255, 200, 200, 200), 1 );
	m_SegmentRect.SetColor( D3DCOLOR_ARGB(255, 160, 160, 160), 2 );
	m_SegmentRect.SetColor( D3DCOLOR_ARGB(255, 200, 200, 200), 3 );
	m_pFont = pFont;

}


void CGraphSegment::Draw()
{
	// draw rectangular area of this segment
	m_SegmentRect.Draw();
	char acSegmentID[16];

	// display the ID of this segment
	_itoa( m_iSegmentID , acSegmentID, 10 );
//	m_pFont->Print( acSegmentID, m_vIDPosition, 0xFFDDDDDD );
	m_pFont->Print( acSegmentID, m_vIDPosition, 0xFF000000 );
}
