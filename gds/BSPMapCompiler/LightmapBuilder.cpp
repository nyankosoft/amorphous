#include "_LightmapBuilder.h"
#include <typeinfo>

#include "3DMath/AABTree.h"
#include "Support/SafeDelete.h"
#include "Support/StringAux.h"
#include "Support/Log/DefaultLog.h"

#include "LightmapLightingManager.h"
//#include "AmbientOcclusionLightmapBuilder.h"


void CLightmapOption::LoadFromFile( CTextFileScanner& scanner )
{
	scanner.TryScanLine( "TexelSize",			fTexelSize );
	scanner.TryScanLine( "TextureWidth",		TextureWidth );
	scanner.TryScanLine( "TextureWidth",		TextureHeight );

	bool light_dir_map = false;
	if( scanner.TryScanBool( "LightDirectionMap", "Yes/No", light_dir_map) )
	{
		bCreateLightDirectionMap = true;
	}

	scanner.TryScanLine( "AO.SceneResolution",	AO_SceneResolution );
//	scanner.TryScanLine( "EnvLightResolution",	EnvLightResolution );
}



//================================================================================
// CLightmapBuilder::Methods()                                 - CLightmapBuilder
//================================================================================

// Set default value
// - At present, 'point light' and single 'ambient light' are supported.
// - (Previous) Get the pointer to the array of lights in the map via 'pMapCompiler'
CLightmapBuilder::CLightmapBuilder()
{
	m_fMaxAllowedLightmapArea = 25.0f;
	m_iMargin = 1;

	m_iLightmapCreationFlag = 0;
//	m_iLightmapCreationFlag |= LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE;

//	m_iTextureWidth = 256;
//	m_TexelSize = 1.0f;
//	m_pLightingSimulator = NULL;

}


CLightmapBuilder::~CLightmapBuilder()
{
	Clear();
}


void CLightmapBuilder::Clear()
{
	m_vecLightmap.clear();
	m_vecLightmapTexture.clear();
}


void CLightmapBuilder::Init()
{
	Clear();

///	m_pLightingSimulator = new CLightingForLightmap_SimpleRaytrace<CMapFace>;
//	m_pLightingSimulator->RaiseOptionFlag( LF_IGNORE_ANGLE_FACTOR );
//	m_pLightingSimulator->RaiseOptionFlag( LF_USE_HEMISPHERIC_LIGHT );

}


void CLightmapBuilder::SetOption( const CLightmapOption& option )
{
//	SetTextureWidth( option.TextureWidth );
//	SetTextureHeight( option.TextureHeight );
//	m_TexelSize = option.fTexelSize;

	if( option.bCreateLightDirectionMap )
		m_iLightmapCreationFlag |= LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE;

}


