
#include "BSPMapCompiler.h"
#include "lightmapbuilder.h"
#include <typeinfo>

#include "Support/Log/DefaultLog.h"


#include "LightingForLightmap_SimpleRaytrace.h"
#include "AmbientOcclusionLightmapBuilder.h"




//================================================================================
// CLightmapBuilder::Methods()                                 - CLightmapBuilder
//================================================================================

//Set default value
//Get the pointer to the array of lights in the map via 'pMapCompiler'
//At present, 'point light' and single 'ambient light' are supported.
CLightmapBuilder::CLightmapBuilder()
{
	m_pMapCompiler = NULL;

	m_iNumCurrentLightmaps = 0;
	m_fMaxAllowedLightmapArea = 25.0f;
	m_iTextureWidth = 256;
	m_iMargin = 1;

//	m_TexelsPerMeter = 2;
	m_TexelSize = 1.0f;

	m_pLightingSimulator = NULL;

	m_iLightmapCreationFlag = 0;
//	m_iLightmapCreationFlag |= LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE;

}


CLightmapBuilder::~CLightmapBuilder()
{
	Clear();
}


void CLightmapBuilder::Clear()
{
	m_vecLightmap.clear();
	m_vecLightmapTexture.clear();
	SafeDelete( m_pLightingSimulator );
}


void CLightmapBuilder::Init( CBSPMapCompiler* pMapCompiler )
{
	this->m_pMapCompiler = pMapCompiler;

//	this->m_pvecpLight = pMapCompiler->GetLight();

	Clear();

	m_pLightingSimulator = new CLightingForLightmap_SimpleRaytrace<CMapFace>;

//	m_pLightingSimulator->RaiseOptionFlag( LF_IGNORE_ANGLE_FACTOR );
//	m_pLightingSimulator->RaiseOptionFlag( LF_USE_HEMISPHERIC_LIGHT );

//	m_pLightingSimulator->m_pMapCompiler = pMapCompiler;

	// get lights from map data
	// m_pvecpLight is a pointer to the array of pointers. 
	m_pLightingSimulator->SetLights( *(pMapCompiler->GetLight()) );

}


void CLightmapBuilder::SetOption( const CLightmapOption& option )
{
//	m_TexelsPerMeter = 1.0f / option.fTexelSize;
	SetTextureWidth( option.TextureWidth );
	SetTextureHeight( option.TextureHeight );

	m_TexelSize = option.fTexelSize;

	if( option.bCreateLightDirectionMap )
		m_iLightmapCreationFlag |= LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE;

}

bool CLightmapBuilder::CreateLightmapTexture( LightmapDesc& desc )

//bool CLightmapBuilder::CreateLightmapTexture( vector<CMapFace>& rvecFace, CPolygonMesh<CMapFace>& rPolygonMesh )
{
	if( !desc.pvecFace || !desc.pPolygonMesh )
		return false;

	CPolygonMesh<CMapFace>& rPolygonMesh = *(desc.pPolygonMesh);
	vector<CMapFace>& rvecFace = *(desc.pvecFace);

	size_t i, iNumFaces = rvecFace.size();

	SetOption( desc.Option );

	// create lightmaps
	// calculate the brightness of the points on the polygons

	// group faces which are close to one another and on the same plane
	// lightmaps are created during this call
	GroupFaces( rvecFace );

	const size_t iNumLightmaps = m_vecLightmap.size();

	// calculate the 2 orthogonal axes for the lightmap ('vRight' & 'vUp')
	// and determine the dimension of the lightmap
	for( i=0; i<iNumLightmaps; i++ )
		CalculateLightMapPosition(m_vecLightmap[i], rvecFace);

	for( i=0; i<iNumLightmaps; i++ )
	{
		m_vecLightmap[i].ComputeNormalsOnLightmap( rvecFace );

//		ComputePointsOnLightmap(m_vecLightmap[i], rvecFace);
//		ComputeNormalsOnLightmap(m_vecLightmap[i], rvecFace);
	}


	// calculate the color of points on each lightmap
///	m_pLightingSimulator->Calculate( m_vecLightmap, rPolygonMesh );

	// 10:29 2007/07/04
	AmbientOcclusionLightmapBuilder ao;
	ao.Calculate( m_vecLightmap, desc );


	// transform the light direction vectors on each lightmap into local space
	TransformLightDirectionToLocalFaceCoord( rvecFace );

	LOG_PRINT( " - Light direction maps have been transformed into tangent spaces." );

	// create lightmap textures
	PackLightmaps();

	LOG_PRINT( " - Lightmaps have been packed to textures." );

	// set lightmap texture id to each polygon
	SetLightmapTextureIndicesToPolygons( rvecFace );

	// update uv values of polygons
	size_t iNumLightmapTextures = m_vecLightmapTexture.size();
	for( i=0; i<iNumLightmapTextures; i++ )
		m_vecLightmapTexture[i].SetTextureUV( m_vecLightmap, rvecFace );

	LOG_PRINT( " - Lightmap texture uv coords have been calculated." );


	// copy texels on the lightmaps to lightmap textures
	UpdateLightmapTextures();

	LOG_PRINT( " - Updated lightmap textures." );

//	FillMarginRegions();

///	ApplySmoothing( 0.60f );
///	ApplySmoothing( 0.60f );


	return true;
}


