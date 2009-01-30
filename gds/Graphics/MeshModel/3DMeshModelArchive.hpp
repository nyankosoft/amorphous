#ifndef  __3DMESHMODELARCHIVE_H__
#define  __3DMESHMODELARCHIVE_H__


#include <string>
#include <vector>

#include "3DMath/Vector3.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/TextureCoord.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "Graphics/32BitColor.hpp"

#include "Support/Serialization/SerializationEx.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;


#include "Support/2DArray.hpp"


//namespace GameLib1
//{

namespace MeshModel
{


/// CMMA_ - Class of Mesh Model Archive

//=========================================================================================
// CMMA_VertexSet
//=========================================================================================

class CMMA_VertexSet : public IArchiveObjectBase
{
public:

	enum eProperty
	{
		NUM_MAX_BLEND_MATRICES_PER_VERTEX = 4,
	};

	enum eVertexFlag
	{
		VF_POSITION			= (1 << 0),
		VF_NORMAL			= (1 << 1),
		VF_DIFFUSE_COLOR	= (1 << 2),
		VF_2D_TEXCOORD		= (1 << 3),
		VF_BUMPMAP			= (1 << 4), ///< usually requires tangent and binormal vector for each vertex. VF_NORMAL must be raised when the flag
		VF_WEIGHT			= (1 << 5),
		VF_2D_TEXCOORD0		= VF_2D_TEXCOORD,
		VF_2D_TEXCOORD1		= (1 << 6),
		VF_2D_TEXCOORD2		= (1 << 7),
		VF_2D_TEXCOORD3		= (1 << 8),
		VF_COLORVERTEX		= VF_POSITION|VF_NORMAL|VF_DIFFUSE_COLOR,
		VF_TEXTUREVERTEX	= VF_POSITION|VF_NORMAL|VF_DIFFUSE_COLOR|VF_2D_TEXCOORD,
		VF_WEIGHTVERTEX		= VF_POSITION|VF_NORMAL|VF_DIFFUSE_COLOR|VF_2D_TEXCOORD|VF_WEIGHT,
		VF_BUMPVERTEX		= VF_POSITION|VF_NORMAL|VF_DIFFUSE_COLOR|VF_2D_TEXCOORD|VF_BUMPMAP,
		VF_BUMPWEIGHTVERTEX	= VF_POSITION|VF_NORMAL|VF_DIFFUSE_COLOR|VF_2D_TEXCOORD|VF_BUMPMAP|VF_WEIGHT,
		VF_SHADOWVERTEX		= VF_POSITION|VF_NORMAL,
		VF_SHADOWWEIGHTVERTEX	= VF_POSITION|VF_NORMAL|VF_WEIGHT
	};

	CMMA_VertexSet() : m_VertexFormatFlag(0) {}

	int GetNumVertices() const { return (int)vecPosition.size(); }

	unsigned int GetVertexFormat() const { return m_VertexFormatFlag; }

	void SetVertexFormat( const unsigned int flag ) { m_VertexFormatFlag = flag; }

	void RaiseVertexFormatFlag( const unsigned int flag ) { m_VertexFormatFlag |= flag; }

	void GetBlendMatrixIndices_4Bytes( int iVertexIndex, unsigned char *pIndices );

	void GetBlendMatrixWeights_4Floats( int iVertexIndex, float *pWeight );

	void Resize( int i );

	void Clear();

//	inline void Addvertex( CGeneral3DVertex& src_vertex );

	/// convert and put the vertices to an array of CGeneral3DVertex
	void GetVertices( std::vector<CGeneral3DVertex>& dest_buffer ) const;

	void Serialize( IArchive& ar, const unsigned int version );

public:

	unsigned int m_VertexFormatFlag;

	std::vector<Vector3> vecPosition;

	std::vector<Vector3> vecNormal;

	std::vector<Vector3> vecBinormal;
	std::vector<Vector3> vecTangent;

	std::vector<SFloatRGBAColor> vecDiffuseColor;

	TCFixedVector< vector<TEXCOORD2>, 4 > vecTex;

	std::vector< TCFixedVector<float,NUM_MAX_BLEND_MATRICES_PER_VERTEX> > vecfMatrixWeight;

	std::vector< TCFixedVector<int,NUM_MAX_BLEND_MATRICES_PER_VERTEX> > veciMatrixIndex;

/*	TCFixedVector< vector<float>, 4 > vecfMatrixWeight;
	TCFixedVector< vector <int>, 4 > veciMatrixIndex;
*/

	friend class C3DMeshModelBuilder;
};

/*
inline void CMMA_VertexSet::Addvertex( CGeneral3DVertex& src_vertex )
{
	if( m_VertexFormatFlag & VF_POSITION )
		vecPosition.push_back( src_vertex.m_vPosition );

	if( m_VertexFormatFlag & VF_NORMAL )
		vecNormal.push_back( src_vertex.m_vNormal );

	if( m_VertexFormatFlag & VF_DIFFUSE_COLOR )
		vecDiffuseColor.push_back( src_vertex.m_DiffuseColor );

	...

}*/


//=========================================================================================
// CMMA_TriangleSet
//=========================================================================================

/// store values for arguments in DrawIndexedPrimitive()
class CMMA_TriangleSet : public IArchiveObjectBase
{
public:

