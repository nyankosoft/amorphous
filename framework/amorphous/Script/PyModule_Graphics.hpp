#ifndef __PyModule_Graphics_HPP__
#define __PyModule_Graphics_HPP__

#include "../Graphics/Rect.hpp"
#include "../Graphics/GraphicsElementManager.hpp"
#include "../Graphics/GraphicsEffectManager.hpp"
#include <memory>
#include <map>


namespace amorphous
{
//#include <vld.h>


// global variable externs
extern std::shared_ptr<TextElement> (GraphicsElementManager::*CreateTextWithTLPos)( int, const std::string&, float, float, const SFloatRGBAColor&, int, int, int );
extern std::shared_ptr<TextElement> (GraphicsElementManager::*CreateTextInBox)( int, const std::string&, const SRect&, int, int, const SFloatRGBAColor&, int, int, int );
extern bool (GraphicsElementManager::*LoadTextureWithID)( int, const std::string& );
extern int (GraphicsElementManager::*LoadTextureWithoutID)( const std::string& );
extern bool (GraphicsElementManager::*LoadFontWithID)( int, const std::string&, int, int, float, float, float );
extern int (GraphicsElementManager::*LoadFontWithoutID)( const std::string&, int, int, float, float, float );

// global function externs
extern void RegisterPythonModule_gfx();


//inline GlobalGraphicsElementManagerList( const std::string& name, std::shared_ptr<GraphicsElementManager> pMgr )
typedef std::map< std::string, std::weak_ptr<GraphicsElementManager> > name_to_gfxelementmgr;


inline name_to_gfxelementmgr& GlobalGraphicsElementManagerMap()
{
	static name_to_gfxelementmgr s_mapNameToElementMgr;
	return s_mapNameToElementMgr;
//	s_mapNameToElementMgr[name] = pMgr;
}


inline std::shared_ptr<GraphicsElementManager> GetGraphicsElementManager( const char *name )
{
	name_to_gfxelementmgr::iterator itr = GlobalGraphicsElementManagerMap().find( name );

	if( itr != GlobalGraphicsElementManagerMap().end() )
		return itr->second.lock();
	else
		return std::shared_ptr<GraphicsElementManager>();
}


} // namespace amorphous



#endif /* __PyModule_Graphics_HPP__ */
