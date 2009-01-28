
#include "ParticleThreadManager.h"

#include "Support/Log/DefaultLog.h"

//#include "yaneSDK/Thread/index.h"

using namespace std;


//==================================================================================================
// CParticleThreadManager
//==================================================================================================

// define the singleton instance
CSingleton<CParticleThreadManager> CParticleThreadManager::m_obj;


CParticleThreadManager::CParticleThreadManager()
{
}


CParticleThreadManager::~CParticleThreadManager()
{
}


void CParticleThreadManager::AddParticleSetBaseEntity( CBE_ParticleSet *pBaseEntity )
{
	m_vecpParticleSetBaseEntity.push_back( pBaseEntity );
}

/*
int CParticleThreadManager::
{
	if( !pcTextureFilename )
		return -1;	// invalid filename

	size_t i, num_base_entities = m_vecpParticleSetBaseEntity.size();
	for( i=0; i<num_base_entities; i++ )
	{
		if( !strcmp(pcTextureFilename, m_vecpParticleSetBaseEntity[i].GetTextureFilename().c_str()) )
		{
			// requested texture was found in the list - no need to load texture. just return the ID
			m_vecpParticleSetBaseEntity[i].AddRefCount();
			return i;
		}
	}

	// not found in the list - need to load as a new texture

//	if( m_vecpParticleSetBaseEntity.back().LoadTexture() )
	if( m_vecpParticleSetBaseEntity.back().GetTexture() != NULL )
	{
		// new texture has been successfully loaded
		return i;
	}
	else
	{
		g_Log.Print( "CParticleThreadManager::LoadTexture() - texture file '%s' was not found.", pcTextureFilename );
		m_vecpParticleSetBaseEntity.pop_back();
        return -1;	// texture file was not found
	}
}*/

/*
void CParticleThreadManager::
{
}
*/
