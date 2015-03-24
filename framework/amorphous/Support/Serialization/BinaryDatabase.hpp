#ifndef  __BinaryDatabase_HPP__
#define  __BinaryDatabase_HPP__


#include "Archive.hpp"
#include "ArchiveObjectBase.hpp"

#include "../Log/DefaultLog.hpp"


namespace amorphous
{

namespace serialization
{


inline bool is_db_filepath_and_keyname( const std::string& filepath )
{
	return ( filepath.find("::") != std::string::npos );
}


inline bool decompose_into_db_filepath_and_keyname( const std::string& src_filepath,
												    std::string& db_filepath,
												    std::string& keyname
												    )
{
	const size_t pos = src_filepath.find( "::" );
	if( pos == std::string::npos )
		return false;

	// found "::" in filename
	// - "(binary database filename)::(key)"
	db_filepath = src_filepath.substr( 0, pos );
	keyname     = src_filepath.substr( pos + 2, src_filepath.length() );

	return true;
}


template<class KeyType>
class CBinaryDatabase : public IArchive
{

	////// inner class definitions //////

	/// Header
	/// holds position to the key & data offset piar in the file
	class Header : public IArchiveObjectBase
	{
	public:
		long m_TableOffset;	// offset to the key & data offset pair table in the file

		Header() : m_TableOffset(0) {}

		virtual void Serialize( IArchive& ar, const unsigned int version )
		{
			ar & m_TableOffset;
		}
	};


	FILE *m_pFile;

	Header m_Header;

	/// holds pairs of key & data offset
	std::map<KeyType, long> m_KeyOffsetPair;

//	long m_CurrentDataOffset;

/*
	class DataHeader
	{
	public:
		int type_id;	// for type checking the object before serialization
	}*/

	void SetMode( int mode ) { m_Mode = mode; }

	inline long GetCurrentDataOffset() const { return m_Header.m_TableOffset; }

	inline void SetCurrentDataOffset( long offset ) { m_Header.m_TableOffset = offset; }

	bool KeyAlreadyExists( const KeyType& key )
	{
		typename std::map<KeyType, long>::const_iterator key_itr = m_KeyOffsetPair.find(key);
		if( key_itr != m_KeyOffsetPair.end() )
			return true;	// the specified key already exists in the map
		else
			return false;
	}

	bool SetReadPos( const KeyType& key )
	{
		typename std::map<KeyType, long>::const_iterator key_itr = m_KeyOffsetPair.find(key);
		if( key_itr == m_KeyOffsetPair.end() )
			return false;	// the specified key was not found in the map

		SetMode( MODE_INPUT );

		// set the file pointer to the data position
		fseek( m_pFile, key_itr->second, SEEK_SET );

		return true;
	}

	bool SetWritePos( const KeyType& key )
	{
		if( KeyAlreadyExists(key) )
			return false;

		SetMode( MODE_OUTPUT );

		m_KeyOffsetPair[key] = GetCurrentDataOffset();

		// set the file pointer to the data position
		fseek( m_pFile, GetCurrentDataOffset(), SEEK_SET );

		return true;
	}

	// called right after the data has been written to the disk
	void UpdateWritePos( const KeyType& key )
	{
		SetCurrentDataOffset( ftell(m_pFile) );
	}


public:

	enum eDBMode
	{
		DB_MODE_APPEND,	// reads data from / appends data to an existing database
		DB_MODE_NEW,	// creates a new database. deletes the existing file if there is one
	};

//	CBinaryDatabase();
//	~CBinaryDatabase();
//	bool Open( const char *pFilename );
//	void Close();
//	void AddData( KeyType& key, T& obj );
//	void AddPolymorphicData;

	CBinaryDatabase()
	:
	m_pFile(nullptr)
//	m_CurrentDataOffset(0)
	{
	}


	~CBinaryDatabase()
	{
		Close();
	}


	bool Open( std::string filename, int mode = DB_MODE_APPEND )
	{
		if( m_pFile )
			return false;

		if( mode == DB_MODE_NEW )
		{
			// deletes the old one and create an empty file
			FILE *fp = fopen( filename.c_str(), "wb" );

			if( !fp )
			{
				LOG_PRINT_ERROR( std::string(" - Cannot open / create a file '") + filename + "' in the binary writing mode. Could be an invalid directory path." );
				return false;
			}

			fclose(fp);
		}

		m_pFile = fopen( filename.c_str(), "rb+" );

		if( !m_pFile )
			return false;

		char byte;
		if( fread( &byte, sizeof(char), 1, m_pFile ) == 0 )
		{
			SetCurrentDataOffset( sizeof(Header) );
			return true;	// opened a new file
		}
		else
		{
			// opened an existing database file - load the header and key&offest pair

			// reset the file pointer position to the head of the file
			fseek( m_pFile, 0, SEEK_SET );

			SetMode( MODE_INPUT );

			// load the header
			(*this) & m_Header;

			// load key & offset pairs
			fseek( m_pFile, GetCurrentDataOffset(), SEEK_SET );

//			(*this) & m_KeyOffsetPair;
			IArchive::operator &( m_KeyOffsetPair );

			return true;
		}
	}


	void Close()
	{
		if( !m_pFile )
		{
//			assert( !"CBinaryDatabase::Close() - caution: the redundant Close() calls has been made." );
			return;
		}

		SetMode( MODE_OUTPUT );

		// update the header
		fseek( m_pFile, 0, SEEK_SET );
		(*this) & m_Header;

		// save key & data pair to the file
		fseek( m_pFile, GetCurrentDataOffset(), SEEK_SET );
//		(*this) & m_KeyOffsetPair;
		IArchive::operator &( m_KeyOffsetPair );

		fclose( m_pFile );
		m_pFile = nullptr;
	}

