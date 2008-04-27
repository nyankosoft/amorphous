#ifndef __LWO2_SURFACE_H__
#define __LWO2_SURFACE_H__


#include "LWO2_Common.h"


#include <vector>
#include <string>
using namespace std;



// SBSP : Structure of Basic Surface Parameters

//=========================================================================================
// SBSP_VertexColorMap
//=========================================================================================

struct SBSP_VertexColorMap
{
	float fIntensity;
	int iEnvelope;
	UINT4 uiVMapType;
	string strName;

	SBSP_VertexColorMap()
	{
		fIntensity = 0;
		iEnvelope = 0;
		uiVMapType = 0;
	}
};



//=========================================================================================
// CLWO2_Surface
//=========================================================================================

class CLWO2_SurfaceBlock
{
public:

	string m_strOrdinalString;

	/// holds a texture channel ID
	UINT4 m_Channel;

	UINT4 m_ImageTag;

	string m_strUVMapName;

public:

	CLWO2_SurfaceBlock() { m_Channel = 0; m_ImageTag = 0; }

	UINT4 GetImageTag() const { return m_ImageTag; }
};


//=========================================================================================
// CLWO2_Surface
//=========================================================================================

class CLWO2_Surface
{
public:

	enum eBaseShadingValue
	{
		SHADE_DIFFUSE = 0,
		SHADE_SPECULAR,
		SHADE_LUMINOSITY,
		SHADE_REFLECTION,
		SHADE_TRANSPARENCY,
		SHADE_TRANSLUCENCY,
		NUM_BASE_SHADING_VALUES
	};

private:

	string m_strName;

//	string m_strUVMapName;
//	UINT4 imagetag;

	vector<CLWO2_SurfaceBlock> m_vecSurfaceBlock;

	string m_strComment;

	SBSP_VertexColorMap VMap;

	// TODO: save envelope values
	float m_afBaseShadingValue[NUM_BASE_SHADING_VALUES];

	float m_fMaxSmoothingAngle;	// [rad]

public:

	CLWO2_Surface();

//	bool operator==(CLWO2_Surface& surf){ return (memcmp(this, &surf, sizeof(CLWO2_Surface)) == 0); }
//	bool operator<(CLWO2_Surface& surf){return (strcmp(pName, surf.pName) < 0);}

	void Clear();

	void ReadOneSurface(UINT4 chunksize, FILE* fp);

	void ReadSurfaceBlock(UINT2 wBlockSize, FILE* fp);


	float GetMaxSmoothingAngle() { return m_fMaxSmoothingAngle; }	// returns angle below which smoothing should be applied to adjacent polygons 

	string& GetName() { return m_strName; }

//	string& GetUVMapName() { return m_strUVMapName; }

//	UINT4 GetImageTag() { return imagetag; }

	vector<CLWO2_SurfaceBlock>& GetSurfaceBlock() { return m_vecSurfaceBlock; }

	CLWO2_SurfaceBlock *GetSurfaceBlockByChannel( UINT4 uiChannelID );

	string& GetComment() { return m_strComment; }

	SBSP_VertexColorMap& GetVertexColorMap() { return VMap; }

	float GetBaseShadingValue( int iType ) { return m_afBaseShadingValue[iType]; }

};



#endif  /*  __LWO2_SURFACE_H__  */