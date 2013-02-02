#ifndef __BillboardArrayMeshGenerator_H__
#define __BillboardArrayMeshGenerator_H__


#include <string>

#include "Support/MTRand.hpp"
#include "Graphics/Direct3D/Mesh/D3DXMeshObject.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{


class BillboardArrayMesh : public CD3DXMeshObject
{
protected:

	void CreateVertexElemenets( CMMA_VertexSet& rVertexSet );

public:

	BillboardArrayMesh() {}

	~BillboardArrayMesh() {}

//	virtual HRESULT LoadFromFile( const std::string& filename );

	virtual bool LoadFromArchive( C3DMeshModelArchive& rArchive, const std::string& filename, U32 option_flags );

	virtual void LoadVertices( void*& pVBData, C3DMeshModelArchive& archive );
};


class BillboardArrayMeshGenerator
{
	C3DMeshModelArchive m_MeshArchive;

	float m_fParticleRadius;

	void SetTextureCoordinates( int num_particles, int num_tex_edge_divisions );

	void SetVertices( int num_particles );

	void SetIndices( int num_particles );

	inline void SetTexCoord( int rect_index, int vert_index, const TEXCOORD2& tex );

public:

	BillboardArrayMeshGenerator() {}
	~BillboardArrayMeshGenerator() {}

	bool Generate( int num_particles, float particle_radius, int num_tex_edge_divisions );

	bool GenerateForMultipleParticleGroups( int num_particles_per_group,
		                                    int num_particle_groups,
		                                    float particle_radius,
											int num_tex_edge_divisions );

	inline float GetRandRadius();

	C3DMeshModelArchive& GetMeshArchive() { return m_MeshArchive; }
};


inline float BillboardArrayMeshGenerator::GetRandRadius()
{
	float r = m_fParticleRadius;
	return r * RangedRand( 0.9f, 1.1f );
}

} // namespace amorphous



#endif  /* __BillboardArrayMeshGenerator_H__ */
