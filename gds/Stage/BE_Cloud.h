#ifndef __BE_CLOUD_H__
#define __BE_CLOUD_H__

#include "BE_ParticleSet.h"

#include <vector>
#include <string>


class CBE_Cloud : public CBE_ParticleSet
{
private:

public:

	CBE_Cloud();
	virtual ~CBE_Cloud();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	void Draw(CCopyEntity* pCopyEnt); 
	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_CLOUD; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};

#endif  /*  __BE_CLOUD_H__  */
