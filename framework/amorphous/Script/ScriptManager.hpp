#ifndef  __GSF_ScriptManager_H__
#define  __GSF_ScriptManager_H__

//#include "Script/PythonHeaders.hpp"
#include <vector>
#include <string>
#include <sys/stat.h>
#include <boost/python.hpp>
#include <thread>
#include "ScriptArchive.hpp"
#include "amorphous/Support/singleton.hpp"


namespace amorphous
{


//=========================================================================
// CPythonUserCount
//=========================================================================

class CPythonUserCount
{
	int m_RefCount;

	boost::mutex m_Mutex;

protected:

	static singleton<CPythonUserCount> m_obj;

public:

	CPythonUserCount()
	:
	m_RefCount(0)
	{}

	static CPythonUserCount* Get() { return m_obj.get(); }

	void IncRefCount()
	{
		boost::mutex::scoped_lock scoped_lock(m_Mutex);

		m_RefCount++;

		if( m_RefCount == 1 )
		{
			if( !Py_IsInitialized() )
			{
				boost::thread::id thread_id = boost::this_thread::get_id();
				Py_Initialize();
			}
			else
			{
				LOG_PRINT( " Skipped Py_Initialize(). Somebody other than the engine has already called Py_Initialize()." );
			}
		}
	}

	void DecRefCount()
	{
		boost::mutex::scoped_lock scoped_lock(m_Mutex);

		m_RefCount--;
		
		if( m_RefCount == 0 )
			Py_Finalize();
		else if( m_RefCount < 0 )
		{
			LOG_PRINT( " An invalid call. ref count was attempted to be decremented below zero." );
			Py_Finalize();
		}
	}
};


inline CPythonUserCount& PythonUserCount()
{
	return (*CPythonUserCount::Get());
}




//=========================================================================
// ScriptManager
//=========================================================================

class ScriptManager
{
protected:

	class PythonEventScript
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
		PythonEventScript()
			:
		m_pInitCallback(NULL),
		m_pEventCallback(NULL),
		m_bIsDone(false),
		m_LastModifiedTime(0)
		{}

		PythonEventScript(PyObject* pCallback)
			:
		m_pInitCallback(NULL),
		m_pEventCallback(pCallback),
		m_bIsDone(false),
		m_LastModifiedTime(0)
		{}

		virtual ~PythonEventScript() {}
	};

	/// each element holds one script file content
	std::vector<PythonEventScript> m_vecEventScript;

	PythonEventScript *m_pTargetScript;

	/// turned on if non archived script files are found in the reousrce directory
	/// - they are reloaded when modified at runtime
	bool m_bLoadFromNonArchivedFiles;

private:

	bool LoadScriptFromFile( const std::string& filename );

	void ReloadUpdatedScriptFiles();

protected:

	/// Execute the script and register the callback from the script.
	virtual bool LoadScript( const stream_buffer& buffer, PythonEventScript& dest_script ) { return false; }

	std::string GetExtraErrorInfo();

public:

	ScriptManager();

	virtual ~ScriptManager();

//	bool LoadScriptFromArchiveFile( const std::string& filename );

	/// called from CStage::InitEventScriptManager()
	bool LoadScriptArchiveFile( const std::string& filename );

	/// for debugging
	bool ExecuteScript( const stream_buffer& buffer );

	void InitScripts();

	/// called every frame
	virtual void Update();

	void AddEventCallback( PyObject* pEventCallback );

	void AddInitCallback( PyObject* pEventCallback );

	static bool ms_UseBoostPythonModules;
};


} // namespace amorphous



#endif		/*  __GSF_ScriptManager_H__  */
