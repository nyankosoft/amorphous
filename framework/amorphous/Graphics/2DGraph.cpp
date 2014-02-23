//===============================================================================
// 2DGraph.cpp
//   - draw graphs using Direct3D
//===============================================================================

#include "2DGraph.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Graphics/Font/BitstreamVeraSansMono_Bold_256.hpp"
#include "Graphics/2DPrimitive/2DPrimitiveRenderer.hpp"


namespace amorphous
{

using namespace std;
using boost::shared_ptr;


//================================================================================
// CGraphSegment
//================================================================================

C2DGraph::C2DGraph()
{
	m_fMaxValue = -FLT_MAX;
	m_fMinValue =  FLT_MAX;
	m_iMaxNumData = 0;
	m_vMin = Vector2(0,0);
	m_vMax = Vector2(0,0);

	m_BackGroundRect.SetColor( 0xAA000000 );
	m_BackGroundRect.SetZDepth( 0.1f );

	m_iIndicatorPosition = 0;
//	ZeroMemory(m_avIndicator, sizeof(TLVERTEX) * 2);
	m_avIndicator[0].m_DiffuseColor.SetARGB32( 0xBB202020 );	// light gray
	m_avIndicator[1].m_DiffuseColor.SetARGB32( 0xBB202020 );
	m_avIndicator[0].m_fRHW = 1.0f;
	m_avIndicator[1].m_fRHW = 1.0f;

	// create the font object that is shared by all the graph segments in this graph
	TextureFont *pFont = new TextureFont;
	pFont->InitFont( g_BitstreamVeraSansMono_Bold_256 );
	pFont->SetFontSize( 8, 12 );
	m_pGraphSegmentIDFont.reset( pFont );
}

C2DGraph::~C2DGraph()
{
	m_pGraphSegmentIDFont->Release();
}


void C2DGraph::SetData(vector<int> *pveciData, U32 dwColor)
{
	vector<float> vecfTemp;

	for( size_t i=0; i<pveciData->size(); i++ )
		vecfTemp.push_back( (float)pveciData->at(i) );

	SetData( &vecfTemp );
}

void C2DGraph::SetData(vector<float> *pvecfData, U32 dwColor)
{
	SGraphData new_graph;

	// set color
	new_graph.m_dwGraphColor = dwColor;

	// copy the data from the source buffer 'pvecfData'
	for(size_t i=0; i<pvecfData->size(); i++)
	{
		float fNewDataValue = pvecfData->at(i);

		if( m_fMaxValue < fNewDataValue )
			m_fMaxValue	= fNewDataValue;
		if( fNewDataValue < m_fMinValue )
			m_fMinValue	= fNewDataValue;

		new_graph.m_vecfData.push_back( fNewDataValue );
	}

	m_vecGraphData.push_back( new_graph );

	const int num_data = (int)new_graph.m_vecfData.size();

	if( m_iMaxNumData < num_data )
	{
		m_iMaxNumData = num_data;
		m_Vertices.resize( 0 );
		m_Vertices.resize( num_data );
	}
}

void C2DGraph::SetData(vector<Vector3> *pvecvData)
{
	const size_t num_data = pvecvData->size();
	vector<float> vecfTempData;
	U32 adwColor[3] = { 0xCCFF0000, 0xCC00FF00, 0xCC0000FF };	// rgb for xyz
	for(int i=0; i<3; i++)
	{
		vecfTempData.clear();
		for(size_t j=0; j<num_data; j++)
		{
			vecfTempData.push_back( (pvecvData->at(j))[i] );
		}
		SetData( &vecfTempData, adwColor[i] );
	}
}

void C2DGraph::Clear()
{
	m_vecGraphData.resize( 0 );
	m_fMaxValue = -FLT_MAX;
	m_fMinValue =  FLT_MAX;
}

void C2DGraph::SetPosition( const AABB2& aabb )
{
	SetPosition( aabb.vMin.x, aabb.vMax.x, aabb.vMin.y, aabb.vMax.y );
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
		m_Vertices[i].m_vPosition.x
			= ( ((float)i / (float)iMaxNumData) * 0.9f + 0.05f ) * fWidth;

		// global position
		m_Vertices[i].m_vPosition.x += sx;
		
	}

