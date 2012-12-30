#ifndef __PYMODULE_TEXTMESSAGE_H__
#define __PYMODULE_TEXTMESSAGE_H__


#include "fwd.hpp"
#include "../Script/PythonHeaders.hpp"


namespace amorphous
{


//extern void SetTextMessageManager( CTextMessageManager* pTextMessageMgr );
extern void SetTextMsgMgrForScriptCallback( CTextMessageManager* pTextMessageMgr );

extern PyMethodDef g_PyModuleTextMessageMethod[];

} // namespace amorphous



#endif  /*  __PYMODULE_TEXTMESSAGE_H__  */
