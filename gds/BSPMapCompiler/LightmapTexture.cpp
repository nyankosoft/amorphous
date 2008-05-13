#include "Lightmap.h"
#include "LightmapTexture.h"

#include "Support/BitmapImage.h"
#include "Support/Log/DefaultLog.h"



bool SaveToImageFile( const C2DArray<SFloatRGBColor>& texel, const std::string& filepath )
{
	int x,y;
	int width  = texel.size_x();
	int height = texel.size_y();

	CBitmapImage img( width, height, 32 );

	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{
			img.SetPixel( x, y, texel(x,y) );
		}
	}

	return img.SaveToFile( filepath );
}



void CLightmapTexture::Resize( int width, int height )
{
	m_vecTexel.resize( width, height );

	m_vecTexelState.resize( width, height, (char)LMP_TEXEL_UNFILLED );

	m_vecvLightDirMap.resize( width, height );

	m_vecTexelState_LightDirMap.resize( width, height, (char)LMP_TEXEL_UNFILLED );

	SRect rect = SRect( 0, 0, width-1, height-1 );
	m_LightmapTree.SetRectangle( rect );
}


bool CLightmapTexture::AddLightmap( CLightmap& rLightmap, int index )
{
	const SRect& orig_rect = rLightmap.GetRectangle();
	SRect padded_rect;

	padded_rect.left   = orig_rect.left;
	padded_rect.top    = orig_rect.top;
	padded_rect.right  = orig_rect.right + 2;
	padded_rect.bottom = orig_rect.bottom + 2;

	if( CRectNode::INVALID_INDEX == m_LightmapTree.Insert( padded_rect, index ) )
		return false;	// couldn't find a place to put the lightmap
	else
	{
		m_vecLightmapIndex.push_back( index );
		return true;
	}
}


/*
void CLightmapTexture::SetLightmapTextureIndexToFaces( int index,
													   vector<CLightmap>& rvecLightmap
													    )
{
	const size_t iNumLightmaps = m_vecLightmapIndex.size();
	for( size_t i=0; i<iNumLightmaps; i++ )
	{
//		rvecLightmap[ m_vecLightmapIndex[i] ].SetLightmapTextureIndexToFaces( index, rvecFace );
	}

}
*/

void CLightmapTexture::SetTextureUV( vector<CLightmap>& rvecLightmap, int tex_coord_index )
{
	const size_t iNumLightmaps = m_vecLightmapIndex.size();
	int texture_width  = m_vecTexel.size_x();
	int texture_height = m_vecTexel.size_y();


	SRect *pRect = NULL;
	for( size_t i=0; i<iNumLightmaps; i++ )
	{
		pRect = m_LightmapTree.GetRectangle( m_vecLightmapIndex[i] );
		if( pRect )
			rvecLightmap[ m_vecLightmapIndex[i] ].SetTextureUV( *pRect, texture_width, texture_height, tex_coord_index );
	}
}


void CLightmapTexture::UpdateTexture()
{
	vector<CLightmap>& rvecLightmap = *m_pvecLightmap;
	const size_t iNumLightmaps = m_vecLightmapIndex.size();
//	int texture_width  = m_vecTexel.size_x();
//	int texture_height = m_vecTexel.size_y();
	int x,y;
	int s,t;

	SRect *pRect = NULL;
	for( size_t i=0; i<iNumLightmaps; i++ )
	{
		pRect = m_LightmapTree.GetRectangle( m_vecLightmapIndex[i] );
		if( !pRect )
			continue;

		CLightmap& rLightmap = rvecLightmap[ m_vecLightmapIndex[i] ];
		const SRect local_rect = rLightmap.GetRectangle();	// size of the lightmap

		for( y=0; y<local_rect.GetHeight(); y++ )
		{
			for( x=0; x<local_rect.GetWidth(); x++ )
			{
				s = pRect->left + 1 + x;
				t = pRect->top  + 1 + y;

				m_vecTexel(s,t) = rLightmap.GetTexelColor( x, y );

				m_vecTexelState(s,t) = LMP_TEXEL_FILLED;

				m_vecvLightDirMap(s,t) = rLightmap.GetLightDirection(x,y);

				m_vecTexelState_LightDirMap(s,t) = LMP_TEXEL_FILLED;
			}
		}

//		MessageBox( NULL, "copied a lightmap to a texture.", "Message", MB_OK );

		// expand the texels around the lightmap
		// so that it can work well with texture linear filtering
		SRect src_rect = *pRect;
		src_rect.left   += 1;
		src_rect.top    += 1;
		src_rect.right  -= 1;
		src_rect.bottom -= 1;
		ExpandTexels( src_rect );

//		MessageBox( NULL, "expanded a lightmap.", "Message", MB_OK );

		ApplySmoothing( *pRect );
		ApplySmoothing( *pRect );
		ApplySmoothing( *pRect );

//		MessageBox( NULL, "applied smoothing.", "Message", MB_OK );
	}
}


