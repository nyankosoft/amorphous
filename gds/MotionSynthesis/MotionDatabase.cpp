#include "MotionDatabase.hpp"
#include "MotionBlender.hpp"
#include "HumanoidMotionTable.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"

using namespace std;
using namespace boost;
using namespace msynth;


CMotionDatabase::CMotionDatabase( const std::string& database_filename )
:
m_DatabaseFilepath(database_filename)
{
	LoadFromFile( database_filename );
}


boost::shared_ptr<CMotionPrimitive> CMotionDatabase::GetMotionPrimitive( const std::string& name )
{
	shared_ptr<CMotionPrimitive> pMotion( new CMotionPrimitive() );

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

		return shared_ptr<CMotionPrimitive>();
	}
}


bool CMotionDatabase::LoadFromFile( const std::string& filepath )
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


void CMotionDatabase::SaveToFile( const std::string& filepath )
{
}


bool CMotionDatabase::GetHumanoidMotionTable( const std::string& table_name, CHumanoidMotionTable& dest )
{
	return m_DB.GetData( table_name, dest );
}


CAnnotation CMotionDatabase::GetAnnotation( const std::string& annot_name )
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
