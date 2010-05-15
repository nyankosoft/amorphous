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

private:

	void MainLoop( const std::string& desc_filepath );

	void UpdateBuildInfo( const CResourceBuildInfo& last_build_info );

public:

	CAutoResourceArchiver() {}

	CResourceArchiver& ResourceArchiver() { return m_ResourceArchiver; }

	void Init( const std::string& resource_make_file );

	void SaveBuildInfoToDisk();

	// Start monitoring the resource with a separate thread.
	void StartMonitoring() { start_thread(); }

	void run() { MainLoop( m_RootDescFilepath ); }
};




#endif /* __AutoResourceArchiver_HPP__ */
