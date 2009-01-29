#ifndef __BE_DIRECTIONALLIGHT_H__
#define __BE_DIRECTIONALLIGHT_H__

#include "BE_Light.h"


class CBE_DirectionalLight : public CBE_Light
{
private:

	CHemisphericDirectionalLight m_DirLight;

public:

	CBE_DirectionalLight();
	~CBE_DirectionalLight() {}

	void Init();

	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);

//	void Draw(CCopyEntity* pCopyEnt); 

	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}

	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );

//	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

//	void CheckEntitiesInLightRange( CCopyEntity* pCopyEnt, short sLightIndex );

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_DIRECTIONALLIGHT; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

//	bool CheckRayToCamera(CCopyEntity* pCopyEnt);

//	void DeleteDynamicLight(CCopyEntity* pCopyEnt);


//	void ReleaseGraphicsResources();
//	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};

#endif  /*  __BE_DIRECTIONALLIGHT_H__  */
