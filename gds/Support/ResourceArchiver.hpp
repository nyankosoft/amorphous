#ifndef  __ResourceArchiver_HPP__
#define  __ResourceArchiver_HPP__


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


class CResourceArchiverUnitOutput
{
public:

	/// Files detected during the compilation and added as targets of compilation.
	/// maps of input filepath -> output filepath
	std::map<std::string,std::string> m_ExtraTargets;

	void Reset()
	{
		m_ExtraTargets.clear();
	}
};


class CResourceArchiverUnit
{
public:

	CResourceArchiverUnit() {}

	virtual ~CResourceArchiverUnit() {}

	/// Return Result::SUCCESS to indicate that the file has been added to the db
	/// Return any error code to indicate that the file has NOT been added to the db 
	virtual Result::Name AddFileToDB( const std::string& keyname, const boost::filesystem::path& input_path, CBinaryDatabase<std::string>& db ) { return Result::SUCCESS; }

//	virtual Result::Name CreateArchive( const std::string& input_filepath, boost::shared_ptr<IArchiveObjectBase>& pArchive ) { return Result::SUCCESS; }
	virtual Result::Name CreateArchive( const std::string& input_filepath, const std::string& output_filepath, CResourceArchiverUnitOutput& out ) { return Result::SUCCESS; }
};


class CImageArchiverUnit : public CResourceArchiverUnit
{
public:

	bool IsImageFormatSupported( const boost::filesystem::path& image_filepath ) const
	{
		std::string ext = image_filepath.extension();
		boost::algorithm::to_lower( ext );

		if( ext == ".bmp"
		 || ext == ".jpg"
		 || ext == ".png"
		 || ext == ".dds"
		 || ext == ".tga" )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Result::Name AddFileToDB( const std::string& keyname, const boost::filesystem::path& input_path, CBinaryDatabase<std::string>& db )
	{
		using namespace std;

		if( IsImageFormatSupported( input_path ) )
		{
			CImageArchive img_archive( input_path.string() );
			bool added = db.AddData( keyname, img_archive );

			return Result::SUCCESS;
		}
		else
			return Result::INVALID_ARGS;
	}

//	Result::Name CreateArchive( const std::string& input_filepath, CImageArchive& img_archive )
//	{
//		int res = img_archive.LoadFromFile( input_filepath );
//	}

	Result::Name CreateArchive( const std::string& input_filepath, const std::string& output_filepath, CResourceArchiverUnitOutput& out )
//	Result::Name CreateArchive( const std::string& input_filepath, boost::shared_ptr<IArchiveObjectBase>& pArchive )
	{
		using namespace std;

		if( !IsImageFormatSupported( input_filepath ) )
			return Result::INVALID_ARGS;

		CImageArchive img_archive( input_filepath );
		if( !img_archive.IsValid() )
			return Result::UNKNOWN_ERROR;

		bool saved = img_archive.SaveToFile( output_filepath );

//		boost::shared_ptr<CImageArchive> pImgArchive( new CImageArchive( input_filepath ) );
//		pArchive = pImgArchive;

		return saved ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}
};


class CLastTimestampInfo
{
public:

//	std::string m_DBFilepath;

	std::map< std::string, timestamp_type > m_mapFilepathToModTime;
};


class CResourceFileInfo : public IArchiveObjectBase
{
public:

	std::string m_Keyname;
	timestamp_type m_LastModifiedTime;

	CResourceFileInfo()
		:
	m_LastModifiedTime(0)
	{}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_Keyname;

		if( sizeof(m_LastModifiedTime) == sizeof(U32) )
		{
			ar & m_LastModifiedTime;
		}
		else
		{
			// TODO: support serialization of 8-byte integers
			if( ar.GetMode() == IArchive::MODE_OUTPUT )
			{
				U32 val = (U32)m_LastModifiedTime;
				ar & val;
			}
			else
			{
				U32 val = 0;
				ar & val;
				m_LastModifiedTime = val;
			}
		}
	}
};


class CResourceCompileInfo : public IArchiveObjectBase
{
public:

	enum Type
	{
		TYPE_INVALID,
		TYPE_ARCHIVE,
		TYPE_DATABASE,
//		TYPE_COPY,
		NUM_TYPES
	};

	Type m_Type;

	std::map< std::string, CResourceFileInfo > m_mapInputFilepathToModTime;

	std::string m_OutputPath;

public:

	CResourceCompileInfo( Type type = TYPE_INVALID )
		:
	m_Type(type)
	{}

	void PrintInfo()
	{
		using namespace std;

		printf( "[in] " );
		map<string,CResourceFileInfo>::iterator itr = m_mapInputFilepathToModTime.begin();
		int index = 0;
		for( ; itr != m_mapInputFilepathToModTime.end(); itr++, index++ )
		{
			const std::string& input_filepath = itr->first;
			printf( "%s%s",
				input_filepath.c_str(),
				(index < m_mapInputFilepathToModTime.size() - 1) ? ", " : "" );
		}

		printf( "\n" );
		printf( "[out] %s\n", m_OutputPath.c_str() );
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_mapInputFilepathToModTime;
		ar & m_OutputPath;
	}
};


