#ifndef __LWO2_LAYER_H__
#define __LWO2_LAYER_H__


#include "fwd.hpp"
#include "LWO2_Surface.hpp"

#include "3DMath/Vector3.hpp"

#include "Graphics/FloatRGBAColor.hpp"

#include "Support/memory_helpers.hpp"


#include <vector>
#include <list>
#include <string>


// TODO: support multiple PNTS / POLS chunks in a layer


#define LWO2_INVALID_POLYGON_INDEX	-1


/**
 * holds an index to the most recent PNTS chunk and the corresponding uv coordinates (u,v)
 */
struct SIndexAndUV
{
	/// index to the most recent PNTS chunk
	int i;

	/// index to a polygon in the most recent POLS chunk? ( used in VMAD chunk )
	int PolygonIndex;

	float u;
	float v;

	SIndexAndUV() : i(0), PolygonIndex(LWO2_INVALID_POLYGON_INDEX), u(0), v(0) {}

	bool operator<( const SIndexAndUV& iuv ) { return ( this->i < iuv.i ); };
};



//=========================================================================================
// SVertexColor_LWO2
//=========================================================================================

/*
 * a pair of index and ARGB color for a vertex
 */
struct SVertexColor_LWO2
{
	/// index to a point in the most recent PNTS chunk
	int iIndex;

    /// index to a polygon in the most recent POLS chunk? ( used in VMAD chunk )
	int iPolygonIndex;

	float fAlpha, fRed, fGreen, fBlue;

	SVertexColor_LWO2() { iIndex = 0; iPolygonIndex = LWO2_INVALID_POLYGON_INDEX; fAlpha = fRed = fGreen = fBlue = 0.0f; }

	bool operator<( const SVertexColor_LWO2& vert_color_map ) { return ( this->iIndex < vert_color_map.iIndex ); };


	/// return value has the same format as D3DCOLOR
	UINT4 GetColor_UINT4_ARGB();
};


//=========================================================================================
// CLWO2_VertexColorMap
//=========================================================================================

class CLWO2_VertexColorMap
{
public:

	std::string strName;

	int iNumIndices;

	SVertexColor_LWO2 *paVertexColor;

public:

	CLWO2_VertexColorMap() : iNumIndices(0), paVertexColor(NULL) {}

	~CLWO2_VertexColorMap() { Release(); }

	void Release() { iNumIndices = 0; SafeDelete(paVertexColor); }

	std::string& GetName() { return strName; }

	inline bool FindVertexColor( UINT4 pnt_index, SFloatRGBAColor& rDestColor );

	CLWO2_VertexColorMap(const CLWO2_VertexColorMap& vc_map);
	CLWO2_VertexColorMap operator=(CLWO2_VertexColorMap vc_map);
};



//==================================================================================
// CLWO2_TextureUVMap
//==================================================================================

class CLWO2_TextureUVMap
{
public:

	std::string strName;

	std::vector<SIndexAndUV> vecIndexUV;

	CLWO2_TextureUVMap() {}

	~CLWO2_TextureUVMap(){}

};



//==================================================================================
// CLWO2_PointSelectionSet
//==================================================================================

class CLWO2_PointSelectionSet
{
	std::string m_strName;
	std::vector<UINT4> m_vecPointIndex;

public:

	CLWO2_PointSelectionSet() {}

	const std::string& GetName() const { return m_strName; }

	std::vector<UINT4>& GetPointIndex() { return m_vecPointIndex; }

	int GetNumPoints() const { return (int)m_vecPointIndex.size(); }

	void SetName( const char *pcName ) { m_strName = pcName; }

	friend class CLWO2_Layer;
};


/**
 *   stores polygon info
 */
class CLWO2_Face
{
	std::vector<UINT4> m_vecPointIndex;

	int m_iSurfaceIndex;

	int m_iPartIndex;

	Vector3 m_vFaceNormal;

public:

	CLWO2_Face(){ m_iSurfaceIndex = 0; m_iPartIndex = -1; m_vFaceNormal = Vector3(0,0,0); }

//	CLWO2_Face(const CLWO2_Face& face);
	~CLWO2_Face() {}

	inline int GetNumPoints() const { return (int)m_vecPointIndex.size(); }

	inline std::vector<UINT4>& GetVertexIndex() { return m_vecPointIndex; }

	inline UINT4 GetVertexIndex( int i ) const { return m_vecPointIndex[i]; }

	inline Vector3 GetFaceNormal() const { return m_vFaceNormal; }

	inline int GetSurfaceIndex() const { return m_iSurfaceIndex; }

	inline int GetPartIndex() const { return m_iPartIndex; }

	friend class CLWO2_Layer;
};


// represents named groups of polygons (Parts)
class CLWO2_PolygonGroup
{
public:

//	std::string m_strName;

	/// indices to the faces in the most recent POLS chunk
	std::vector<UINT4> m_vecPolygonIndex;

	/// tag indices
	std::vector<UINT2> m_vecTag;

//	CLWO2_PolygonGroup() {}
//	string& GetName() { return m_strName; }
	friend class CLWO2_Layer;
};



class CLWO2_WeightMap
{
	std::string m_strName;

	std::vector<int> m_vecPntIndex;
	std::vector<float> m_vecfWeight;

public:

	const std::string& GetName() const { return m_strName; }

	int GetNumMaps() const { return (int)m_vecPntIndex.size(); }

	inline void GetWeightMap( int index, UINT4& iPntIndex, float& fWeight );

	/// finds a weight value for a vertex. returns true if the weight is found
	inline bool FindWeight( UINT4 pnt_index, float& rfDestWeight );

