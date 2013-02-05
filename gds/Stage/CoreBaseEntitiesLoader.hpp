#ifndef __CoreBaseEntitiesLoader_HPP__
#define __CoreBaseEntitiesLoader_HPP__


#include <vector>
#include "fwd.hpp"


namespace amorphous
{


class CBE_Light;


class CoreBaseEntitiesLoader
{
	void AddBlast( const char *name, float base_damage, float max_blast_radius, float blast_duration, float impulse, std::vector<BaseEntity *>& pBaseEntities );

	void AddExplosion( const char *name, const char *blast_name, float anim_time_offset, std::vector<BaseEntity *>& pBaseEntities );

	void AddDefaultLight( CBE_Light *pLight, const char *name, std::vector<BaseEntity *>& pBaseEntities );

	void AddPhysicsBaseEntity( const char *name, std::vector<BaseEntity *>& pBaseEntities );

//	void AddPhysicsObjects();

public:

	CoreBaseEntitiesLoader(){}
	~CoreBaseEntitiesLoader(){}

	void LoadCoreBaseEntities( std::vector<BaseEntity *>& pBaseEntities );
};


} // amorphous



#endif /* __CoreBaseEntitiesLoader_HPP__ */
