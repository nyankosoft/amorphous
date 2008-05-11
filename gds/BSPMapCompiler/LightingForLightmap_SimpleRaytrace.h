#ifndef  __MAPCOMPILER_LIGHTINGFORLIGHTMAP_SIMPLERAYTRACE_H__
#define  __MAPCOMPILER_LIGHTINGFORLIGHTMAP_SIMPLERAYTRACE_H__

//#include "LightingForLightmap.h"

#include <vector>
using namespace std;

#include "3DMath/AABTree.h"
#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/LightStructs.h"
#include "Support/StatusDisplay/StatusDisplay.h"
#include "Support/StatusDisplay/StatusDisplayRenderer_D3DX.h"

#include "Lightmap.h"

//#include "3DMath/PolygonMesh.h"

//class CLight;


enum eFlag
{
	LF_IGNORE_ANGLE_FACTOR				= (1 << 0),				// ignore angle attenuation
	LF_CREATE_LIGHT_DIRECTION_TEXTURE	= (1 << 1),	// create light direction maps (for bump mapping)
	LF_USE_HEMISPHERIC_LIGHT			= (1 << 2),	// use hemispheric lighting to create lightmaps
};

//==========================================================================================
// CLightingForLightmap_SimpleRaytrace
//==========================================================================================

template <class T>
class CLightingForLightmap_SimpleRaytrace// : public CLightingForLightmap<T>
{
	
	/// pointer to the array of pointers to light objects  (ライトオブジェクトのポインタの配列、へのポインタ)
	vector<CLight *>* m_pvecpLight;

	/// point lights
	vector<CPointLight *> m_vecpPointLight;

	/// directional lights
	vector<CDirectionalLight *>m_vecpDirectionalLight;

	/// zone ambient lights (experimental)
	vector<CZoneAmbientLight *>m_vecpZoneAmbientLight;

	/// ambient light: global & uniform lighting for the entire map
	float m_fAmbientIntensity;
	SFloatRGBColor m_AmbientColor;

	float m_fContrast;

	float m_fSurfaceErrorTolerance;

	float m_fDirectionalLightDistance;

	unsigned int m_OptionFlag;
	
	int m_NumTotalLightmapTexels;
	
	int m_NumProcessedLightmapTexels;

	void CalculateLightmapTexelIntensity( CLightmap& rLightmap, CNonLeafyAABTree<T>& rMesh );

	void CalculateLightmapTexelIntensityDL( CLightmap& rLightmap, CNonLeafyAABTree<T>& rMesh );

public:

	inline CLightingForLightmap_SimpleRaytrace();

	void SetLights( vector<CLight *>& rvecpLight ) { m_pvecpLight = &rvecpLight; }

//	bool Calculate( vector<CLightmap>& rvecLightmap, CPolygonMesh<T>& rMesh );

	void ScaleIntensityAndAddAmbientLight( vector<CLightmap>& rvecLightmap );


	inline void RaiseOptionFlag( const unsigned int flag ) { m_OptionFlag |= flag; }

	inline unsigned int GetOptionFlag() const { return m_OptionFlag; }

	inline void ClearOptionFlag( const unsigned int flag ) { m_OptionFlag &= (~flag); }
};


//================================ inline implementations ================================

template <class T>
inline CLightingForLightmap_SimpleRaytrace<T>::CLightingForLightmap_SimpleRaytrace()
{
	vector<T> rvecTemp;

	m_fContrast = 0.8f;

	m_fSurfaceErrorTolerance = 0.032f;

	m_fDirectionalLightDistance = 200.0;

	m_OptionFlag = 0;
	
	m_NumTotalLightmapTexels = 0;
	
	m_NumProcessedLightmapTexels = 0;
}

class CLightRayTrace
{
	boost::shared_ptr<CLight> m_pLight;

	CAABTree<CIndexedPolygon> *m_pGeometry;

public:

	friend class CLightmapRayTracer;
};


class CPointLightRayTrace
{
	boost::shared_ptr<CLight> m_pLight;

	Vector3 m_vLightPosition;

public:

	inline virtual SFloatRGBColor CalcLightAmount( CLightmap& rLightmap, int x, int y );

};


class CDirectionalLightRayTrace
{

	Vector3 m_vLightDirection;

public:

	inline virtual SFloatRGBColor CalcLightAmount( CLightmap& rLightmap, int x, int y );

};


/// task executed by a thread
class CLightmapRaytraceTask
{
	CLightmap *m_pLightmap;

	vector<CLightRayTrace *> *m_pLightRayTrace;

	bool m_bDone

public:

	CLightmapRaytraceTask() : m_bDone(false) {}

	void thread_main();
};


void CLightmapRaytraceTask::thread_main()
{
	size_t m_pLightRayTrace;
	const size_t num_lights = m_pLightRayTrace->size();
	for( size_t i=0; i<num_lights; i++ )
	{
		const 
	}
}


