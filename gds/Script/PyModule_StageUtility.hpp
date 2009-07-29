#ifndef __PyModule_StageUtility_HPP__
#define __PyModule_StageUtility_HPP__

#include <gds/Graphics/FloatRGBAColor.hpp>
#include <gds/3DMath/Matrix34.hpp>
#include <gds/3DMath/AABB3.hpp>
#include <gds/Support/Vec3_StringAux.hpp>
#include <gds/Stage/fwd.hpp>
#include <gds/Stage/StageUtility.hpp>
#include <boost/thread.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>


namespace stage_util
{

using namespace boost;


// global function externs
extern void RegisterPythonModule_stage_util();


//inline GlobalGraphicsElementManagerList( const std::string& name, boost::shared_ptr<CGraphicsElementManager> pMgr )
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



inline boost::shared_ptr<CStageLightUtility> CreateStageLightUtility()
{
	boost::shared_ptr<CStageLightUtility> pUtil( new CStageLightUtility( GetStageForScript() ) );
	return pUtil;
}


inline boost::shared_ptr<CStageMiscUtility> CreateStageMiscUtility()
{
	boost::shared_ptr<CStageMiscUtility> pUtil( new CStageMiscUtility( GetStageForScript() ) );
	return pUtil;
}


inline boost::shared_ptr<CStageEntityUtility> CreateStageEntityUtility()
{
	boost::shared_ptr<CStageEntityUtility> pUtil( new CStageEntityUtility( GetStageForScript() ) );
	return pUtil;
}



} // stage_util



#endif /* __PyModule_StageUtility_HPP__ */
