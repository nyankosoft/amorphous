
#include "LightmapLightingManager.h"

using namespace std;
using namespace boost;


void CLightmapRaytraceTask::RunTask()
{
//	size_t m_pLightRaytrace;
	vector<CLightRaytrace *>& light_raytrace = m_pLightRaytrace;
	const size_t num_lights = light_raytrace.size();
	for( size_t i=0; i<num_lights; i++ )
	{
		const int num_points_x = m_pLightmap->GetNumPoints_X();
		const int num_points_y = m_pLightmap->GetNumPoints_Y();

		int x,y;
		for( x=0; x<num_points_x; x++ )
		{
			for( y=0; y<num_points_y; y++ )
			{
				light_raytrace[i]->CalcLightAmount( *m_pLightmap, x, y );
			}
		}
	}

	m_bDone = true;
}


void CLightmapRaytraceTask::operator()()
{
	ThreadMain();
}


void CLightmapRaytraceTask::ThreadMain()
{
	while(1)
	{
		CLightmap *pLightmap = m_pMgr->GetLightmapForRaytraceTask();

		if( pLightmap )
		{
			m_pLightmap = pLightmap;
			RunTask();
		}
		else
			break; // no more lightmap to update
	}
}



CLightmapLightingManager::CLightmapLightingManager()
{
	m_pvecpLight = NULL;

	m_pvecLightmap = NULL;

	m_fContrast = 0.8f;

	m_fSurfaceErrorTolerance = 0.032f;

	m_fDirectionalLightDistance = 200.0;

	m_OptionFlag = 0;
	
	m_NumTotalLightmapTexels = 0;
	
	m_NumProcessedLightmapTexels = 0;

	m_CurrentLightmapIndex = 0;
}


/// Scale the intensity value of the lightmap texel
/// Add the ambient light to the intensity of the each lightmap texel
void CLightmapLightingManager::ScaleIntensityAndAddAmbientLight()
{
	vector<CLightmap>& rvecLightmap = *m_pvecLightmap;

	size_t i;
	int x,y;
	float maxintensityR, maxintensityG, maxintensityB;
	float minintensityR, minintensityG, minintensityB;
	maxintensityR = maxintensityG = maxintensityB = -99999;
	minintensityR = minintensityG = minintensityB = 99999;

	int iNumTexels = 0;
	int width, height;
	const size_t iNumLightmaps = rvecLightmap.size();

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

				const SFloatRGBColor& rTexel = rLightmap.GetTexelColor(x,y);
				if( rTexel.fRed   > maxintensityR )	maxintensityR = rTexel.fRed;
				if( rTexel.fRed   < minintensityR )	minintensityR = rTexel.fRed;
				if( rTexel.fGreen > maxintensityG )	maxintensityG = rTexel.fGreen;
				if( rTexel.fGreen < minintensityG )	minintensityG = rTexel.fGreen;
				if( rTexel.fBlue  > maxintensityB )	maxintensityB = rTexel.fBlue;
				if( rTexel.fBlue  < minintensityB )	minintensityB = rTexel.fBlue;
			}
	}

	SFloatRGBColor ambcolor = this->m_AmbientLight.Color * this->m_AmbientLight.fIntensity;
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

				SFloatRGBColor texel = rLightmap.GetTexelColor(x,y);
				texel.fRed   = ( texel.fRed   / scaleR ) * ( 1.0 - ambcolor.fRed   ) + ambcolor.fRed;
				texel.fGreen = ( texel.fGreen / scaleG ) * ( 1.0 - ambcolor.fGreen ) + ambcolor.fGreen;
				texel.fBlue  = ( texel.fBlue  / scaleB ) * ( 1.0 - ambcolor.fBlue  ) + ambcolor.fBlue;
				if( texel.fRed   >= 1.0 ) texel.fRed = 1.0;
				if( texel.fGreen >= 1.0 ) texel.fGreen = 1.0;
				if( texel.fBlue  >= 1.0 ) texel.fBlue = 1.0;
				rLightmap.SetTexelColor( x, y, texel );
			}
	}
}