/**
 \param lightmap [in,out] lightmap to update
 \param x [in]
 \param y [in]

*/
inline void CPointLightRayTrace::CalcLightAmount( CLightmap& rLightmap, int x, int y )
{
	const SPlane& plane = rLightmap.GetPlane();

	float dist = plane.GetDistanceFromPoint( m_vLightPosition );

	if( dist < 0.0 )
		continue;			//The light is behind the faces

	vLightmapPoint = rLightmap.GetPoint(x,y);

	// check the ray from the point on the lightmap to the position of the light
	ray.vStart = pPointLight->vPosition;
///	ray.vGoal  = rLightmap.GetPoint(x,y);
	ray.fFraction = 1.0f;

	vToLight = m_vLightPosition - vLightmapPoint;
	fRealDist = Vec3Length(&vToLight);
	vDirToLight = vToLight / fRealDist;
//	ray.vGoal = vLightmapPoint + vToLight_n * m_fSurfaceErrorTolerance;
	ray.vGoal = vLightmapPoint + plane.normal * 0.01f;//m_fSurfaceErrorTolerance;

	m_pGeometry->RayTrace( ray );

	if( ray.fFraction < 1.0f )
	{
		// ray is blocked
		vLightDir = rLightmap.GetLightDirection(x,y);
		vLightDir += vDirToLight * 0.1f;//pPointLight->fIntensity * 0.1f;
		rLightmap.SetLightDirection( x, y, vLightDir );

		return;
	}

		// the light is not obstructed and reaching the target point
///			vToLight = pPointLight->vPosition - rLightmap.GetPoint(x,y);
///			fRealDist = Vec3Length(&vToLight);

	SFloatRGBColor color = m_pLight->CalcLightAmount( vLightmapPoint, rLightmap.GetNormal(x,y) );

	rLightmap.AddTexelColor( x, y, color );

//		fAttenuation = 1.0f / ( pPointLight->fAttenuation0
//					          + pPointLight->fAttenuation1 * fRealDist
//						      + pPointLight->fAttenuation2 * fRealDist * fRealDist );

//		if (fAttenuation>1) fAttenuation=1;

//		fAngle = Vec3Dot( vToLight_n, rLightmap.GetNormal(x,y) );	// use normal calculated for each lightmap texel (for smooth shading)

/*		if( m_OptionFlag & LF_IGNORE_ANGLE_FACTOR )
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
*/

		// ========================= normal pixel intensity calculation ===========================
//		color = rLightmap.GetTexelColor(x,y);
//		rLightmap.SetTexelColor(x,y, color + pPointLight->Color * fPower );

		// ========= replace lightmap colors with normal directions (visual debugging) ============
//				Vector3& rvNormal = rLightmap.avNormal[ LMTexelAt(x,y) ];
//				rLightmap.intensity[ LMTexelAt(x,y) ].fRed   = (float)fabs(rvNormal.x);
//				rLightmap.intensity[ LMTexelAt(x,y) ].fGreen = (float)fabs(rvNormal.y);
//				rLightmap.intensity[ LMTexelAt(x,y) ].fBlue  = (float)fabs(rvNormal.z);

		if( true /*m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE*/ )
		{
//					vNormal = rLightmap.GetNormal(x,y);
//					rLightmap.SetNormal(x,y, vNormal + vToLight_n * pPointLight->fIntensity );
			vLightDir = rLightmap.GetLightDirection(x,y);
			vLightDir += vDirToLight * pPointLight->fIntensity * 100.0f;
			rLightmap.SetLightDirection( x, y, vLightDir );
		}

//		num_processed_texels++;
//				CStatusDisplay::Get()->UpdateProgress( "Lightmap Raytrace",
//				                                       (float)(num_processed_texels) / (float)num_total_texels );

//		Sleep( 5 );
}


