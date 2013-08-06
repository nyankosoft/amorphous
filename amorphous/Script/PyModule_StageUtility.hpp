#ifndef __PyModule_StageUtility_HPP__
#define __PyModule_StageUtility_HPP__

#include "amorphous/Support/Vec3_StringAux.hpp"
#include "amorphous/Stage/StageUtility.hpp"
#include <boost/thread.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>


namespace amorphous
{


namespace stage_util
{

using namespace boost;


// global function externs
extern void RegisterPythonModule_stage_util();


//inline GlobalGraphicsElementManagerList( const std::string& name, boost::shared_ptr<GraphicsElementManager> pMgr )
typedef std::map< boost::thread::id, boost::weak_ptr<CStage> > thread_id_to_stage_map;

inline thread_id_to_stage_map& ThreadIDToStageMap()
{
	static thread_id_to_stage_map s_mapThreadIDToStage;
	return s_mapThreadIDToStage;
}


inline boost::shared_ptr<CStage> GetStageForScript()
{
	thread_id_to_stage_map::iterator itr = ThreadIDToStageMap().find( boost::this_thread::get_id() );

	if( itr != ThreadIDToStageMap().end() )
		return itr->second.lock();
	else
		return boost::shared_ptr<CStage>();
}


inline void RegisterStageForScript( boost::shared_ptr<CStage> pStage )
{
	ThreadIDToStageMap()[boost::this_thread::get_id()] = pStage;
}


inline void UnregisterStageForScript()
{
	thread_id_to_stage_map::iterator itr = ThreadIDToStageMap().find( boost::this_thread::get_id() );

	if( itr != ThreadIDToStageMap().end() )
		ThreadIDToStageMap().erase( itr );
	else
		LOG_PRINT_WARNING( "The requested stage was not found in the list" );
}



inline boost::shared_ptr<StageCameraUtility> CreateStageCameraUtility()
{
	boost::shared_ptr<StageCameraUtility> pUtil( new StageCameraUtility( GetStageForScript() ) );
	return pUtil;
}


inline boost::shared_ptr<StageLightUtility> CreateStageLightUtility()
{
	boost::shared_ptr<StageLightUtility> pUtil( new StageLightUtility( GetStageForScript() ) );
	return pUtil;
}


inline boost::shared_ptr<StageMiscUtility> CreateStageMiscUtility()
{
	boost::shared_ptr<StageMiscUtility> pUtil( new StageMiscUtility( GetStageForScript() ) );
	return pUtil;
}


inline boost::shared_ptr<StageEntityUtility> CreateStageEntityUtility()
{
	boost::shared_ptr<StageEntityUtility> pUtil( new StageEntityUtility( GetStageForScript() ) );
	return pUtil;
}



} // stage_util


} // namespace amorphous



#endif /* __PyModule_StageUtility_HPP__ */
