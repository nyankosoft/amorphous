#ifndef __LWO2_SURFACE_H__
#define __LWO2_SURFACE_H__


#include <vector>
#include <string>
#include "LWO2_Common.hpp"


namespace amorphous
{


// SBSP : Structure of Basic Surface Parameters

//=========================================================================================
// SBSP_VertexColorMap
//=========================================================================================

struct SBSP_VertexColorMap
{
	float fIntensity;
	int iEnvelope;
	UINT4 uiVMapType;
	std::string strName;

	SBSP_VertexColorMap()
	{
		fIntensity = 0;
		iEnvelope = 0;
		uiVMapType = 0;
	}
};



//=========================================================================================
// LWO2_Surface
//=========================================================================================

class LWO2_SurfaceBlock
{
public:

	std::string m_strOrdinalString;

	/// holds a texture channel ID
	UINT4 m_Channel;

	UINT4 m_ImageTag;

	std::string m_strUVMapName;

public:

	LWO2_SurfaceBlock() { m_Channel = 0; m_ImageTag = 0; }

	UINT4 GetImageTag() const { return m_ImageTag; }
};


//=========================================================================================
// LWO2_Surface
//=========================================================================================

class LWO2_Surface
{
public:

	enum eBaseShadingValue
	{
		SHADE_DIFFUSE = 0,
		SHADE_SPECULAR,
		SHADE_GLOSSINESS,
		SHADE_LUMINOSITY,
		SHADE_REFLECTION,
		SHADE_TRANSPARENCY,
		SHADE_TRANSLUCENCY,
		NUM_BASE_SHADING_VALUES
	};

private:

	std::string m_strName;

//	string m_strUVMapName;
//	UINT4 imagetag;

	std::vector<LWO2_SurfaceBlock> m_vecSurfaceBlock;

	std::string m_strComment;

	SBSP_VertexColorMap VMap;

	// TODO: save envelope values
	float m_afBaseShadingValue[NUM_BASE_SHADING_VALUES];

	float m_fMaxSmoothingAngle;	///< [rad]

public:

	LWO2_Surface();

	void Clear();

	void ReadOneSurface(UINT4 chunksize, FILE* fp);

	void ReadSurfaceBlock(UINT2 wBlockSize, FILE* fp);

	/// Returns the angle below which smoothing should be applied to adjacent polygons 
	float GetMaxSmoothingAngle() const { return m_fMaxSmoothingAngle; }

	const std::string& GetName() const { return m_strName; }

//	string& GetUVMapName() { return m_strUVMapName; }

//	UINT4 GetImageTag() { return imagetag; }

	const std::vector<LWO2_SurfaceBlock>& GetSurfaceBlock() const { return m_vecSurfaceBlock; }

	std::vector<LWO2_SurfaceBlock>& GetSurfaceBlock() { return m_vecSurfaceBlock; }

	const LWO2_SurfaceBlock *GetSurfaceBlockByChannel( UINT4 uiChannelID ) const;

	const std::string& GetComment() const { return m_strComment; }

	const SBSP_VertexColorMap& GetVertexColorMap() const { return VMap; }

	SBSP_VertexColorMap& GetVertexColorMap() { return VMap; }

	float GetBaseShadingValue( int iType ) const { return m_afBaseShadingValue[iType]; }

};

} // amorphous



#endif  /*  __LWO2_SURFACE_H__  */
