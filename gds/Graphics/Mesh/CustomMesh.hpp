#ifndef  __CustomMesh_HPP__
#define  __CustomMesh_HPP__


#include <vector>
#include "gds/base.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/VertexFormat.hpp"
#include "gds/Graphics/General3DVertex.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/MeshModel/3DMeshModelArchive.hpp"
using namespace MeshModel;


class CCustomMesh : public CMeshImpl
{
public:

	enum VertexColorFormat
	{
		VCF_ARGB32,
		VCF_FRGBA,
		NUM_VERTEX_COLOR_FORMATS
	};

private:

	typedef U16 dest_index_type;

	static VertexColorFormat ms_DefaultVertexDiffuseColorFormat;

	std::vector<CMMA_TriangleSet> m_TriangleSets;

public:

	U32 m_VertexFlags;
	uint m_VertexSize;
	std::vector<uchar> m_VertexBuffer;
	std::vector<uchar> m_IndexBuffer;
	int m_ElementOffsets[VEE::NUM_VERTEX_ELEMENTS];

	inline void SetVec3Elements( const std::vector<Vector3>& src, VEE::ElementName element, U32 flag );

public:

	/// used by physics::CMeshData
	U32 m_NumUpdatedVertices;

	/// used by physics::CMeshData
	U32 m_NumUpdatedIndices;

public:

	CCustomMesh();

	U32 GetVertexFormatFlags() const { return m_VertexFlags; }

	uint GetVertexSize() const { return m_VertexSize; }

	int GetVertexElementOffset( VEE::ElementName vert_element ) const { return m_ElementOffsets[vert_element]; }

	void SetVertices( const std::vector<CGeneral3DVertex>& vertices, U32 vertex_format_flags );

	Vector3 GetPosition( int i ) { Vector3 v(Vector3(0,0,0)); memcpy( &v, &(m_VertexBuffer[0]) + m_VertexSize * i, sizeof(Vector3) ); return v; }

	inline void SetPositions( const std::vector<Vector3>& positions )  { SetVec3Elements( positions, VEE::POSITION, VFF::POSITION ); }
	inline void SetNormals( const std::vector<Vector3>& normals )      { SetVec3Elements( normals,   VEE::NORMAL,   VFF::NORMAL ); }
	inline void SetTangents( const std::vector<Vector3>& tangents )    { SetVec3Elements( tangents,  VEE::TANGENT,  VFF::TANGENT ); }
	inline void SetBinormals( const std::vector<Vector3>& binormals )  { SetVec3Elements( binormals, VEE::BINORMAL, VFF::BINORMAL ); }
	inline void Set2DTexCoords( const std::vector<TEXCOORD2>& tex_coords, int tex_coord_index )
	{
		const std::vector<TEXCOORD2>& src = tex_coords;
		int element = VEE::TEXCOORD2_0 + tex_coord_index;
		U32 flag = VFF::TEXCOORD2_0 << tex_coord_index;

		if( !(m_VertexFlags & flag) || m_VertexBuffer.empty() )
			return;

		const int num = (int)src.size();
		const int offset = m_ElementOffsets[element];
		for( int i=0; i<num; i++ )
			memcpy( &(m_VertexBuffer[0]) + m_VertexSize * i + offset, &(src[i]), sizeof(TEXCOORD2) );
	}

	void SetDiffuseColors( const std::vector<SFloatRGBAColor>& diffuse_colors );

	inline void GetPositions( std::vector<Vector3>& dest )
	{
		const uint num_verts = GetNumVertices();
		dest.resize( num_verts );
		const uint offset = m_ElementOffsets[VEE::POSITION];
		for( uint i=0; i<num_verts; i++ )
			memcpy( &(dest[i]), &(m_VertexBuffer[0]) + m_VertexSize * i + offset, sizeof(Vector3) );
	}

	void InitVertexBuffer( int num_vertices, U32 vertex_format_flags );

	void InitIndexBuffer( int num_indices, uint index_size = sizeof(U16) ) { m_IndexBuffer.resize( num_indices * index_size ); }

	uchar *GetVertexBufferPtr() { return (0 < m_VertexBuffer.size()) ? &(m_VertexBuffer[0]) : NULL; }

	uchar *GetIndexBufferPtr() { return (0 < m_IndexBuffer.size()) ? &(m_IndexBuffer[0]) : NULL; }

	const uchar *GetVertexBufferPtr() const { return (0 < m_VertexBuffer.size()) ? &(m_VertexBuffer[0]) : NULL; }

	const uchar *GetIndexBufferPtr() const { return (0 < m_IndexBuffer.size()) ? &(m_IndexBuffer[0]) : NULL; }

	uint GetNumVertices() const { return (0 < m_VertexSize) ? ((uint)m_VertexBuffer.size() / m_VertexSize) : 0; }

	uint GetNumIndices() const { return (uint)m_IndexBuffer.size() / sizeof(U16); }

	uint GetIndexSize() const { return sizeof(dest_index_type); }

	/// Returns the i-th vertex index
	U16 GetIndex( int i ) const
	{
		U16 dest = 0;
		memcpy( &dest, &(m_IndexBuffer[0]) + sizeof(U16) * i, sizeof(U16) );
		return dest;
	}

	template<typename src_index_type>
	inline void SetIndices( const std::vector<src_index_type>& src );

//	void SetIndices( const std::vector<U32>& src );

	/// Returns true on success
	bool LoadFromFile( const std::string& mesh_archive_filepath );

	bool LoadFromArchive( C3DMeshModelArchive& archive ) { return LoadFromArchive( archive, "", 0 ); }

	bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

	void Render();

	void Render( CShaderManager& rShaderMgr );

	void RenderSubset( CShaderManager& rShaderMgr, int material_index ) {}

	static void SetDefaultVertexDiffuseColorFormat( VertexColorFormat vcf ) { ms_DefaultVertexDiffuseColorFormat = vcf; }
};

//================================ inline implementations ================================

inline void CCustomMesh::SetVec3Elements( const std::vector<Vector3>& src, VEE::ElementName element, U32 flag )
{
	if( !(m_VertexFlags & flag) || m_VertexBuffer.empty() )
		return;

	const uint vertex_size = m_VertexSize;
	const int num = (int)src.size();
	for( int i=0; i<num; i++ )
		memcpy( &(m_VertexBuffer[0]) + vertex_size * i + m_ElementOffsets[element], &(src[i]), sizeof(Vector3) );
}


template<typename src_index_type>
inline void CCustomMesh::SetIndices( const std::vector<src_index_type>& src )//, uint dest_index_size )
{
	const size_t dest_index_size = sizeof(dest_index_type);
	const size_t num_indices = src.size();
	m_IndexBuffer.resize( dest_index_size * num_indices );
	for( size_t i=0; i<num_indices; i++ )
	{
		dest_index_type dest_index = (dest_index_type)src[i];
		memcpy( &(m_IndexBuffer[0]) + i * dest_index_size, &dest_index, dest_index_size );
	}
}



#endif /* __CustomMesh_HPP__ */
