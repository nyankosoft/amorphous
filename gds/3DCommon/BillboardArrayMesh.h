#ifndef __BillboardArrayMeshGenerator_H__
#define __BillboardArrayMeshGenerator_H__


#include <string>

#include "GameCommon/MTRand.h"
#include "3DCommon/D3DXMeshObject.h"
#include "3DCommon/FVF_BillboardVertex.h"
#include "3DCommon/MeshModel/3DMeshModelArchive.h"
using namespace MeshModel;


class CBillboardArrayMesh : public CD3DXMeshObject
{
protected:

	void CreateVertexElemenets( CMMA_VertexSet& rVertexSet );

public:

	CBillboardArrayMesh() {}

	~CBillboardArrayMesh() {}

//	virtual HRESULT LoadFromFile( const std::string& filename );

	virtual bool LoadFromArchive( C3DMeshModelArchive& rArchive, const std::string& filename, U32 option_flags );

	virtual void LoadVertices( void*& pVBData, C3DMeshModelArchive& archive );
};


class CBillboardArrayMeshGenerator
{
	C3DMeshModelArchive m_MeshArchive;

	float m_fParticleRadius;

	void SetTextureCoordinates( int num_particles, int num_tex_edge_divisions );

	void SetVertices( int num_particles );

	void SetIndices( int num_particles );

	inline void SetTexCoord( int rect_index, int vert_index, const TEXCOORD2& tex );

public:

	CBillboardArrayMeshGenerator() {}
	~CBillboardArrayMeshGenerator() {}

	bool Generate( int num_particles, float particle_radius, int num_tex_edge_divisions );

	bool GenerateForMultipleParticleGroups( int num_particles_per_group,
		                                    int num_particle_groups,
		                                    float particle_radius,
											int num_tex_edge_divisions );

	inline float GetRandRadius();

	C3DMeshModelArchive& GetMeshArchive() { return m_MeshArchive; }
};


inline float CBillboardArrayMeshGenerator::GetRandRadius()
{
	float r = m_fParticleRadius;
	return r * RangedRand( 0.9f, 1.1f );
}


#endif  /* __BillboardArrayMeshGenerator_H__ */
