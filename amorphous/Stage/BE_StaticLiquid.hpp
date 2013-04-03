#ifndef __BE_StaticLiquid_HPP__
#define __BE_StaticLiquid_HPP__

#include "BaseEntity.hpp"


namespace amorphous
{
//#include "CopyEntity.hpp"



class CBE_StaticLiquid : public BaseEntity
{
public:

	CBE_StaticLiquid();

	virtual ~CBE_StaticLiquid() {}

	 void Init();

	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);

	void Draw(CCopyEntity* pCopyEnt);

	void OnEntityDestroyed(CCopyEntity* pCopyEnt);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	unsigned int GetArchiveObjectID() const { return BE_STATICLIQUID; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	friend class CoreBaseEntitiesLoader;
};

} // namespace amorphous



#endif  /*  __BE_StaticLiquid_HPP__  */