bool CLightmapBuilder::CreateLightmapTexture( CLightmapDesc& desc )
{
	LOG_FUNCTION_SCOPE();

	if( !desc.m_pMesh )//|| !desc.pPolygonMesh )
		return false;

	m_Desc = desc;

	SetOption( desc.m_Option );

	// A lightmap can be shared by polygons only if they are on the same plane
	// - Unweld vertices of polygons that are not on the same plane
	UnweldVerticesOfPolygonsOnDifferentPlanes( desc.m_pMesh->GetPolygonBuffer() );

	// Create lightmaps

	// group faces which are close to one another and on the same plane
	// - Instances of lightmap are created inside the function
	GroupFaces();

	const size_t num_lightmaps = m_vecLightmap.size();

	// calculate the 2 orthogonal axes for the lightmap ('vRight' & 'vUp')
	// and determine the dimension of the lightmap
	for( size_t i=0; i<num_lightmaps; i++ )
		CalculateLightMapPosition( m_vecLightmap[i] );

	for( size_t i=0; i<num_lightmaps; i++ )
	{
		m_vecLightmap[i].ComputeNormalsOnLightmap();
	}

	CNonLeafyAABTree<CIndexedPolygon> polygon_tree;
	polygon_tree.Build( m_Desc.m_pMesh->GetPolygonBuffer() );

	CLightmapLightingManager lighting_mgr;
	lighting_mgr.CreateLightmaps( m_Desc.m_pvecpLight, &m_vecLightmap, &polygon_tree );


	// calculate the color of points on each lightmap
///	m_pLightingSimulator->Calculate( m_vecLightmap, rPolygonMesh );

	// 10:29 2007/07/04
	if( /* use_ao == */ false )
	{
///		AmbientOcclusionLightmapBuilder ao;
///		ao.Calculate( m_vecLightmap, desc );
	}


	// transform the light direction vectors on each lightmap into local space
	TransformLightDirectionToLocalFaceCoord();

	LOG_PRINT( " - Light direction maps have been transformed into tangent spaces." );

	// create lightmap textures
	PackLightmaps();

	LOG_PRINT( " - Lightmaps have been packed to textures." );

	// set lightmap texture id to each polygon
//	SetLightmapTextureIndicesToPolygons();

	// [in,out] mesh
	// [in] current material buffer
	// [out] updated material buffer
	UpdateMeshMaterials();

	// update uv values of polygons
	const size_t num_lightmap_textures = m_vecLightmapTexture.size();
	for( size_t i=0; i<num_lightmap_textures; i++ )
		m_vecLightmapTexture[i].SetTextureUV( m_vecLightmap, m_Desc.m_LightmapTextureCoordsIndex );

	LOG_PRINT( " - Lightmap texture uv coords have been calculated." );


	// copy texels on the lightmaps to lightmap textures
	UpdateLightmapTextures();

	LOG_PRINT( " - Updated lightmap textures." );

	for( size_t i=0; i<num_lightmap_textures; i++ )
	{
		m_vecLightmapTexture[i].SaveTextureImageToFile( fmt_string( "lightmap%03d.bmp", i ) );
	}

//	FillMarginRegions();

///	ApplySmoothing( 0.60f );
///	ApplySmoothing( 0.60f );

	return true;
}


void CLightmapBuilder::UpdateMeshMaterials()
{
	vector<CMMA_Material> new_material_buffer;
	vector<CIndexedPolygon>& orig_polygon_buffer = m_Desc.m_pMesh->GetPolygonBuffer();
	vector<CMMA_Material>& src_material_buffer = m_Desc.m_pMesh->GetMaterialBuffer();

	new_material_buffer.reserve( src_material_buffer.size() );

	const size_t num_lightmap_textures = m_vecLightmapTexture.size();
	for( size_t i=0; i<num_lightmap_textures; i++ )
	{
		m_vecLightmapTexture[i].UpdateMaterials(
			src_material_buffer,
//			orig_polygon_buffer,
			new_material_buffer,
			m_Desc.m_LightmapTextureArchiveIndex,
			m_Desc.m_OutputDatabaseFilepath
			);
	}

	// replace the original material buffer with the new one
	m_Desc.m_pMesh->GetMaterialBuffer() = new_material_buffer;
}


