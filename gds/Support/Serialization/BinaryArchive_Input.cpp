#include "BinaryArchive_Input.hpp"
#include "ArchiveObjectBase.hpp"

using namespace GameLib1::Serialization;

using namespace std;

/*
CBinaryArchive_Input::CBinaryArchive_Input( const char *pcFilename,
											const unsigned long archive_id,
										    unsigned int flag )*/
CBinaryArchive_Input::CBinaryArchive_Input( const string& filename,
											const char *pStringID,
										    unsigned int flag )
{
	m_InputFileStream.open( filename.c_str(), ios::in|ios::binary );

	m_Mode = MODE_INPUT;

	if( pStringID )
        m_strUserDefinedID = pStringID;
	else
		m_strUserDefinedID = "";
//		m_strUserDefinedID.clear();

}


static bool s_bOldArchive = false;

//#define __ARCHIVE_VERSION_2__


bool CBinaryArchive_Input::operator>> ( IArchiveObjectBase& obj )
{
	if( !m_InputFileStream.is_open() )
		return false;

	// test if the beginning part of the archive is the valid predifined id string
	char c, acStr[BA_STRING_LENGTH];
	for( int i=0; i<BA_STRING_LENGTH; i++ )
	{
		HandleData( &c, sizeof(char) );
		acStr[i] = ~c;
	}
	if( strcmp(s_acBinaryArchiveString,acStr) != 0 )
	{
		return false;	// invalid archive

		// load as an old archive
/*		streampos pos = m_InputFileStream.tellg();
		pos -= BA_STRING_LENGTH;
		m_InputFileStream.seekg( pos );
		s_bOldArchive = true;
		(*this) & obj;
		return true;*/
	}

	s_bOldArchive = false;

#ifdef  __ARCHIVE_VERSION_2__

	// test if the archive has the same id defined by the user
	unsigned long id;
	HandleData( &id, sizeof(long) );
//	if( id != m_UserDefinedArchiveID )
//		return false;	// user defined IDs does not match

#else

	string strID;
	size_t len = m_strUserDefinedID.length();
	if( 0 < len )
	{
		strID.resize(len+1);
		for( size_t i=0; i<len+1; i++ )
		{
			HandleData( &c, sizeof(char) );
			strID[i] = ~c;
		}
	}

	if( m_strUserDefinedID != strID )
		return false;

#endif  /*  __ARCHIVE_VERSION_2__  */

	// load binary archive data
	(*this) & obj;


	return true;
}


IArchive& CBinaryArchive_Input::operator & (IArchiveObjectBase& rData)
{
	unsigned int uiVersion = 0;

	if( !s_bOldArchive )					///
	{										///
	// read the version info
	HandleData( &uiVersion, sizeof(int) );
	}										///

	rData.Serialize( *this, uiVersion );

	return (*this);
}


//	--- for loading primitive data

IArchive& CBinaryArchive_Input::operator & (int& nData)
{
	HandleData( &nData, sizeof(int) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (unsigned int& nData)
{
	HandleData( &nData, sizeof(unsigned int) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (short& sData)
{
	HandleData( &sData, sizeof(short) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (unsigned short& usData)
{
	HandleData( &usData, sizeof(unsigned short) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (char& cData)
{
	HandleData( &cData, sizeof(char) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (unsigned char& cData)
{
	HandleData( &cData, sizeof(unsigned char) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (bool& bData)
{
	HandleData( &bData, sizeof(bool) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (long& ulData)
{
	HandleData( &ulData, sizeof(long) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (unsigned long& ulData)
{
	HandleData( &ulData, sizeof(unsigned long) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (float& fData)
{
	HandleData( &fData, sizeof(float) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (double& fData)
{
	HandleData( &fData, sizeof(double) );
	return (*this);
}


IArchive& CBinaryArchive_Input::operator & (string& strData)
{
	int i, iNumChars;
	HandleData( &iNumChars, sizeof(int) );

	string strTemp;
	strTemp.resize(iNumChars);
	char c;
	for( i=0; i<iNumChars; i++ )
	{
		HandleData( &c, sizeof(char) );
		strTemp[i] = c;
	}

	strData = strTemp;

	return (*this);
}


void CBinaryArchive_Input::HandleData( void *pData, const int size )
{
	m_InputFileStream.read( (char *)pData, size);
}
