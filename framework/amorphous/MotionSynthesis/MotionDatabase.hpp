#ifndef  __MotionDatabase_H__
#define  __MotionDatabase_H__


#include "amorphous/Support/Serialization/BinaryDatabase.hpp"
#include "amorphous/Support/Serialization/Serialization.hpp"
#include "fwd.hpp"
#include "MotionPrimitive.hpp"


namespace amorphous
{

using namespace serialization;


namespace msynth
{


class MotionDatabase// : public IArchiveObjectBase
{
	CBinaryDatabase<std::string> m_DB;

	/// retrieved from database with the reserved keyname "__AnnotationTable__"
	std::vector<std::string> m_vecAnnotationName;

	std::string m_DatabaseFilepath;

private:

	bool CreateMotionPrimitivesFromScriptFile( const std::string& script_filename );

//	void ProcessFiles();

public:

	MotionDatabase() {}

	MotionDatabase( const std::string& database_filename );

	~MotionDatabase() {}

	std::shared_ptr<MotionPrimitive> GetMotionPrimitive( const std::string& name );

	/// returns true on success
	bool GetHumanoidMotionTable( const std::string& table_name, HumanoidMotionTable& dest );

	CAnnotation GetAnnotation( const std::string& annot_name );

	bool LoadFromFile( const std::string& filepath );

	void SaveToFile( const std::string& filepath );

//	bool Build( const std::string& source_script_filename );

//	void Serialize( IArchive & ar, const unsigned int version )
//	{
//	}

	friend class MotionDatabaseBuilder;
};


} // namespace msynth

} // namespace amorphous



#endif  /*  __MotionDatabase_H__  */



/*
bool MotionDatabase::CreateMotionPrimitivesFromScriptFile( const std::string& script_filename )
{
	CTextFileScanner scanner;
	if( !scanner.OpenFile( script_filename ) )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + script_filename );
		return false;
	}


	return true;
}


bool MotionDatabase::Build( const std::string& source_script_filename )
{
	CTextFileScanner scanner;
	if( !scanner.OpenFile( source_script_filename ) )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + source_script_filename );
		return false;
	}

	// change to the directory of 'source_script_filename'
	dir_stack dir_stk( get_path( source_script_filename ) );

	vector<string> vecScriptFilename;
	string tag, filename, database_filename;
	for( ; !scanner.End(); scanner.NextLine() )
	{
		tag = scanner.GetTagString();

		if( scanner.TryScanLine( "input", filename ) )
			vecScriptFilename.push_back( filename );

		scanner.TryScanLine( "output", database_filename );
	}

	if( database_filename.length() == 0 )
	{
		LOG_PRINT_ERROR( " - output filename is not specified." );
		return false;
	}

	if( vecScriptFilename.size() == 0 )
	{
		LOG_PRINT_ERROR( " - no input file is specified." );
		return false;
	}

	size_t i, num_input_files = vecScriptFilename.size();
	for( i=0; i<num_input_files; i++ )
	{
		CreateMotionPrimitivesFromScriptFile(  vecScriptFilename[i])
	}

	OutputDatabaseFile( database_filename );

	// restore the previous working directory
	dir_stk.prevdir();

	return true;
}
*/
