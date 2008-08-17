#ifndef __ARCHIVEBASE_H__
#define __ARCHIVEBASE_H__


#include <vector>
#include <map>
#include <string>
using namespace std;

#include <boost/shared_ptr.hpp>

#include <assert.h>

#include "ArchiveObjectFactory.h"


namespace GameLib1
{

namespace Serialization
{

#define BA_STRING_LENGTH	16
static const char s_acBinaryArchiveString[BA_STRING_LENGTH] = "_BINARYARCHIVE_";


/*
enum eArchiveFlag
{
	ARCHIVE_FLAG1 = (1 << 0),
	ARCHIVE_FLAG2 = (1 << 1),
	ARCHIVE_FLAG3 = (1 << 2),
	ARCHIVE_FLAG4 = (1 << 3),
};
*/


class IArchiveObjectBase;


class IArchive
{
protected:

	int m_Mode;

//	unsigned long m_UserDefinedArchiveID;

	/// primitive validation devise to prevent the loading of wrong archive files
	std::string m_strUserDefinedID;

public:

	enum eArchiveMode
	{
		MODE_OUTPUT,
		MODE_INPUT
	};

	virtual ~IArchive() {}

	int GetMode() const { return m_Mode; }

	///	archive object
	virtual IArchive& operator & (IArchiveObjectBase& rData) = 0;

	///	serialize the primitive data types
	virtual IArchive& operator & (int& nData)=0;
	virtual IArchive& operator & (unsigned int& nData)=0;
	virtual IArchive& operator & (bool& bData)=0;
	virtual IArchive& operator & (char& cData)=0;
	virtual IArchive& operator & (unsigned char& cData)=0;
	virtual IArchive& operator & (short& sData)=0;
	virtual IArchive& operator & (unsigned short& usData)=0;
	virtual IArchive& operator & (long& ulData)=0;
	virtual IArchive& operator & (unsigned long& ulData)=0;
	virtual IArchive& operator & (float& sData)=0;
	virtual IArchive& operator & (double& sData)=0;
	virtual IArchive& operator & (std::string& strData)=0;

	/* other operators

	template<class T>	IArchive& operator & (vector<T>& vecData)

	template<class T>	void Array( T* pData, const int iArraySize )

	/// NOTE: riArraySize argument is [in] for input archive and [out] for output archive
	template<class T>	void DArray( T*& pData, int& riArraySize )

	template<class T>	IArchive& PointerVector( vector<T *>& vecpData )

	template<class T>	void Polymorphic( T**& ppData, int iArraySize )

	template<class T>	void Polymorphic( vector<T *>& vecpData )

  */

	template<class T>
	IArchive& operator & (vector<T>& vecData)
	{
		size_t n,i;

		if( m_Mode == MODE_OUTPUT )
		{
			n = vecData.size();
			HandleData( &n, sizeof(size_t) );	// record array size
		}
		else // i.e. ( m_Mode == MODE_INPUT )
		{
			HandleData( &n, sizeof(size_t) );	// get array size
			vecData.resize( n );
		}

		for( i=0; i<n; i++ )
			(*this) & vecData[i];

		return (*this);
	}


	template<class K, class T>
	IArchive& operator & (map<K,T>& mapData)
	{
		size_t n,i;

		if( m_Mode == MODE_OUTPUT )
		{
			n = mapData.size();
			HandleData( &n, sizeof(size_t) );	// record array size

			typename map<K,T>::iterator itr;
			for( itr=mapData.begin(); itr!=mapData.end(); itr++ )
			{
				(*this) & const_cast<K&> (itr->first);
				(*this) & itr->second;
			}
		}
		else // i.e. ( m_Mode == MODE_INPUT )
		{
			HandleData( &n, sizeof(size_t) );	// get array size

			mapData.clear();
			for( i=0; i<n; i++ )
			{
				K key;
				T data;
				(*this) & key;
				(*this) & data;
				mapData[key] = data;
			}
		}

		return (*this);
	}

	
	template<class T>
	void Array( T* pData, const int iArraySize )
	{
		int i;
		if( 1 < iArraySize )
		{
			for( i=0; i<iArraySize; i++ )
				(*this) & pData[i];
		}
		else
			(*this) & (*pData);
	}

	template<class T>
	void DArray( T*& pData, int& riArraySize )
	{
		(*this) & riArraySize;	// save / load array size

		if( m_Mode == MODE_INPUT )
		{	// release & allocate memory
			if( pData )
			{
				if( 1 < riArraySize )
					delete [] pData;
				else
					delete pData;
			}

			if( 1 < riArraySize )
				pData = new T [riArraySize];
			else
				pData = new T;
		}

		int i;
		if( 1 < riArraySize )
		{
			for( i=0; i<riArraySize; i++ )
				(*this) & pData[i];
		}
		else
			(*this) & (*pData);
	}

