#ifndef __RandomTerrainMeshGenerator_HPP__
#define __RandomTerrainMeshGenerator_HPP__


#include "amorphous/Utilities/PerlinAux.hpp"
#include "GridMeshGenerator.hpp"


namespace amorphous
{


class RandomTerrainMeshGenerator : public GridMeshGenerator
{
public:

	PerlinNoiseParams m_Params;

public:

	RandomTerrainMeshGenerator(
		float width,
		float height,
		unsigned int num_divisions_0,
		unsigned int num_divisions_1,
		unsigned int axis = 1,
		int sign = 1,
		bool double_sided = false
		);

	~RandomTerrainMeshGenerator(){}

	Result::Name Generate();
};


} // namespace amorphous


#endif /* __RandomTerrainMeshGenerator_HPP__ */
