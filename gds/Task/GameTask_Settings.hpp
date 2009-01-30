
#ifndef  __GAMETASK_SETTINGS_H__
#define  __GAMETASK_SETTINGS_H__


#include "GameTask.hpp"

class CStage;
class CInputHandler;


class CGM_DialogManager;

class CGameTask_Settings : public CGameTask
{
	CInputHandler *m_pInputHandler;

	CGM_DialogManager *m_pDialogManager;

public:
	CGameTask_Settings();
	~CGameTask_Settings();

	int FrameMove( float dt );
	void Render( float dt );

};


#endif  /*  __GAMETASK_SETTINGS_H__  */