
#include "BVH/BVHPlayer.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"

#include "MotionBlender.hpp"
#include "MotionDatabase.hpp"
#include "HumanoidMotionTable.hpp"

using namespace msynth;


CMotionDatabase::CMotionDatabase( const std::string& database_filename )
{
	LoadFromFile( database_filename );
}


boost::shared_ptr<CMotionPrimitive> CMotionDatabase::GetMotionPrimitive( const std::string& name )
{
	boost::shared_ptr<CMotionPrimitive> pMotion
		= boost::shared_ptr<CMotionPrimitive>( new CMotionPrimitive() );

	bool success = m_DB.GetData( name, *(pMotion.get()) );

	if( !success )
	{
		LOG_PRINT_ERROR( " - failed to get the following motion primitive from database: " + name );
	}

	// How would I go about skeleton data?

	return pMotion;
}


bool CMotionDatabase::LoadFromFile( const std::string& filepath )
{
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
