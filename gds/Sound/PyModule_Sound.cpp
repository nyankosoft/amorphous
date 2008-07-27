#include "PyModule_Sound.h"
#include "Sound/SoundManager.h"

using namespace std;


PyObject* Play( PyObject* self, PyObject* args )
{
	char *sound_name;
	int volume = 100;
	int result = PyArg_ParseTuple( args, "s|i", &sound_name, &volume );

//	CSoundHandle snd_handle;
//	snd_handle.SetResourceName( sound_name );

//	SoundManager().Play( snd_handle );
	SoundManager().Play( sound_name );

	return Py_None;
}


PyObject* Play3D( PyObject* self, PyObject* args )
{
	char *sound_name;
	Vector3 pos = Vector3(0,0,0);
	int volume = 100;
//	int result = PyArg_ParseTuple( args, "(sfff|f)", &base_name, &pos.x, &pos.y, &pos.z, &ang_h );	// error
	int result = PyArg_ParseTuple( args, "sfff|ff", &sound_name, &pos.x, &pos.y, &pos.z,
		                                            &volume );

	SoundManager().PlayAt( sound_name, pos );

	return Py_None;
}


PyObject* PlayStream( PyObject* self, PyObject* args )
{
	char *sound_name;
	int volume = 100;
	int sound_source_id = 0;

	int result = PyArg_ParseTuple( args, "s|i", &sound_name, &volume );

	SoundManager().PlayStream( sound_name );

//	PyObject *obj = Py_BuildValue( "i", sound_source_id );

	return Py_None;
}


PyMethodDef g_PyModuleSoundMethod[] =
{
	{ "Play",       Play,	    METH_VARARGS, "plays a non-3D sound" },
	{ "Play3D",     Play3D,     METH_VARARGS, "plays a sound at a given position (world coordinates)" },
//	{ "PlayStream", PlayStream, METH_VARARGS, "plays a non-3D, stream sound (mainly for background music)" },
//	{ "CreateSoundSource", CreateSoundSource, METH_VARARGS, "" },
//	{ "Stop",              Stop,              METH_VARARGS, "" },
//	{ "Resume",            Resume,            METH_VARARGS, "" },
//	{ "Play",              Play,              METH_VARARGS, "" },
	{ NULL, NULL }
};