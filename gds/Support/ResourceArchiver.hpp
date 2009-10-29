#ifndef  __ResourceArchiver_HPP__
#define  __ResourceArchiver_HPP__


#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "../base.hpp"
#include "Support/TextFileScanner.hpp"
#include "Support/SerializableStream.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
#include "Support/ImageArchive.hpp"


using namespace GameLib1::Serialization;



typedef uint timestamp_type;


class CResourceArchiverUnit
{
public:

	CResourceArchiverUnit() {}

	virtual ~CResourceArchiverUnit() {}

	/// Return Result::SUCCESS to indicate that the file has been added to the db
	/// Return any error code to indicate that the file has NOT been added to the db 
	virtual Result::Name AddFileToDB( const std::string& keyname, const boost::filesystem::path& input_path, CBinaryDatabase<std::string>& db ) = 0;
};

/*
#include "LightWave.hpp"

class CLWArchiverUnit : public CResourceArchiverUnit
{
public:

	Result::Name AddFileToDB( const boost::filesystem::path& input_path, CBinaryDatabase<std::string>& db )
	{
		using namespace std;
		using namespace MeshModel;

		string ext = input_path.extension();

		if( ext == ".lwo" )
		{
			int num_meshes = 0;
			for( int i=0; i<num_meshes; i++ )
				db.AddData( mesh_archive );
		}
	}
};
*/

class CImageArchiverUnit : public CResourceArchiverUnit
{
public:

	Result::Name AddFileToDB( const std::string& keyname, const boost::filesystem::path& input_path, CBinaryDatabase<std::string>& db )
	{
		using namespace std;

		string ext = input_path.extension();
		boost::algorithm::to_lower( ext );

		if( ext == ".bmp"
		 || ext == ".jpg"
		 || ext == ".png"
		 || ext == ".tga" )
		{
			CImageArchive img_archive( input_path.string() );
			bool added = db.AddData( keyname, img_archive );

			return Result::SUCCESS;
		}
		else
			return Result::INVALID_ARGS;
	}
};


class CLastTimestampInfo
{
public:

//	std::string m_DBFilepath;

	std::map< std::string, timestamp_type > m_mapFilepathToModTime;
};


class CFilepathAndTimestamp
{
public:
	std::map< std::string, CLastTimestampInfo > m_mapDBFilepathToTimestampInfo;
};


/**
 - Creates an image archive from an image file
   - Uses an FreeImage library to retrieve the width and height of the image


 - Usage:

source code
----------------------------------
void some_function()
{
	CResourceArchiver archiver;
	archiver.AddArchiverUnit( shared_ptr<CImageArchiverUnit>( new CImageArchiverUnit ) );
	archiver.CreateDBs( "desc" );
}


content of file 'desc' (text file)
----------------------------------
input  image1.jpg
input  image2.jpg
input  image3.jpg
output images.bin
close

*/
class CResourceArchiver
{
	typedef std::pair<std::string,boost::filesystem::path> keyname_and_filepath;

	bool NeedToUpdateDB( const std::vector<keyname_and_filepath>& vecInputFilepath, const std::string& output_filepath );

	Result::Name AddData( CBinaryDatabase<std::string>& db, const std::string& keyname, const boost::filesystem::path& input_path );

	Result::Name CreateDB( const std::vector<keyname_and_filepath>& vecInputFilepath, const std::string& output_filepath );

public:

//	std::string m_OutputFilepath;

	std::vector< boost::shared_ptr<CResourceArchiverUnit> > m_vecpArchiverUnit;

	CFilepathAndTimestamp m_LastBuildInfo;

public:

	inline CResourceArchiver();

	inline Result::Name CreateDBs( const std::string& filepath );

	void AddArchiverUnit( boost::shared_ptr<CResourceArchiverUnit> pUnit ) { m_vecpArchiverUnit.push_back( pUnit ); }

	/// Client does not have to release the memory
	void AddArchiverUnit( CResourceArchiverUnit *pUnit ) { m_vecpArchiverUnit.push_back( boost::shared_ptr<CResourceArchiverUnit>( pUnit ) ); }

//	inline CResourceArchiver( const std::string& filepath );

	/// returns true on success
//	inline bool SetFormatFromFileExtension( const std::string& image_ext );
//	inline bool IsValid();
//	inline virtual void Serialize( IArchive& ar, const unsigned int version );
};


// ============================ inline implementations ============================

inline CResourceArchiver::CResourceArchiver()
{}

/*
inline CResourceArchiver::CResourceArchiver( const std::string& filepath )
{
	// get the suffix from the filename
//	size_t dot_pos = image_filename.rfind(".");
//	if( dot_pos == std::string::npos )
//		return;

	// load image data to buffer
//	CSerializableStream::LoadBinaryStream(image_filename);
}


//inline bool CResourceArchiver::IsValid(){}
inline void CResourceArchiver::Serialize( IArchive& ar, const unsigned int version )
{
	CSerializableStream::Serialize( ar, version );

	ar & (int&)m_Format;

}
*/

