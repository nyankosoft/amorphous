#ifndef  __2DGRAPH_H__
#define  __2DGRAPH_H__

#include <vector>
using namespace std;

#include "../3DCommon/2DRect.h"

enum {GRAPHTYPE_POINT, GRAPHTYPE_LINE};


//==============================================================================
// SGraphData
// - used to hold one graph data
//==============================================================================

struct SGraphData
{
	vector<float> m_vecfData;
	DWORD m_dwGraphColor;
	float fMaxValue;
	float fMinValue;

	SGraphData()
	{
		fMaxValue = -99999;
		fMinValue =  99999;
	}
	SGraphData operator=(SGraphData graph_data)
	{
		this->m_dwGraphColor = graph_data.m_dwGraphColor;
		this->m_vecfData.assign( graph_data.m_vecfData.begin(), graph_data.m_vecfData.end() );
		return *this;
	}
};


//==============================================================================
// CGraphSegment
// - used to highlight a portion of the graph
//==============================================================================

class CFont;

struct CGraphSegment
{
	int m_iSegmentID;
	int m_iStart;
	int m_iEnd;

	C2DRect m_SegmentRect;

	/// the position to display 'segment ID'
	D3DXVECTOR2 m_vIDPosition;

    /// font to display the ID (shared by all the segments under the same parent graph)
	CFont* m_pFont;

public:
	CGraphSegment()
	{
		m_iSegmentID = 0;
		m_iStart = 0;
		m_iEnd = 0;
		m_pFont = NULL;
	}

	void SetSegmentID( int iNewSegmentID ) { m_iSegmentID = iNewSegmentID; }
	void SetSegment( int iSegmentID, int iStart, int iEnd, DWORD dwColor, CFont* pFont );
	void UpdatePosition(int iNumData, float sx, float ex, float sy, float ey);
	void Draw();

};


//==============================================================================
// C2DGraph
// - 2D graph drawn in a rectangular area
//==============================================================================

class C2DGraph
{
protected:

	vector<SGraphData> m_vecGraphData;
	float m_fMaxValue;
	float m_fMinValue;

	int m_iMaxNumData;

	D3DXVECTOR2 m_vMin;
	D3DXVECTOR2 m_vMax;

	TLVERTEX* m_paVertex;
	C2DRect m_BackGroundRect;

	int m_iGraphType;

	TLVERTEX m_avIndicator[2];
	int m_iIndicatorPosition;

	//used to hightlight particular portions of the graph
	vector<CGraphSegment> m_vecGraphSegment;
	CFont* m_pGraphSegmentIDFont;

public:

	C2DGraph();

	C2DGraph(const C2DGraph& r2DGraph);	//copy constructor

	~C2DGraph();

	C2DGraph operator=(C2DGraph r2DGraph);

	virtual void Draw();

	virtual void SetPosition(float sx, float ex, float sy, float ey);

	void SetIndicatorPosition(int iIndex);

	void ChangeBackgroundRectColor(DWORD dwColor, int iVertexNum = 4);

	void SetGraphType(int iGraphType) { m_iGraphType = iGraphType; }

	virtual void SetData(vector<float> *pvecfData, DWORD dwColor = 0xFF00BB00);
	void SetData(vector<int> *pveciData, DWORD dwColor = 0xFF00BB00);
	void SetData(vector<D3DXVECTOR3> *pvecvData);

	float GetGraphData(int iGraphNum, int iPosition) { return m_vecGraphData[iGraphNum].m_vecfData[iPosition]; }

	void AddSegment(int iSegmentID, int iStart, int iEnd, DWORD dwColor);
	void SetSegmentID( int iSegmentNum, int iNewSegmentID );

};

#endif		/*  __2DGRAPH_H__  */