class CResourceBuildInfo : public IArchiveObjectBase
{
public:
	std::vector<CResourceCompileInfo> m_CompileInfo;

	/// Returns the last moified time of the requested path.
	/// Returns 0 if the requested path was not found.
	timestamp_type GetLastCompiledTime( const std::string& input_filepath ) const
	{
		const CResourceFileInfo *pInfo = GetResourceFileInfo( input_filepath );
		if( pInfo )
			return pInfo->m_LastModifiedTime;
		else
			return 0;
	}

	const CResourceFileInfo *GetResourceFileInfo( const std::string& input_filepath ) const
	{
		using namespace std;

		const size_t num_elements = m_CompileInfo.size();
		for( size_t i=0; i<num_elements; i++ )
		{
//			map<string,timestamp_type>::const_iterator itr
			map<string,CResourceFileInfo>::const_iterator itr
				= m_CompileInfo[i].m_mapInputFilepathToModTime.find( input_filepath );

			if( itr != m_CompileInfo[i].m_mapInputFilepathToModTime.end() )
			{
				return &(itr->second);
//				return itr->second.m_LastModifiedTime;
			}
		}

		return NULL;
//		return 0;
	}

	CResourceFileInfo *GetResourceFileInfo( const std::string& input_filepath )
	{
		using namespace std;

		const size_t num_elements = m_CompileInfo.size();
		for( size_t i=0; i<num_elements; i++ )
		{
//			map<string,timestamp_type>::const_iterator itr
			map<string,CResourceFileInfo>::iterator itr
				= m_CompileInfo[i].m_mapInputFilepathToModTime.find( input_filepath );

			if( itr != m_CompileInfo[i].m_mapInputFilepathToModTime.end() )
			{
				return &(itr->second);
//				return itr->second.m_LastModifiedTime;
			}
		}

		return NULL;
//		return 0;
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_CompileInfo;
	}
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
public:

	typedef std::pair<std::string,boost::filesystem::path> keyname_and_filepath;

private:

	bool NeedToUpdateDB( const std::vector<keyname_and_filepath>& vecInputFilepath, const std::string& output_filepath );

	Result::Name AddData( CBinaryDatabase<std::string>& db, const std::string& keyname, const boost::filesystem::path& input_path );

	Result::Name CreateDB( const std::vector<keyname_and_filepath>& vecInputFilepath, const std::string& output_filepath );

	Result::Name CreateArchive( const std::string& input_filepath, const boost::filesystem::path& output_filepath, CResourceArchiverUnitOutput& out );

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

	Result::Name LoadCompileInfo( const std::string& desc_filepath, CResourceBuildInfo& build_info );

	Result::Name CreateDB( const std::string& desc_filepath, CResourceCompileInfo& compile_info );

	Result::Name CompileResource( CResourceCompileInfo& compile_info, CResourceArchiverUnitOutput& out );
};


// ============================ inline implementations ============================

inline CResourceArchiver::CResourceArchiver()
{}


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


inline bool CResourceArchiver::NeedToUpdateDB( const std::vector<keyname_and_filepath>& vecInputFilepath, const std::string& output_filepath )
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
inline Result::Name CResourceArchiver::CreateDB( const std::vector<keyname_and_filepath>& vecInputFilepath, const std::string& output_filepath )
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


inline Result::Name CResourceArchiver::CreateArchive( const std::string& input_filepath,
									   const boost::filesystem::path& output_filepath,
									   CResourceArchiverUnitOutput& out )
{
	for( size_t i=0; i<m_vecpArchiverUnit.size(); i++ )
	{
		out.Reset();
		Result::Name ret = m_vecpArchiverUnit[i]->CreateArchive( input_filepath, output_filepath.string(), out );
		if( ret == Result::SUCCESS )
			return Result::SUCCESS;
	}

	return Result::UNKNOWN_ERROR;
}


inline Result::Name CResourceArchiver::CompileResource( CResourceCompileInfo& compile_info, CResourceArchiverUnitOutput& out )
{
	using namespace std;
	using namespace boost;
	using namespace boost::filesystem;

	if( compile_info.m_mapInputFilepathToModTime.empty() )
		return Result::INVALID_ARGS;

	map<string,CResourceFileInfo>::iterator itr;
	vector<CResourceArchiver::keyname_and_filepath> vecInputFilepath;

	string output_filepath = compile_info.m_OutputPath;
	switch( compile_info.m_Type )
	{
	case CResourceCompileInfo::TYPE_ARCHIVE:
		return CreateArchive( compile_info.m_mapInputFilepathToModTime.begin()->first, output_filepath, out );
//		break;

	case CResourceCompileInfo::TYPE_DATABASE:
		for( itr = compile_info.m_mapInputFilepathToModTime.begin();
			 itr != compile_info.m_mapInputFilepathToModTime.end();
			 itr++ )
		{
			const string& input_path = itr->first;
			const string& keyname    = itr->second.m_Keyname;
			vecInputFilepath.push_back( keyname_and_filepath( keyname, input_path ) );
		}
		return CreateDB( vecInputFilepath, compile_info.m_OutputPath );
//		break;

	default:
		break;
	}

	return Result::UNKNOWN_ERROR;
}


