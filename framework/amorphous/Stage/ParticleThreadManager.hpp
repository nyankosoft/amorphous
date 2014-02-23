
#ifndef  __ParticleThreadManager_H__
#define  __ParticleThreadManager_H__


#include <vector>
#include <string>

#include "Support/singleton.hpp"


namespace amorphous
{


class CBE_ParticleSet;

/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
*/
class CParticleThreadManager
{
private:

	std::vector<CBE_ParticleSet *> m_vecpParticleSetBaseEntity;

protected:
///	CParticleThreadManager();		//singleton

	static singleton<CParticleThreadManager> m_obj;

public:

	static CParticleThreadManager* Get() { return m_obj.get(); }

	CParticleThreadManager();

	virtual ~CParticleThreadManager();

	void AddParticleSetBaseEntity( CBE_ParticleSet *pBaseEntity );
};

} // namespace amorphous



#endif		/*  __ParticleThreadManager_H__  */
