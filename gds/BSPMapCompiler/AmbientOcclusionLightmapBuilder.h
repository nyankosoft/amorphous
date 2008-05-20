#ifndef  __AMBIENTOCCLUSIONLIGHTMAPBUILDER_H__
#define  __AMBIENTOCCLUSIONLIGHTMAPBUILDER_H__

//#include "LightingForLightmap.h"


//#include "Support/StatusDisplay/StatusDisplay.h"
//#include "Support/StatusDisplay/StatusDisplayRenderer_D3DX.h"

#include <vector>

#include "Lightmap.h"
//#include "MapFace.h"

#include "3DCommon/fwd.h"
#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/LightStructs.h"
#include "3DCommon/ShaderHandle.h"
#include "3DCommon/MeshObjectHandle.h"
#include "3DCommon/MeshModel/3DMeshModelArchive.h"
using namespace MeshModel;

//#include "3DMath/PolygonMesh.h"

class LightmapDesc;
//class CPolygonMesh;

/*
enum eFlag
{
	LF_IGNORE_ANGLE_FACTOR				= (1 << 0),				// ignore angle attenuation
	LF_CREATE_LIGHT_DIRECTION_TEXTURE	= (1 << 1),	// create light direction maps (for bump mapping)
	LF_USE_HEMISPHERIC_LIGHT			= (1 << 2),	// use hemispheric lighting to create lightmaps
};
*/


//==========================================================================================
// AmbientOcclusionLightmapBuilder
//==========================================================================================

class AmbientOcclusionLightmapBuilder
{
protected:

	enum ShaderTechnique
	{
		TECH_ENVIRONMENT_LIGHT,
		TECH_OCCULUSION_MESH,
		NUM_TECHNIQUES
	};

	std::vector<CMeshObjectHandle> m_vecMesh;
//	std::vector<CD3DXMeshObjectBase *> m_vecpMesh;

	CMeshObjectHandle m_EnvLightMesh;
//	CD3DXMeshObjectBase *m_pEnvLightMesh;

	CShaderHandle m_Shader;


	unsigned int m_OptionFlag;
	
	int m_NumTotalLightmapTexels;
	
	int m_NumProcessedLightmapTexels;

	int m_NumRenderTargetTexels;

protected:

	void CalculateLightmapTexelIntensity( CLightmap& rLightmap, CTextureRenderTarget& tex_render_target );

	inline SFloatRGBColor GetEnvLightIntensity( CTextureRenderTarget& tex_render_target );

public:

	inline AmbientOcclusionLightmapBuilder();

	~AmbientOcclusionLightmapBuilder();

	void Release();

//	bool Calculate( AOLightmapDesc& desc );
	bool Calculate( std::vector<CLightmap>& rvecLightmap, LightmapDesc& desc );

//	void ScaleIntensityAndAddAmbientLight( vector<CLightmap>& rvecLightmap );


//	inline void RaiseOptionFlag( const unsigned int flag ) { m_OptionFlag |= flag; }
//	inline unsigned int GetOptionFlag() const { return m_OptionFlag; }
//	inline void ClearOptionFlag( const unsigned int flag ) { m_OptionFlag &= (~flag); }

	void RenderScene();
};


//================================ inline implementations ================================

inline AmbientOcclusionLightmapBuilder::AmbientOcclusionLightmapBuilder()
{
//	m_fDirectionalLightDistance = 200.0;

	m_OptionFlag = 0;
	
	m_NumTotalLightmapTexels = 0;
	
	m_NumProcessedLightmapTexels = 0;

	m_NumRenderTargetTexels = 0;
}


/*
template <class T>
void AmbientOcclusionLightmapBuilder<T>::CreateMeshObject()
{
	C3DMeshModelArchive archive;
	CMMA_TriangleSet triangle_set;
	vector<int> index_buffer;
	CMMA_VertexSet vertex_set;
	for()
	{
		for()
		{
		}
	}
}
*/

/*
/// Scale the intensity value of the lightmap texel
/// Add the ambient light to the intensity of the each lightmap texel
template <class T>
void AmbientOcclusionLightmapBuilder<T>::ScaleIntensityAndAddAmbientLight( vector<CLightmap>& rvecLightmap )
{
	int i,x,y;
	float maxintensityR, maxintensityG, maxintensityB;
	float minintensityR, minintensityG, minintensityB;
	maxintensityR = maxintensityG = maxintensityB = -99999;
	minintensityR = minintensityG = minintensityB = 99999;

	int iNumTexels = 0;
	int width, height;
	int iNumLightmaps = rvecLightmap.size();

	for(i=0; i<iNumLightmaps; i++)
	{
		CLightmap& rLightmap = rvecLightmap[i];
		width  = rLightmap.GetRectangle().GetWidth();
		height = rLightmap.GetRectangle().GetHeight();

		for(y=0; y<height; y++)
			for(x=0; x<width; x++)
			{
				if( !rLightmap.ValidPoint(x,y) )
					continue;

				SFloatRGBColor& rTexel = rLightmap.GetTexelColor(x,y);
				if( rTexel.fRed   > maxintensityR )	maxintensityR = rTexel.fRed;
				if( rTexel.fRed   < minintensityR )	minintensityR = rTexel.fRed;
				if( rTexel.fGreen > maxintensityG )	maxintensityG = rTexel.fGreen;
				if( rTexel.fGreen < minintensityG )	minintensityG = rTexel.fGreen;
				if( rTexel.fBlue  > maxintensityB )	maxintensityB = rTexel.fBlue;
				if( rTexel.fBlue  < minintensityB )	minintensityB = rTexel.fBlue;
			}
	}

	SFloatRGBColor ambcolor = this->m_AmbientColor * this->m_fAmbientIntensity;
	float contrast = m_fContrast;
	float scaleR = ( maxintensityR - minintensityR ) * contrast;
	float scaleG = ( maxintensityG - minintensityG ) * contrast;
	float scaleB = ( maxintensityB - minintensityB ) * contrast;
	if( scaleR == 0.0 ) scaleR = contrast;
	if( scaleG == 0.0 ) scaleG = contrast;
	if( scaleB == 0.0 ) scaleB = contrast;

	for(i=0; i<iNumLightmaps; i++)
	{
		CLightmap& rLightmap = rvecLightmap[i];
		width  = rLightmap.GetRectangle().GetWidth();
		height = rLightmap.GetRectangle().GetHeight();

		for(y=0; y<height; y++)
			for(x=0; x<width; x++)
			{
				if( !rLightmap.ValidPoint(x,y) )
					continue;

				SFloatRGBColor& rTexel = rLightmap.GetTexelColor(x,y);
				rTexel.fRed   = ( rTexel.fRed   / scaleR ) * ( 1.0 - ambcolor.fRed   ) + ambcolor.fRed;
				rTexel.fGreen = ( rTexel.fGreen / scaleG ) * ( 1.0 - ambcolor.fGreen ) + ambcolor.fGreen;
				rTexel.fBlue  = ( rTexel.fBlue  / scaleB ) * ( 1.0 - ambcolor.fBlue  ) + ambcolor.fBlue;
				if( rTexel.fRed   >= 1.0 ) rTexel.fRed = 1.0;
				if( rTexel.fGreen >= 1.0 ) rTexel.fGreen = 1.0;
				if( rTexel.fBlue  >= 1.0 ) rTexel.fBlue = 1.0;
			}
	}
}*/


#endif  /*  __AMBIENTOCCLUSIONLIGHTMAPBUILDER_H__  */
