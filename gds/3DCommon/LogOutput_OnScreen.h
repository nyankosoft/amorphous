#ifndef  __LOGOUTPUT_ONSCREEN_H__
#define  __LOGOUTPUT_ONSCREEN_H__

#include <vector>
#include <string>

#include "../base.h"
#include "Support/Log/LogOutputBase.h"
#include "Support/2DArray.h"
#include "3DMath/Vector2.h"
#include "3DCommon/FontBase.h"
#include "3DCommon/GraphicsComponentCollector.h"


/**
 * displays log texts on screen using D3D
 *
 */
class CLogOutput_OnScreen : public CLogOutputBase, public CGraphicsComponent
{
protected:

	CFontBase* m_pFont;	///< owned reference
	CFontBase* m_pBorrowedFont;	///< borrowed reference

	/// owned reference of the borrowed font must be released & reloaded with Reload() & Reload()
	/// when the ReleaseGraphicsResources() or LoadGraphicsResources() is called.
	/// Otherwise 'm_pBorrowedFont' will be an invalid pointer after graphics release & reload
	/// is done.

//	C2DArray<char> m_TextBuffer;
	std::vector<std::string> m_TextBuffer;
	std::vector<U32> m_TextColor;

	int m_EndRowIndex;

	int m_NumRows;

	/// how many logs have been taken
	int m_NumOutputLines;

	Vector2 m_vTopLeftPos;

private:

	void Init( int num_rows );

public:

	CLogOutput_OnScreen( const std::string& font_name, int font_width, int font_height,
                         int num_rows = 16, int num_chars = 64 );

	CLogOutput_OnScreen( int num_rows = 16, int num_chars = 64 );	///< use this with a borrowed font

	~CLogOutput_OnScreen();

	void SetBorrowedFont( CFontBase* pBorrowedFont ) { m_pBorrowedFont = pBorrowedFont; }

	void SetTopLeftPos(	Vector2 vTopLeftPos ) { m_vTopLeftPos = vTopLeftPos; }

	virtual void ReleaseGraphicsResources();

	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );

	virtual void Print( const CLogMessage& msg );

	void Render();
};


#endif		/*  __LOGOUTPUT_ONSCREEN_H__  */