void CLightmapBuilder::GroupFaces()
{
	//vector<int>& rveciGroupedFacesIndex, 
	vector<CIndexedPolygon>& polygon_buffer = m_Desc.m_pMesh->GetPolygonBuffer();

	std::vector<int> vecTargetPolygonIndex;
	const size_t num_polygons = m_Desc.m_pMesh->GetPolygonBuffer().size();
 
	for( size_t i=0; i<num_polygons; i++ )
	{
		if( m_Desc.m_vecEnableLightmapForSurface[ polygon_buffer[i].m_MaterialIndex ] )
			vecTargetPolygonIndex.push_back( (int)i );
	}

	const size_t num_lightmapped_polygons = vecTargetPolygonIndex.size();

	int j, k;

	vector<int> Grouped;
	Grouped.resize( num_polygons, 0 );

	float totalarea, area2;

	totalarea = 0;

	while( 1 )
	{
		m_vecLightmap.push_back( CLightmap() );
		CLightmap& rLightmap = m_vecLightmap.back();

		rLightmap.SetPolygonBuffer( &m_Desc.m_pMesh->GetPolygonBuffer() );

		// First, set the seed for the face-group
		size_t i;
		int index0 = 0;
		for( i=0; i<num_lightmapped_polygons; i++ )
		{
			index0 = vecTargetPolygonIndex[i];
			const CIndexedPolygon& polygon0 = polygon_buffer[ index0 ];

			if( Grouped[index0] )
				continue; // the lightmap has been already created for this face.

			rLightmap.AddFaceIndex( index0 );
			Grouped[index0] = 1;
			totalarea = polygon0.CalculateArea();
			break;
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
		CIndexedPolygon& polygon0 = polygon_buffer[ index0 ];

		for( j=i+1; j<num_lightmapped_polygons; j++ )
		{
			int index1 = vecTargetPolygonIndex[j];
			CIndexedPolygon& polygon1 = polygon_buffer[ index1 ];
			if( Grouped[index1] )
				continue;	//this face already has a lightmap

			if( !AreOnSamePlane( polygon0, polygon1 ) )
				continue;	// pFace0 & pFace1 exist on different planes - cannot share lightmaps

			for(k=0; k<rveciGroupedFacesIndex.size(); k++)
			{
//				if( pFace1->SharingPointWith( rvecFace[ rveciGroupedFacesIndex[k] ] ) )
				if( polygon1.SharesPointWith( polygon_buffer[ rveciGroupedFacesIndex[k] ] ) )
					break;
			}

			if( k == rveciGroupedFacesIndex.size() )
				continue;	// sharing no point: the faces are seperate and shouldn't be packed into the same lightmap

			area2 = polygon1.CalculateArea();
			if( m_fMaxAllowedLightmapArea < totalarea + area2 )
				continue;

			// add to the current face group
			Grouped[index1] = 1;
			totalarea += area2;
			rveciGroupedFacesIndex.push_back( index1 );
		}
	}

	return;
}


/// Defines the 2 orthogonal axis vectors on the lightmap plane
/// and calculate the size of the rectangle which the lightmap covers
void CLightmapBuilder::CalculateLightMapPosition( CLightmap& lightmap )
{
	int j, iNumVertices;

	vector<int>& rveciGroupedFacesIndex = lightmap.m_vecGroupedFaceIndex;

	vector<CIndexedPolygon>& polygon_buffer = m_Desc.m_pMesh->GetPolygonBuffer();
	const CIndexedPolygon& polygon0 = polygon_buffer[ rveciGroupedFacesIndex[0] ];
	const SPlane& rPlane = polygon0.GetPlane();

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

	const size_t num_polygons = rveciGroupedFacesIndex.size();
	for(size_t i=0; i<num_polygons; i++)
	{
		CIndexedPolygon& polygon = polygon_buffer[ rveciGroupedFacesIndex.at(i) ];

		iNumVertices = polygon.GetNumVertices();
		for(j=0; j<iNumVertices; j++)
		{
			// up & right are measured in world coordinate
			v = polygon.GetVertex(j).m_vPosition;
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

	float texels_per_meter = 1.0f / GetTexelSize();
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
	const int iNumLightmaps = (int)m_vecLightmap.size();
	bool bPacked = false;

	for( int i=0; i<iNumLightmaps; i++ )
	{
		CLightmap& rLightmap = m_vecLightmap[i];
		for( int j=0; j<m_vecLightmapTexture.size(); j++ )
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
			m_vecLightmapTexture.push_back( CLightmapTexture(&m_vecLightmap) );
			m_vecLightmapTexture.back().Resize( GetLightmapTextureWidth(), GetLightmapTextureHeight() );
			m_vecLightmapTexture.back().AddLightmap( rLightmap, i );
		}
	}

}

/*
void CLightmapBuilder::SetLightmapTextureIndicesToPolygons()
{
	int i, iNumLightmapTextures = m_vecLightmapTexture.size();

	for( i=0; i<iNumLightmapTextures; i++ )
	{
		m_vecLightmapTexture[i].SetLightmapTextureIndexToFaces( i, m_vecLightmap );
	}
}
*/

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


void CLightmapBuilder::TransformLightDirectionToLocalFaceCoord()
{
/*	const size_t iNumLightmaps = m_vecLightmap.size();

	for( size_t i=0; i<iNumLightmaps; i++ )
	{
		m_vecLightmap[i].TransformLightDirectionToLocalFaceCoord( rvecFace );
	}*/
}


void CLightmapBuilder::SaveLightmapTexturesToImageFiles( const std::string& dirpath_and_bodyname,
														const std::string& img_file_suffix )
{

	const int num_textures = (int)m_vecLightmapTexture.size();
	for( int i=0; i<num_textures; i++)
	{
		m_vecLightmapTexture[i].SaveTextureImageToFile(
			dirpath_and_bodyname + fmt_string("%03d",i) + img_file_suffix );
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
