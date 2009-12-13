#ifndef  __2DGraph_H__
#define  __2DGraph_H__

#include <vector>
#include "3DMath/AABB2.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"


enum {GRAPHTYPE_POINT, GRAPHTYPE_LINE};


//==============================================================================
// SGraphData
// - used to hold one graph data
//==============================================================================

struct SGraphData
{
	std::vector<float> m_vecfData;
	U32 m_dwGraphColor;
	float fMaxValue;
	float fMinValue;

	SGraphData()
	{
		fMaxValue = -99999;
		fMinValue =  99999;
	}
};


//==============================================================================
// CGraphSegment
// - used to highlight a portion of the graph
//==============================================================================

struct CGraphSegment
{
	int m_iSegmentID;
	int m_iStart;
	int m_iEnd;

	C2DRect m_SegmentRect;

	/// the position to display 'segment ID'
	Vector2 m_vIDPosition;

    /// font to display the ID (shared by all the segments under the same parent graph)
	CFontBase* m_pFont;

public:

	CGraphSegment()
	{
		m_iSegmentID = 0;
		m_iStart = 0;
		m_iEnd = 0;
		m_pFont = NULL;
	}

	void SetSegmentID( int iNewSegmentID ) { m_iSegmentID = iNewSegmentID; }
	void SetSegment( int iSegmentID, int iStart, int iEnd, U32 dwColor, CFontBase* pFont );
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

	std::vector<SGraphData> m_vecGraphData;
	float m_fMaxValue;
	float m_fMinValue;

	int m_iMaxNumData;

	Vector2 m_vMin;
	Vector2 m_vMax;

	TLVERTEX* m_paVertex;
	C2DRect m_BackGroundRect;

	int m_iGraphType;

	TLVERTEX m_avIndicator[2];
	int m_iIndicatorPosition;

	// used to hightlight particular portions of the graph
	std::vector<CGraphSegment> m_vecGraphSegment;

	CFontBase* m_pGraphSegmentIDFont;

public:

	C2DGraph();

	C2DGraph(const C2DGraph& r2DGraph);	//copy constructor

	~C2DGraph();

	C2DGraph operator=(C2DGraph r2DGraph);

	virtual void Draw();

	virtual void SetPosition(float sx, float ex, float sy, float ey);

	void SetPosition( const AABB2& aabb );

	void SetIndicatorPosition(int iIndex);

	void ChangeBackgroundRectColor( U32 dwColor, int iVertexNum = 4 );

	void SetGraphType( int iGraphType ) { m_iGraphType = iGraphType; }

	virtual void SetData( std::vector<float> *pvecfData, U32 dwColor = 0xFF00BB00 );
	void SetData( std::vector<int> *pveciData, U32 dwColor = 0xFF00BB00 );
	void SetData( std::vector<Vector3> *pvecvData );

	float GetGraphData( int iGraphNum, int iPosition ) { return m_vecGraphData[iGraphNum].m_vecfData[iPosition]; }

	void AddSegment( int iSegmentID, int iStart, int iEnd, U32 dwColor );
	void SetSegmentID( int iSegmentNum, int iNewSegmentID );

};


#endif		/*  __2DGraph_H__  */
