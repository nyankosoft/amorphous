#ifndef __RectSetMesh_HPP__
#define __RectSetMesh_HPP__


#include "gds/Support/MTRand.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"
#include "gds/Graphics/MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{


class CRectSetMesh : public CCustomMesh
{
protected:

	uint m_NumRects;

//	void CreateVertexElemenets( CMMA_VertexSet& rVertexSet );

public:

	CRectSetMesh();

	~CRectSetMesh() {}

	Result::Name Init( uint num_rects, U32 vertex_format_flags = VFF::POSITION | VFF::NORMAL | VFF::DIFFUSE_COLOR | VFF::TEXCOORD2_0 );

//	Result::Name LoadFromFile( const std::string& filename );

	bool LoadFromArchive( C3DMeshModelArchive& rArchive, const std::string& filename, U32 option_flags );

	void LoadVertices( void*& pVBData, C3DMeshModelArchive& archive );

	void SetRectMinMax( int rect_index, const Vector2& vMin, const Vector2& vMax );

	void SetRectMinMax( int rect_index, const Vector3& vMin, const Vector3& vMax );

	void SetRectPosition( int rect_index,
		const Vector3& vTopLeft,
		const Vector3& vTopRight,
		const Vector3& vBottomRight,
		const Vector3& vBottomLeft );

	void SetRectMinMax( int rect_index, float min_x, float min_y, float max_x, float max_y );

	void SetRectVertexPosition( int rect_index, int vert_index, const Vector2& vPos );

	void SetRectVertexPosition( int rect_index, int vert_index, const Vector3& vPos );

	void SetRectVertexPosition( int rect_index, int vert_index, const float x, const float y );

	void SetVertexPosition( int vert_index, const float x, const float y );

	Vector2 GetRectVertexPosition( int rect_index, int vert_index );

	/// normal

	void SetRectNormal( int rect_index, const Vector3& vNormal );

	/// texture coord

	void SetTextureCoordMinMax( int rect_index, float u_min, float v_min, float u_max, float v_max );

	void SetTextureCoordMinMax( int rect_index, const TEXCOORD2& vMin, const TEXCOORD2& vMax );

	void SetRectVertexTextureCoord( int rect_index, int vert_index, float u, float v );

	TEXCOORD2 GetTopLeftTextureCoord( int rect_index );

	TEXCOORD2 GetBottomRightTextureCoord( int rect_index );


	/// color

	void SetColor( const SFloatRGBAColor& color );

	void SetRectColor( int rect_index, const SFloatRGBAColor& color );

	void SetRectVertexColor( int rect_index, int vert_index, const SFloatRGBAColor& color );

	void SetColorARGB32( U32 argb32_color );

	void SetRectColorARGB32( int rect_index, U32 argb32_color );


	/// FVF vertex

	//	 virtual void SetRectVertex( int rect_index, int vert_index, const TLVERTEX& vSrc );

	//	 virtual void SetVertex( int vert_index, const TLVERTEX& vSrc );


	int GetNumRects();

	void SetNumRects( int num_rects );

	void AddRects( int num_rects );

	void ClearRects();
};


class CRectSetMeshGenerator
{
	C3DMeshModelArchive m_MeshArchive;

	float m_fParticleRadius;

	void SetTextureCoordinates( int num_particles, int num_tex_edge_divisions );

	void SetVertices( int num_particles );

	void SetIndices( int num_particles );

	inline void SetTexCoord( int rect_index, int vert_index, const TEXCOORD2& tex );

public:

	CRectSetMeshGenerator() {}
	~CRectSetMeshGenerator() {}

	bool Generate( int num_particles, float particle_radius, int num_tex_edge_divisions );

	bool GenerateForMultipleParticleGroups( int num_particles_per_group,
		                                    int num_particle_groups,
		                                    float particle_radius,
											int num_tex_edge_divisions );

	inline float GetRandRadius();

	C3DMeshModelArchive& GetMeshArchive() { return m_MeshArchive; }
};


inline float CRectSetMeshGenerator::GetRandRadius()
{
	float r = m_fParticleRadius;
	return r * RangedRand( 0.9f, 1.1f );
}

} // namespace amorphous



#endif  /* __RectSetMesh_HPP__ */
