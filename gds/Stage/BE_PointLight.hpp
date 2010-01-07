#ifndef __BE_PointLight_H__
#define __BE_PointLight_H__

#include "BE_Light.hpp"
#include "BEC_Billboard.hpp"
#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/HemisphericLight.hpp"


#define CEDL_HAS_PARENT	1	// TODO: better parent/child control system


class CBE_PointLight : public CBE_Light, CBEC_Billboard
{
private:

	std::string m_GlareTextureFilepath;
	CTextureHandle m_GlareTexture;

	TEXTUREVERTEX m_avGlareRect[4];

	float m_fGlareDiameter;

	float m_fTimer;

	CHemisphericPointLight m_PointLight;

public:

	CBE_PointLight();
	~CBE_PointLight() {}

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	void Draw(CCopyEntity* pCopyEnt); 
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	void CheckEntitiesInLightRange( CCopyEntity* pCopyEnt, int light_index );

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_POINTLIGHT; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	bool CheckRayToCamera(CCopyEntity* pCopyEnt);

	void DeleteDynamicLight(CCopyEntity* pCopyEnt);
};

#endif  /*  __BE_PointLight_H__  */
