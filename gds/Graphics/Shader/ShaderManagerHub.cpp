#include "ShaderManagerHub.hpp"

using namespace std;


CSingleton<CShaderManagerHub> CShaderManagerHub::m_obj;


CShaderManagerHub::CShaderManagerHub()
{
}


void CShaderManagerHub::RegisterShaderManager( CShaderManager* pShaderMgr )
{
	m_vecpShaderManager.push_back( pShaderMgr );
}


bool CShaderManagerHub::ReleaseShaderManager( CShaderManager* pShaderMgr )
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