	friend class CLWO2_Layer;
};


class CLWO2_Bone
{
	std::string m_strName;
	UINT4 m_aiPointIndex[2];

public:

	const std::string& GetName() const { return m_strName; }
	UINT4 GetVertexIndex( int i ) { return m_aiPointIndex[i]; }

	friend class CLWO2_Layer;
};


class CLWO2_BoneWeightMap
{
public:
	UINT4 iBoneIndex;
	UINT4 iWeightMapTagIndex;
};


//==================================================================================
// CLWO2_Layer
//==================================================================================

class CLWO2_Layer
{
	int m_iLayerIndex;

	std::string m_strLayerName;

	/// buffer to store vertices
	std::vector<Vector3> m_vecPoint;

	std::vector<CLWO2_Face> m_vecFace;

	std::vector<CLWO2_TextureUVMap> m_vecTexuvmap;

	std::vector<CLWO2_TextureUVMap> m_vecTexVMAD;

	std::vector<CLWO2_VertexColorMap> m_vecVertexColorMap;

	std::vector<CLWO2_PointSelectionSet> m_vecPointSelectionSet;

	std::vector<CLWO2_PolygonGroup> m_vecPolygonGroup;

	std::vector<CLWO2_WeightMap> m_vecVertexWeightMap;

	std::vector<CLWO2_Bone> m_vecBone;

	/// binds bone indices and tag indices of the correspnding weight maps
	std::vector<CLWO2_BoneWeightMap> m_vecBoneWeightMap;

	std::vector<Vector3> m_vecVertexNormal;

public:

	CLWO2_Layer() { m_iLayerIndex = 0; }

//	CLWO2_Layer(const CLWO2_Layer& layer);

	~CLWO2_Layer() {}

	void ReadLayerChunk(UINT4& chunksize, FILE* fp);

	void ReadVertices(UINT4& chunksize, FILE* fp);

	void ReadVertexMap(UINT4& chunksize, FILE* fp);

	void ReadVMADChunk(UINT4& chunksize, FILE* fp);

	UINT4 ReadPols(UINT4& chunksize, FILE* fp);

	void ReadPTAG(UINT4& ptagsize, CLWO2_Object& rObject, FILE* fp);

	bool GetUV( float& u, float& v, int iPointIndex, CLWO2_TextureUVMap *pTexUVMap );

	Vector3 GetInterpolatedNormal( CLWO2_Face& rFace, int iPntIndex );

	void ComputeFaceNormals();

	void ComputeVertexNormals();


	std::string& GetName() { return m_strLayerName; }

	int GetLayerIndex() const { return m_iLayerIndex; }

	std::vector<Vector3>& GetVertex() { return m_vecPoint; }

	std::vector<Vector3>& GetVertexNormal() { return m_vecVertexNormal; }

	std::vector<CLWO2_Face>& GetFace() { return m_vecFace; }

	std::vector<CLWO2_PolygonGroup>& GetPolygonGroup() { return m_vecPolygonGroup; }

	const std::string& GetPolygonGroupName();

	std::vector<CLWO2_TextureUVMap>& GetTextureUVMap() { return m_vecTexuvmap; }

	std::vector<CLWO2_VertexColorMap>& GetVertexColorMap() { return m_vecVertexColorMap; }

	std::vector<CLWO2_WeightMap>& GetVertexWeightMap() { return m_vecVertexWeightMap; }

	std::vector<CLWO2_PointSelectionSet>& GetPointSelectionSet() { return m_vecPointSelectionSet; }

	bool GetVertexColor( SFloatRGBAColor& color, const int iPntIndex, CLWO2_Surface& rSurf );

	std::vector<CLWO2_Bone>& GetBone() { return m_vecBone; }

//	std::vector<Vector3>& GetVertexNormal() { return m_vecVertexNormal; }

	std::vector<CLWO2_BoneWeightMap>& GetBoneWeightMap() { return m_vecBoneWeightMap; }

	bool operator==(CLWO2_Layer& layer){ return ( m_iLayerIndex == layer.m_iLayerIndex ); }
	bool operator<(CLWO2_Layer& layer){return ( m_iLayerIndex < layer.m_iLayerIndex );}

};


// --------------------- inline implementations ---------------------

inline void CLWO2_WeightMap::GetWeightMap( int index, UINT4& iPntIndex, float& fWeight )
{
	iPntIndex = m_vecPntIndex[index];
	fWeight = m_vecfWeight[index];
}


inline bool CLWO2_VertexColorMap::FindVertexColor( UINT4 pnt_index, SFloatRGBAColor& rDestColor )
{
	int i, num_maps = iNumIndices;	//GetNumMaps();
	for( i=0; i<num_maps; i++ )
	{
		if( paVertexColor[i].iIndex == pnt_index )
		{
			// found the corresponding color for the vertex
			rDestColor.SetRGBA( paVertexColor[i].fRed,
				                paVertexColor[i].fGreen,
								paVertexColor[i].fBlue,
								paVertexColor[i].fAlpha );
			return true;
		}
	}

	return false;
}


inline bool CLWO2_WeightMap::FindWeight( UINT4 pnt_index, float& rfDestWeight )
{
	int i, num_maps = GetNumMaps();
	for( i=0; i<num_maps; i++ )
	{
		if( m_vecPntIndex[i] == pnt_index )
		{
			// found the weight value for 'pnt_index'
			rfDestWeight = m_vecfWeight[i];
			return true;
		}
	}

	return false;
}



#endif  /*  __LWO2_LAYER_H__  */
