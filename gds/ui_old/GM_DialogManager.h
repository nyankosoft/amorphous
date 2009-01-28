#ifndef  __GM_DIALOGMANAGER_H__
#define  __GM_DIALOGMANAGER_H__


#include "GM_CaptionParameters.h"
#include "GM_CaptionRenderRoutine.h"

#include <d3dx9.h>

#include <vector>
#include <string>
using namespace std;


//===========================================================================
// CGM_TextureNode
//===========================================================================

class CGM_TextureNode
{
public:
	string strFilename;
	LPDIRECT3DTEXTURE9 pTexture;

	CGM_TextureNode() { pTexture = NULL; }
};



class CGM_Dialog;
class CGM_DialogDesc;
class CGM_FontDesc;
struct SInputData;
class CFontBase;


//===========================================================================
// CGM_DialogManager
//===========================================================================

class CGM_DialogManager
{
	vector<CGM_Dialog *> m_vecpDialog;

	CGM_Dialog *m_pRootDialog;

	/// font resources
	vector<CFontBase *> m_vecpFontCache;

	/// texture resources
	vector<CGM_TextureNode> m_vecpTextureCache;

	/// parameters for displaying caption text
	CGM_CaptionParameters m_CaptionParam;

	bool m_bCaption;

	/// holds a control to be captioned
	CGM_Control *m_pControlForCaption;

public:

	CGM_DialogManager();
	~CGM_DialogManager();

	void Init();
	void Release();

	CGM_Dialog *AddDialog( CGM_DialogDesc &rDialogDesc );

	inline CFontBase *GetFont( int iIndex ) { return m_vecpFontCache[iIndex]; }

	bool AddFont( int index, CGM_FontDesc& rFontDesc );

	inline LPDIRECT3DTEXTURE9 GetTexture( int iIndex )  { return m_vecpTextureCache[iIndex].pTexture; }

	void LoadTextureFromFile( int iIndex, const char *pFilename );

	bool HandleInput( SInputData& input );

	void SetCaptionParameters( CGM_CaptionParameters& param ) { m_CaptionParam = param; }

	void EnableCaption( bool enable ) { m_bCaption = enable; }

	/// set a control to be captioned
	inline void SetControlForCaption( CGM_Control* pControl );

	void Render( float fElapsedTime );

};


#endif		/*  __GM_DIALOGMANAGER_H__  */


//=============================== inline implementations ===============================


inline void CGM_DialogManager::SetControlForCaption( CGM_Control* pControl )
{
	m_pControlForCaption = pControl;

	if( m_CaptionParam.pCaptionRenderRoutine )
		m_CaptionParam.pCaptionRenderRoutine->CaptionControlChanged( pControl );
}

