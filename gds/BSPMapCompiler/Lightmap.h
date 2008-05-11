#ifndef __MAPCOMPILER_LIGHTMAP_H__
#define __MAPCOMPILER_LIGHTMAP_H__


#include "fwd.h"

#include "3DMath/Matrix34.h"
#include "3DCommon/IndexedPolygon.h"
#include "3DCommon/FloatRGBColor.h"
#include "Support/2DArray.h"
#include "Graphics/Rect.h"
using namespace Graphics;


//==========================================================================================
// CLightmap
//==========================================================================================

/**
 - lightmap for a group of polygons
   - Positions of sampling points are calculated each time when GetPoint() is called and are not stored to local member variables
*/
class CLightmap
{
	SPlane m_Plane;

	Matrix34 m_GlobalPose;

	float m_fStartUp;
	float m_fStartRight;

	float m_fScaleU;
	float m_fScaleV;

	/// Bounding rectangle on the texture
	/// - Unit : texel
	/// - Does not include padding texels
	SRect m_Rect;

	/// indices to grouped faces
	/// grouped faces that are pointed by these indices share this lightmap
	std::vector<int> m_vecGroupedFaceIndex;

	// Positions of sampling points are calculated each time when GetPoint() is called
//	C2DArray<Vector3> m_vecvPoint;

	C2DArray<Vector3> m_vecvNormal;

//	C2DArray<char> m_vecTexelState;

	// whether a texel is on one of the stored polygons
//	C2DArray<bool> m_vecbValidTexel;
	C2DArray<char> m_vecbValidTexel;

	C2DArray<SFloatRGBColor> m_vecIntensity;

	/// holds average directions to light
//	C2DArray<char> m_vecTexelState_LightDirMap;

	C2DArray<Vector3> m_vecvLightDir;

	std::vector<CIndexedPolygon> *m_vecpPolygonBuffer;

private:

	std::vector<CIndexedPolygon>& GetPolygonBuffer() { return *m_vecpPolygonBuffer; }

public:

	CLightmap();

	void SetPolygonBuffer( std::vector<CIndexedPolygon> *vecpPolygonBuffer ) { m_vecpPolygonBuffer = vecpPolygonBuffer; }

	int GetNumPolygons() const { return (int)m_vecGroupedFaceIndex.size(); }

	/// returns a NON-const reference to a polygon
	CIndexedPolygon& GetPolygon( int index ) const { return (*m_vecpPolygonBuffer)[m_vecGroupedFaceIndex[index]]; }

	void Clear();

	/// Allocate buffer by the size width x height
	/// - The buffer for positions of each points is not allocated
	void SetSize( const int width, int const height );

	void AddFaceIndex( int i ) { m_vecGroupedFaceIndex.push_back(i); }

	std::vector<int>& GetGroupedFacesIndex() { return m_vecGroupedFaceIndex; }

//	inline int GetWidth() const ;
//	inline int GetHeight() const ;

	/// returns the number of texels on the lightmap
	inline int GetNumPoints_X() const { return m_Rect.GetWidth(); }
	inline int GetNumPoints_Y() const { return m_Rect.GetHeight(); }
	inline int GetNumPoints() const { return GetNumPoints_X() * GetNumPoints_Y(); }

	inline const SRect& GetRectangle() const { return m_Rect; }

	const SFloatRGBColor& GetTexelColor( int x, int y ) { return m_vecIntensity(x,y); }

	void SetTexelColor( int x, int y, const SFloatRGBColor& color ) { m_vecIntensity(x,y) = color; }

	void AddTexelColor( int x, int y, const SFloatRGBColor& color ) { m_vecIntensity(x,y) += color; }

	inline Vector3 GetPoint( int x, int y ) const;

	inline char ValidPoint( int x, int y ) { return m_vecbValidTexel(x,y); }

	/// get surface normal
	inline Vector3 GetNormal( int x, int y ) const { return m_vecvNormal(x,y); }

	inline Vector3 GetLightDirection( int x, int y ) const { return m_vecvLightDir(x,y); }

	inline Matrix34 GetWorldPose() const { return m_GlobalPose; }

	inline void SetLightDirection( int x, int y, Vector3 vLightDir ) { m_vecvLightDir(x,y) = vLightDir; }

	void SetNormal( int x, int y, const Vector3& vNormal ) { m_vecvNormal(x,y) = vNormal; }

	inline SPlane& GetPlane() { return m_Plane; }

//	void SetLightmapTextureIndexToFaces( int index );

	void SetTextureUV( const SRect& rRect,
					   const int iTextureWidth,
					   const int iTextureHeight,
					   const int tex_coord_index );

	void ComputeNormalsOnLightmap();

	void TransformLightDirectionToLocalFaceCoord();

	void ApplySmoothing();

	friend class CLightmapBuilder;
};

// =========================== inline implemenrations ===========================


inline Vector3 CLightmap::GetPoint( int x, int y ) const
{
	return m_GlobalPose.vPosition
		 + m_GlobalPose.matOrient.GetColumn(0) * m_fScaleU * ( (float)(x+0.5f) / (float)(m_Rect.GetWidth()) )
		 + m_GlobalPose.matOrient.GetColumn(1) * m_fScaleV * ( (float)(y+0.5f) / (float)(m_Rect.GetHeight()) );
}


#endif  /*  __MAPCOMPILER_LIGHTMAP_H__  */
