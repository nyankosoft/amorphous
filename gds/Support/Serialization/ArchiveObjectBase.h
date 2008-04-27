
#ifndef __ARCHIVEOBJECTBASE_H__
#define __ARCHIVEOBJECTBASE_H__


#include <string>


namespace GameLib1
{

namespace Serialization
{


class IArchive;


class IArchiveObjectBase
{

/**
	base class for archive objects
*/
protected:

//	int m_iArchiveObjectID;

public:

	enum eDefaultID
	{
		INVALID_ID = -1,
	};

	IArchiveObjectBase() {}

	virtual ~IArchiveObjectBase() {} // place holder

	/// classes that use serialization have to inherit this function
	virtual void Serialize( IArchive& ar, const unsigned int version ) = 0;


	/// for polymorphic classes
//	void SetArchiveObjectID( unsigned int id ) { m_iArchiveObjectID = id; }

	virtual unsigned int GetArchiveObjectID() const { return INVALID_ID; }

//	virtual void Save( IArchive&, unsigned int version ) {}
//	virtual void Load( IArchive&, unsigned int version ) {}

	virtual unsigned int GetVersion() const { return 0; }

	bool SaveToFile( const std::string& filename );

	bool LoadFromFile( const std::string& filename );

	bool SaveToCompressedFile( const std::string& filename );

	bool LoadFromCompressedFile( const std::string& filename );

/**

========== sample 01 ( primitive types and thier vectors )==========

class CArchiveObjectClass : public IArchiveObjectBase
{
	int m_iData;
	vector<int> m_veciData;
	string m_strData;
	vector<COtherArchiveObject> m_vecData;

	void Serialize( IArchive& ar, const unsigned int version )
	{
		// enumerate all the member variables to serialize
		ar & m_iData;
		ar & m_veciData;
		ar & m_strData;
		ar & m_vecData;
	}
};

============== sample 03 ( derived classes )==========

class CBaseClass : public IArchiveObjectBase
{
	int m_iData;

public:

	/// override GetArchiveObjectID() and make it return user defined id for the derived class
	virtual unsigned int GetArchiveObjectID() const { return 0; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_iData;
	}

	enum { DERIVED_0, DERIVED_1 }
};


class CDerived0 : public CBaseClass
{
	string m_strData;
public:

	CDerived0() {}

	/// override GetArchiveObjectID() and make it return user defined id for the derived class
	virtual unsigned int GetArchiveObjectID() const { return CBaseClass::DERIVED_0; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		// do the serializations of the base class first
		CBaseClass::Serialize(ar,version);

		ar & m_strData;
	}
}


class CFactory : public IArchiveObjectFactory
{
public:

	virtual IArchiveObjectBase *CreateObject( const unsigned int id )
	{
		switch( id )
		{
		case CBaseClass::DERIVED_0:	return new CDerived0();
		case CBaseClass::DERIVED_1:	return new CDerived1();
		default: return NULL;
		}
	}
};



============== sample 04 ( dealing with added member variables )==========


class CArchiveObjectClass : public IArchiveObjectBase
{
public:

	int m_iData;
	vector<int> m_veciData;	// newly added member variable

	// 1. override GetVersion() and set the version above 0,
	//    or any value greater than previous version if you have already using GetVersion()
	virtual unsigned int GetVersion() const { return 1; }

	// 2. serialize the newly added variable if its version is greater than
	//    or equal to the version you set in GetVersion()
	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_iData;

		if( 1 <= version )
        	ar & m_veciData;
	}

	// version is set for each class. For example, the version set above does not affect
	// version of any ohter classes
	// TODO: what about base & derived classes?
};


*/
};


}  /*  Serialization  */


}  /*  GameLib1  */


#endif  /*  __ARCHIVEOBJECTBASE_H__  */
