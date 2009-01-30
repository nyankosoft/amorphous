#include "InputHandler_StageDebug.hpp"
#include "Stage/Stage.hpp"

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


void CInputHandler_StageDebug::WriteEntityTreeToFile( shared_ptr<CStage> pStage )
{
	char dest_filepath[512], time_str[64];

	ulong current_time_ms = GlobalTimer().GetTimeMS();

	if( 1000 < (current_time_ms - m_EntityTreeFileLastOutputTime) )	// don't output more than once in a second
	{
		CEntitySet* pEntSet = pStage->GetEntitySet();

		sprintf( time_str, "%.3f", (double)current_time_ms / 1000.0 );

		string stage_script_name = pStage->GetScriptName();
		replace_chars( stage_script_name, '/', '-' );
		replace_chars( stage_script_name, '\\', '-' );

		sprintf( dest_filepath, "entity_tree-%s[%s].txt", stage_script_name.c_str(), time_str );

		pEntSet->WriteEntityTreeToFile(dest_filepath);

		m_EntityTreeFileLastOutputTime = current_time_ms;
	}
}


void CInputHandler_StageDebug::ProcessInput(SInputData& input)
{
	boost::shared_ptr<CStage> pStage = m_pStage.lock();
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
