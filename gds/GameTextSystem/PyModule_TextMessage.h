#ifndef __PYMODULE_TEXTMESSAGE_H__
#define __PYMODULE_TEXTMESSAGE_H__


#include <Python.h>

class CTextMessageManager;

//extern void SetTextMessageManager( CTextMessageManager* pTextMessageMgr );
extern void SetTextMsgMgrForScriptCallback( CTextMessageManager* pTextMessageMgr );

extern PyMethodDef g_PyModuleTextMessageMethod[];


#endif  /*  __PYMODULE_TEXTMESSAGE_H__  */