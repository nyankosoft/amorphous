#ifndef  __STATICGEOMETRYARCHIVEFG_H__
#define  __STATICGEOMETRYARCHIVEFG_H__


#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "gds/Graphics/MeshModel/3DMeshModelArchive.hpp"
using namespace MeshModel;


class CMeshSubset : public IArchiveObjectBase
{
public:

	int MeshIndex;
//	int MaterialIndex;
	std::vector<int> vecMaterialIndex;
	int ShaderIndex;
	int ShaderTechniqueIndex;
	int ProjectionMatrixIndex;

	AABB3 AABB;

public:

	CMeshSubset()
		:
	MeshIndex(0),
//	MaterialIndex(0),
	ShaderTechniqueIndex(0),
	ProjectionMatrixIndex(0)
	{}

	const AABB3& GetAABB() const { return AABB; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & MeshIndex;
		ar & vecMaterialIndex;
		ar & ShaderIndex;
		ar & ShaderTechniqueIndex;
		ar & ProjectionMatrixIndex;
		ar & AABB;
	}
};


class CStaticGeometryMeshGroup : public IArchiveObjectBase
{
public:

	/// meshes to be rendered as the group [StartIndex,EndIndex]
	int m_StartIndex;
	int m_EndIndex; ///< index of the last mesh (inclusive)

	std::vector<CShaderTechniqueHandle> m_vecShaderTechnique;

	/// depth shift value = near-clip dist * m_fDepthShiftFactor
	/// higher shift value increases the likelihood that polygons
	/// go behind others
	float m_fDepthShiftFactor;

public:

	CStaticGeometryMeshGroup() : m_StartIndex(0), m_EndIndex(0), m_fDepthShiftFactor(0) {}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_StartIndex & m_EndIndex;
		ar & m_vecShaderTechnique;
		ar & m_fDepthShiftFactor;
	}
};


class CTriangleMeshArchive : public IArchiveObjectBase
{
public:

	/// vertices for triangles
	std::vector<Vector3> vecVertex;

	/// index for vertices
	/// every 3 indices represent vertices that makes a triangle
	std::vector<int> vecIndex;

	/// index to material for each triangle
	std::vector<int> vecMaterialIndex;

public:

	void Scale( float factor )
	{
		size_t i, num_vertices = vecVertex.size();
		for( i=0; i<num_vertices; i++ )
		{
			vecVertex[i] * factor;
		}
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & vecVertex & vecIndex & vecMaterialIndex;
	}
};


class CStaticGeometryArchiveFG : public IArchiveObjectBase
{
public:

	/// [0]: terrain
	/// [1]: buildings (shader technique name hard-coded to "BuildingsFG")
	std::vector<CStaticGeometryMeshGroup> m_vecMeshGroup;

	// mesh archives for static geometry
	// e.g.) terrain, static buildings, etc.
//	std::vector<C3DMeshModelArchive> m_vecMeshArchive;
	std::vector<std::string> m_vecMeshArchiveKey;

	// mesh archive for skybox
	C3DMeshModelArchive m_SkyboxMeshArchive;

	/// triangle mesh for collision (A)
	/// - takes some time to compile when there are many vertices and indices
	/// - registered to physics simulator
	CTriangleMeshArchive m_CollisionMeshArchive;

	/// usu. "CollisionMesh"
	std::string m_CollisionMeshArchiveKey;

	/// triangle mesh for collision (B)
	/// - stores the filename of the collision mesh archive
	/// - MUST be an archive of CBSPTreeForTriangleMesh
	/// - resistered to physics simulator
	std::string m_CollisionMeshArchiveFilename;

	/// aabb that contains all the terrain & building meshes
	AABB3 m_AABB;

	SFloatRGBAColor m_AmbientColor;

	SFloatRGBAColor m_FogColor;

	float m_FogStartDist;

	float m_FarClipDist;

//	vector<unsigned char> m_TileTextureData;

	std::string m_strTileTextureFilename;

	float m_fTileTextureBlendWeight;

//	std::string m_ShaderTechnique;

	SFloatRGBColor m_SpecularColor;

	float m_fSpecularIntensity;

public:

	CStaticGeometryArchiveFG();

	AABB3 GetAABB() const { return m_AABB; }

	void Scale( float factor );

	void WriteToTextFile( const std::string& filename );

	virtual unsigned int GetVersion() const { return 2; }

	void Serialize( IArchive& ar, const unsigned int version );

	const static std::string ms_CollisionMeshTreeKey;
};


#endif  /*  __STATICGEOMETRYARCHIVEFG_H__  */
