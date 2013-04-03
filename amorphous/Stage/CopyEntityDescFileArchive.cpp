
#include "CopyEntityDescFileArchive.hpp"


namespace amorphous
{


/*
CCopyEntityDescFileArchive::CCopyEntityDescFileArchive()
{
}


CCopyEntityDescFileArchive::~CCopyEntityDescFileArchive()
{
}
*/

void CCopyEntityDescFileData::Serialize( IArchive& ar, const unsigned int version )
{
	ar & strBaseEntityName;

	CCopyEntityDesc& desc = CopyEntityDesc;

//	pBaseEntityHandle;

	ar & desc.WorldPose;

	ar & desc.vVelocity & desc.fSpeed;

//	pParent;

	ar & desc.sGroupID;

	ar & desc.f1 & desc.f2 & desc.f3 & desc.f4;
	ar & desc.v1 & desc.v2;

	ar & desc.iExtraDataIndex;


	ar & vecChild;
}


void CCopyEntityDescFileArchive::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecCopyEntityDesc;
}


} // namespace amorphous