	// each element of the vector must be a pointer to a single object, not a pointer to an array of objects
	template<class T>
	IArchive& PointerVector( vector<T *>& vecpData )
	{
		int n,i;

		if( m_Mode == MODE_OUTPUT )
		{
			n = vecpData.size();
			HandleData( &n, sizeof(int) );	// record array size
		}
		else // i.e. ( m_Mode == MODE_INPUT )
		{
			// release all the entries
			for( i=0; i<vecpData.size(); i++ )
			{
				if( vecpData[i] )
				{
					delete vecpData[i];
					vecpData[i] = NULL;
				}
			}

			HandleData( &n, sizeof(int) );	// get array size
			vecpData.resize( n );

			for( i=0; i<vecpData.size(); i++ )
				vecpData[i] = new T;
		}

		for( i=0; i<n; i++ )
			(*this) & (*vecpData[i]);

		return (*this);
	}


	template<class T>
	void Polymorphic( T**& ppData, int array_size, IArchiveObjectFactory& rFactory )
	{
		int i;
		unsigned int id;

		if( m_Mode == MODE_OUTPUT )
		{
//			(*this) & array_size;	// record array size

			for( i=0; i<array_size; i++ )
			{
				id = ppData[i]->GetArchiveObjectID();

				(*this) & id;	// record id for this object

				(*this) & (*ppData[i]);	// the object must override the Serialize function
			}
		}
		else // i.e. ( m_Mode == MODE_INPUT )
		{
//			(*this) & array_size;	// get array size

			for( i=0; i<array_size; i++ )
			{
				(*this) & id;	// get id for this object

				ppData[i] = (T *)rFactory.CreateObject(id);

				(*this) & (*ppData[i]);	// the object must override the Serialize function
			}
		}
	}


	template<class T>
	void Polymorphic( T*& data, IArchiveObjectFactory& rFactory )
	{
		unsigned int id;

		if( m_Mode == MODE_OUTPUT )
		{
			id = data->GetArchiveObjectID();

			(*this) & id;	// record id of this object

			(*this) & (*data);	// the object must override the Serialize() function
		}
		else // i.e. ( m_Mode == MODE_INPUT )
		{
			(*this) & id;	// get id of this object

//			assert( id != IArchiveObjectBase::INVALID_ID );

			data = (T *)rFactory.CreateObject(id);

			assert( data != NULL && "IArchive::Polymorphic() - unable to create an object for a polymorphic class");

			(*this) & (*data);	// the object must override the Serialize() function
		}
	}


	template<class T>
	void Polymorphic( vector<boost::shared_ptr<T>>& vecpData, IArchiveObjectFactory& rFactory )
	{
		size_t i, array_size = 0;
		unsigned int id;

		if( m_Mode == MODE_OUTPUT )
		{
			array_size = vecpData.size();

			(*this) & array_size;	// record array size

			for( i=0; i<array_size; i++ )
			{
				id = vecpData[i]->GetArchiveObjectID();

				(*this) & id;	// record id for this object

				(*this) & (*(vecpData[i].get()));	// the object must override the Serialize function
			}
		}
		else // i.e. ( m_Mode == MODE_INPUT )
		{
			(*this) & array_size;	// get array size

			vecpData.resize(array_size);

			for( i=0; i<array_size; i++ )
			{
				(*this) & id;	// get id for this object

				vecpData[i] = boost::shared_ptr<T>( dynamic_cast<T*>(rFactory.CreateObject(id)) );

				(*this) & (*(vecpData[i].get()));	// the object must override the Serialize function
			}
		}
	}


	template<class T>
	void Polymorphic( vector<T>& vecpData, IArchiveObjectFactory& rFactory )
	{
		size_t i, array_size = 0;
		unsigned int id;

		if( m_Mode == MODE_OUTPUT )
		{
			array_size = vecpData.size();

			(*this) & array_size;	// record array size

			for( i=0; i<array_size; i++ )
			{
				id = vecpData[i]->GetArchiveObjectID();

				(*this) & id;	// record id for this object

				(*this) & (*vecpData[i]);	// the object must override the Serialize function
			}
		}
		else // i.e. ( m_Mode == MODE_INPUT )
		{
			(*this) & array_size;	// get array size

			vecpData.resize(array_size);

			for( i=0; i<array_size; i++ )
			{
				(*this) & id;	// get id for this object

				vecpData[i] = dynamic_cast<T>(rFactory.CreateObject(id));

				(*this) & (*vecpData[i]);	// the object must override the Serialize function
			}
		}
	}

	virtual void HandleData( void *pData, const int size ) = 0;

};


}	/*  Serialization  */


}	/*  GameLib1  */


#endif  /*  __ARCHIVEBASE_H__  */