inline int get_new_index( const vector<int>& OldToNewMatIndex, int old_index )
{
	const size_t num_indices = OldToNewMatIndex.size();
	for( size_t i=0; i<num_indices; i++ )
	{
		if( OldToNewMatIndex[i] == old_index )
			return (int)i;
	}
	return -1;
}


bool CLightmapTexture::AddTexturesToDatabase( CBinaryDatabase<std::string>& db )
{
	int texture_data;

	return db.AddData( m_KeyName, texture_data );
}


void CLightmapTexture::UpdateMaterials(
	 vector<CMMA_Material>& src_material_buffer,
//	 vector<CIndexedPolygon>& src_polygon_buffer,
	 vector<CMMA_Material>& new_material_buffer,
	 int texture_archive_index,
	 const std::string& db_filepath
)
{
	vector<int> OldToNewMatIndex;

	int new_mat_index_offset = (int)new_material_buffer.size();

	const int num_lightmaps = GetNumLightmaps();
	for( int i=0; i<num_lightmaps; i++ )
	{
		const CLightmap& lightmap = GetLightmap( i );

		const int num_polygons = lightmap.GetNumPolygons();
		for( int j=0; j<num_polygons; j++ )
		{
			CIndexedPolygon& polygon = lightmap.GetPolygon( j );
			int new_index = get_new_index( OldToNewMatIndex, polygon.m_MaterialIndex );
			if( new_index == -1 )
			{
				new_index = (int)OldToNewMatIndex.size();
				OldToNewMatIndex.push_back( polygon.m_MaterialIndex );
			}

			polygon.m_MaterialIndex = new_mat_index_offset + new_index;
		}
	}

	const size_t num_materials_to_add = OldToNewMatIndex.size();
	for( size_t i=0; i<num_materials_to_add; i++ )
	{
		// copy the source material
		new_material_buffer.push_back( src_material_buffer[OldToNewMatIndex[i]] );

		CMMA_Material& new_material = new_material_buffer.back();

		// add lightmap texture
		while( (int)new_material.vecTexture.size() <= texture_archive_index )
			new_material.vecTexture.push_back( CMMA_Texture() );

//		"(database filepath)::(key name)";
		new_material.vecTexture[texture_archive_index].strFilename = db_filepath + "::" + m_KeyName;

//		new_material.Texture[texture_archive_index].strFilename = m_TextureImageFilepath;
	}
}


