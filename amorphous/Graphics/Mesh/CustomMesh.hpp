#ifndef  __CustomMesh_HPP__
#define  __CustomMesh_HPP__


#include <vector>
#include "amorphous/base.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/VertexFormat.hpp"
#include "amorphous/Graphics/General3DVertex.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{


class CustomMesh : public MeshImpl
{
public:

	enum VertexColorFormat
	{
		VCF_ARGB32,
		VCF_FRGBA,
		NUM_VERTEX_COLOR_FORMATS
	};

private:

	static VertexColorFormat ms_DefaultVertexDiffuseColorFormat;

	std::vector<CMMA_TriangleSet> m_TriangleSets;

public:

	U32 m_VertexFlags;
	uint m_VertexSize;
	uint m_IndexSize; ///< the index size in bytes (2 or 4). Default: 2 (U16)
	std::vector<uchar> m_VertexBuffer;
	std::vector<uchar> m_IndexBuffer;
	int m_ElementOffsets[VEE::NUM_VERTEX_ELEMENTS];

	/// Indices of z-sorted polygons. Used when RenderZSorted() is called.
	std::vector<uchar> m_ZSortedIndexBuffer;

	inline Vector3 GetVec3Element( unsigned int vertex_index, VEE::ElementName element ) const;

	inline void SetVec3Elements( const Vector3 *src, const unsigned int num_src_elements, VEE::ElementName element, U32 flag );

	inline void SetVec3Elements( const std::vector<Vector3>& src, VEE::ElementName element, U32 flag );

	inline void SetVec3Element( uint vertex_index, const Vector3& src, VEE::ElementName element, U32 flag );

	template<typename src_index_type, typename dest_index_type>
	inline void SetIndices( const std::vector<src_index_type>& src, uint dest_index_size );

public:

	/// used by physics::CMeshData
	U32 m_NumUpdatedVertices;

	/// used by physics::CMeshData
	U32 m_NumUpdatedIndices;

public:

	CustomMesh();

	U32 GetVertexFormatFlags() const { return m_VertexFlags; }

	uint GetVertexSize() const { return m_VertexSize; }

	int GetVertexElementOffset( VEE::ElementName vert_element ) const { return m_ElementOffsets[vert_element]; }

	void SetVertices( const std::vector<General3DVertex>& vertices, U32 vertex_format_flags );

	inline void AddVertices( const unsigned int num_vertices );

	inline Vector3 GetPosition( uint vertex_index ) const { return GetVec3Element( vertex_index, VEE::POSITION ); }
	inline Vector3 GetNormal(   uint vertex_index ) const { return GetVec3Element( vertex_index, VEE::NORMAL ); }
	inline Vector3 GetTangent(  uint vertex_index ) const { return GetVec3Element( vertex_index, VEE::TANGENT ); }
	inline Vector3 GetBinormal( uint vertex_index ) const { return GetVec3Element( vertex_index, VEE::BINORMAL ); }
	inline TEXCOORD2 Get2DTexCoord( uint vertex_index, int tex_coord_index ) const;
	inline SFloatRGBAColor GetDiffuseColor( uint vertex_index ) const;

	inline void SetPositions(   const Vector3 *positions, unsigned int num_elements ) { SetVec3Elements( positions, num_elements, VEE::POSITION, VFF::POSITION ); }
	inline void SetNormals(     const Vector3 *normals,   unsigned int num_elements ) { SetVec3Elements( normals,   num_elements, VEE::NORMAL,   VFF::NORMAL ); }
	inline void SetTangents(    const Vector3 *tangents,  unsigned int num_elements ) { SetVec3Elements( tangents,  num_elements, VEE::TANGENT,  VFF::TANGENT ); }
	inline void SetBinormals(   const Vector3 *binormals, unsigned int num_elements ) { SetVec3Elements( binormals, num_elements, VEE::BINORMAL, VFF::BINORMAL ); }
	inline void Set2DTexCoords( const TEXCOORD2 *tex_coords, unsigned int num_elements, int tex_coord_index );

	inline void SetPositions(   const std::vector<Vector3>& positions ) { SetVec3Elements( positions, VEE::POSITION, VFF::POSITION ); }
	inline void SetNormals(     const std::vector<Vector3>& normals )   { SetVec3Elements( normals,   VEE::NORMAL,   VFF::NORMAL ); }
	inline void SetTangents(    const std::vector<Vector3>& tangents )  { SetVec3Elements( tangents,  VEE::TANGENT,  VFF::TANGENT ); }
	inline void SetBinormals(   const std::vector<Vector3>& binormals ) { SetVec3Elements( binormals, VEE::BINORMAL, VFF::BINORMAL ); }
	inline void Set2DTexCoords( const std::vector<TEXCOORD2>& tex_coords, int tex_coord_index );

