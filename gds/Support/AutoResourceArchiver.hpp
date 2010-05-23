#ifndef __AutoResourceArchiver_HPP__
#define __AutoResourceArchiver_HPP__


#include "ResourceArchiver.hpp"
#include "Support/thread_starter.hpp"


class CAutoResourceArchiver : public thread_class
{
//	std::map<std::string,CResourceCompileInfo> m_mapInputPathToCompileInfo;
	CResourceBuildInfo m_BuildInfo;

	CResourceArchiver m_ResourceArchiver;

	std::string m_RootDescFilepath;

	bool m_StopMonitoring;

private:

	void MainLoop( const std::string& desc_filepath );

	void UpdateBuildInfo( const CResourceBuildInfo& last_build_info );

public:

	CAutoResourceArchiver()
		:
	m_StopMonitoring(false)
	{}

	CResourceArchiver& ResourceArchiver() { return m_ResourceArchiver; }

	void Init( const std::string& resource_make_file );

	void SaveBuildInfoToDisk();

	// Start monitoring the resource with a separate thread.
	void StartMonitoring() { start_thread(); }

	void StopMonitoring() { m_StopMonitoring = true; }

	void run();
};




#endif /* __AutoResourceArchiver_HPP__ */
