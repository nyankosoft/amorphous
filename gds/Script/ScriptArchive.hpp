#ifndef  __ScriptArchive_H__
#define  __ScriptArchive_H__


#include "gds/Support/SerializableStream.hpp"


namespace amorphous
{


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
	std::vector<SerializableStream> m_vecBuffer;

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
void UpdateScriptArchives( const std::string & src_dir_path,
						   const std::string & output_dir_path );


} // namespace amorphous



#endif  /*  __ScriptArchive_H__  */
