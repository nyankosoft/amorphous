#include "InputHandler_StageDebug.hpp"
#include "Stage/Stage.hpp"
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>


namespace amorphous
{


using namespace std;
using namespace boost;


inline void replace_chars( std::string& src, char from, char to )
{
	for( size_t i=0; i<src.length(); i++ )
	{
		if( src[i] == from )
			src[i] = to;
	}
}


void InputHandler_StageDebug::WriteEntityTreeToFile( std::shared_ptr<CStage> pStage )
{
	using namespace gregorian;

	char dest_filename[512], time_str[64];

	ulong current_time_ms = GlobalTimer().GetTimeMS();

	if( 500 < (current_time_ms - m_EntityTreeFileLastOutputTime) )	// don't output more than once in half a second
	{
		EntityManager* pEntSet = pStage->GetEntitySet();

		sprintf( time_str, "%.3f", (double)current_time_ms / 1000.0 );

		string stage_script_name = pStage->GetScriptName();
		replace_chars( stage_script_name, '/', '-' );
		replace_chars( stage_script_name, '\\', '-' );

		// create the directory for entity tree files (YYYYMMDD)
		filesystem::path entity_tree_directory = "./debug/entity_trees-" + to_iso_string(day_clock::local_day());
		boost::filesystem::create_directories( entity_tree_directory );

		sprintf( dest_filename, "entity_tree-%s[%s].txt", stage_script_name.c_str(), time_str );

		filesystem::path dest_filepath = entity_tree_directory / dest_filename;

		// save the entity tree to disk
		pEntSet->WriteEntityTreeToFile(dest_filepath.string());

		m_EntityTreeFileLastOutputTime = current_time_ms;
	}
}


void InputHandler_StageDebug::ProcessInput(InputData& input)
{
	std::shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return;

	switch( input.iGICode )
	{
	case GIC_F4:
		if( input.iType == ITYPE_KEY_PRESSED )
                WriteEntityTreeToFile( pStage );
		break;

	default:
		break;
	}
}


} // namespace amorphous
