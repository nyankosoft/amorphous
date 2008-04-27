
#ifndef __MAPFACE_H__
#define __MAPFACE_H__


#include "BaseFace.h"



class CMapFace : public CFace
{
	int m_iTypeFlag;

public:
	short m_sTextureID;
	short m_sLightMapID;
	short m_sNode;
	bool m_bFlag;
	char m_acSurfaceName[128];
	int m_iSurfaceMaterialIndex;
	int m_iPolygonIndex;

	vector<D3DXVECTOR3> m_vecvLightDir;

public:

	CMapFace();
	CMapFace(const CMapFace& face);
	CMapFace operator=(CMapFace face);

	MAPVERTEX& GetMapVertex(int iIndex);
	void Split(CFace& front, CFace& back, SPlane& cutplane);
	D3DXVECTOR3 GetInterpolatedNormal( D3DXVECTOR3& rvPosition );
	bool IsInSweptVolume( D3DXVECTOR3& rvPosition, float& fDist );

	inline int GetSurfaceIndex() { return m_iSurfaceMaterialIndex; }

	inline bool IsCollidable();

	inline void SetLightmapTextureUV( int iVertexIndex, const TEXCOORD2& texcoord );
	inline void SetLightmapTextureIndex( int index ) { m_sLightMapID = index; }

	/// get transformation into this face space based on the first texture coordinates
	/// used for bump mapping
	void GetTransformationToLocalFaceCoord_Tex0( D3DXMATRIX& rMat );

	void GetTransformationToTangentSpace_Tex0( D3DXMATRIX& rMat );

	/// calculate transformation into tangent space
	/// and save it to 'vNormal' and 'vTangent' in each vertex
	void CalculateTangentSpace_Tex0();

	/// raises particular type flag
	void RaiseTypeFlag( int flag ) { m_iTypeFlag |= flag; }

	/// clears particular type flag
	void ClearTypeFlag( int flag ) { m_iTypeFlag &= ~flag; }

	/// reads particular type flag
	int ReadTypeFlag( int flag ) { return ( m_iTypeFlag & flag ); }

	int GetTypeFlag() { return m_iTypeFlag; }


	enum eMapFaceTypeFlag
	{
		TYPE_NOCLIP			= (1 << 0),
		TYPE_INVISIBLE		= (1 << 1),
		TYPE_LIGHTSOURCE	= (1 << 2),
		TYPE_SMOOTHSURFACE	= (1 << 3),

		TYPE_MAIN			= (1 << 4),
		TYPE_INTERIOR		= (1 << 5),
	};

};


//=========================== inline implementations ===========================

void CMapFace::SetLightmapTextureUV( int iVertexIndex, const TEXCOORD2& texcoord )
{
	m_pVertices[iVertexIndex].vTex1 = texcoord;
}


inline bool CMapFace::IsCollidable()
{
	if( GetTypeFlag() & TYPE_NOCLIP || GetTypeFlag() & TYPE_LIGHTSOURCE )
		return false;
	else
		return true;
}


#endif  /*  __MAPFACE_H__  */
