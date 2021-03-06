#include "GraphicsResourceEntries.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Support/ImageArchive.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{
using namespace serialization;

using namespace std;


/**
 Graphics Resource
 - Graphics resources are not sharable if the loading modes are different
   even if all the other properties are the same.
 Rationale:
 - Sharing resources with different loading methods complicates loading process.
   - What if the user tries to load a resource(A) synchronously and the same resource(B) is being loaded asynchronously.
     1. The user wants to load (A) synchronously. i.e., no rendering before loading the resource
     2. (B) is being loaded asynchronously. If the system simply cancel the synchronous loading of (A),
	    some rendering may be done before loading (B).
		-> User's request mentioned in 1. is not satisfied.

*/


inline bool str_includes( const std::string& src, const std::string& target )
{
	if( src.find(target.c_str()) != std::string::npos )
		return true;
	else
		return false;
}



//==================================================================================================
// GraphicsResourceEntry
//==================================================================================================

void GraphicsResourceEntry::GetStatus( std::string& dest_buffer )
{
	dest_buffer += "ref: ";

	char buffer[16];
	memset( buffer, 0, sizeof(buffer) );
	sprintf( buffer, "%02d", m_iRefCount );
	dest_buffer += buffer;

	if( GetResource() )
		GetResource()->GetStatus( dest_buffer );
}


} // namespace amorphous