	bool KeyExists( const KeyType& key )
	{
		return KeyAlreadyExists( key );
	}


	///// serialization operators /////

	///	serialize the primitive data types
	virtual IArchive& operator & (int& nData)			{ HandleData( &nData, sizeof(int) ); return (*this); }
	virtual IArchive& operator & (unsigned int& nData)	{ HandleData( &nData, sizeof(unsigned int) ); return (*this); }
	virtual IArchive& operator & (bool& bData)			{ HandleData( &bData, sizeof(bool) ); return (*this); }
	virtual IArchive& operator & (char& cData)			{ HandleData( &cData, sizeof(char) ); return (*this); }
	virtual IArchive& operator & (unsigned char& cData)	{ HandleData( &cData, sizeof(unsigned char) ); return (*this); }
	virtual IArchive& operator & (short& sData)			{ HandleData( &sData, sizeof(short) ); return (*this); }
	virtual IArchive& operator & (unsigned short& usData) { HandleData( &usData, sizeof(unsigned short) ); return (*this); }
	virtual IArchive& operator & (long& lData)			{ HandleData( &lData, sizeof(long) ); return (*this); }
	virtual IArchive& operator & (unsigned long& ulData) { HandleData( &ulData, sizeof(unsigned long) ); return (*this); }
	virtual IArchive& operator & (float& fData)			{ HandleData( &fData, sizeof(float) ); return (*this); }
	virtual IArchive& operator & (double& dData)		{ HandleData( &dData, sizeof(double) ); return (*this); }

	virtual IArchive& operator & (std::string& strData)
	{
		int i, num_chars;

		if( GetMode() == MODE_OUTPUT )
		{
			// record the string length
			num_chars = (int)strData.size();
			HandleData( &num_chars, sizeof(int) );
		}
		else // i.e. GetMode() == MODE_INPUT
		{
			// read the string length
			HandleData( &num_chars, sizeof(int) );
			strData.resize(num_chars);
		}

		// read/write the string
        for( i=0; i<num_chars; i++ )
			HandleData( &strData[i], sizeof(char) );

		return (*this);
	}

	virtual IArchive& operator & (IArchiveObjectBase& rData)
	{
		unsigned int uiVersion;
			
		if( GetMode() == MODE_OUTPUT )
		{
			// get the current version of this class
			uiVersion = rData.GetVersion();

			// record the version info   
			HandleData( &uiVersion, sizeof(int) );
		}
		else // i.e. GetMode() == MODE_INPUT
		{
			// read the version info   
			HandleData( &uiVersion, sizeof(int) );
		}

		rData.Serialize(*this, uiVersion);

		return (*this);
	}

	template<class T>
	bool AddData( const KeyType& key, T& obj )
	{
		if( !SetWritePos(key) )
			return false;

		(*this) & obj;

		UpdateWritePos(key);
		return true;
	}

	template<class T>
	bool AddData( const KeyType& key, std::vector<T>& vecObject )
	{
		if( !SetWritePos(key) )
			return false;

//		(this*) & vecObject;
		IArchive::operator &( vecObject );

		UpdateWritePos(key);
		return true;
	}

	template<class T>
	bool AddPolymorphicData( const KeyType& key, T*& pObject, IArchiveObjectFactory& rFactory )
	{
		if( !SetWritePos(key) )
			return false;

//		(this*).Polymorphic( pObject, rFactory );
		this->Polymorphic( pObject, rFactory );

		UpdateWritePos(key);
		return true;
	}

	template<class T>
	bool AddPolymorphicData( const KeyType& key, std::vector<T>& vecpObject, IArchiveObjectFactory& rFactory )
	{
		if( !SetWritePos(key) )
			return false;

		this->Polymorphic( vecpObject, rFactory );

		UpdateWritePos(key);
		return true;
	}


	template<class T>
	bool GetData( const KeyType& key, T& obj )
	{
		if( !SetReadPos(key) )
			return false;

		(*this) & obj;
		return true;
	}

	template<class T>
	bool GetData( const KeyType& key, std::vector<T>& vecObject )
	{
		if( !SetReadPos(key) )
			return false;

		IArchive::operator &( vecObject );
		return true;
	}


	template<class T>
	bool GetPolymorphicData( const KeyType& key, T*& pObject, IArchiveObjectFactory& rFactory )
	{
		if( !SetReadPos(key) )
			return false;

		(*this).Polymorphic( pObject, rFactory );

		return true;
	}


	template<class T>
	bool GetPolymorphicData( const KeyType& key, std::vector<T>& vecpObject, IArchiveObjectFactory& rFactory )
	{
		if( !SetReadPos(key) )
			return false;

		(*this).Polymorphic( vecpObject, rFactory );

		return true;
	}


	virtual void HandleData( void *pData, const int size )
	{
		switch( GetMode() )
		{
		case MODE_OUTPUT:
			fwrite( pData, size, 1, m_pFile );
			break;

		case MODE_INPUT:
			fread( pData, size, 1, m_pFile );
			break;

		default:
			assert( !"CBinaryDatabase::HandleData() - an invalid mode detected" );
			break;
		}
	}
};

} // namespace serialization

} // namespace amorphous


#endif		/*  __BinaryDatabase_HPP__  */