vector<CLightRaytrace *> CLightmapLightingManager::CreateLightRaytraceTasks( vector<shared_ptr<CLight>> *pvecpLight )
{
	vector<shared_ptr<CLight>> rvecpLight = *pvecpLight;

	// classify lights
	const size_t num_lights = rvecpLight.size();

//	CAmbientLight *pAmbientLight;

	vector<CLightRaytrace *> vecpLightRaytraceTask;

	for( size_t i=0; i<num_lights; i++ )
	{
		switch( rvecpLight[i]->GetLightType() )
		{
		case CLight::POINT:
		case CLight::HEMISPHERIC_POINT:
			vecpLightRaytraceTask.push_back( new CPointLightRaytrace(rvecpLight[i]) );
			break;

		case CLight::DIRECTIONAL:
		case CLight::HEMISPHERIC_DIRECTIONAL:
			vecpLightRaytraceTask.push_back( new CDirectionalLightRaytrace(rvecpLight[i]) );
			break;

		case CLight::AMBIENT:
			{
				CAmbientLight *pAmbientLight = dynamic_cast<CAmbientLight *>(rvecpLight[i].get());
				if( pAmbientLight )
					m_AmbientLight = *pAmbientLight;
			}
			break;

//		case CLight::ZONE_AMBIENT:
//			m_vecpZoneAmbientLight.push_back( (CZoneAmbientLight *)(m_pvecpLight->at(i)) );
//			break;

		default:
			break;
		}
	}

	return vecpLightRaytraceTask;
}

shared_ptr<CAABTree<CIndexedPolygon>> CreateAABTreeCopy( CAABTree<CIndexedPolygon> *pGeometry )
{
	switch( pGeometry->GetTreeType() )
	{
	case CAABTree<CIndexedPolygon>::NON_LEAFY:
		{
			CNonLeafyAABTree<CIndexedPolygon> *pNonLeafy = dynamic_cast<CNonLeafyAABTree<CIndexedPolygon> *>(pGeometry);
			return shared_ptr<CAABTree<CIndexedPolygon>>( new CNonLeafyAABTree<CIndexedPolygon>( *pNonLeafy ) );
		}
		break;

	case CAABTree<CIndexedPolygon>::LEAFY:
		{
			CLeafyAABTree<CIndexedPolygon> *pLeafy = dynamic_cast<CLeafyAABTree<CIndexedPolygon> *>(pGeometry);
			return shared_ptr<CAABTree<CIndexedPolygon>>( new CLeafyAABTree<CIndexedPolygon>( *pLeafy ) );
		}
		break;

	default:
		break;
	}

	return shared_ptr<CAABTree<CIndexedPolygon>>();
}


#define NUM_MAX_RAYTRACE_TASKS 16

boost::mutex gs_TaskThreadStart;


static CLightmapRaytraceTask *gs_pTask = NULL;

static void SetTargetRaytraceTask( CLightmapRaytraceTask *pTask )
{
	boost::mutex::scoped_lock scoped_lock(gs_TaskThreadStart);

	gs_pTask = pTask;
}

static void StartRaytraceTask()
{
	boost::mutex::scoped_lock scoped_lock(gs_TaskThreadStart);

	if( gs_pTask )
	{
		gs_pTask->ThreadMain();
		gs_pTask = NULL;
	}
}


bool CLightmapLightingManager::CreateLightmaps( vector<shared_ptr<CLight>> *pvecpLight,
											    vector<CLightmap> *pvecLightmap,
											    CAABTree<CIndexedPolygon> *pGeometry )
{
	if( !pvecpLight || pvecpLight->size() == 0 )
		return false;	// No light in the map

	m_pvecpLight   = pvecpLight;
	m_pvecLightmap = pvecLightmap;
	m_pGeometry    = pGeometry;

	// create raytrace tasks for each light

//	vector<CLightRaytrace *> vecpLightRaytrace = CreateLightRaytraceTasks( pvecpLight );

	const int num_threads = 2;

//	vector<CLightmapRaytraceTask> vecRaytraceTask;
//	vecRaytraceTask.resize( num_threads, CLightmapRaytraceTask(this) );
	CLightmapRaytraceTask vecRaytraceTask[NUM_MAX_RAYTRACE_TASKS];

	vector<boost::thread *> vecpThread;
	vecpThread.resize( num_threads );

	for( int i=0; i<num_threads; i++ )
	{
		vecRaytraceTask[i].SetLightmapLightingManager( this );

		// copy the polygon tree
		// - Each thread need a copy for ray check because the polygon tree is
		//   not thread-safe and cannot be shared by threads
		vecRaytraceTask[i].m_pGeometry = CreateAABTreeCopy( m_pGeometry );

		// light raytrace tasks
		// - needs be created for each raytrace task
		vecRaytraceTask[i].SetLightRaytraceTasks( CreateLightRaytraceTasks( pvecpLight ) );

		// start the thread
//		vecpThread[i] = new boost::thread( vecRaytraceTask[i] );
	}

	boost::thread_group raytrace_threads;
	for( int i=0; i<num_threads; i++ )
	{
		// add threads to thread group
		// - ownerships are transferred to raytrace_threads
//		raytrace_threads.add_thread( vecpThread[i] );
		SetTargetRaytraceTask( &vecRaytraceTask[i] );
		raytrace_threads.create_thread( &StartRaytraceTask );
	}

	raytrace_threads.join_all();

	return true;
}

/*
bool CLightmapLightingManager::Calculate( vector<CLightmap>& rvecLightmap )
{
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
*/
