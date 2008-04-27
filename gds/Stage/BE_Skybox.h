#ifndef __BE_Skybox_H__
#define __BE_Skybox_H__

#include "BaseEntity.h"

#include <vector>
#include <string>


class CBE_Skybox : public CBaseEntity
{
private:

	/// load texture separately from the mesh
	CTextureHandle m_SkyboxTexture;

	// CBaseEntity::m_MeshProperty is used for the mesh of the skybox
	
public:

	CBE_Skybox();
	virtual ~CBE_Skybox();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	void Draw(CCopyEntity* pCopyEnt); 
//	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_SKYBOX; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	bool LoadSkyboxTexture( const std::string& texture_filename );

};

#endif  /*  __BE_Skybox_H__  */
