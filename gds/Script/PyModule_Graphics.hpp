#ifndef __PyModule_Graphics_HPP__
#define __PyModule_Graphics_HPP__

#include <gds/Graphics/Rect.hpp>
#include <gds/Graphics/GraphicsElementManager.hpp>
#include <gds/Graphics/GraphicsEffectManager.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>


namespace amorphous
{
//#include <vld.h>


// global variable externs
extern boost::shared_ptr<TextElement> (GraphicsElementManager::*CreateTextWithTLPos)( int, const std::string&, float, float, const SFloatRGBAColor&, int, int, int );
extern boost::shared_ptr<TextElement> (GraphicsElementManager::*CreateTextInBox)( int, const std::string&, const SRect&, int, int, const SFloatRGBAColor&, int, int, int );
extern bool (GraphicsElementManager::*LoadTextureWithID)( int, const std::string& );
extern int (GraphicsElementManager::*LoadTextureWithoutID)( const std::string& );
extern bool (GraphicsElementManager::*LoadFontWithID)( int, const std::string&, int, int, float, float, float );
extern int (GraphicsElementManager::*LoadFontWithoutID)( const std::string&, int, int, float, float, float );

// global function externs
extern void RegisterPythonModule_gfx();


//inline GlobalGraphicsElementManagerList( const std::string& name, boost::shared_ptr<GraphicsElementManager> pMgr )
typedef std::map< std::string, boost::weak_ptr<GraphicsElementManager> > name_to_gfxelementmgr;


inline name_to_gfxelementmgr& GlobalGraphicsElementManagerMap()
{
	static name_to_gfxelementmgr s_mapNameToElementMgr;
	return s_mapNameToElementMgr;
//	s_mapNameToElementMgr[name] = pMgr;
}


inline boost::shared_ptr<GraphicsElementManager> GetGraphicsElementManager( const char *name )
{
	name_to_gfxelementmgr::iterator itr = GlobalGraphicsElementManagerMap().find( name );

	if( itr != GlobalGraphicsElementManagerMap().end() )
		return itr->second.lock();
	else
		return boost::shared_ptr<GraphicsElementManager>();
}


} // namespace amorphous



#endif /* __PyModule_Graphics_HPP__ */