	// set positions of the segment rects
	for(i=0; i<(int)m_vecGraphSegment.size(); i++)
	{
		m_vecGraphSegment[i].UpdatePosition( iMaxNumData,
			sx + fWidth  * 0.05f, ex - fWidth  * 0.05f,
			sy + fHeight * 0.05f, ey - fHeight * 0.05f );
	}

	// set position of the background rectangle
	m_BackGroundRect.SetPosition( m_vMin, m_vMax );

	// set the VERTICAL length of the indicator
	m_avIndicator[0].m_vPosition.y = sy + fHeight * 0.1f;
	m_avIndicator[1].m_vPosition.y = ey - fHeight * 0.1f;
}


void C2DGraph::ChangeBackgroundRectColor(U32 dwColor, int iVertexNum)
{
	this->m_BackGroundRect.SetCornerColor( iVertexNum, dwColor );
}


void C2DGraph::SetIndicatorPosition(int iIndex)
{
	if(iIndex < 0 || m_iMaxNumData <= iIndex)
		return;

	m_iIndicatorPosition = iIndex;

	// update the HORIZONTAL position of the indicator in the graph
	int iMaxNumData = m_iMaxNumData;
	float fWidth = m_vMax.x - m_vMin.x;
	m_avIndicator[0].m_vPosition.x
		= ( ((float)m_iIndicatorPosition / (float)iMaxNumData) * 0.9f + 0.05f ) * fWidth;
	m_avIndicator[0].m_vPosition.x += m_vMin.x;
	m_avIndicator[1].m_vPosition.x = m_avIndicator[0].m_vPosition.x;

}

void C2DGraph::Draw()
{
//	ShaderManager *pShaderMgr = GetNoLightingShader().GetShaderManager();
//	Get2DPrimitiveRenderer().Render( ?, &(m_Vertices[0]), m_Vertices.size(), PrimitiveType::CONNECTED_LINES );

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

	int iMaxNumData = m_iMaxNumData;

	// draw segments
	const size_t num_graph_segs = m_vecGraphSegment.size();
	for(size_t i=0; i<num_graph_segs; i++)
		m_vecGraphSegment[i].Draw();

	const size_t num_graphs = m_vecGraphData.size();
	for(size_t i=0; i<num_graphs; i++)
	{
		// set graph data
		const size_t num_data = m_vecGraphData[i].m_vecfData.size();
		U32 dwColor = m_vecGraphData[i].m_dwGraphColor;
		for(size_t j=0; j<num_data; j++)
		{
			// local position
			m_Vertices[j].m_vPosition.y
				= ( ((m_vecGraphData[i].m_vecfData[j] - fOffsetY) / fDataScale) * 0.8f + 0.1f ) * fHeight;

			// global position
			m_Vertices[j].m_vPosition.y = fMaxY - m_Vertices[j].m_vPosition.y;

			// set color
			m_Vertices[j].color = dwColor;
		}

		// draw graph data
		pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, (int)num_data - 1, m_Vertices, sizeof(TLVERTEX) );

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
	if( iSegmentNum < 0 || (int)this->m_vecGraphSegment.size() <= iSegmentNum )
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


void CGraphSegment::SetSegment(int iSegmentID, int iStart, int iEnd, U32 dwColor, shared_ptr<FontBase> pFont)
{
	m_iSegmentID = iSegmentID;
	m_iStart     = iStart;
	m_iEnd       = iEnd;
//	m_SegmentRect.SetColor( dwColor );
	m_SegmentRect.SetZDepth(0.05f);
	m_SegmentRect.SetCornerColor( 0, SFloatRGBAColor( 240.0f / 255.0f, 240.0f / 255.0f, 240.0f / 255.0f, 1.0f ) );
	m_SegmentRect.SetCornerColor( 1, SFloatRGBAColor( 200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f ) );
	m_SegmentRect.SetCornerColor( 2, SFloatRGBAColor( 160.0f / 255.0f, 160.0f / 255.0f, 160.0f / 255.0f, 1.0f ) );
	m_SegmentRect.SetCornerColor( 3, SFloatRGBAColor( 200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f ) );
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
	m_pFont->DrawText( acSegmentID, m_vIDPosition, 0xFF000000 );
}


} // amorphous
