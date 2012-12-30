#ifndef __BE_IndividualEntity_H__
#define __BE_IndividualEntity_H__


#include "fwd.hpp"
#include "CopyEntity.hpp"
#include "BE_PhysicsBaseEntity.hpp"


namespace amorphous
{


class CBE_IndividualEntity : public CBE_PhysicsBaseEntity
{
public:

	CBE_IndividualEntity();

	virtual ~CBE_IndividualEntity();

	/// initialization of the base entity
	virtual void Init();

	/// initialization of each copy entity
	virtual void InitCopyEntity( CCopyEntity* pCopyEnt );

	virtual void Act(CCopyEntity* pCopyEnt);

	virtual void UpdatePhysics( CCopyEntity *pCopyEnt, float dt ) { pCopyEnt->UpdatePhysics( dt ); }

	virtual void Draw(CCopyEntity* pCopyEnt);

	virtual void RenderStage(CCopyEntity* pCopyEnt);

	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );

	virtual void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) { pCopyEnt_Self->HandleMessage( rGameMessage ); }

//	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_INDIVIDUALENTITY; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};

} // namespace amorphous



#endif  /*  __BE_IndividualEntity_H__  */
