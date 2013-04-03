#ifndef __EntityCollGroups_H__
#define __EntityCollGroups_H__


namespace amorphous
{

/**
 * entity collision groups for physics simulator
 */
enum eEntityCollGroup
{
	ENTITY_COLL_GROUP_STATICGEOMETRY = 0,
	ENTITY_COLL_GROUP_PLAYER,
	ENTITY_COLL_GROUP_ITEM,
	ENTITY_COLL_GROUP_DOOR,
	ENTITY_COLL_GROUP_OTHER_ENTITIES,
	ENTITY_COLL_GROUP_NOCLIP,
	NUM_ENTITY_COLL_GROUPS
};

} // namespace amorphous


#endif /* __EntityCollGroups_H__ */