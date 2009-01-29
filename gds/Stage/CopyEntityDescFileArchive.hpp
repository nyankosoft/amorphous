#ifndef  __COPYENTITYDESCFILEARCHIVE_H__
#define  __COPYENTITYDESCFILEARCHIVE_H__


#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;

#include <vector>
#include <string>
using namespace std;

#include "CopyEntityDesc.h"


//===============================================================================================
// CCopyEntityDescFileData
//===============================================================================================

class CCopyEntityDescFileData : public IArchiveObjectBase
{
public:

	string strBaseEntityName;
	CCopyEntityDesc CopyEntityDesc;

	vector<CCopyEntityDescFileData> vecChild;

	void Serialize( IArchive& ar, const unsigned int version );

};



//===============================================================================================
// CCopyEntityDescFileArchive
//===============================================================================================

class CCopyEntityDescFileArchive : public IArchiveObjectBase
{
	vector<CCopyEntityDescFileData> m_vecCopyEntityDesc;


public:

//	CCopyEntityDescFileArchive();
//	~CCopyEntityDescFileArchive();

	size_t GetNumEntityDescs() const { return m_vecCopyEntityDesc.size(); }
	CCopyEntityDescFileData& GetCopyEntityDesc( int index ) { return m_vecCopyEntityDesc[index]; }

	void AddCopyEntityDescData( CCopyEntityDescFileData& rDescData ) { m_vecCopyEntityDesc.push_back( rDescData ); }

	void Serialize( IArchive& ar, const unsigned int version );
};



#endif		/*  __COPYENTITYDESCFILEARCHIVE_H__  */