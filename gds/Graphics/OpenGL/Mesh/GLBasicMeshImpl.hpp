#ifndef __GLBasicMeshImpl_HPP__
#define __GLBasicMeshImpl_HPP__


#include "gds/base.hpp"
#include <gl/gl.h>											// Header File For The OpenGL32 Library
#include <gl/glu.h>											// Header File For The GLu32 Library
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/Mesh/MeshFactory.hpp"
#include "gds/Graphics/MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{
using namespace serialization;


class CGLBasicMeshImpl : public CMeshImpl
{
private:

	bool m_VBOSupported;

	// Mesh Data
//	int				m_nVertexCount;								// Vertex Count

	// Vertex Buffer Object Names
	GLuint m_PositionBuffer;							// Vertex VBO Name
	GLuint m_NormalBuffer;							    // vertex normal?
	GLuint m_DiffuseColorBuffer;						// diffuse color?
	GLuint m_TexCoordBuffer;							// Texture Coordinate VBO Name

	GLuint m_IndexBuffer;

	std::vector<int> m_vecClientState;

	std::vector<CMMA_TriangleSet> m_vecTriangleSet;

	int m_NumIndices;

	std::vector<unsigned short> m_vecIndex;

private:

	void BuildVBOs( C3DMeshModelArchive& archive );

	void BuildVBOs_ARB( C3DMeshModelArchive& archive );

	void BindBuffers_ARB();

public:

	CGLBasicMeshImpl();

	CGLBasicMeshImpl( const std::string& filename );

	virtual ~CGLBasicMeshImpl();

	void Release();

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

	bool LoadFromFile( const std::string& filename, U32 option_flags = 0 );

//	bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, std::vector<D3DVERTEXELEMENT9>& vecVertexElement );

	bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, U32 vertex_format_flags );

	/// render object by using the fixed function pipeline
	void Render();

	virtual void Render( CShaderManager& rShaderMgr );

	virtual void RenderSubset( CShaderManager& rShaderMgr, int material_index );

	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex,
								std::vector<CShaderTechniqueHandle>& vecShaderTechnique );

	virtual void RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */);
};

/*
class CGLProgressiveMeshImpl : public CGLBasicMeshImpl
{
public:

	CGLProgressiveMeshImpl();

	virtual ~CGLProgressiveMeshImpl() {}
};


class CGLSkeletalMeshImpl : public CGLProgressiveMeshImpl
{
public:
};
*/

class CGLMeshImplFactory : public CMeshImplFactory
{
public:

//	CMeshImpl* CreateMeshImpl( CMeshType::Name mesh_type );

	CMeshImpl* CreateBasicMeshImpl();
	CMeshImpl* CreateProgressiveMeshImpl();
	CMeshImpl* CreateSkeletalMeshImpl();
};


} // namespace amorphous



#endif	/*  __GLBasicMeshImpl_HPP__  */
