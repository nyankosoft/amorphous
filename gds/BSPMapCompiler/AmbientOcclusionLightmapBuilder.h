#ifndef  __AMBIENTOCCLUSIONLIGHTMAPBUILDER_H__
#define  __AMBIENTOCCLUSIONLIGHTMAPBUILDER_H__

//#include "LightingForLightmap.h"


#include "Support/StatusDisplay/StatusDisplay.h"
#include "Support/StatusDisplay/StatusDisplayRenderer_D3DX.h"

#include <vector>
using namespace std;


#include "Lightmap.h"
#include "MapFace.h"

#include "3DCommon/fwd.h"
#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/LightStructs.h"
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

//template <class T>
class AmbientOcclusionLightmapBuilder// : public CLightingForLightmap<T>
{
protected:

	enum ShaderTechnique
	{
		TECH_ENVIRONMENT_LIGHT,
		TECH_OCCULUSION_MESH,
		NUM_TECHNIQUES
	};

	vector<CD3DXMeshObjectBase *> m_vecpMesh;

	CD3DXMeshObjectBase *m_pEnvLightMesh;

	CShaderManager *m_pShaderMgr;


	unsigned int m_OptionFlag;
	
	int m_NumTotalLightmapTexels;
	
	int m_NumProcessedLightmapTexels;

	int m_NumRenderTargetTexels;

protected:

//	void CalculateLightmapTexelIntensity( CLightmap& rLightmap, CPolygonMesh<T>& rMesh );

//	void CalculateLightmapTexelIntensityDL( CLightmap& rLightmap, CPolygonMesh<T>& rMesh );

	void CalculateLightmapTexelIntensity( CLightmap& rLightmap, CTextureRenderTarget& tex_render_target );

	inline SFloatRGBColor GetEnvLightIntensity( CTextureRenderTarget& tex_render_target );

public:

	inline AmbientOcclusionLightmapBuilder();

	~AmbientOcclusionLightmapBuilder();

	void Release();

//	bool Calculate( AOLightmapDesc& desc );
	bool Calculate( vector<CLightmap>& rvecLightmap, LightmapDesc& desc );

//	void ScaleIntensityAndAddAmbientLight( vector<CLightmap>& rvecLightmap );


//	inline void RaiseOptionFlag( const unsigned int flag ) { m_OptionFlag |= flag; }
//	inline unsigned int GetOptionFlag() const { return m_OptionFlag; }
//	inline void ClearOptionFlag( const unsigned int flag ) { m_OptionFlag &= (~flag); }

	void RenderScene();

	/// use when the old raytrace scheme is necessary
//	CBSPMapCompiler *m_pMapCompiler;
};


//================================ inline implementations ================================

//template <class T>
inline AmbientOcclusionLightmapBuilder::AmbientOcclusionLightmapBuilder()
:
m_pShaderMgr(NULL),
m_pEnvLightMesh(NULL)
{
//	m_fDirectionalLightDistance = 200.0;

	m_OptionFlag = 0;
	
	m_NumTotalLightmapTexels = 0;
	
	m_NumProcessedLightmapTexels = 0;

	m_NumRenderTargetTexels = 0;

//	m_pMapCompiler = NULL;
}