void CLightmapBuilder::ComputeNormalsOnLightmap(vector<CMapFace>& rvecFace)
{
}


void CLightmapBuilder::GroupFaces( vector<CMapFace>& rvecFace )
{//vector<int>& rveciGroupedFacesIndex, 

	int i, j, k, iNumFaces = rvecFace.size();

	CMapFace *pFace0, *pFace1;

	// We use 'm_bFlag' to record if a mapface has its lightmap
	// (m_bFlag == false) lightmap has not been allocated.
	// (m_bFlag == true)  light map has been already allocated.
	for(i=0; i<iNumFaces; i++)
		rvecFace[i].m_bFlag = false;

	float totalarea, area2;

	totalarea = 0;

	while( 1 )
	{
		m_vecLightmap.push_back( CLightmap() );
		CLightmap& rLightmap = m_vecLightmap.back();

		// First, set the seed for the face-group
		for(i=0; i<iNumFaces; i++)
		{
			pFace0 = &rvecFace[i];

			if(pFace0->m_bFlag)
				continue; // the lightmap has been already created for this face.

			if( pFace0->ReadTypeFlag(CMapFace::TYPE_INVISIBLE) || pFace0->ReadTypeFlag(CMapFace::TYPE_LIGHTSOURCE) )
			{
				// invisible faces and light source faces are not lit
				pFace0->m_bFlag = true;
				continue;
			}
			else
			{
//				rveciGroupedFacesIndex.push_back( i );
				rLightmap.AddFaceIndex( i );
				pFace0->m_bFlag = true;
				totalarea = pFace0->CalculateArea();
				break;
			}
		}

		if( rLightmap.GetGroupedFacesIndex().size() == 0 )
		{
			// the lightmaps have been made for all the faces in 'rvecFace'.
//			m_vecLightmap.erase( &m_vecLightmap.back() );
			m_vecLightmap.erase( m_vecLightmap.begin() + m_vecLightmap.size() - 1 );
			return;
		}

		if( m_fMaxAllowedLightmapArea < totalarea )
			continue;		// this mapface is large enough to create a lightmap only for it.

		// Second, collect the faces that are close neighbors to the 'face'

		if( m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
			continue; // turn off face grouping when light direction map textures are created

		vector<int>& rveciGroupedFacesIndex = rLightmap.GetGroupedFacesIndex();

		for(j=i+1; j<iNumFaces; j++)
		{
			pFace1 = &rvecFace[j];
			if(pFace1->m_bFlag)
				continue;	//this face already has a lightmap

			if( pFace1->ReadTypeFlag(CMapFace::TYPE_INVISIBLE) || pFace1->ReadTypeFlag(CMapFace::TYPE_LIGHTSOURCE) )
			{	//invisible faces and light source faces are not lit
				pFace1->m_bFlag = true;
				continue;
			}

			if( !pFace1->HasSamePlaneWith(*pFace0) )
				continue;	// pFace0 & pFace1 exist on different planes - cannot share lightmaps

			for(k=0; k<rveciGroupedFacesIndex.size(); k++)
			{
				if( pFace1->SharingPointWith( rvecFace[ rveciGroupedFacesIndex[k] ] ) )
					break;
			}
			if( k == rveciGroupedFacesIndex.size() )
				continue;	//sharing no point: the faces are seperate and shouldn't be packed into the same lightmap

			area2 = pFace1->CalculateArea();
			if( m_fMaxAllowedLightmapArea < totalarea + area2 )
				continue;

			// add to the current face group
			pFace1->m_bFlag = true;
			totalarea += area2;
			rveciGroupedFacesIndex.push_back(j);
		}
	}

	return;

}


// Defines the 2 orthogonal axis vectors on the lightmap plane
// and calculate the size of the rectangle which the lightmap covers
void CLightmapBuilder::CalculateLightMapPosition( CLightmap& lightmap, vector<CMapFace>& rvecFace )
{
	int i,j, iNumVertices, iNumFaces;

	vector<int>& rveciGroupedFacesIndex = lightmap.m_vecGroupedFaceIndex;

	CMapFace& rFace = rvecFace.at( rveciGroupedFacesIndex[0] );
	SPlane& rPlane = rFace.GetPlane();

	lightmap.m_Plane = rPlane;


	Vector3 vRight, vUp, v;

	// calculate 2 mutually orthogonal unit vectors 'vUp' & 'vRight' on the lightmap plane.
	vUp = Vector3( 0.0, 1.0, 0.0 );
	if( rPlane.normal.x == 0.0 && rPlane.normal.z == 0.0 )	// horizontal surface
		vUp = Vector3( -rPlane.normal.y, 0.0, 0.0 );

	Vec3Cross( vRight, vUp, rPlane.normal );
	Vec3Normalize( vRight, vRight );
	Vec3Cross( vUp, rPlane.normal, vRight );
	Vec3Normalize( vUp, vUp );
	// 2 orthogonal unit vectors, 'vUp' & 'vRight' are now obtained

	lightmap.m_GlobalPose.matOrient.SetColumn( 0, vRight );
	lightmap.m_GlobalPose.matOrient.SetColumn( 1, vUp );
	lightmap.m_GlobalPose.matOrient.SetColumn( 2, rPlane.normal );

	double minup,minright,maxup,maxright;
	minup = 99999;		maxup = -99999;
	minright = 99999;	maxright = -99999;

	iNumFaces = rveciGroupedFacesIndex.size();
	for(i=0; i<iNumFaces; i++)
	{
		CMapFace& rFace = rvecFace.at( rveciGroupedFacesIndex.at(i) );

		iNumVertices = rFace.GetNumVertices();
		for(j=0; j<iNumVertices; j++)
		{
			// up & right are measured in world coordinate
			v = rFace.GetVertex(j);
			double up = Vec3Dot( v, vUp );
			double right = Vec3Dot( v, vRight );
			if( minup > up ) minup = up;
			if( maxup < up ) maxup = up;
			if( minright > right ) minright = right;
			if( maxright < right ) maxright = right;
		}
	}

	lightmap.m_fStartUp = minup;
	lightmap.m_fStartRight = minright;
	lightmap.m_GlobalPose.vPosition
		= lightmap.m_fStartUp * vUp
		+ lightmap.m_fStartRight * vRight
		+ rPlane.dist * rPlane.normal ;

	lightmap.m_fScaleU = maxright - minright;
	lightmap.m_fScaleV = maxup - minup;

	float texels_per_meter = 1.0f / m_TexelSize;
	lightmap.m_Rect.left = 0;
	lightmap.m_Rect.top  = 0;
	lightmap.m_Rect.right  = (int)(lightmap.m_fScaleU * (float)texels_per_meter);
	lightmap.m_Rect.bottom = (int)(lightmap.m_fScaleV * (float)texels_per_meter);

	if( lightmap.m_Rect.right == 0 )
		lightmap.m_Rect.right = 1;	// there should be at least one texel

	if( lightmap.m_Rect.bottom == 0 )
		lightmap.m_Rect.bottom = 1;	// there should be at least one texel

	lightmap.SetSize( lightmap.m_Rect.GetWidth(), lightmap.m_Rect.GetHeight() );
}


void CLightmapBuilder::PackLightmaps()
{
	int i, iNumLightmaps = m_vecLightmap.size();
	int j;//, iNumLightmapTextures = m_vecLightmapTexture.size();
	bool bPacked = false;

	for( i=0; i<iNumLightmaps; i++ )
	{
		CLightmap& rLightmap = m_vecLightmap[i];
		for( j=0; j<m_vecLightmapTexture.size(); j++ )
		{
			bPacked = m_vecLightmapTexture[j].AddLightmap( rLightmap, i );

			if( bPacked )
			{
//				rLightmap.SetTextureUV( rRect, rvecFace, width, height )
				break;
			}
		}

		if( !bPacked )
		{
			// couldn't find a place for lightmap in any of the existing textures
			// - create a new texture
			m_vecLightmapTexture.push_back( CLightmapTexture() );
			m_vecLightmapTexture.back().Resize( m_iTextureWidth, m_iTextureHeight );
			m_vecLightmapTexture.back().AddLightmap( rLightmap, i );
		}
	}

}


void CLightmapBuilder::SetLightmapTextureIndicesToPolygons( vector<CMapFace>& rvecFace )
{
	int i, iNumLightmapTextures = m_vecLightmapTexture.size();

	for( i=0; i<iNumLightmapTextures; i++ )
	{
		m_vecLightmapTexture[i].SetLightmapTextureIndexToFaces( i, m_vecLightmap, rvecFace );
	}
}


/*
void CLightmapBuilder::SetTextureCoords( vector<CMapFace>& rvecFace )
{}
*/

void CLightmapBuilder::UpdateLightmapTextures()
{
	int i, iNumLightmapTextures = m_vecLightmapTexture.size();

	for( i=0; i<iNumLightmapTextures; i++ )
	{
		m_vecLightmapTexture[i].UpdateTexture( m_vecLightmap );
	}
}


void CLightmapBuilder::FillMarginRegions()
{
	int i, iNumLightmapTextures = m_vecLightmapTexture.size();

	for( i=0; i<iNumLightmapTextures; i++ )
	{
		m_vecLightmapTexture[i].FillMarginRegions();
	}
}


void CLightmapBuilder::ApplySmoothing( float fCenterWeight )
{
	int i, iNumLightmapTextures = m_vecLightmapTexture.size();

	for( i=0; i<iNumLightmapTextures; i++ )
	{
		m_vecLightmapTexture[i].ApplySmoothing(fCenterWeight);
	}
}


void CLightmapBuilder::ApplySmoothingToLightmaps()
{
	const size_t iNumLightmaps = m_vecLightmap.size();

	for( size_t i=0; i<iNumLightmaps; i++ )
	{
		m_vecLightmap[i].ApplySmoothing();
	}
}


void CLightmapBuilder::TransformLightDirectionToLocalFaceCoord( vector<CMapFace>& rvecFace )
{
	const size_t iNumLightmaps = m_vecLightmap.size();

	for( size_t i=0; i<iNumLightmaps; i++ )
	{
		m_vecLightmap[i].TransformLightDirectionToLocalFaceCoord( rvecFace );
	}
}


void CLightmapBuilder::OutputLightmapTexturesToBMPFiles( const char *pcBodyFileName )
{

	int i, iNumLightmapTextures = m_vecLightmapTexture.size();
	char acFilename[512];

	for(i=0; i<iNumLightmapTextures; i++)
	{
		sprintf( acFilename, "%s_%02d", pcBodyFileName, i );
		m_vecLightmapTexture[i].OutputToBMPFiles( acFilename );
	}
}




/* ========================== create lightmap texture (old) ==========================

	vector<int> aiGroupedFacesIndex;

	CLightmap lightmap;
	int iNumLightmaps = m_iNumCurrentLightmaps;	// how many lightmaps have been created
	SFloatRGBColor* pLightmapTex;
	Vector3 *pvLightDirTex;
	if (m_apLightmapTexture.size() == 0)
		pLightmapTex = NULL;	//Reached here for the first time
	else
		pLightmapTex = m_apLightmapTexture.back();

	// the number of total texels in a texture for lightmaps
	int iNumTextureTexels = m_iTextureWidth * m_iTextureWidth;

	while(1)
	{
		aiGroupedFacesIndex.clear();

		// Select close faces which can share the same lightmap for memory saving.
		SelectCloseFaces(&aiGroupedFacesIndex, paFaces);
		if( aiGroupedFacesIndex.size() == 0)
			break;	//No face chosen: Lightmaps were created for all the faces in 'paFaces'

		//====================  Lightmap Processing Pipeline Starts ====================


		// Set uv of lightmap to each vertex of the selected faces 
		SetUpLightMapUV(lightmap, &aiGroupedFacesIndex, paFaces);

		// Make vectors (vertices) that point to the texels in a lightmap	(world coordinate)
		SetUpLightMapPoints(lightmap);

		// compute normals for each point on the lightmap
		ComputeNormalsOnLightmap( lightmap, aiGroupedFacesIndex, *paFaces );

		// Determine the intensity of each texel of this lightmap
		CalculateLightmapTexelIntensity(lightmap);

		if( m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
		{	// Experiment with light direction texture
			TransformLightDirectionToLocalFaceCoord( rvecFace.at(aiGroupedFacesIndex[0]), lightmap );
		}

		//=====================  Lightmap Processing Pipeline Ends =====================

		// How many lightmaps the current texture contains
		int iNumLightmapsInTexture = iNumLightmaps % m_iNumLightmapsPerTexture;

		if( iNumLightmapsInTexture == 0 )
		{	// need to allocate new texture memory.  This occures at the first time 
			// of the loop and every time the texture is filled with the lightmaps
			iCurrentTextureID = m_apLightmapTexture.size();

			// allocate texture memory. each texel is recorded as 'SFloatRGBColor' object, which represents 
			// RGB color as 3 floating point values
			pLightmapTex = new SFloatRGBColor [ iNumTextureTexels ];
			memset( pLightmapTex, 0, sizeof(SFloatRGBColor) * iNumTextureTexels );
			m_apLightmapTexture.push_back( pLightmapTex );

			// allocate texel state buffer
			m_apTexelState[iCurrentTextureID] = new char [ iNumTextureTexels ];
			memset( m_apTexelState[iCurrentTextureID], LMP_TEXEL_UNFILLED, sizeof(char) * iNumTextureTexels );

			if( m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
			{	// create texture to record light directions
				pvLightDirTex = new Vector3 [ iNumTextureTexels ];
//				memset( pvLightDirTex, 0, sizeof(Vector3) * iNumTextureTexels );
				for( i=0; i<iNumTextureTexels; i++ )
					pvLightDirTex[i] = Vector3(0,0,1);	// set default light direction ( positive z )
				m_vecpvLightDirMap.push_back( pvLightDirTex );

				// allocate texel state buffer
				m_apTexelState_LightDirMap[iCurrentTextureID] = new char [ iNumTextureTexels ];
				memset( m_apTexelState_LightDirMap[iCurrentTextureID],
					    LMP_TEXEL_UNFILLED, sizeof(char) * iNumTextureTexels );
			}
		}

		// Put the processed lightmap in the texture.
		int iOffsetX, iOffsetY;
		iOffsetX = iNumLightmapsInTexture % ( m_iTextureWidth / m_iLightmapWidth ) * m_iLightmapWidth;
		iOffsetY = iNumLightmapsInTexture / ( m_iTextureWidth / m_iLightmapWidth ) * m_iLightmapWidth;

		int x,y;
		for(x=0; x<m_iLightmapWidth; x++)
		{
			for(y=0; y<m_iLightmapWidth; y++)
			{
				if( lightmap.acTexelState[ LMTexelAt(x,y) ] == LMP_TEXEL_UNFILLED )
					continue;	// don't copy unfilled texel

				SetTexelColor( iCurrentTextureID, iOffsetX + x, iOffsetY + y, lightmap.intensity[ LMTexelAt(x,y) ] );
//				pLightmapTex[ TexelAt( iOffsetX + x , iOffsetY + y ) ]
//					= lightmap.intensity[ LMTexelAt(x,y) ];

				if( m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
				{
					SetValueToLightDirTex( iCurrentTextureID, iOffsetX + x , iOffsetY + y,
						                   lightmap.avLightDir[ LMTexelAt(x,y) ] );
				}
			}
		}

		// Modify the uv values of each lightmap for the current texture
		int iNumGroupedFaces = aiGroupedFacesIndex.size();
		float u,v;
		for(i=0; i<iNumGroupedFaces; i++)
		{
			CMapFace& rFace = rvecFace.at( aiGroupedFacesIndex[i] );
			for(s=0; s<rFace.GetNumVertices(); s++)
			{
				MAPVERTEX vertex = rFace.GetMAPVERTEX(s);

				u = (iOffsetX / (float)m_iTextureWidth)
					+ vertex.vTex1.u / (float)( m_iTextureWidth / m_iLightmapWidth );

				v = (iOffsetY / (float)m_iTextureWidth)
					+ vertex.vTex1.v / (float)( m_iTextureWidth / m_iLightmapWidth );

				rFace.SetLightmapUV(s, u, v);
				rFace.m_sLightMapID = m_apLightmapTexture.size() - 1;	//ID of the Lightmap Texture used by 'rFace'. start from 0
			}
		}
		iNumLightmaps++;
	}
	m_iNumCurrentLightmaps = iNumLightmaps;

	// fill margin regions to prevent unfilled texels from being sampled by linear interpolation during rendering
//	FillMarginRegions();

	return true;
}
/*
void CLightmapBuilder::SetUpLightMapUV(CLightmap& rLightmap,
									   vector<int>& rveciGroupedFacesIndex,
									   vector<CMapFace>& rvecFace)
{
	float localu, localv;

	int i, j, iNumVertices, iNumGroupedFaces = rveciGroupedFacesIndex.size();

	for(i=0; i<iNumGroupedFaces; i++)
	{
		CMapFace& rFace = rvecFace.at( rveciGroupedFacesIndex.at(i) );
		iNumVertices = rFace.GetNumVertices();
		for(j=0; j<iNumVertices; j++)
		{
			Vector3& v = rFace.GetVertex((short)j);
			localu
				= ( Vec3Dot( v, rLightmap.vRight ) - rLightmap.fStartRight ) 
				/ rLightmap.fScaleU;
			localv
				= ( Vec3Dot( v, rLightmap.vUp ) - rLightmap.fStartUp )
				/ rLightmap.fScaleV;

			//shrink the uv value to fit within the actual lightmap area 
			//and avoid entering the margin area which surrounds the lightmap
			localu = (float)m_iMargin / (float)m_iLightmapWidth
				+ localu * (float)(m_iLightmapWidth - m_iMargin * 2) / (float)m_iLightmapWidth;
			localv = (float)m_iMargin / (float)m_iLightmapWidth
				+ localv * (float)(m_iLightmapWidth - m_iMargin * 2) / (float)m_iLightmapWidth;

			rFace.SetLightmapUV((short)j, localu, localv);
			//Note: this is the temporary uv
			//We have to change these uv later according to the position
			//of this lightmap in a texture image,
			//because one texture contains multiple lightmaps
		}
	}
	return;

}


void CLightmapBuilder::SetUpLightMapPoints(CLightmap& rLightmap)
{
	int x,y;
	int iLightmapWidth = m_iLightmapWidth;

	double patchu = rLightmap.fScaleU / (double)(iLightmapWidth - m_iMargin * 2);
	double patchv = rLightmap.fScaleV / (double)(iLightmapWidth - m_iMargin * 2);
	memset(rLightmap.intensity, 0, sizeof(SFloatRGBColor) * iLightmapWidth * iLightmapWidth);
	for(y=0; y<iLightmapWidth - m_iMargin * 2; y++)
	{	
		for(x=0; x<iLightmapWidth - m_iMargin * 2; x++)
		{
			rLightmap.vPoints[ LMTexelAt( x + m_iMargin, y + m_iMargin ) ]
				= rLightmap.vOrigin + 
					( patchu * (double)x + patchu / 2.0 ) * rLightmap.vRight +
					( patchv * (double)y + patchv / 2.0 ) * rLightmap.vUp + 
					m_fSurfaceErrorTolerance * rLightmap.plane.normal;
		}
	}

}


//Calculate the intensity of R, G and B on every texel in the lightmap
void CLightmapBuilder::CalculateLightmapTexelIntensity(CLightmap& rLightmap)
{
	int i,x,y;
	Vector3 vToLight, vToLight_n;
	int iNumLights = m_papLight->size();
	CPointLight* pPointLight = NULL;
	float fRealDist, fAttenuation, fPower, fAngle;

	for(i=0; i<iNumLights; i++)
	{
		//if( typeid(*m_papLight->at(i)) == typeid(CPointLight) )
		pPointLight = dynamic_cast<CPointLight *> (m_papLight->at(i));
		if(!pPointLight)
			continue;	// at present, only the point lights are supported

		SPlane& rPlane = rLightmap.plane;
		for(y=m_iMargin; y<m_iLightmapWidth - m_iMargin; y++)
		{
			for(x=m_iMargin; x<m_iLightmapWidth - m_iMargin; x++)
			{
				float dist =  Vec3Dot( &pPointLight->vPosition, &rPlane.normal ) - rPlane.dist;
				if( dist < 0.0 ) continue;			//The light is behind the faces

				if( m_pMapCompiler->LineCheck( pPointLight->vPosition, rLightmap.vPoints[ LMTexelAt(x,y) ] )
					== CONTENTS_SOLID )
					continue;

				// the light is not obstructed and reaching the target point
				vToLight = pPointLight->vPosition - rLightmap.vPoints[ LMTexelAt(x,y) ];
				fRealDist = Vec3Length(&vToLight);

				fAttenuation = 1.0f / ( pPointLight->fAttenuation0
							          + pPointLight->fAttenuation1 * fRealDist
								      + pPointLight->fAttenuation2 * fRealDist * fRealDist );
				if (fAttenuation>1) fAttenuation=1;

				// normalize light direction
				vToLight_n = vToLight / fRealDist;
//				Vec3Normalize(&vToLight_n, &vToLight);

//				fAngle = Vec3Dot( &vToLight_n, &rPlane.normal );	// use face normal (for flat shading)
				fAngle = Vec3Dot( &vToLight_n, &rLightmap.avNormal[ LMTexelAt(x,y) ]);	// use normal calculated for each lightmap texel (for smooth shading)

				if( m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
				{	// exclude angle factor
					// angle attenuation is calculated by using the light direction map during runtime
					fPower = pPointLight->fIntensity * fAttenuation;
				}
				else
				{
					fPower = pPointLight->fIntensity * fAttenuation * fAngle;
				}

				if( fPower < 0.0 )
					continue;

				// ========================= normal pixel intensity calculation ===========================
				rLightmap.intensity[ LMTexelAt(x,y) ] += pPointLight->color * fPower;

				// ========= replace lightmap colors with normal directions (visual debugging) ============
//				Vector3& rvNormal = rLightmap.avNormal[ LMTexelAt(x,y) ];
//				rLightmap.intensity[ LMTexelAt(x,y) ].fRed   = (float)fabs(rvNormal.x);
//				rLightmap.intensity[ LMTexelAt(x,y) ].fGreen = (float)fabs(rvNormal.y);
//				rLightmap.intensity[ LMTexelAt(x,y) ].fBlue  = (float)fabs(rvNormal.z);

				if( m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
				{
//					rLightmap.avLightDir[ LMTexelAt(x,y) ] += vToLight_n;// * pPointLight->fIntensity;
					rLightmap.avLightDir[ LMTexelAt(x,y) ] += vToLight_n * pPointLight->fIntensity;
				}

				rLightmap.acTexelState[ LMTexelAt(x,y) ] = LMP_TEXEL_FILLED;	// mark as a filled texel

			}
		}
	}
/*	for(i=0; i<m_iMargin; i++)
	{
		for(x=0; x<m_iLightmapWidth; x++)
		{
			rLightmap.intensity[ LMTexelAt( x, m_iMargin -i -1 ) ]
				= rLightmap.intensity[ LMTexelAt( x, m_iMargin -i ) ];
			rLightmap.intensity[ LMTexelAt( x, m_iLightmapWidth-1 - m_iMargin +i +1 ) ]
				= rLightmap.intensity[ LMTexelAt( x, m_iLightmapWidth-1 - m_iMargin +i ) ];
		}
		for(y=0; y<m_iLightmapWidth; y++)
		{
			rLightmap.intensity[ LMTexelAt( m_iMargin -i -1, y ) ]
				= rLightmap.intensity[ LMTexelAt( m_iMargin -i, y ) ];
			rLightmap.intensity[ LMTexelAt( m_iLightmapWidth-1 - m_iMargin +i +1, y ) ]
				= rLightmap.intensity[ LMTexelAt( m_iLightmapWidth-1 - m_iMargin +i, y ) ];
		}
	}*/
/*
}
*/