void CLightmapTexture::ApplySmoothing( SRect& rect )
{
	int i,j,k;
//	int iNumSamples = 0;
	SFloatRGBColor color;
//	int x[8] = { -1,  0,  1, -1,  1, -1,  0,  1};
//	int y[8] = { -1, -1, -1,  0,  0,  1,  1,  1};
	int x,y;
	int _x[9] = { -1,  0,  1, -1,  0,  1, -1,  0,  1};
	int _y[9] = { -1, -1, -1,  0,  0,  0,  1,  1,  1};
	float weight[9] = { 0.06f, 0.09f, 0.06f,
		                0.09f, 0.40f, 0.09f,
						0.06f, 0.09f, 0.06f };
	float weight_sum;

	int width  = rect.GetWidth();
	int height = rect.GetHeight();
	int m = 0; // margin

	for( j=m; j<height-m; j++ )
	{
		for( i=m; i<width-m; i++ )
		{
			color = SFloatRGBColor( 0,0,0 );
			weight_sum = 0;
			for( k=0; k<9; k++ )
			{
				x = rect.left + i + _x[k];
				y = rect.top  + j + _y[k];
				if( x < rect.left || rect.right < x || y < rect.top || rect.bottom < y )
					continue;

				color += Texel( x, y ) * weight[k];
				weight_sum += weight[k];
			}
			Texel(rect.left + i, rect.top + j) = color / weight_sum;
		}
	}

	if( m_vecvLightDirMap.size_x() == 0 || m_vecvLightDirMap.size_y() == 0 )
		return;

//	if( !() )
//		return;

	// fill margin region of light direction texture
	Vector3 vLightDir;
	for( j=m; j<height-m; j++ )
	{
		for( i=m; i<width-m; i++ )
		{
			vLightDir = Vector3( 0,0,0 );
			for( k=0; k<9; k++ )
			{
				x = rect.left + i + _x[k];
				y = rect.top  + j + _y[k];
				if( x < rect.left || rect.right < x || y < rect.top || rect.bottom < y )
					continue;

				vLightDir += LightDirection( x, y ) * weight[k];
				weight_sum += weight[k];
			}
//			LightDirection(rect.left + i, rect.top + j) = vLightDir / weight_sum;
//			LightDirection(rect.left + i, rect.top + j) = Vec3Normalize( vLightDir );
			Vec3Normalize( LightDirection(rect.left + i, rect.top + j), vLightDir );
		}
	}
}


void CLightmapTexture::ExpandTexels( SRect& rect )
{
	int i, width, height;
/*	SFloatRGBColor frame_v = SFloatRGBColor( 1, 0, 1 );
	SFloatRGBColor frame_h = SFloatRGBColor( 1, 1, 0 );
	Vector3 dir_v = Vector3(1.0f,0.5f,0.5f);
	Vector3 dir_h = Vector3(0.5f,1.0f,0.5f);*/
	SFloatRGBColor color;
	Vector3 vDir;

	height = rect.GetHeight();
	for( i=0; i<height; i++ )
	{
		// left column
		m_vecTexel( rect.left - 1, rect.top + i ) = m_vecTexel( rect.left, rect.top + i );
		m_vecvLightDirMap( rect.left - 1, rect.top + i ) = m_vecvLightDirMap( rect.left, rect.top + i );

		// right column
		m_vecTexel( rect.right + 1, rect.top + i ) = m_vecTexel( rect.right, rect.top + i );
		m_vecvLightDirMap( rect.right + 1, rect.top + i ) = m_vecvLightDirMap( rect.right, rect.top + i );
	}

	width = rect.GetWidth();
//	for( i=-1; i<width+2; i++ )
	for( i=-1; i<width+1; i++ )
	{
		// top row
		m_vecTexel( rect.left + i, rect.top - 1 ) = m_vecTexel( rect.left + i, rect.top );
		m_vecvLightDirMap( rect.left + i, rect.top - 1 ) = m_vecvLightDirMap( rect.left + i, rect.top );

		// bottom row
		m_vecTexel( rect.left + i, rect.bottom + 1 ) = m_vecTexel( rect.left + i, rect.bottom );	///// error
		m_vecvLightDirMap( rect.left + i, rect.bottom + 1 ) = m_vecvLightDirMap( rect.left + i, rect.bottom );	///// error
/*
		// test
		color = m_vecTexel( rect.left + i, rect.top );
		m_vecTexel( rect.left + i, rect.top - 1 ) = color;

		color = m_vecTexel( rect.left + i, rect.bottom );
		m_vecTexel( rect.left + i, rect.bottom + 1 ) = color;

		vDir = m_vecvLightDirMap( rect.left + i, rect.top );
		m_vecvLightDirMap( rect.left + i, rect.top - 1 ) = vDir;

		vDir = m_vecvLightDirMap( rect.left + i, rect.bottom );
		m_vecvLightDirMap( rect.left + i, rect.bottom + 1 ) = vDir;
*/	}
}