inline SFloatRGBColor CDirectionalLightRayTrace::CalcLightAmount( CLightmap& rLightmap, int x, int y )
{
//				float dist = rPlane.GetDistanceFromPoint( pDirLight->vPosition );
//				if( dist < 0.0 )

	const Vector3 vDirToLight = - m_pLight->GetDirection();

	const SPlane& plane = rLightmap.GetPlane();

	float d = Vec3Dot( vDirToLight, v.normal );
	if( d < 0 )
	{
		// The light is behind the faces
		// add small light direction
		vLightDir = rLightmap.GetLightDirection(x,y);
		vLightDir += ( rPlane.normal + Vector3( 0, vDirToLight.y, 0 ) ) * 0.01f;
		rLightmap.SetLightDirection( x, y, vLightDir );

		return;
	}

	const Vecto3 vLightmapPoint = rLightmap.GetPoint(x,y);

	ray.vStart = vLightmapPoint + vDirToLight * 100.0f;//m_fDirectionalLightDistance;
//	ray.vGoal  = vLightmapPoint + vDirToLight * m_fSurfaceErrorTolerance;
	ray.vGoal  = vLightmapPoint + plane.normal * 0.01f;
	ray.fFraction = 1.0f;

	m_pGeometry->RayTrace( ray );

	if( ray.fFraction < 1.0f )
	{
		// ray is blocked
		vLightDir = rLightmap.GetLightDirection(x,y);
		vLightDir += vDirToLight * 0.01f;
		rLightmap.SetLightDirection( x, y, vLightDir );

		return;
	}

	SFloatRGBColor color = m_pLight->CalcLightAmount( vLightmapPoint, rLightmap.GetNormal(x,y) );

	rLightmap.AddTexelColor( x, y, color );

	// the light is not obstructed and reaching the target point
/*
//				fAngle = Vec3Dot( vToLight_n, rPlane.normal );	// use face normal (for flat shading)
	fAngle = Vec3Dot( vDirToLight, rLightmap.GetNormal(x,y) );	// use normal calculated for each lightmap texel (for smooth shading)

	if( true ) // m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
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
*/
	if( true )//m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
	{
		vLightDir = rLightmap.GetLightDirection(x,y);
		vLightDir += vDirToLight; // * pDirLight->fIntensity;
		rLightmap.SetLightDirection( x, y, vLightDir );
	}

}


template <class T>
bool CLightingForLightmap_SimpleRaytrace<T>::Calculate( vector<CLightmap>& rvecLightmap, CPolygonMesh<T>& rMesh )
{

	if( m_pvecpLight->size() == 0 )
		return false;	// No light in the map

	m_AmbientColor = SFloatRGBColor(0,0,0);
	m_fAmbientIntensity = 0;

	int i, num_lightmaps = rvecLightmap.size();

	// calc the total number of lightmap texels
	// used to report progress
	int num_total_lightmap_texels = 0;
	for( i=0; i<num_lightmaps; i++ )
	{
		num_total_lightmap_texels += rvecLightmap[i].GetNumPoints();
	}


	m_NumTotalLightmapTexels = num_total_lightmap_texels;
	m_NumProcessedLightmapTexels = 0;

	// classify lights
	int iNumLights = m_pvecpLight->size();
	CAmbientLight *pAmbientLight;
	for( i=0; i<iNumLights; i++ )
	{
		switch( m_pvecpLight->at(i)->GetLightType() )
		{
		case CLight::POINT_LIGHT:
			m_vecpPointLight.push_back( (CPointLight *)(m_pvecpLight->at(i)) );
			break;

		case CLight::DIRECTIONAL_LIGHT:
			m_vecpDirectionalLight.push_back( (CDirectionalLight *)(m_pvecpLight->at(i)) );
			break;

		case CLight::AMBIENT_LIGHT:
			pAmbientLight = (CAmbientLight *)(m_pvecpLight->at(i));
			this->m_fAmbientIntensity = pAmbientLight->fIntensity;
			this->m_AmbientColor = pAmbientLight->Color;
			break;

		case CLight::ZONE_AMBIENT_LIGHT:
			m_vecpZoneAmbientLight.push_back( (CZoneAmbientLight *)(m_pvecpLight->at(i)) );
			break;
		break;

		default:
			break;
		}
	}

	CStatusDisplay::Get()->RegisterTask( "Lightmap Raytrace" );

	for( i=0; i<num_lightmaps; i++ )
	{
		CalculateLightmapTexelIntensity( rvecLightmap[i], rMesh );

		CalculateLightmapTexelIntensityDL( rvecLightmap[i], rMesh );

		CStatusDisplay::Get()->UpdateProgress( "Lightmap Raytrace", (float)(i+1) / (float)num_lightmaps );
	}

	ScaleIntensityAndAddAmbientLight(rvecLightmap);

	return true;
}


template <class T>
void CLightingForLightmap_SimpleRaytrace<T>::CalculateLightmapTexelIntensity( CLightmap& rLightmap,
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

			}
		}
	}
	
	m_NumProcessedLightmapTexels = num_processed_texels;

}

template <class T>
void CLightingForLightmap_SimpleRaytrace<T>::CalculateLightmapTexelIntensityDL( CLightmap& rLightmap,
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
			}
		}
	}

}


/// Scale the intensity value of the lightmap texel
/// Add the ambient light to the intensity of the each lightmap texel
template <class T>
void CLightingForLightmap_SimpleRaytrace<T>::ScaleIntensityAndAddAmbientLight( vector<CLightmap>& rvecLightmap )
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
}


#endif  /*  __MAPCOMPILER_LIGHTINGFORLIGHTMAP_SIMPLERAYTRACE_H__  */