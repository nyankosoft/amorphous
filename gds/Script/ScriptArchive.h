#ifndef  __SCRIPTARCHIVE_H__
#define  __SCRIPTARCHIVE_H__


#include "Support/SerializableStream.hpp"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;


/**
 * stores scripts as text data
 */
class CScriptArchive : public IArchiveObjectBase
{
public:

	// save the filenames so that we can find the source
	// if any of the scripts caused problems at runtime
	std::vector<std::string> m_vecSourceFilename;

	// each element stores a script file
	std::vector<CSerializableStream> m_vecBuffer;

	CScriptArchive() {}
	~CScriptArchive() {}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_vecBuffer & m_vecSourceFilename;
	}

};


/**
 * \param dir_path root directory for script directory
 */
void UpdateScriptArchives( const fs::path & dir_path,
						   const std::string & output_dir_path );



#endif  /*  __SCRIPTARCHIVE_H__  */
