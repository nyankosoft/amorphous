#ifndef __BE_CameraController_H__
#define __BE_CameraController_H__

#include "BaseEntity.h"
#include "BaseEntityHandle.h"
#include "CopyEntity.h"
#include "EntityHandle.h"
#include "Graphics/TextureRenderTarget.h"
#include "Support/FixedVector.h"

#include "../base.h"


class CInputHandler_Cutscene;

/**
- Camera controller entity holds one or more camera entities as its child entities
- Camera controller entity is set as the camera entity of the stage when
  at least one of its camera entity has motion path for the time
*/
class CBE_CameraController : public CBaseEntity
{
private:

	enum Params
	{
		NUM_MAX_ACTIVE_CAMERAS = 2
	};

//	CCamera m_Camera;

	// TODO: stack is probably better
	CEntityHandle<> m_PrevCameraEntity;

	/// handles input during the cutscene.
	/// e.g.) skipping the cutscene by start button
	CInputHandler_Cutscene *m_pInputHandler;

	bool m_bUseCutsceneInputHandler;

	bool m_bEndingCutscene;

	U32 m_CutsceneEndStartedTime;

	static CTextureRenderTarget ms_aTextureRenderTarget[NUM_MAX_ACTIVE_CAMERAS];

	static bool ms_TextureRenderTargetsInitialized;

	static uint ms_NumAvailableTextureRenderTargets;

private:

	void EndCutscene( CCopyEntity* pCopyEnt );

	void SkipCutscene( CCopyEntity* pCopyEnt );

	void GetActiveCameraIndices( CCopyEntity* pCopyEnt,
		TCFixedVector<uint,CCopyEntity::NUM_MAX_CHILDREN_PER_ENTITY>& active_cam_indices );

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

	virtual void CreateRenderTasks(CCopyEntity* pCopyEnt);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_CAMERACONTROLLER; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void UpdateCamera( CCopyEntity *pCopyEnt ) {}
//	virtual CCamera *GetCamera() { return &m_Camera; }

	friend CInputHandler_Cutscene;
};

#endif  /*  __BE_CameraController_H__  */
