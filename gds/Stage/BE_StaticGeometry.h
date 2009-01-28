#ifndef __BE_StaticGeometry_H__
#define __BE_StaticGeometry_H__

#include "BaseEntity.h"

#include <vector>
#include <string>


class CStaticGeometryBase;

class CBE_StaticGeometry : public CBaseEntity
{
private:

	/// borrowed reference
	/// owned reference managed by CStage
	std::vector<CStaticGeometryBase *> m_vecpStaticGeometry;

	enum Params
	{
		NUM_MAX_STATIC_GEOMETRIES = 256,
	};

public:

	CBE_StaticGeometry();
	virtual ~CBE_StaticGeometry();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	void Draw(CCopyEntity* pCopyEnt); 
	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_STATICGEOMETRY; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};

#endif  /*  __BE_StaticGeometry_H__  */
