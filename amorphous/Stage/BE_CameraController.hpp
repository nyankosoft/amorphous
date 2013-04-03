#ifndef __BE_CameraController_H__
#define __BE_CameraController_H__

#include "BaseEntity.hpp"
#include "BaseEntityHandle.hpp"
#include "CopyEntity.hpp"
#include "EntityHandle.hpp"
#include "Graphics/TextureRenderTarget.hpp"
#include "Support/FixedVector.hpp"

#include "../base.hpp"


namespace amorphous
{


class InputHandler_Cutscene;

/**
- Camera controller entity holds one or more camera entities as its child entities
- Camera controller entity is set as the camera entity of the stage when
  at least one of its camera entity has motion path for the time
*/
class CBE_CameraController : public BaseEntity
{
private:

	enum Params
	{
		NUM_MAX_ACTIVE_CAMERAS = 2
	};

//	Camera m_Camera;

	// TODO: stack is probably better
	EntityHandle<> m_PrevCameraEntity;

	/// handles input during the cutscene.
	/// e.g.) skipping the cutscene by start button
	InputHandler_Cutscene *m_pInputHandler;

	bool m_bUseCutsceneInputHandler;

	bool m_bEndingCutscene;

	U32 m_CutsceneEndStartedTime;

	static boost::shared_ptr<TextureRenderTarget> ms_apTextureRenderTarget[NUM_MAX_ACTIVE_CAMERAS];

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

	void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	virtual void RenderStage( CCopyEntity* pCopyEnt );

	virtual void CreateRenderTasks(CCopyEntity* pCopyEnt);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_CAMERACONTROLLER; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void UpdateCamera( CCopyEntity *pCopyEnt ) {}
//	virtual Camera *GetCamera() { return &m_Camera; }

	friend class InputHandler_Cutscene;
	friend class CoreBaseEntitiesLoader;
};
} // namespace amorphous



#endif  /*  __BE_CameraController_H__  */
