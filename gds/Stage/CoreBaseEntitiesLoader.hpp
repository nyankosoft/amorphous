#ifndef __CoreBaseEntitiesLoader_HPP__
#define __CoreBaseEntitiesLoader_HPP__


#include <vector>
#include "fwd.hpp"


namespace amorphous
{


class CBE_Light;


class CCoreBaseEntitiesLoader
{
	void AddBlast( const char *name, float base_damage, float max_blast_radius, float blast_duration, float impulse, std::vector<CBaseEntity *>& pBaseEntities );

	void AddExplosion( const char *name, const char *blast_name, float anim_time_offset, std::vector<CBaseEntity *>& pBaseEntities );

	void AddDefaultLight( CBE_Light *pLight, const char *name, std::vector<CBaseEntity *>& pBaseEntities );

	void AddPhysicsBaseEntity( const char *name, std::vector<CBaseEntity *>& pBaseEntities );

//	void AddPhysicsObjects();

public:

	CCoreBaseEntitiesLoader(){}
	~CCoreBaseEntitiesLoader(){}

	void LoadCoreBaseEntities( std::vector<CBaseEntity *>& pBaseEntities );
};


} // amorphous



#endif /* __CoreBaseEntitiesLoader_HPP__ */