void CLightmapTexture::FillMarginRegions()
{
	int i,j,k;
	int iNumSamples = 0;
	SFloatRGBColor color;
	int x[8] = { -1,  0,  1, -1,  1, -1,  0,  1};
	int y[8] = { -1, -1, -1,  0,  0,  1,  1,  1};

	int texture_width  = m_vecTexel.size_x();
	int texture_height = m_vecTexel.size_y();

//		memset( pacTexelState, LMP_TEXEL_UNFILLED, sizeof(char) * m_iTextureWidth * m_iTextureWidth );

	for( i=0; i<texture_width; i++ )
	{
		for( j=0; j<texture_height; j++ )
		{
			if( m_vecTexelState(i,j) != LMP_TEXEL_UNFILLED )
				continue;

			// found an unfilled texel - fill it with the averaged color of adjacent & filled texels
			iNumSamples = 0;
			color.fRed = color.fGreen = color.fBlue = 0;
			for(k=0; k<8; k++)
			{
				if( 0<=i+x[k] && i+x[k]<texture_width && 0<=j+y[k] && j+y[k]<texture_height &&
					m_vecTexelState(i+x[k],j+y[k]) == LMP_TEXEL_FILLED )
				{
					color += Texel(i+x[k], j+y[k]);
					iNumSamples++;
				}
			}

			if( 0 < iNumSamples )
			{
				Texel(i,j) = color / (float)iNumSamples;
				m_vecTexelState(i,j) = LMP_TEXEL_PREV_FILLED;
			}
		}
	}

	for( i=0; i<texture_width; i++ )
	{
		for( j=0; j<texture_height; j++ )
		{
			if( m_vecTexelState(i,j) == LMP_TEXEL_PREV_FILLED )
				m_vecTexelState(i,j) = LMP_TEXEL_FILLED;
		}
	}

	if( m_vecvLightDirMap.size_x() == 0 || m_vecvLightDirMap.size_y() == 0 )
		return;

//	if( !(m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE) )
//		return;

	// fill margin region of light direction texture
	Vector3 vNormal;
	for( i=0; i<texture_width; i++ )
	{
		for( j=0; j<texture_height; j++ )
		{
			if( m_vecTexelState_LightDirMap(i,j) != LMP_TEXEL_UNFILLED )
				continue;

			iNumSamples = 0;
			vNormal = Vector3(0,0,0);
			for( k=0; k<8; k++ )
			{
				if( 0<=i+x[k] && i+x[k]<texture_width && 0<=j+y[k] && j+y[k]<texture_height &&
					m_vecTexelState_LightDirMap(i+x[k],j+y[k]) == LMP_TEXEL_FILLED )
				{
					vNormal += m_vecvLightDirMap(i+x[k], j+y[k]);
					iNumSamples++;
				}
			}

			if( 0 < iNumSamples )
			{
//				vNormal /= (float)iNumSamples;
				Vec3Normalize( &vNormal, &vNormal );
				m_vecvLightDirMap(i,j) = vNormal;
				m_vecTexelState_LightDirMap(i,j) = LMP_TEXEL_PREV_FILLED;
			}
		}
	}

	for( i=0; i<texture_width; i++ )
	{
		for( j=0; j<texture_height; j++ )
		{
			if( m_vecTexelState_LightDirMap(i,j) == LMP_TEXEL_PREV_FILLED )
				m_vecTexelState_LightDirMap(i,j) = LMP_TEXEL_FILLED;
		}
	}
}


void CLightmapTexture::ApplySmoothing( float fCenterWeight )
{
	int i,j,k;
	int iNumSamples = 0;
	SFloatRGBColor color;
	int x[8] = { -1,  0,  1, -1,  1, -1,  0,  1};
	int y[8] = { -1, -1, -1,  0,  0,  1,  1,  1};
	int _x[9] = { -1,  0,  1, -1,  0,  1, -1,  0,  1};
	int _y[9] = { -1, -1, -1,  0,  0,  0,  1,  1,  1};
	float weight[9] = { 0.06f, 0.09f, 0.06f,
		                0.09f, 0.40f, 0.09f,
						0.06f, 0.09f, 0.06f };

	int texture_width  = m_vecTexel.size_x();
	int texture_height = m_vecTexel.size_y();

	for( i=1; i<texture_width-1; i++ )
	{
		for( j=1; j<texture_height-1; j++ )
		{
/*			color = Texel(i,j) * fCenterWeight;
			for(k=0; k<8; k++)
			{
				color += Texel(i+x[k], j+y[k]) * (1.0f - fCenterWeight) * 0.125f;
			}
			Texel(i,j) = color;*/

			color = SFloatRGBColor( 0,0,0 );
			for( k=0; k<9; k++ )
				color += Texel(i+_x[k], j+_y[k]) * weight[k];
			Texel(i,j) = color;

		}
	}

	if( m_vecvLightDirMap.size_x() == 0 || m_vecvLightDirMap.size_y() == 0 )
		return;

//	if( !(m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE) )
//		return;

	// fill margin region of light direction texture
//	Vector3 vNormal;
	Vector3 vLightDir;
	for( i=1; i<texture_width-1; i++ )
	{
		for( j=1; j<texture_height-1; j++ )
		{
/*			vNormal = LightDirection(i,j) * fCenterWeight;	//Vector3(0,0,0);
			for( k=0; k<8; k++ )
				vNormal += m_vecvLightDirMap(i+x[k], j+y[k]) * (1.0f - fCenterWeight) * 0.125f;
			Vec3Normalize( vNormal, vNormal );
			LightDirection(i,j) = vNormal;*/

			vLightDir = Vector3( 0,0,0 );
			for( k=0; k<9; k++ )
				vLightDir += LightDirection(i+_x[k], j+_y[k]) * weight[k];
			LightDirection(i,j) = vLightDir;
		}
	}

}