inline Result::Name CResourceArchiver::CreateDBs( const std::string& desc_filepath )
{
	using namespace std;
	using namespace boost;
	using namespace boost::filesystem;

	path descfile_directory = path(desc_filepath).parent_path();

	CResourceBuildInfo build_info;
	Result::Name res = LoadCompileInfo( desc_filepath, build_info );

//	vector<CResourceCompileInfo>& compile_info = build_info.m_CompileInfo;
	const int num_elements = (int)build_info.m_CompileInfo.size();
	for( int i=0; i<num_elements; i++ )
	{
		CResourceCompileInfo& compile_info = build_info.m_CompileInfo[i];

		CResourceArchiverUnitOutput out;
		CompileResource( compile_info, out );
	}
}
	

inline Result::Name CResourceArchiver::LoadCompileInfo( const std::string& desc_filepath, CResourceBuildInfo& build_info )
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

	vector<CResourceCompileInfo>& compile_info = build_info.m_CompileInfo;

	// set the first compile info
//	compile_info.push_back( CResourceCompileInfo() );

	string line;
	string input_path, output_filepath, include_path;
	vector<keyname_and_filepath> vecInputFilepath;
	for( ; !scanner.End(); scanner.NextLine() )
	{
		string tag = scanner.GetTagString();
		scanner.GetCurrentLine( line );

		if( scanner.TryScanLine( "input", input_path ) )
		{
			if( compile_info.empty()
			 || compile_info.back().m_Type != CResourceCompileInfo::TYPE_DATABASE )
			{
				compile_info.push_back( CResourceCompileInfo(CResourceCompileInfo::TYPE_DATABASE) );
			}

			CResourceFileInfo rfi;
			rfi.m_Keyname = input_path;

			string input_file_path = path(descfile_directory / input_path).string();
			compile_info.back().m_mapInputFilepathToModTime[input_file_path] = rfi;
			vecInputFilepath.push_back( keyname_and_filepath( input_path, descfile_directory / input_path ) );
			continue;
		}

		if( scanner.TryScanLine( "output", output_filepath ) )
		{
			if( compile_info.empty()
			 || compile_info.back().m_Type != CResourceCompileInfo::TYPE_DATABASE )
			{
				compile_info.push_back( CResourceCompileInfo(CResourceCompileInfo::TYPE_DATABASE) );
			}

//			if( 0 < build_info.m_CompileInfo.size() )
				build_info.m_CompileInfo.back().m_OutputPath = path(descfile_directory / output_filepath).string();
			continue;
		}

		if( tag == "close" )
		{
/*			if( 0 < vecInputFilepath.size()
			 && 0 < output_filepath.length() )
			{
				CreateDB( vecInputFilepath, output_filepath );
			}*/

//			vecInputFilepath.resize( 0 );
			output_filepath = "";

			// add the compile info for the next input & output paths
//			compile_info.push_back( CResourceCompileInfo() );
		}

		if( tag == "compile" )
		{
			vector<string> strings;
			SeparateStrings( strings, line.c_str(), " \t" );

			// Needs at least 2 strings, the tag "compile" and an input filepath
			if( strings.size() <= 1 )
				continue;

//			if( compile_info.empty()
//			 || compile_info.back().m_Type != CResourceCompileInfo::TYPE_ARCHIVE )
//			{
//				compile_info.push_back( CResourceCompileInfo(CResourceCompileInfo::TYPE_ARCHIVE) );
//			}

			compile_info.push_back( CResourceCompileInfo(CResourceCompileInfo::TYPE_ARCHIVE) );
			CResourceCompileInfo& rci = compile_info.back();
			input_path = strings[1];
			remove_newline_char_at_the_end( input_path );
//			rci.m_InputPaths.push_back( input_path );
//			rci.m_InputPath = input_path;
//			rci.m_mapInputFilepathToModTime[ input_path ] = 0;
			string input_file_path = path(descfile_directory / input_path).string();
			rci.m_mapInputFilepathToModTime[input_file_path] = CResourceFileInfo();

			if( 3 <= strings.size() )
			{
				remove_newline_char_at_the_end( strings[2] );
				rci.m_OutputPath = strings[2];
			}

//			m_mapInputPathToCompileInfo[input_path] = ;

//			CreateArchive( input_path, output_filepath );
		}

		if( scanner.TryScanLine( "include", include_path ) )
		{
			LoadCompileInfo( include_path, build_info );
		}
	}

//	if( !compile_info.back().IsValid() )
//		compile_info.erase( compile_info );

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
