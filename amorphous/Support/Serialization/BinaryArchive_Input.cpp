#include "BinaryArchive_Input.hpp"
#include "ArchiveObjectBase.hpp"
#include "../Log/DefaultLog.hpp"

using namespace amorphous::serialization;

using namespace std;


BinaryArchive_Input::BinaryArchive_Input( const string& filename,
											const char *pStringID,
										    unsigned int flag )
{
	m_InputFileStream.open( filename.c_str(), ios::in|ios::binary );

	if( !m_InputFileStream.is_open() )
		LOG_PRINT_WARNING( " Failed to open the input file stream: " + filename );

	m_Mode = MODE_INPUT;

	/// Read from the stream later.
	m_OptionFlags = 0;

	if( pStringID )
        m_strUserDefinedID = pStringID;
	else
		m_strUserDefinedID = "";
//		m_strUserDefinedID.clear();

}


bool BinaryArchive_Input::operator>> ( IArchiveObjectBase& obj )
{
	if( !m_InputFileStream.is_open() )
		return false;

	// test if the beginning part of the archive is the valid predifined id string
	const int archive_string_data_length
		= strlen(sg_pBinaryArchiveIdentifierString)
		+ strlen(sg_pBinaryArchiveVersionString)
		+ 1; // The terminating null character is included in the archived data
	char c = 0;
	vector<char> archive_string_data;
	archive_string_data.resize( archive_string_data_length, 0 );
	for( int i=0; i<archive_string_data_length; i++ )
	{
		HandleData( &c, sizeof(char) );
		archive_string_data[i] = ~c;
	}

	// The last character should be a NULL character
	if( !archive_string_data.back() == 0 )
		return false;

	int archive_ver = 0;
	string archive_string = &archive_string_data[0];
	string bin_archive_name = archive_string.substr(0, strlen(sg_pBinaryArchiveIdentifierString));
	if( bin_archive_name == sg_pBinaryArchiveIdentifierString )
	{
		// Version 01 or higher
		archive_ver = atoi( archive_string.substr(strlen(sg_pBinaryArchiveIdentifierString)).c_str() );
	}
	else
	{
		// The oldest version of an archive that has "_BINARYARCHIVE_" as a verification string,
		// or not a binary archive file.
		const char *original_archive_string = "_BINARYARCHIVE_";
		if( archive_string == original_archive_string )
			archive_ver = 0;
		else
			return false;	// An invalid archive
	}

	// Load the archive option flags if 1 <= archive version
	if( 1 <= archive_ver )
		(*this) & m_OptionFlags;

	string strID;
	size_t len = m_strUserDefinedID.length();
	if( 0 < len )
	{
		strID.resize(len+1,0);
		for( size_t i=0; i<len+1; i++ )
		{
			HandleData( &c, sizeof(char) );
			strID[i] = ~c;
		}
	}

	if( m_strUserDefinedID != strID )
		return false;

	// load binary archive data
	(*this) & obj;


	return true;
}


IArchive& BinaryArchive_Input::operator & (IArchiveObjectBase& rData)
{
	unsigned int uiVersion = 0;

	// read the version info
	HandleData( &uiVersion, sizeof(int) );

	rData.Serialize( *this, uiVersion );

	return (*this);
}


//	--- for loading primitive data

IArchive& BinaryArchive_Input::operator & (int& nData)
{
	HandleData( &nData, sizeof(int) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (unsigned int& nData)
{
	HandleData( &nData, sizeof(unsigned int) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (short& sData)
{
	HandleData( &sData, sizeof(short) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (unsigned short& usData)
{
	HandleData( &usData, sizeof(unsigned short) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (char& cData)
{
	HandleData( &cData, sizeof(char) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (unsigned char& cData)
{
	HandleData( &cData, sizeof(unsigned char) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (bool& bData)
{
	HandleData( &bData, sizeof(bool) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (long& ulData)
{
	HandleData( &ulData, sizeof(long) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (unsigned long& ulData)
{
	HandleData( &ulData, sizeof(unsigned long) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (float& fData)
{
	HandleData( &fData, sizeof(float) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (double& fData)
{
	HandleData( &fData, sizeof(double) );
	return (*this);
}


IArchive& BinaryArchive_Input::operator & (string& strData)
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


void BinaryArchive_Input::HandleData( void *pData, const int size )
{
	if( m_OptionFlags & ArchiveOptionFlags::AOF_OBFUSCATE )
	{
		char read = 0;
		for( int i=0; i<size; i++ )
		{
			m_InputFileStream.read( &read, 1 );
			read = read ^ ms_ObfucationBits[m_ObfucationBitIndex];
			m_ObfucationBitIndex = (m_ObfucationBitIndex+1) % NUM_OBFUSCATION_BYTES;
			((char *)pData)[i] = read;
		}
	}
	else
		m_InputFileStream.read( (char *)pData, size);
}
