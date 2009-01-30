#ifndef  __GSF_ScriptManager_H__
#define  __GSF_ScriptManager_H__

#include "ScriptArchive.hpp"

#include <vector>
#include <string>

#include <sys/stat.h>

#include <Python.h>


class CScriptManager
{
	class CEventScript
	{
	public:

		/// called every frame
		PyObject* m_pEventCallback;

		/// called once at the start of the stage
		PyObject* m_pInitCallback;

		/// set to true if the event callback rerurns 1
		bool m_bIsDone;

		std::string filename;

		enum eEventState
		{
			EVENT_WAITING	= 0,
			EVENT_DONE		= 1,
		};

		/// last modified time of non archived file
		/// set to 0 when the file is not found
		time_t m_LastModifiedTime;

	public:
		CEventScript()
			:
		m_pInitCallback(NULL),
		m_pEventCallback(NULL),
		m_bIsDone(false),
		m_LastModifiedTime(0)
		{}

		CEventScript(PyObject* pCallback)
			:
		m_pInitCallback(NULL),
		m_pEventCallback(pCallback),
		m_bIsDone(false),
		m_LastModifiedTime(0)
		{}

		virtual ~CEventScript() {}
	};

	/// each element holds one script file content
	std::vector<CEventScript> m_vecEventScript;

	CEventScript *m_pTargetScript;

	/// turned on if non archived script files are found in the reousrce directory
	/// - they are reloaded when modified at runtime
	bool m_bLoadFromNonArchivedFiles;

	bool LoadScriptFromFile( const std::string& filename );

    bool LoadScript( const stream_buffer& buffer, CEventScript& dest_script );

	void ReloadUpdatedScriptFiles();

public:

	CScriptManager();

	virtual ~CScriptManager();

	bool GetScriptFiles( std::vector<std::string>& vecDestScriptFilename );

	/// load script files under the specified directory
//	bool LoadScripts( const std::string& directory );

//	bool LoadScriptFromArchiveFile( const std::string& filename );

	/// called from CStage::InitEventScriptManager()
	bool LoadScriptArchiveFile( const std::string& filename );

	void InitScripts();

	/// called every frame
	void Update();

	void AddEventCallback( PyObject* pEventCallback );

	void AddInitCallback( PyObject* pEventCallback );

	void AddModule( const std::string& module_name, PyMethodDef method[] );
};



#endif		/*  __GSF_ScriptManager_H__  */
