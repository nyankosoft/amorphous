#ifndef __BE_CameraController_H__
#define __BE_CameraController_H__

#include "BaseEntity.h"
#include "BaseEntityHandle.h"

#include "EntityMotionPathRequest.h"

#include "../base.h"


class CInputHandler_Cutscene;

class CBE_CameraController : public CBaseEntity
{
private:

//	CCamera m_Camera;

	// TODO: stack is probably better
	CCopyEntity* m_pPrevCameraEntity;

	/// handles input during the cutscene.
	/// e.g.) skipping the cutscene by start button
	CInputHandler_Cutscene *m_pInputHandler;

	bool m_bUseCutsceneInputHandler;

	bool m_bEndingCutscene;

	U32 m_CutsceneEndStartedTime;

private:

	void EndCutscene( CCopyEntity* pCopyEnt );

	void SkipCutscene( CCopyEntity* pCopyEnt );

public:

	CBE_CameraController();
	~CBE_CameraController();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
//	void Draw(CCopyEntity* pCopyEnt);
//	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
//	void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );

	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	virtual void RenderStage( CCopyEntity* pCopyEnt );

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_CAMERACONTROLLER; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void UpdateCamera( CCopyEntity *pCopyEnt ) {}
//	virtual CCamera *GetCamera() { return &m_Camera; }

	friend CInputHandler_Cutscene;
};

#endif  /*  __BE_CameraController_H__  */