	inline void SetPosition(     uint vertex_index, const Vector3& position )   { SetVec3Element( vertex_index, position, VEE::POSITION, VFF::POSITION ); }
	inline void SetNormal(       uint vertex_index, const Vector3& normal )     { SetVec3Element( vertex_index, normal,   VEE::NORMAL,   VFF::NORMAL ); }
	inline void SetTangent(      uint vertex_index, const Vector3& tangent )    { SetVec3Element( vertex_index, tangent,  VEE::TANGENT,  VFF::TANGENT ); }
	inline void SetBinormal(     uint vertex_index, const Vector3& binormal )   { SetVec3Element( vertex_index, binormal, VEE::BINORMAL, VFF::BINORMAL ); }
	inline void Set2DTexCoord(   uint vertex_index, const TEXCOORD2& tex_coord, int tex_coord_index );
	inline void SetDiffuseColor( uint vertex_index, const SFloatRGBAColor& color );

	/// Sets the same normal to all the vertices. Used to render a single rectangle.
	void SetNormals( const Vector3& normal );

	void SetDiffuseColors( const std::vector<SFloatRGBAColor>& diffuse_colors );

	void SetDiffuseColors( const SFloatRGBAColor& diffuse_color );

	void SetBlendWeights( const std::vector< TCFixedVector<float,CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX> >& vecfMatrixWeight );

	void SetBlendIndices( const std::vector< TCFixedVector<int,CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX> >& veciMatrixIndex );

	inline void GetPositions( std::vector<Vector3>& dest ) const;

	void InitVertexBuffer( int num_vertices, U32 vertex_format_flags );

	inline void InitIndexBuffer( int num_indices, uint index_size = sizeof(U16) );

	uchar *GetVertexBufferPtr() { return (0 < m_VertexBuffer.size()) ? &(m_VertexBuffer[0]) : NULL; }

	uchar *GetIndexBufferPtr() { return (0 < m_IndexBuffer.size()) ? &(m_IndexBuffer[0]) : NULL; }

	const uchar *GetVertexBufferPtr() const { return (0 < m_VertexBuffer.size()) ? &(m_VertexBuffer[0]) : NULL; }

	const uchar *GetIndexBufferPtr() const { return (0 < m_IndexBuffer.size()) ? &(m_IndexBuffer[0]) : NULL; }

	const uchar *GetZSortedIndexBufferPtr() const { return (0 < m_ZSortedIndexBuffer.size()) ? &(m_ZSortedIndexBuffer[0]) : NULL; }

	uint GetNumVertices() const { return (0 < m_VertexSize) ? ((uint)m_VertexBuffer.size() / m_VertexSize) : 0; }

	void GetVertexPositions( std::vector<Vector3>& dest_vertices ) const { GetPositions( dest_vertices ); }

	uint GetNumTriangles() const { return GetNumIndices() / 3; }

	uint GetNumIndices() const { return (uint)m_IndexBuffer.size() / sizeof(U16); }

	void GetVertexIndices( std::vector<unsigned int>& dest );

	uint GetIndexSize() const { return m_IndexSize; }

	/// Returns the i-th vertex index
	U16 GetIndex( int i ) const
	{
		U16 dest = 0;
		const unsigned int index_size = m_IndexSize;
		memcpy( &dest, &(m_IndexBuffer[0]) + sizeof(U16) * i, sizeof(U16) );
		return dest;
	}

	const std::vector<CMMA_TriangleSet>& GetTriangleSets() const { return m_TriangleSets; }

	void SetTriangleSets( const std::vector<CMMA_TriangleSet>& triangle_sets ) { m_TriangleSets = triangle_sets; }

	template<typename src_index_type>
	inline void SetIndices( const std::vector<src_index_type>& src, uint dest_index_size = sizeof(U16) );

	inline void CopyVertexTo( uint src_vertex_index, CustomMesh& dest, uint dest_vertex_index ) const;

//	void SetIndices( const std::vector<U32>& src );

	/// Returns true on success
	bool LoadFromFile( const std::string& mesh_archive_filepath );

