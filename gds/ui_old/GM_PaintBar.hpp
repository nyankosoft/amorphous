#ifndef  __GM_PAINTBAR_H__
#define  __GM_PAINTBAR_H__

#include <vector>
using namespace std;

#include "GM_Control.h"
#include "GM_GraphicElement.h"


class CGM_PaintBarDesc;
struct SInputData;


class CGM_PaintBar : public CGM_Control
{
public:

	struct Segment
	{
		int left, right;

		int type;

		Segment() { left = right = type = 0; }

		inline bool OverlapsWith( Segment& region );
		inline bool ContainsPosition( int x );
		inline void MergeWith( Segment& region );
	};

private:

	/// holds painted regions
	vector<Segment> m_vecPaintedRegion;

	unsigned int m_BarFlag;

	CGM_TextureRectElement m_BarRect;

	/// represented in local coord of the paint bar
	int m_iStartPosition;

	/// represented in local coord of the paint bar
	int m_iCurrentPosition;

	bool m_bPressed;

	int m_iSegmentEpsilon;

//	virtual void UpdateRects(); 

public:

	CGM_PaintBar( CGM_Dialog *pDialog, CGM_PaintBarDesc *pDesc );
	~CGM_PaintBar() {}

//	virtual bool IsPointInside( SPoint& pt ); 
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
//    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );

    virtual bool HandleMouseInput( SInputData& input );
  
	virtual void Render( float fElapsedTime );

	void AddSegment( Segment& region );

	void Clamp( int& left, int& right );

	void CheckDeleteSegment( const int pos_x, const int non_target_type );

	/// access to segment data
	vector<Segment>& GetSegments() { return m_vecPaintedRegion; }

	/// set a vector of segments. all the previous data are cleared
	void SetSegments(vector<Segment>& vecSegment) { m_vecPaintedRegion = vecSegment; }

	/// clear all the existing segments
	void ClearSegments() { m_vecPaintedRegion.clear(); }


	enum eTypeFlag
	{
		TYPE_MERGE_OVERLAPS = (1 << 0)
	};

};



#endif		/*  __GM_PAINTBAR_H__  */