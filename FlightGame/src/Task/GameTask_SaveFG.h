#ifndef  __GAMETASK_SAVEFG_H__
#define  __GAMETASK_SAVEFG_H__


#include "GameTask_SaveLoadFG.h"

#include "GameCommon/SaveDataManager.h"
#include "GameCommon/CriticalDamping.h"
#include "3DMath/Matrix34.h"
#include "UI/ui_fwd.h"

#include <vector>
#include <string>


enum TASK_SAVE_UI_ID
{
	ID_UI_SAVE_BACK,
	ID_UI_SAVE_NEXT,
	ID_UI_SAVE_PREVPAGE,
	ID_UI_SAVE_NEXTPAGE,
	ID_UI_DLG_SAVE_CONFIRM_OVERWRITE,
	ID_UI_SAVE_CONFIRM_OVERWRITE_YES,
	ID_UI_SAVE_CONFIRM_OVERWRITE_NO,
	NUM_SAVE_UI_IDS
};


/// - Has attributes to save / write on its own.
/// - Saves the extra info like play time and money
///   which are displayed on the save data slots
class CExtraSaveDataWriter : public CSaveDataComponent
{
public:

	virtual const char *GetKeyString() const { return "ExtraSaveDataWriter"; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		// do nothing
	}

	virtual void SaveTo( CBinaryDatabase<std::string>& db );
};


/*
class CUIRender_SaveFG : public CUIRender_SaveLoadFG
{
public:

	CUIRender_SaveFG() {}
	virtual ~CUIRender_SaveFG() {}
};
*/

class CGameTask_SaveFG : public CGameTask_SaveLoadFG
{
public:

	enum sub_menu
	{
		SM_INVALID = -1,
		SM_SELECT_SAVESLOT = 0,
		SM_CONFIRM_OVERWRITE,
		NUM_SUB_MENUS
	};


protected:

	CExtraSaveDataWriter m_ExtraSaveDataWriter;

/*	CGM_DialogManagerSharedPtr m_apDialogManager[NUM_SUB_MENUS];
	CUIRender_SaveFG *m_pUIRendererManager;
	/// access to ui controls (borrowed references)
	CGM_ListBox *m_pSaveSlotListBox;
*/
protected:

	virtual void InitMenu();

	CGM_Dialog *CreateOerwriteConfirmationDialog();

	virtual void RenderTaskMenu();

//	void InitStage();

public:

	CGameTask_SaveFG();
	virtual ~CGameTask_SaveFG();

	virtual int FrameMove( float dt );
//	virtual void Render();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	// \param -1 > save to the currently selected slot
	bool SaveCurrentState( int index = -1 );
};


#endif  /*  __GAMETASK_SAVEFG_H__  */