	bool LoadFromArchive( C3DMeshModelArchive& archive ) { return LoadFromArchive( archive, "", 0 ); }

	bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

	void Render();

	void Render( ShaderManager& rShaderMgr );

	void RenderZSorted( ShaderManager& rShaderMgr );

	void RenderSubset( ShaderManager& rShaderMgr, int material_index );

	static void SetDefaultVertexDiffuseColorFormat( VertexColorFormat vcf ) { ms_DefaultVertexDiffuseColorFormat = vcf; }
};

//================================ inline implementations ================================

inline Vector3 CustomMesh::GetVec3Element( unsigned int vertex_index, VEE::ElementName element ) const
{
//	U32 flag = VFF::TEXCOORD2_0 << tex_coord_index;

	if( /*!(m_VertexFlags & flag) ||*/ m_VertexBuffer.size() <= vertex_index )
		return Vector3(0,0,0);

	Vector3 v(Vector3(0,0,0));
	memcpy( &v, &(m_VertexBuffer[0]) + m_VertexSize * vertex_index + m_ElementOffsets[element], sizeof(Vector3) );
	return v;
}


inline void CustomMesh::SetVec3Elements( const Vector3 *src, const unsigned int num_src_elements, VEE::ElementName element, U32 flag )
{
	if( !(m_VertexFlags & flag) || m_VertexBuffer.empty() )
		return;

	const uint vertex_size = m_VertexSize;
	for( unsigned int i=0; i<num_src_elements; i++ )
		memcpy( &(m_VertexBuffer[0]) + vertex_size * i + m_ElementOffsets[element], &(src[i]), sizeof(Vector3) );
}


inline void CustomMesh::SetVec3Elements( const std::vector<Vector3>& src, VEE::ElementName element, U32 flag )
{
	if( src.empty() )
		return;

	SetVec3Elements( &(src[0]), (unsigned int)src.size(), element, flag );
}


inline void CustomMesh::SetVec3Element( uint vertex_index, const Vector3& src, VEE::ElementName element, U32 flag )
{
	if( !(m_VertexFlags & flag) || m_VertexBuffer.empty() )
		return;

	const uint vertex_size = m_VertexSize;
	memcpy( &(m_VertexBuffer[0]) + vertex_size * vertex_index + m_ElementOffsets[element], &src, sizeof(Vector3) );
}


inline void CustomMesh::AddVertices( const unsigned int num_vertices )
{
	if( m_VertexFlags == 0 || m_VertexSize == 0 )
		return;

	m_VertexBuffer.insert( m_VertexBuffer.end(), m_VertexSize * num_vertices, 0 );
}


inline TEXCOORD2 CustomMesh::Get2DTexCoord( unsigned int vertex_index, int tex_coord_index ) const
{
	U32 flag = VFF::TEXCOORD2_0 << tex_coord_index;

	if( !(m_VertexFlags & flag) || m_VertexBuffer.size() <= vertex_index )
		return TEXCOORD2(0,0);

	TEXCOORD2 tex_coord( TEXCOORD2(0,0) );
	int offset = m_ElementOffsets[VEE::TEXCOORD2_0 + tex_coord_index];
	memcpy( &tex_coord, &(m_VertexBuffer[0]) + m_VertexSize * vertex_index + offset, sizeof(TEXCOORD2) );
	return tex_coord;
}


inline SFloatRGBAColor CustomMesh::GetDiffuseColor( unsigned int vertex_index ) const
{
	if( !(m_VertexFlags & VFF::DIFFUSE_COLOR) || m_VertexBuffer.size() <= vertex_index )
		return SFloatRGBAColor::White();

	SFloatRGBAColor color( SFloatRGBAColor::White() );
	int offset = m_ElementOffsets[VEE::DIFFUSE_COLOR];

	if( ms_DefaultVertexDiffuseColorFormat == VCF_ARGB32 )
	{
		U32 argb32( 0xFFFFFFFF );
		memcpy( &argb32, &(m_VertexBuffer[0]) + m_VertexSize * vertex_index + offset, sizeof(U32) );
		color.SetARGB32( argb32 );
	}
	else if( ms_DefaultVertexDiffuseColorFormat == VCF_FRGBA )
	{
		memcpy( &color, &(m_VertexBuffer[0]) + m_VertexSize * vertex_index + offset, sizeof(SFloatRGBAColor) );
	}

	return color;
}