bool CLightmapTexture::SaveTextureImageToFile( const std::string& filepath )
{
	return SaveToImageFile( m_vecTexel, filepath );
}


//	shared_ptr<CBitmapImage> pImage = CreateBitMapImage(
//	SaveImageFile( padwImageData, filepath );

/*
	// output light direction map if there is one
	if( m_vecvLightDirMap.size_x() != 0 )//m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
	{
		Vector3 vLightDir;
		DWORD dwColor;
		i = 0;
		for( y=0; y<height ; y++ )
		{
			for( x=0; x<width; x++ )
			{	// convert each pixel into DWORD and store in the array
//				vLightDir = m_vecpvLightDirMap[i][j];

				vLightDir = LightDirection(x,y);
				vLightDir = ( vLightDir + Vector3(1,1,1) ) * 0.5f;

				dwColor = D3DCOLOR_XRGB( ((int)(vLightDir.x * 255.0f)),
									     ((int)(vLightDir.y * 255.0f)),
										 ((int)(vLightDir.z * 255.0f)) );

				padwImageData[i++] = dwColor;
			}
		}

		sprintf( acFilename, "%s_LightDir.bmp", image_body_filename.c_str() );

		bmp_image.OutputImage_24Bit( acFilename, width, height, padwImageData );
	}
*/

//	delete [] padwImageData;


/*
void CLightmapTexture::OutputToBMPFiles( const std::string& image_body_filename )
{
	CBMPImageExporter bmp_image;
	char acFilename[512];

	int i;
	int x,y;
	int width  = m_vecTexel.size_x();
	int height = m_vecTexel.size_y();

	DWORD *padwImageData = new DWORD [ width * height ];

	i = 0;
	for( y=0; y<height ; y++ )
	{
		for( x=0; x<width; x++ )
		{	// convert each pixel into DWORD and store in the array
//			padwImageData[i++] = Texel(x,y).ConvertToD3DCOLOR();
			padwImageData[i++] = Texel(x,y).GetARGB32();
		}
	}

	sprintf( acFilename, "%s.bmp", image_body_filename.c_str() );

	bmp_image.OutputImage_24Bit( acFilename, width, height, padwImageData );


	// output light direction map if there is one
	if( m_vecvLightDirMap.size_x() != 0 //m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
	{
		Vector3 vLightDir;
		DWORD dwColor;
		i = 0;
		for( y=0; y<height ; y++ )
		{
			for( x=0; x<width; x++ )
			{	// convert each pixel into DWORD and store in the array
//				vLightDir = m_vecpvLightDirMap[i][j];

				vLightDir = LightDirection(x,y);
				vLightDir = ( vLightDir + Vector3(1,1,1) ) * 0.5f;

				dwColor = D3DCOLOR_XRGB( ((int)(vLightDir.x * 255.0f)),
									     ((int)(vLightDir.y * 255.0f)),
										 ((int)(vLightDir.z * 255.0f)) );

				padwImageData[i++] = dwColor;
			}
		}

		sprintf( acFilename, "%s_LightDir.bmp", image_body_filename.c_str() );

		bmp_image.OutputImage_24Bit( acFilename, width, height, padwImageData );
	}

	delete [] padwImageData;
}
*/