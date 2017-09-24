#include "RandomTerrainMeshGenerator.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/GraphicsResources.hpp"


namespace amorphous
{


RandomTerrainMeshGenerator::RandomTerrainMeshGenerator(
	float width,
	float height,
	unsigned int num_divisions_0,
	unsigned int num_divisions_1,
	unsigned int axis,
	int sign,
	bool double_sided
)
:
GridMeshGenerator(width,height,num_divisions_0,num_divisions_1,axis,sign,double_sided)
{
}


Result::Name RandomTerrainMeshGenerator::Generate()
{
	// First, generate the grid mesh
	Result::Name res = GridMeshGenerator::Generate();

	if( res != Result::SUCCESS )
		return res;

	// Then, change the height of the vertices to transform
	// the grid mesh into a terrain mesh.

	const PerlinNoiseParams params = m_Params;

	Perlin pn(
		params.octaves,
		params.freq,
		params.amp,
		params.seed
		);

	float width  = m_fWidth;
	float height = m_fHeight;

	std::vector<Vector3>& positions = m_MeshArchive.GetVertexSet().vecPosition;

	float (*pPerlinFunc) (Perlin&,float,float,float,float);
	pPerlinFunc = params.tilable ? TilablePerlin : StdPerlin;

	const size_t num_vertices = positions.size();
	for( size_t i=0; i<num_vertices; i++ )
	{
		float x = positions[i].x;
		float z = positions[i].z;

		float fx = (float)x / (float)width;
		float fz = (float)z / (float)height;

		float f = pPerlinFunc( pn, fx, fz, 1.0f, 1.0f );

		positions[i].y = f;
	}

	return Result::SUCCESS;
}


} // namespace amorphous
