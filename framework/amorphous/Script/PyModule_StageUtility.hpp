#ifndef __PyModule_StageUtility_HPP__
#define __PyModule_StageUtility_HPP__

#include "amorphous/Support/Vec3_StringAux.hpp"
#include "amorphous/Stage/StageCameraUtility.hpp"
#include "amorphous/Stage/StageLightUtility.hpp"
#include <thread>
#include <boost/python.hpp>
#include <memory>
#include <map>


namespace amorphous
{


namespace stage_util
{



// global function externs
extern void RegisterPythonModule_stage_util();


//inline GlobalGraphicsElementManagerList( const std::string& name, std::shared_ptr<GraphicsElementManager> pMgr )
typedef std::map< boost::thread::id, std::weak_ptr<CStage> > thread_id_to_stage_map;

inline thread_id_to_stage_map& ThreadIDToStageMap()
{
	static thread_id_to_stage_map s_mapThreadIDToStage;
	return s_mapThreadIDToStage;
}


inline std::shared_ptr<CStage> GetStageForScript()
{
	thread_id_to_stage_map::iterator itr = ThreadIDToStageMap().find( boost::this_thread::get_id() );

	if( itr != ThreadIDToStageMap().end() )
		return itr->second.lock();
	else
		return std::shared_ptr<CStage>();
}


inline void RegisterStageForScript( std::shared_ptr<CStage> pStage )
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



//inline std::shared_ptr<StageCameraUtility> CreateStageCameraUtility()
inline StageCameraUtility CreateStageCameraUtility()
{
//	std::shared_ptr<StageCameraUtility> pUtil( new StageCameraUtility( GetStageForScript() ) );
//	return pUtil;
	StageCameraUtility util(GetStageForScript());
	return util;
}


//inline std::shared_ptr<StageLightUtility> CreateStageLightUtility()
inline StageLightUtility CreateStageLightUtility()
{
//	std::shared_ptr<StageLightUtility> pUtil( new StageLightUtility( GetStageForScript() ) );
//	return pUtil;
	StageLightUtility util(GetStageForScript());
	return util;
}


//inline std::shared_ptr<StageMiscUtility> CreateStageMiscUtility()
inline StageMiscUtility CreateStageMiscUtility()
{
//	std::shared_ptr<StageMiscUtility> pUtil( new StageMiscUtility( GetStageForScript() ) );
//	return boost::ref(pUtil);
	StageMiscUtility util(GetStageForScript());
	return util;
}


//inline std::shared_ptr<StageEntityUtility> CreateStageEntityUtility()
inline StageEntityUtility CreateStageEntityUtility()
{
//	std::shared_ptr<StageEntityUtility> pUtil( new StageEntityUtility( GetStageForScript() ) );
//	return pUtil;
	StageEntityUtility util(GetStageForScript());
	return util;
}



} // stage_util


} // namespace amorphous



#endif /* __PyModule_StageUtility_HPP__ */
