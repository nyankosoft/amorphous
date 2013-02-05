#ifndef __STAGELOADER_H__
#define __STAGELOADER_H__

#include <string>
#include "fwd.hpp"

#include <boost/thread/thread.hpp>


namespace amorphous
{


class StageLoader
{
public:

	/// create a stage object and initialize it with the given script
	/// same as
	/// - pStage = CreateStage();
	/// - then pStage->Initialize( script_name );
	CStageSharedPtr LoadStage( const std::string& script_name );

	/// creates an empty stage
	CStageSharedPtr CreateStage();
};


class CASyncStageLoader
{
	std::string m_StageScriptName;

	CStageSharedPtr m_pStage;

	bool m_bIsLoaded;

	bool m_bFailedToLoadStage;

	bool m_bExitLoaderThread;

	boost::thread *m_pLoader;
//	boost::shared_ptr<boost::thread> m_pLoader;

public:

	CASyncStageLoader();

	~CASyncStageLoader();

	/// \return false if unable to load stage.
	/// Creates a thread to load the stage, have the thread start loading the stage, and return.
	bool LoadStage( const std::string& script_name );

	/// creates an empty stage before loading
	/// - use this when you want to do some settings before initializing stage
	///   - collision group settings
	void CreateStageInstance();

	bool IsStageLoaded() const { return m_bIsLoaded; }

	bool FailedToLoadStage() const { return m_bFailedToLoadStage; }

	CStageSharedPtr GetStage() { return m_pStage; }

	void operator()();

	CASyncStageLoader& operator=( const CASyncStageLoader& loader )
	{
		assert( !"cannot be copied!\n" );
	}

	void Join();

	friend class CASyncStageLoaderThread;
};

} // namespace amorphous



#endif /* __STAGELOADER_H__ */
