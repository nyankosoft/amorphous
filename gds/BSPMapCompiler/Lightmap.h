#ifndef __MAPCOMPILER_LIGHTMAP_H__
#define __MAPCOMPILER_LIGHTMAP_H__


#include "Graphics/Rect.h"
using namespace Graphics;

#include "3DMath/Matrix34.h"

#include "3DCommon/FloatRGBColor.h"

#include "Support/2DArray.h"

#include "MapFace.h"

class CLightmapBuilder;


//==========================================================================================
// CLightmap
//==========================================================================================

class CLightmap
{
	SPlane m_Plane;

	Matrix34 m_GlobalPose;

	float m_fStartUp;
	float m_fStartRight;

	float m_fScaleU;
	float m_fScaleV;

	/// bounding rectangle (does not include padding texels)
	SRect m_Rect;

	/// indices to grouped faces
	/// grouped faces that are pointed by these indices share this lightmap
	vector<int> m_vecGroupedFaceIndex;

	C2DArray<Vector3> m_vecvPoint;

	C2DArray<Vector3> m_vecvNormal;

//	C2DArray<char> m_vecTexelState;

	// whether a texel is on one of the stored polygons
//	C2DArray<bool> m_vecbValidTexel;
	C2DArray<char> m_vecbValidTexel;

	C2DArray<SFloatRGBColor> m_vecIntensity;

	/// holds average directions to light
//	C2DArray<char> m_vecTexelState_LightDirMap;

	C2DArray<Vector3> m_vecvLightDir;

public:

	CLightmap();

	void Clear();

	void SetSize( const int width, int const height );

	void AddFaceIndex( int i ) { m_vecGroupedFaceIndex.push_back(i); }

	vector<int>& GetGroupedFacesIndex() { return m_vecGroupedFaceIndex; }

//	inline int GetWidth() const ;
//	inline int GetHeight() const ;

	/// returns the number of texels on the lightmap
	inline int GetNumPoints_X() const { return m_Rect.GetWidth(); }
	inline int GetNumPoints_Y() const { return m_Rect.GetHeight(); }
	inline int GetNumPoints() const { return GetNumPoints_X() * GetNumPoints_Y(); }

	inline SRect& GetRectangle() { return m_Rect; }

	SFloatRGBColor& GetTexelColor( int x, int y ) { return m_vecIntensity(x,y); }

	void SetTexelColor( int x, int y, const SFloatRGBColor& color ) { m_vecIntensity(x,y) = color; }

	inline Vector3 GetPoint( int x, int y ) const;

//	inline bool ValidPoint( int x, int y ) { return m_vecbValidTexel(x,y); }
	inline char ValidPoint( int x, int y ) { return m_vecbValidTexel(x,y); }

	/// get surface normal
	inline Vector3 GetNormal( int x, int y ) { return m_vecvNormal(x,y); }

	inline Vector3 GetLightDirection( int x, int y ) { return m_vecvLightDir(x,y); }

	inline Matrix34 GetWorldPose() const { return m_GlobalPose; }

	inline void SetLightDirection( int x, int y, Vector3 vLightDir ) { m_vecvLightDir(x,y) = vLightDir; }

	void SetNormal( int x, int y, const Vector3& vNormal ) { m_vecvNormal(x,y) = vNormal; }

	inline SPlane& GetPlane() { return m_Plane; }

	void SetLightmapTextureIndexToFaces( int index, vector<CMapFace>& rvecFace );

	void SetTextureUV( SRect& rRect,
					   vector<CMapFace>& rvecFace,
					   const int iTextureWidth,
					   const int iTextureHeight );

	void ComputeNormalsOnLightmap( vector<CMapFace>& rvecFace );

	void TransformLightDirectionToLocalFaceCoord( vector<CMapFace>& rvecFace );

	void ApplySmoothing();

	friend class CLightmapBuilder;
};


inline Vector3 CLightmap::GetPoint( int x, int y ) const
{
	return m_GlobalPose.vPosition
		 + m_GlobalPose.matOrient.GetColumn(0) * m_fScaleU * ( (float)(x+0.5f) / (float)(m_Rect.GetWidth()) )
		 + m_GlobalPose.matOrient.GetColumn(1) * m_fScaleV * ( (float)(y+0.5f) / (float)(m_Rect.GetHeight()) );
}

#endif  /*  __MAPCOMPILER_LIGHTMAP_H__  */