	int m_iStartIndex;

	/// MinIndex & NumVertex arguments in DrawIndexedPrimitive() are used
	/// as hints for Direct3D to optimize memory access during software vertex processing.
	/// The following two member variables correspond to each of the above two arguments in
	/// DrawIndexPrimitive()
	int m_iMinIndex;
	int m_iNumVertexBlocksToCover;

	/// primitive count
	/// triangles are rendered as triangle lists
	int m_iNumTriangles;

	/// bounding box that contains the triangles in this triangle set
	AABB3 m_AABB;

public:

	void Serialize( IArchive& ar, const unsigned int version );

	virtual unsigned int GetVersion() const { return 1; }
};



//=========================================================================================
// CMMA_Texture
//=========================================================================================

class CMMA_Texture : public IArchiveObjectBase
{
public:

	enum eTextureType
	{
		FILENAME,
		ARCHIVE_32BITCOLOR,
		ARCHIVE_FLOATCOLOR
	};

	unsigned int type;

	std::string strFilename;

	C2DArray<S32BitColor> vecTexelData;
	
	C2DArray<SFloatRGBAColor> vecfTexelData;

public:

	CMMA_Texture() : type(FILENAME) {}

	void Serialize( IArchive& ar, const unsigned int version );

};



//=========================================================================================
// CMMA_Material
//=========================================================================================

class CMMA_Material : public IArchiveObjectBase
{
public:

	std::string Name;

	float fSpecular;

//	SFloatRGBAColor Diffuse;
//	SFloatRGBAColor Ambient;
//	SFloatRGBAColor Specular;
//	SFloatRGBAColor Emissive;

	std::vector<CMMA_Texture> vecTexture;

	/// minimum alpha value of diffuse colors of vertices that belong to the material
	float fMinVertexDiffuseAlpha;

//	CMMA_Texture SurfaceTexture;
//	CMMA_Texture NormalMapTexture;

public:

	CMMA_Material() { SetDefault(); }

	void Serialize( IArchive& ar, const unsigned int version );

	void SetDefault();

	// version 1: changed from SurfaceTexture & NormalMapTexture
	// to vecTexture
	virtual unsigned int GetVersion() const { return 3; }
};



//=========================================================================================
// CMMA_Bone
//=========================================================================================

class CMMA_Bone : public IArchiveObjectBase
{
public:
	std::string strName;

	Vector3 vLocalOffset;

	/// transforms vertices from model space to local bone space
	Matrix34 BoneTransform;

	std::vector<CMMA_Bone> vecChild;

	int GetNumBones_r() const;

	void Serialize( IArchive& ar, const unsigned int version );
};



//=========================================================================================
// C3DMeshModelArchive
//=========================================================================================


/// - Create image archives and add them to db
/// - Change the texture file names stored in m_vecMeshArchive
/// - Texture image is not saved if it already exists in the db
extern void AddTexturesToBinaryDatabase( C3DMeshModelArchive& mesh_archive,
								         const std::string& db_filepath,
								         CBinaryDatabase<std::string> &db,
								         bool bUseTextureBasenameForKey = true );


class C3DMeshModelArchive : public IArchiveObjectBase
{
	CMMA_VertexSet m_VertexSet;

	/// indices to draw polygons as triangle lists
	std::vector<unsigned int> m_vecVertexIndex;

	/// stores surface property
	std::vector<CMMA_Material> m_vecMaterial;

	std::vector<CMMA_TriangleSet> m_vecTriangleSet;

	CMMA_Bone m_SkeletonRootBone;

	AABB3 m_AABB;

public:

	C3DMeshModelArchive();

	~C3DMeshModelArchive();

	CMMA_VertexSet& GetVertexSet() { return m_VertexSet; }

	std::vector<CMMA_TriangleSet>& GetTriangleSet() { return m_vecTriangleSet; }

	unsigned int GetNumVertexIndices() const { return (unsigned int)m_vecVertexIndex.size(); }

	std::vector<unsigned int>& GetVertexIndex() { return m_vecVertexIndex; }

	std::vector<CMMA_Material>& GetMaterial() { return m_vecMaterial; }

	CMMA_Bone& GetSkeletonRootBone() { return m_SkeletonRootBone; }

	/// copy a hierarchical structure from another mesh archive
	void CopySkeletonFrom( C3DMeshModelArchive& rMesh );

	int GetNumBones() { return GetSkeletonRootBone().GetNumBones_r(); }

	void SetMaterial( unsigned int index, CMMA_Material& rMaterial );

	const AABB3& GetAABB() const { return m_AABB; }

	void Scale( float factor );

	/// calculates axis-aligned bounding boxes for each triangle set
	void UpdateAABBs();

	void UpdateMinimumVertexDiffuseAlpha();

	void GeneratePointRepresentatives( vector<unsigned short>& rvecusPtRep);

	void WriteToTextFile( const string& filename );


	void Serialize( IArchive& ar, const unsigned int version );

	virtual unsigned int GetVersion() const { return 2; }

	friend class C3DMeshModelBuilder;

};


} // namespace MeshModel

//}	/*  GameLib1  */


#endif		/*  __3DMeshModelArchive_H__  */