/*
//template <class T>
void AmbientOcclusionLightmapBuilder<T>::CalculateLightmapTexelIntensity( CLightmap& rLightmap,
																			  CPolygonMesh<T>& rMesh )
{
	int i,x,y;
	Vector3 vToLight, vToLight_n;
//	int iNumLights = m_pvecpLight->size();
	int iNumLights = m_vecpPointLight.size();
	CPointLight* pPointLight = NULL;
	float fRealDist, fAttenuation, fPower, fAngle;

	int iLightmapWidth  = rLightmap.GetNumPoints_X();
	int iLightmapHeight = rLightmap.GetNumPoints_Y();

	Vector3 vLightmapPoint;
	Vector3 vNormal;
	Vector3 vLightDir;
	SFloatRGBColor color;

	int num_processed_texels = m_NumProcessedLightmapTexels;
	const int num_total_texels = m_NumTotalLightmapTexels;

	SPlane& rPlane = rLightmap.GetPlane();
	SRay ray;
	for(i=0; i<iNumLights; i++)
	{
		pPointLight = (CPointLight *)(m_vecpPointLight[i]);

		for(y=0; y<iLightmapHeight; y++)
		{
			for(x=0; x<iLightmapWidth; x++)
			{
				float dist = rPlane.GetDistanceFromPoint( pPointLight->vPosition );

				if( dist < 0.0 )
					continue;			//The light is behind the faces

				vLightmapPoint = rLightmap.GetPoint(x,y);

				ray.vStart = pPointLight->vPosition;
///				ray.vGoal  = rLightmap.GetPoint(x,y);
				ray.fFraction = 1.0f;

				vToLight = pPointLight->vPosition - vLightmapPoint;
				fRealDist = Vec3Length(&vToLight);
				vToLight_n = vToLight / fRealDist;
				ray.vGoal = vLightmapPoint + vToLight_n * m_fSurfaceErrorTolerance;

				rMesh.RayTrace( ray );

				if( ray.fFraction < 1.0f )
				{
					// ray is blocked
					vLightDir = rLightmap.GetLightDirection(x,y);
					vLightDir += vToLight_n * pPointLight->fIntensity * 0.1f;
					rLightmap.SetLightDirection( x, y, vLightDir );

					continue;
				}

				// the light is not obstructed and reaching the target point
///				vToLight = pPointLight->vPosition - rLightmap.GetPoint(x,y);
///				fRealDist = Vec3Length(&vToLight);

				fAttenuation = 1.0f / ( pPointLight->fAttenuation0
							          + pPointLight->fAttenuation1 * fRealDist
								      + pPointLight->fAttenuation2 * fRealDist * fRealDist );

				if (fAttenuation>1) fAttenuation=1;

				// normalize light direction
///				vToLight_n = vToLight / fRealDist;
//				Vec3Normalize(&vToLight_n, &vToLight);

//				fAngle = Vec3Dot( vToLight_n, rPlane.normal );	// use face normal (for flat shading)
				fAngle = Vec3Dot( vToLight_n, rLightmap.GetNormal(x,y) );	// use normal calculated for each lightmap texel (for smooth shading)

				if( m_OptionFlag & LF_IGNORE_ANGLE_FACTOR )
				{
					// exclude angle factor
					// angle attenuation is calculated by using the light direction map during runtime
					fPower = pPointLight->fIntensity * fAttenuation;
				}
				else if( m_OptionFlag & LF_USE_HEMISPHERIC_LIGHT )
				{
					float f = ( fAngle + 1.0f ) * 0.5f;
					fPower = pPointLight->fIntensity * fAttenuation * f;
				}
				else
				{
					fPower = pPointLight->fIntensity * fAttenuation * fAngle;
				}

				if( fPower < 0.0 )
					continue;

				// ========================= normal pixel intensity calculation ===========================
				color = rLightmap.GetTexelColor(x,y);
				rLightmap.SetTexelColor(x,y, color + pPointLight->Color * fPower );

				// ========= replace lightmap colors with normal directions (visual debugging) ============
//				Vector3& rvNormal = rLightmap.avNormal[ LMTexelAt(x,y) ];
//				rLightmap.intensity[ LMTexelAt(x,y) ].fRed   = (float)fabs(rvNormal.x);
//				rLightmap.intensity[ LMTexelAt(x,y) ].fGreen = (float)fabs(rvNormal.y);
//				rLightmap.intensity[ LMTexelAt(x,y) ].fBlue  = (float)fabs(rvNormal.z);

				if( true )//m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
				{
//					vNormal = rLightmap.GetNormal(x,y);
//					rLightmap.SetNormal(x,y, vNormal + vToLight_n * pPointLight->fIntensity );
					vLightDir = rLightmap.GetLightDirection(x,y);
					vLightDir += vToLight_n * pPointLight->fIntensity * 100.0f;
					rLightmap.SetLightDirection( x, y, vLightDir );
				}

				num_processed_texels++;
//				CStatusDisplay::Get()->UpdateProgress( "Lightmap Raytrace",
//				                                       (float)(num_processed_texels) / (float)num_total_texels );

				Sleep( 5 );

			}
		}
	}
	
	m_NumProcessedLightmapTexels = num_processed_texels;

}

//template <class T>
void AmbientOcclusionLightmapBuilder<T>::CalculateLightmapTexelIntensityDL( CLightmap& rLightmap,
																			    CPolygonMesh<T>& rMesh )
{
	int i,x,y;
//	Vector3 vToLight, vToLight_n;
	int iNumDirectionalLights = m_vecpDirectionalLight.size();
	CDirectionalLight* pDirLight = NULL;
	float fPower, fAngle;

	int iLightmapWidth  = rLightmap.GetNumPoints_X();
	int iLightmapHeight = rLightmap.GetNumPoints_Y();

	Vector3 vNormal;
	Vector3 vLightDir;
//	Vector3 vPseudoDir;
	SFloatRGBColor color;

	SPlane& rPlane = rLightmap.GetPlane();
	SRay ray;
	for(i=0; i<iNumDirectionalLights; i++)
	{
		pDirLight = (CDirectionalLight *)(m_vecpDirectionalLight[i]);

		const Vector3 vDirToLight = - pDirLight->vDirection;

		for(y=0; y<iLightmapHeight; y++)
		{
			for(x=0; x<iLightmapWidth; x++)
			{
//				float dist = rPlane.GetDistanceFromPoint( pDirLight->vPosition );
//				if( dist < 0.0 )

				float d = Vec3Dot( vDirToLight, rPlane.normal );
				if( d < 0 )
				{
					// The light is behind the faces
					// add small light direction
					vLightDir = rLightmap.GetLightDirection(x,y);
					vLightDir += ( rPlane.normal + Vector3( 0, vDirToLight.y, 0 ) ) * 0.01f;
					rLightmap.SetLightDirection( x, y, vLightDir );

					continue;
				}

				ray.vStart = rLightmap.GetPoint(x,y) + vDirToLight * m_fDirectionalLightDistance;
				ray.vGoal  = rLightmap.GetPoint(x,y) + vDirToLight * m_fSurfaceErrorTolerance;
				ray.fFraction = 1.0f;

				rMesh.RayTrace( ray );

				if( ray.fFraction < 1.0f )
				{
					// ray is blocked
					vLightDir = rLightmap.GetLightDirection(x,y);
					vLightDir += vDirToLight * 0.01f;
					rLightmap.SetLightDirection( x, y, vLightDir );

					continue;
				}
				// the light is not obstructed and reaching the target point

//				fAngle = Vec3Dot( vToLight_n, rPlane.normal );	// use face normal (for flat shading)
				fAngle = Vec3Dot( vDirToLight, rLightmap.GetNormal(x,y) );	// use normal calculated for each lightmap texel (for smooth shading)

				if( true )//m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
				{	// exclude angle factor
					// angle attenuation is calculated by using the light direction map during runtime
					fPower = pDirLight->fIntensity;
				}
				else
				{
					fPower = pDirLight->fIntensity * fAngle;
				}

				if( fPower < 0.0 )
					continue;

				// ========================= normal pixel intensity calculation ===========================
				color = rLightmap.GetTexelColor(x,y);
				rLightmap.SetTexelColor(x,y, color + pDirLight->Color * fPower );

				// ========= replace lightmap colors with normal directions (visual debugging) ============
//				Vector3& rvNormal = rLightmap.avNormal[ LMTexelAt(x,y) ];
//				rLightmap.intensity[ LMTexelAt(x,y) ].fRed   = (float)fabs(rvNormal.x);
//				rLightmap.intensity[ LMTexelAt(x,y) ].fGreen = (float)fabs(rvNormal.y);
//				rLightmap.intensity[ LMTexelAt(x,y) ].fBlue  = (float)fabs(rvNormal.z);

				if( true )//m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE/ )
				{
					vLightDir = rLightmap.GetLightDirection(x,y);
					vLightDir += vDirToLight; // * pDirLight->fIntensity;
					rLightmap.SetLightDirection( x, y, vLightDir );
				}

			}
		}
	}

}*/


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
