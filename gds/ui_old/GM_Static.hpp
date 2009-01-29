
#ifndef  __GM_STATIC_H__
#define  __GM_STATIC_H__

#include "GM_Control.h"

#include "GM_GraphicElement.h"

//========================================================================================
// CGM_Static
//========================================================================================

#include <string>
using namespace std;

class CGM_StaticDesc;

class CGM_Static : public CGM_Control
{
protected:
	SPoint m_TextOffset;	// offset of the text position from the top left corner of the control
	string m_strText;

	CGM_FontElement m_DefaultFont;
	CGM_TextureRectElement m_DefaultRect;

public:
	CGM_Static( CGM_Dialog *pDialog, CGM_StaticDesc *pDesc );
	~CGM_Static();

	void Render( float fElapsedTime );

	inline string& GetText() { return m_strText; }

	inline void SetText( const string& strText ) { m_strText = strText; }
};


#endif		/*  __GM_STATIC_H__  */