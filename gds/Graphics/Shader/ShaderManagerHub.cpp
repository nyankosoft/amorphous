#include "ShaderManagerHub.hpp"


namespace amorphous
{

using namespace std;


CSingleton<ShaderManagerHub> ShaderManagerHub::m_obj;


ShaderManagerHub::ShaderManagerHub()
{
}


void ShaderManagerHub::RegisterShaderManager( ShaderManager* pShaderMgr )
{
	m_vecpShaderManager.push_back( pShaderMgr );
}


bool ShaderManagerHub::ReleaseShaderManager( ShaderManager* pShaderMgr )
{
	size_t i, num_shader_mgrs = m_vecpShaderManager.size();
	for( i=0; i<num_shader_mgrs; i++ )
	{
		if( m_vecpShaderManager[i] == pShaderMgr )
		{
			m_vecpShaderManager.erase( m_vecpShaderManager.begin() + i );
			return true;
		}
	}

	return false;	// the requested component was not found
}



} // namespace amorphous
