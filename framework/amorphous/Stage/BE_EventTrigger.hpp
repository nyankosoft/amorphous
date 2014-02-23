#ifndef __BE_EVENTTRIGGER_H__
#define __BE_EVENTTRIGGER_H__

#include "BaseEntity.hpp"


namespace amorphous
{


class CBE_EventTrigger : public BaseEntity
{
private:

public:

	CBE_EventTrigger();
	virtual ~CBE_EventTrigger();

	// void Init() {}
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	// void Act(CCopyEntity* pCopyEnt) {}	//behavior in in one frame
	// void Draw(CCopyEntity* pCopyEnt) {} 
	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	// void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) {}

	virtual unsigned int GetArchiveObjectID() const { return BE_EVENTTRIGGER; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};
} // namespace amorphous



#endif  /*  __BE_EVENTTRIGGER_H__  */
