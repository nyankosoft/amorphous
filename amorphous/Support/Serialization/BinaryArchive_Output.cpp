#include "BinaryArchive_Output.hpp"
#include "ArchiveObjectBase.hpp"

using namespace amorphous::serialization;

using namespace std;


/*CBinaryArchive_Output::CBinaryArchive_Output( const char *pcFilename,
											  const unsigned long archive_id,
											  unsigned int flag )*/

CBinaryArchive_Output::CBinaryArchive_Output( const string& filename,
											  const char *pStringID,
											  unsigned int archive_option_flags )
{
	if( filename.length() == 0 )
		return;

	m_OutputFileStream.open( filename.c_str(), ios::out|ios::binary );

	m_Mode = MODE_OUTPUT;

	m_OptionFlags = archive_option_flags;

	if( pStringID )
        m_strUserDefinedID = pStringID;
	else
		m_strUserDefinedID = "";
//		m_strUserDefinedID.clear();

}


// Output binary archive
// The following content is saved to a binary achive.
// 1. The binary archive string ("BinaryArchive??", where ?? is a version number)
// 2. Option flags (a single unsigned int type value)
// 3. (Optional) a user defined string
// 4. Archive content
bool CBinaryArchive_Output::operator<< ( IArchiveObjectBase& obj )
{
	if( !m_OutputFileStream.is_open() )
		return false;

	// write a predefined string so that an input archive can verify the data
	const int archive_string_data_length
		= strlen(sg_pBinaryArchiveIdentifierString)
		+ strlen(sg_pBinaryArchiveVersionString)
		+ 1;
	char c = 0;
	char archive_string[0xFF];
	memset( archive_string, 0, sizeof(archive_string) );
	strcat( archive_string, sg_pBinaryArchiveIdentifierString);
	strcat( archive_string, sg_pBinaryArchiveVersionString);
	for( int i=0; i<archive_string_data_length; i++ )
	{
		c = ~archive_string[i];
		HandleData( &c, sizeof(char) );
	}

	// Save the archive option flags
	(*this) & m_OptionFlags;

	// Write a user defined string for archive id
	size_t len = m_strUserDefinedID.length();
	if( 0 < len )
	{
		for( size_t i=0; i<len+1; i++ )
		{
			c = ~m_strUserDefinedID[i];
			HandleData( &c, sizeof(char) );
		}
	}

	(*this) & obj;

//	m_OutputFileStream.close();

	return true;
}


IArchive& CBinaryArchive_Output::operator & (IArchiveObjectBase& rData)
{
	// get the current version of this class
	unsigned int uiVersion = rData.GetVersion();

    // record the version info   
	HandleData( &uiVersion, sizeof(int) );

	rData.Serialize(*this, uiVersion);

	return (*this);
}


//	--- for loading primitive data

IArchive& CBinaryArchive_Output::operator & (int& nData)
{
	HandleData( &nData, sizeof(int) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (unsigned int& nData)
{
	HandleData( &nData, sizeof(unsigned int) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (short& sData)
{
	HandleData( &sData, sizeof(short) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (unsigned short& usData)
{
	HandleData( &usData, sizeof(unsigned short) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (char& cData)
{
	HandleData( &cData, sizeof(char) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (unsigned char& cData)
{
	HandleData( &cData, sizeof(unsigned char) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (bool& bData)
{
	HandleData( &bData, sizeof(bool) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (long& ulData)
{
	HandleData( &ulData, sizeof(long) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (unsigned long& ulData)
{
	HandleData( &ulData, sizeof(unsigned long) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (float& fData)
{
	HandleData( &fData, sizeof(float) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (double& fData)
{
	HandleData( &fData, sizeof(double) );
	return (*this);
}


IArchive& CBinaryArchive_Output::operator & (string& strData)
{
	size_t i, num_chars = strData.size();
	HandleData( &num_chars, sizeof(int) );
	for( i=0; i<num_chars; i++ )
		HandleData( &strData[i], sizeof(char) );

	return (*this);
}


void CBinaryArchive_Output::HandleData( void *pData, const int size )
{
	if( m_OptionFlags & ArchiveOptionFlags::AOF_OBFUSCATE )
	{
		char to_write = 0;
		for( int i=0; i<size; i++ )
		{
			to_write = ((char *)pData)[i] ^ ms_ObfucationBits[m_ObfucationBitIndex];
			m_OutputFileStream.write( &to_write, 1 );
			m_ObfucationBitIndex = (m_ObfucationBitIndex+1) % NUM_OBFUSCATION_BYTES;
		}
	}
	else
		m_OutputFileStream.write( (char *)pData, size );
}