inline Result::Name CResourceArchiver::AddData( CBinaryDatabase<std::string>& db, const std::string& keyname, const boost::filesystem::path& input_path )
{
	for( size_t i=0; i<m_vecpArchiverUnit.size(); i++ )
	{
		Result::Name ret = m_vecpArchiverUnit[i]->AddFileToDB( keyname, input_path, db );
		if( ret == Result::SUCCESS )
			return Result::SUCCESS;
	}

	return Result::UNKNOWN_ERROR;
}


bool CResourceArchiver::NeedToUpdateDB( const std::vector<keyname_and_filepath>& vecInputFilepath, const std::string& output_filepath )
{
	return true;
/*
	using namespace std;
	using namespace boost;
	using namespace boost::filesystem;

	map< string, CLastTimestampInfo > map_info

	if( m_LastBuildInfo.m_mapDBFilepathToTimestampInfo.empty() )
		return true; // no last build info

	map< string, CLastTimestampInfo >::iterator itr = m_LastBuildInfo.m_mapDBFilepathToTimestampInfo.find( output_filepath );
	if( itr == m_mapDBFilepathToTimestampInfo.end() )
		return true; // new db

	const CLastTimestampInfo& timestamp_info = itr->second;

	map< string, timestamp_type >::iterator itrToTimestamp;

	// See if all the files in the previous db are listed in the new list.
	// If not, the client decided to remove file(s) from the db and db needs to be rebuilt
	for( itrToTimestamp = timestamp_info.m_mapFilepathToModTime.begin();
		 itrToTimestamp != timestamp_info.m_mapFilepathToModTime.end();
		 itrToTimestamp++ )
	{
		const string path_in_prev_db = itrToTimestamp->first.string();

		vector<path>::iterator itrPath
			= std::find( vecInputFilepath.begin(), vecInputFilepath.end(), path_in_prev_db );

		if( itrPath == vecInputFilepath.end() )
			return true; // A file has been removed from the list
	}


	for( size_t i=0; i<vecInputFilepath.size(); i++ )
	{
		itrToTimestamp
			= timestamp_info.m_mapFilepathToModTime.find( vecInputFilepath[i] );

		if( itrToTimestamp == timestamp_info.m_mapFilepathToModTime.end() )
			return true;

		if( itrToTimestamp->second != vecInputFilepath[i].get_last_mod_time() )
			return true;
	}

	return false;*/
}


/// \param vecInputFilepath [in] list that contains pairs of keyname & path of the file you want to save to database
Result::Name CResourceArchiver::CreateDB( const std::vector<keyname_and_filepath>& vecInputFilepath, const std::string& output_filepath )
{
	CBinaryDatabase<std::string> db;

	if( !NeedToUpdateDB( vecInputFilepath, output_filepath ) )
	{
		return Result::SUCCESS;
	}

	bool res = db.Open( output_filepath, CBinaryDatabase<std::string>::DB_MODE_NEW );
	if( !res )
	{
		return Result::UNKNOWN_ERROR;
	}

	for( size_t i=0; i<vecInputFilepath.size(); i++ )
	{
		AddData( db, vecInputFilepath[i].first, vecInputFilepath[i].second );
	}

	return Result::SUCCESS;
}


Result::Name CResourceArchiver::CreateDBs( const std::string& desc_filepath )
{
	using namespace std;
	using namespace boost;
	using namespace boost::filesystem;

	CTextFileScanner scanner( desc_filepath );

	if( !scanner.IsReady() )
	{
//		printf( "Failed to open file: %s\n", filepath.c_str() );
		return Result::UNKNOWN_ERROR;
	}

	path descfile_directory = path(desc_filepath).parent_path();

	// open the last build info for this desc file if there is one
//	m_LastBuildInfo.LoadFromFile( desc_filepath + ".lbi" );

	string line;
	string input_path, output_filepath;
	vector<keyname_and_filepath> vecInputFilepath;
	for( ; !scanner.End(); scanner.NextLine() )
	{
		string tag = scanner.GetTagString();
		scanner.GetCurrentLine( line );
//		sscanf( line.c_str(), "%d", &id );

		if( scanner.TryScanLine( "input", input_path ) )
		{
			vecInputFilepath.push_back( keyname_and_filepath( input_path, descfile_directory / input_path ) );
			continue;
		}

		if( scanner.TryScanLine( "output", output_filepath ) )
			continue;

		if( tag == "close" )
		{
			if( 0 < vecInputFilepath.size()
			 && 0 < output_filepath.length() )
			{
				CreateDB( vecInputFilepath, output_filepath );
			}

			vecInputFilepath.resize( 0 );
			output_filepath = "";
		}
	}

	return Result::SUCCESS;
}

/*
#include "Support/ResourceArchiver.hpp"

void some_function()
{
	CResourceArchiver archiver;
	archiver.AddArchiverUnit( shared_ptr<CImageArchiverUnit>( new CImageArchiverUnit ) );
//	archiver.CreateDBs( "../resources/resources" );
	archiver.CreateDBs( "../resources/images/ss_imgs" );
}
*/



#endif  /*  __ResourceArchiver_HPP__  */
