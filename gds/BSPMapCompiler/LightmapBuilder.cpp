#include "LightmapBuilder.h"
#include <direct.h>

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


void CLightmapBuilder::ComputeNormalsOnLightmap()
{
	LOG_FUNCTION_SCOPE();

	const size_t num_lightmaps = m_vecLightmap.size();
	for( size_t i=0; i<num_lightmaps; i++ )
	{
		m_vecLightmap[i].ComputeNormalsOnLightmap();
	}
}


bool CLightmapBuilder::UpdateTextureCoordinates()
{
	LOG_FUNCTION_SCOPE();

	unsigned int tex_flags[] =
	{
		CMMA_VertexSet::VF_2D_TEXCOORD0,
		CMMA_VertexSet::VF_2D_TEXCOORD1,
		CMMA_VertexSet::VF_2D_TEXCOORD2,
		CMMA_VertexSet::VF_2D_TEXCOORD3
	};

	m_Desc.m_pMesh->SetVertexFormatFlags( m_Desc.m_pMesh->GetVertexFormatFlags() | tex_flags[m_Desc.m_LightmapTextureCoordsIndex] );

	// Each general vertex must have texture coordinates for lightmap texture
	vector<CGeneral3DVertex>& vert_buffer = *(m_Desc.m_pMesh->GetVertexBuffer().get());
	if( vert_buffer.size() == 0 )
	{
		LOG_PRINT_ERROR( " - Source geometry has no vertex" );
		return false;
	}

	const int num_tex_coords_to_add = m_Desc.m_LightmapTextureCoordsIndex - vert_buffer[0].m_TextureCoord.size() + 1;
	const size_t num_vertices = vert_buffer.size();
	for( size_t i=0; i<num_vertices; i++ )
	{
		for( int j=0; j<num_tex_coords_to_add; j++ )
		{
			vert_buffer[i].m_TextureCoord.push_back( TEXCOORD2(0,0) );
		}
	}

	// update uv values of polygons
	const size_t num_lightmap_textures = m_vecLightmapTexture.size();
	for( size_t i=0; i<num_lightmap_textures; i++ )
		m_vecLightmapTexture[i].SetTextureUV( m_vecLightmap, m_Desc.m_LightmapTextureCoordsIndex );

	return true;
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
//	UnweldVerticesOfPolygonsOnDifferentPlanes( desc.m_pMesh->GetPolygonBuffer() );

	// Create lightmaps

	// group faces which are close to one another and on the same plane
	// - Instances of lightmap are created inside the function
	GroupFaces();

	const size_t num_lightmaps = m_vecLightmap.size();

	// calculate the 2 orthogonal axes for the lightmap ('vRight' & 'vUp')
	// and determine the dimension of the lightmap
	for( size_t i=0; i<num_lightmaps; i++ )
		CalculateLightMapPosition( m_vecLightmap[i] );

	// compute normals for all the sampling points of the lightmaps
	ComputeNormalsOnLightmap();

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

	// [in,out] mesh
	// [in] current material buffer
	// [out] updated material buffer
	UpdateMeshMaterials();

	bool res = UpdateTextureCoordinates();

	if( !res )
		return false;

	// copy texels on the lightmaps to lightmap textures
	UpdateLightmapTextures();

	LOG_PRINT( " - Updated lightmap textures." );

	mkdir( "./temp" );
	const size_t num_lightmap_textures = m_vecLightmapTexture.size();
	for( size_t i=0; i<num_lightmap_textures; i++ )
	{
		m_vecLightmapTexture[i].SaveTextureImageToFile();

//		string image_filepath = fmt_string( "./temp/lightmap%03d.bmp", i );
//		m_vecLightmapTexture[i].SaveTextureImageToFile( image_filepath );
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

	bool m_GroupPolygonsSharingVertices = false;

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
			break;
		}

		if( m_fMaxAllowedLightmapArea < totalarea )
			continue;		// The polygon is large enough to create a lightmap only for it.

		// Second, collect the faces that are close neighbors to the 'face'

		if( !m_GroupPolygonsSharingVertices
		 || m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
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

	// unweld vertices between every pair polygon groups
	const size_t num_lightmaps = m_vecLightmap.size();
	for( size_t i=0; i<num_lightmaps; i++ )
	{
		for( size_t j=i+1; j<num_lightmaps; j++ )
		{
			UnweldVerticesBetween2GroupsOfPolygons(
				polygon_buffer,
				m_vecLightmap[i].GetGroupedFacesIndex(),
				m_vecLightmap[j].GetGroupedFacesIndex()
				);
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

	// TODO: Replace this with CreateOrientFromFwdDir()?

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

			// set the image filepath or database keyname?

			string id_and_ext = fmt_string("%03d.",i) + m_Desc.m_ImageFileFormat;
			m_vecLightmapTexture.back().SetImageFilepath( "./temp/lightmap" + id_and_ext );

//			string keyname = m_Desc.m_OutputDatabaseFilepath + "/" + m_Desc.m_BaseTextureKeyname + id_and_ext;
//			m_vecLightmapTexture.back().SetKeyName( keyname );
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
		m_vecLightmapTexture[i].UpdateTexture();
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

/*
void CLightmapBuilder::AddLightmapTexturesToDB( CBinaryDatabase<std::string>& db )
{
	const int num_textures = (int)m_vecLightmapTexture.size();
	for( int i=0; i<num_textures; i++)
	{
		CImageArchive img( 
		m_vecLightmapTexture[i].SaveTextureImageToFile(
			dirpath_and_bodyname + fmt_string("%03d",i) + img_file_suffix );
	}
}
*/


/*
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
*/