inline void CustomMesh::Set2DTexCoords( const TEXCOORD2 *tex_coords, unsigned int num_elements, int tex_coord_index )
{
	if( !tex_coords )
		return;

	int element = VEE::TEXCOORD2_0 + tex_coord_index;
	U32 flag = VFF::TEXCOORD2_0 << tex_coord_index;

	if( !(m_VertexFlags & flag) || m_VertexBuffer.empty() )
		return;

	const int offset = m_ElementOffsets[element];
	for( unsigned int i=0; i<num_elements; i++ )
		memcpy( &(m_VertexBuffer[0]) + m_VertexSize * i + offset, &(tex_coords[i]), sizeof(TEXCOORD2) );
}


inline void CustomMesh::Set2DTexCoords( const std::vector<TEXCOORD2>& tex_coords, int tex_coord_index )
{
	if( tex_coords.empty() )
		return;

	Set2DTexCoords( &(tex_coords[0]), (unsigned int)tex_coords.size(), tex_coord_index );
}


inline void CustomMesh::Set2DTexCoord( uint vertex_index, const TEXCOORD2& tex_coord, int tex_coord_index )
{
	int element = VEE::TEXCOORD2_0 + tex_coord_index;
	U32 flag = VFF::TEXCOORD2_0 << tex_coord_index;

	if( !(m_VertexFlags & flag) || m_VertexBuffer.empty() )
		return;

	const int offset = m_ElementOffsets[element];
	memcpy( &(m_VertexBuffer[0]) + m_VertexSize * vertex_index + offset, &tex_coord, sizeof(TEXCOORD2) );
}


inline void CustomMesh::SetDiffuseColor( uint vertex_index, const SFloatRGBAColor& diffuse_color )
{
	const int offset = m_ElementOffsets[VEE::DIFFUSE_COLOR];
	const uint write_pos = m_VertexSize * vertex_index + offset;

	if( ms_DefaultVertexDiffuseColorFormat == VCF_ARGB32 )
	{
		if( m_VertexBuffer.size() < write_pos + sizeof(U32) )
			return;

		U32 argb32 = diffuse_color.GetARGB32();
		memcpy( &(m_VertexBuffer[0]) + write_pos, &(argb32), sizeof(U32) );
	}
	else if( ms_DefaultVertexDiffuseColorFormat == VCF_FRGBA )
	{
		if( m_VertexBuffer.size() < write_pos + sizeof(SFloatRGBAColor) )
			return;

		memcpy( &(m_VertexBuffer[0]) + write_pos, &diffuse_color, sizeof(SFloatRGBAColor) );
	}
}


inline void CustomMesh::GetPositions( std::vector<Vector3>& dest ) const
{
	const uint num_verts = GetNumVertices();
	dest.resize( num_verts );
	const uint offset = m_ElementOffsets[VEE::POSITION];
	for( uint i=0; i<num_verts; i++ )
		memcpy( &(dest[i]), &(m_VertexBuffer[0]) + m_VertexSize * i + offset, sizeof(Vector3) );
}


void CustomMesh::InitIndexBuffer( int num_indices, uint index_size )
{
	m_IndexSize = index_size;
	m_IndexBuffer.resize( num_indices * index_size );
}


template<typename src_index_type,typename dest_index_type>
inline void CustomMesh::SetIndices( const std::vector<src_index_type>& src, uint dest_index_size )
{
	const size_t num_indices = src.size();

	InitIndexBuffer( (int)num_indices, dest_index_size );

	for( size_t i=0; i<num_indices; i++ )
	{
		dest_index_type dest_index = (dest_index_type)src[i];
		memcpy( &(m_IndexBuffer[0]) + i * dest_index_size, &dest_index, dest_index_size );
	}
}


template<typename src_index_type>
inline void CustomMesh::SetIndices( const std::vector<src_index_type>& src, uint dest_index_size )
{
	switch( m_IndexSize )
	{
	case 2: SetIndices<src_index_type,U16>( src, dest_index_size ); break;
	case 4: SetIndices<src_index_type,U32>( src, dest_index_size ); break;
	default:
		break;
	}
}


inline void CustomMesh::CopyVertexTo( uint src_vertex_index, CustomMesh& dest, uint dest_vertex_index ) const
{
	memcpy(
		dest.GetVertexBufferPtr() + dest.GetVertexSize() * dest_vertex_index,
		GetVertexBufferPtr() + GetVertexSize() * src_vertex_index,
		GetVertexSize()
		);
}

} // namespace amorphous



#endif /* __CustomMesh_HPP__ */
