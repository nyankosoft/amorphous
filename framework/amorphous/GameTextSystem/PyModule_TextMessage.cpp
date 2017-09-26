#include "PyModule_TextMessage.hpp"
#include "TextMessageManager.hpp"
#include "amorphous/Script/PythonScriptManager.hpp"
#include <map>


namespace amorphous
{

using namespace std;


static CTextMessageManager *gs_pCurrentTextMessageManager = NULL;

inline static CTextMessageManager *GetTextMessageManager() { return gs_pCurrentTextMessageManager; }


/// set text message manager for script callback
void SetTextMsgMgrForScriptCallback( CTextMessageManager* pTextMessageMgr )
{
	gs_pCurrentTextMessageManager = pTextMessageMgr;
}


#define RETURN_PYNONE() { Py_INCREF( Py_None ); return Py_None; }


PyObject* SetTextMessageManager( PyObject* self, PyObject* args )
{
	char *name;
	int result = PyArg_ParseTuple( args, "s", &name );

	SetTextMsgMgrForScriptCallback( TextMessageManagerHub().GetTextMessageManager(name) ); 

	Py_INCREF( Py_None );
	return Py_None;
}



PyObject* StartLoadMessage( PyObject* self, PyObject* args )
{
	if( !GetTextMessageManager() )
		RETURN_PYNONE();

	int priority = 0;
	int index = GetTextMessageManager()->StartLoadMessage( priority );

	return Py_BuildValue( "i", index );
}

/*
PyObject* EndLoadMessage( PyObject* self, PyObject* args )
{
	GetTextMessageManager()->EndLoadMessage();

	return Py_None;
}*/


/**
 * AddMessageRef( const char *speaker, const char *text, float waittime)
 */
PyObject* AddMessageRef( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );

	if( !GetTextMessageManager() )
		return Py_None;

//	MsgBox( "adding a text message referece" );

	char *speaker;
	char *message;
	float wait_time = -1.0f;
	int result = PyArg_ParseTuple( args, "ss|f", &speaker, &message, &wait_time );

    GetTextMessageManager()->AddMessageRef( speaker, message, wait_time );

	return Py_None;
}


PyObject* StartTextMessage( PyObject* self, PyObject* args )
{
	if( !GetTextMessageManager() )
		RETURN_PYNONE();

//	int mode;
//	int result = PyArg_ParseTuple( args, "ii", &mode, &index );

//	int req_result = GetTextMessageManager()->StartTextMessage( mode, index );
	int req_result = 0;

	return Py_BuildValue( "i", req_result );
}


PyObject* StartPreloadedTextMessage( PyObject* self, PyObject* args )
{
	if( !GetTextMessageManager() )
		RETURN_PYNONE();

//	if( !gs_pTextMessageManager )
//		return Py_BuildValue( "i", TextMessageBase::REQ_REJECTED );

	int mode, index;
	int priority = 0;
	int result = PyArg_ParseTuple( args, "ii", &mode, &index );

	int req_result = GetTextMessageManager()->StartTextMessage( mode, index, priority );

	return Py_BuildValue( "i", req_result );
}


//#define TEXTMSG_REQ_ACCEPTED	0
//#define TEXTMSG_REQ_REJECTED	1


/*
PyObject* EndTextMessage( PyObject* self, PyObject* args )
{
	GetTextMessageManager()->EndMessage();

	return Py_None;
}

PyObject* DisplayTextMessage( PyObject* self, PyObject* args )
{
}*/



static PyMethodDef sg_PyModuleTextMessageMethod[] =
{
	{ "SetTextMessageManager",     SetTextMessageManager,     METH_VARARGS, "Sets the target text message manager for the script" },
	{ "StartLoadMessage",          StartLoadMessage,          METH_VARARGS, "starts pre-loading of text message" },
//	{ "EndLoadMessage",            EndLoadMessage,            METH_VARARGS, "ends pre-loading of text message" },
	{ "AddMessageRef",             AddMessageRef,             METH_VARARGS, "send text message" },
	{ "StartTextMessage",          StartTextMessage,          METH_VARARGS, "requests start of text message display. returns true if the request is accepted" },
	{ "StartPreloadedTextMessage", StartPreloadedTextMessage, METH_VARARGS, "requests start of text message display. returns true if the request is accepted" },
//	{ "StartPreloadText",          StartPreloadText,          METH_VARARGS,  },
//	{ "EndPreloadText",            EndPreloadText,            METH_VARARGS,  },
	{NULL, NULL}
};


void RegisterPythonModule_TextMessage( PythonScriptManager& mgr )
{
	mgr.AddModule( "TextMessage", sg_PyModuleTextMessageMethod );
}


} // namespace amorphous
