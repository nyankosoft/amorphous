#include "BinaryArchive_Output.h"
#include "ArchiveObjectBase.h"

using namespace GameLib1::Serialization;

using namespace std;


/*CBinaryArchive_Output::CBinaryArchive_Output( const char *pcFilename,
											  const unsigned long archive_id,
											  unsigned int flag )*/

CBinaryArchive_Output::CBinaryArchive_Output( const string& filename,
											  const char *pStringID,
											  unsigned int flag )
{
	if( filename.length() == 0 )
		return;

	m_OutputFileStream.open( filename.c_str(), ios::out|ios::binary );

	m_Mode = MODE_OUTPUT;

	if( pStringID )
        m_strUserDefinedID = pStringID;
	else
		m_strUserDefinedID = "";
//		m_strUserDefinedID.clear();

}


bool CBinaryArchive_Output::operator<< ( IArchiveObjectBase& obj )
{
	if( !m_OutputFileStream.is_open() )
		return false;

	// write a predefined string so that an input archive can verify the data
	char c;
	for( int i=0; i<BA_STRING_LENGTH; i++ )
	{
		c = ~s_acBinaryArchiveString[i];
		HandleData( &c, sizeof(char) );
	}

	// write a user defined string for archive id
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
	m_OutputFileStream.write( (char *)pData, size );
}
