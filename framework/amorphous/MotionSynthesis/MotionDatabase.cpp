#include "MotionDatabase.hpp"
#include "MotionBlender.hpp"
#include "HumanoidMotionTable.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/StringAux.hpp"


namespace amorphous
{

using namespace std;
using namespace msynth;


MotionDatabase::MotionDatabase( const std::string& database_filename )
:
m_DatabaseFilepath(database_filename)
{
	LoadFromFile( database_filename );
}


std::shared_ptr<MotionPrimitive> MotionDatabase::GetMotionPrimitive( const std::string& name )
{
	std::shared_ptr<MotionPrimitive> pMotion( new MotionPrimitive() );

	bool success = m_DB.GetData( name, *(pMotion.get()) );

	if( success )
	{
		// How would I go about skeleton data?
		return pMotion;
	}
	else
	{
		string msg = fmt_string( " Failed to get the motion primitive '%s' from database '%s'", name.c_str(), m_DatabaseFilepath.c_str() );
		LOG_PRINT_WARNING( msg );

		return std::shared_ptr<MotionPrimitive>();
	}
}


bool MotionDatabase::LoadFromFile( const std::string& filepath )
{
	m_DatabaseFilepath = filepath;

//	return m_DB.Open( filepath, CBinaryDatabase<string>::DB_MODE_APPEND );

	bool success = m_DB.Open( filepath, CBinaryDatabase<string>::DB_MODE_APPEND );

	if( !success )
	{
		LOG_PRINT_ERROR( " - failed to open database: " + filepath );
		return false;
	}

	// load annotation table
	string annot_table_key = "__AnnotationTable__";
	success = m_DB.GetData( annot_table_key, m_vecAnnotationName );
	
	return success;
}


void MotionDatabase::SaveToFile( const std::string& filepath )
{
}


bool MotionDatabase::GetHumanoidMotionTable( const std::string& table_name, HumanoidMotionTable& dest )
{
	return m_DB.GetData( table_name, dest );
}


CAnnotation MotionDatabase::GetAnnotation( const std::string& annot_name )
{
	for( size_t i=0; i<m_vecAnnotationName.size(); i++ )
	{
		const string& name = m_vecAnnotationName[i];

		if( name == annot_name )
		{
			return CAnnotation( annot_name, i );
		}
	}

	LOG_PRINT_WARNING( " - The requested annotation '" + annot_name + "' was not found in the motion database." );

	return CAnnotation( annot_name, -1 );
}


} // namespace amorphous
