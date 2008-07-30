#include "PyModule_Sound.h"
#include "Sound/SoundManager.h"
#include "Support/Log/DefaultLog.h"
#include <string>
#include <map>

using namespace std;


static map<int,CSoundSource *> gs_mapIDToSoundSource;

/// used by PlayStream() and StopStream()
static map<string,CSoundSource *> gs_mapNameToSoundSource;



/// Stop and release all sound sources created from scripts
/// - Used to stop background music and effect sounds when player chooses to leave the stage
void ReleaseAllScriptSounds()
{
	// release sound sources held by id
	map<int,CSoundSource *>::iterator id_itr;
	for( id_itr = gs_mapIDToSoundSource.begin();
		 id_itr != gs_mapIDToSoundSource.end();
		 id_itr++ )
	{
		SoundManager().ReleaseSoundSource( id_itr->second );
	}
	gs_mapIDToSoundSource.clear();

	// release sound sources held by name
	map<string,CSoundSource *>::iterator name_itr;
	for( name_itr = gs_mapNameToSoundSource.begin();
		 name_itr != gs_mapNameToSoundSource.end();
		 name_itr++ )
	{
		SoundManager().ReleaseSoundSource( name_itr->second );
	}
	gs_mapNameToSoundSource.clear();
}


/// for short, non-looped sound
/// - The user does not have to stop the sound
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

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* PlayStream( PyObject* self, PyObject* args )
{
	char *sound_name;
	char *sound_group_name;
	int loop = 0;
	int volume = 100;
//	int sound_source_id = 0;
	double fadein_time = 0.0; // [sec]

	int result = PyArg_ParseTuple( args, "s|isi", &sound_name, &loop, &sound_group_name, &volume );

	if( gs_mapNameToSoundSource.find( sound_name ) != gs_mapNameToSoundSource.end() )
	{
		// A stream sound with the same name has already been loaded.
		Py_INCREF( Py_None );
		return Py_None;
	}

	CSoundDesc desc;
	desc.Loop             = loop == 1 ? true : false;
	desc.Streamed         = true;
	desc.SourceManagement = CSoundSource::Manual;

	CSoundSource *pSource = SoundManager().CreateSoundSource( sound_name, desc );
	if( pSource )
	{
		gs_mapNameToSoundSource[sound_name] = pSource;
		pSource->Play();
	}
	else
	{
		LOG_PRINT_ERROR( "Cannot play streamed sound of: " + string(sound_name) );
	}

//	PyObject *obj = Py_BuildValue( "i", sound_source_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* StopStream( PyObject* self, PyObject* args )
{
	char *sound_name;
	int sound_source_id = 0;
	int loop = 0;
	double fadein_time = 0.0; // [sec]

	int result = PyArg_ParseTuple( args, "s", &sound_name );

	if( strlen(sound_name) == 0 )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	map<string,CSoundSource *>::iterator itr
		= gs_mapNameToSoundSource.find( sound_name );

	if( itr == gs_mapNameToSoundSource.end() )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	CSoundSource *pSource = itr->second;
	if( pSource )
	{
		pSource->Stop();
		SoundManager().ReleaseSoundSource( pSource );
	}

	gs_mapNameToSoundSource.erase( itr );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ReleaseAllSounds( PyObject* self, PyObject* args )
{
//	int result = PyArg_ParseTuple( args, "s", &sound_name );

	ReleaseAllScriptSounds();

	Py_INCREF( Py_None );
	return Py_None;
}


PyMethodDef g_PyModuleSoundMethod[] =
{
	{ "Play",             Play,	            METH_VARARGS, "plays a non-3D sound" },
	{ "Play3D",           Play3D,           METH_VARARGS, "plays a sound at a given position (world coordinates)" },
	{ "PlayStream",       PlayStream,       METH_VARARGS, "plays a non-3D, stream sound (mainly for background music)" },
	{ "StopStream",       StopStream,       METH_VARARGS, "plays a non-3D, stream sound (mainly for background music)" },
	{ "ReleaseAllSounds", ReleaseAllSounds, METH_VARARGS, "" },
//	{ "CreateSoundSource",  CreateSoundSource,  METH_VARARGS, "" },
//	{ "ReleaseSoundSource", ReleaseSoundSource, METH_VARARGS, "" },
//	{ "Stop",              Stop,              METH_VARARGS, "" },
//	{ "Resume",            Resume,            METH_VARARGS, "" },
//	{ "Play",              Play,              METH_VARARGS, "" },
	{ NULL, NULL }
};