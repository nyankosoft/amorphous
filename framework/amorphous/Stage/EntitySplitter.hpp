#ifndef __EntitySplitter_HPP__
#define __EntitySplitter_HPP__


#include "CopyEntity.hpp"


namespace amorphous
{


class MeshSplitResults;


class EntitySplitterParams
{
public:
	std::string front_entity_name_suffix;
	std::string back_entity_name_suffix;
	bool destroy_source_entity;
public:
	EntitySplitterParams()
		:
	front_entity_name_suffix("-front"),
	back_entity_name_suffix("-back"),
	destroy_source_entity(true)
	{}
};


/**
	\brief Split an entity into two.

	Usage:

	StageMiscUtility util;
	EntityHandle<> box = util.CreateBox( ... );
	EntitySplitter entity_splitter;
	Plane split_plane;
	EntityHandle<> split_box0, split_box1;
	SplitSurfaceParams params;
	entity_splitter( box, split_plane, params, split_box0, split_box1 );

	*** Considerations ***
	1. Who should destroy the entity to be split?
	  - Client code or EntitySplitter::Split()?
	  -> Client code. how to deal with overlaps with the split entities.
	    - Requires the client code to make the entity to a 'non-cllidable' one prior to calling EntitySplitter::Split()?


*/
class EntitySplitter
{
	Result::Name EntitySplitter::CreateEntities(
		std::shared_ptr<CCopyEntity> pSourceEntity,
		const Plane& split_plane,
		const MeshSplitResults& split_results,
		const EntitySplitterParams& params,
		EntityHandle<>& dest0,
		EntityHandle<>& dest1
	);

public:
	EntitySplitter(){}
	~EntitySplitter(){}

	/**
	\param[in]  src         An entity to be split
	\param[in]  split_plane A plane to split the entity across
	\param[in]  params
	\param[out] dest0       A split entity
	\param[out] dest1       A split entity
	*/
	Result::Name Split( EntityHandle<>& src, const Plane& split_plane, const EntitySplitterParams& params, EntityHandle<>& dest0, EntityHandle<>& dest1 );
};


} // namespace amorphous


#endif /* __EntitySplitter_HPP__ */
