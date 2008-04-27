
#include "Lightmap.h"



//================================================================================
// CLightmap::Methods()                                               - CLightmap
//================================================================================

CLightmap::CLightmap()
{
	m_fStartRight = m_fStartUp = 0;
	m_fScaleU = m_fScaleV = 0;
}


void CLightmap::Clear()
{
/*	int i, iNumTexels = MAX_LIGHTMAP_WIDTH * MAX_LIGHTMAP_WIDTH;

	SFloatRGBColor default_color;
	for( i=0; i<iNumTexels; i++ )
	{
		vPoints[i]      = Vector3(0,0,0);
		avNormal[i]     = Vector3(0,0,0);
		acTexelState[i] = LMP_TEXEL_UNFILLED;	// mark texels as unfilled
		intensity[i]    = default_color;
		avLightDir[i]   = Vector3(0,0,0);	// direction to light
	}*/
}


void CLightmap::SetSize( const int width, const int height )
{

	Vector3 vZero = Vector3(0,0,0);
//	m_vecvNormal.resize( width, height, Vector3(0,0,0) );
//	m_vecvNormal.resize( width, height, vZero );
	m_vecvNormal.resize( width, height );

//	m_vecTexelState.resize( width, height, LMP_TEXEL_UNFILLED );

	SFloatRGBColor color;
	color.fRed = color.fGreen = 0.0f;
//	color.fBlue = 1.0f;
	color.fBlue = 0.0f;
	m_vecIntensity.resize( width, height, color );

//	m_vecbValidTexel.resize( width, height, true );
	m_vecbValidTexel.resize( width, height, 1 );

	// directions from each point to light
	m_vecvLightDir.resize( width, height, Vector3(0,0,0) );
}


void CLightmap::SetLightmapTextureIndexToFaces( int index, vector<CMapFace>& rvecFace )
{
	int i, iNumFaces = m_vecGroupedFaceIndex.size();

	for( i=0; i<iNumFaces; i++ )
	{
		rvecFace[ m_vecGroupedFaceIndex[i] ].SetLightmapTextureIndex( index );
	}
}


void CLightmap::SetTextureUV( SRect& rRect,
							  vector<CMapFace>& rvecFace,
							  const int iTextureWidth,
							  const int iTextureHeight )
{
	int i, iNumGroupFaces = m_vecGroupedFaceIndex.size();
	int j, iNumVertices;
	TEXCOORD2 texcoord;

	Vector3 vLocalPos;

	for(i=0; i<iNumGroupFaces; i++)
	{
		CMapFace& rFace = rvecFace[ m_vecGroupedFaceIndex[i] ];
		iNumVertices = rFace.GetNumVertices();
		for(j=0; j<iNumVertices; j++)
		{
			// transform into local space
			m_GlobalPose.InvTransform( vLocalPos, rFace.GetVertex(j) );

			texcoord.u = ((float)(rRect.left+1 + 0.5f) / (float)iTextureWidth)
				       + (vLocalPos.x / m_fScaleU) * ((float)(rRect.GetWidth()-2)  / (float)iTextureWidth);

			texcoord.v = ((float)(rRect.top+1 + 0.5f) / (float)iTextureHeight)
				       + (vLocalPos.y / m_fScaleV) * ((float)(rRect.GetHeight()-2) / (float)iTextureHeight);

			rFace.SetLightmapTextureUV( j, texcoord );
		}
	}
}


void CLightmap::ComputeNormalsOnLightmap( vector<CMapFace>& rvecFace )
{
	int i, iNumGroupedFaces = m_vecGroupedFaceIndex.size();

	int x,y;
	int width  = m_Rect.GetWidth(); // m_vecIntensity.size_x();
	int height = m_Rect.GetHeight(); // m_vecIntensity.size_y();

	float fDist, fMinDist;
	int iMinFaceIndex;

//	memset(rLightmap.avNormal, 0, sizeof(Vector3) * iLightmapWidth * iLightmapWidth);

	for(y=0; y<height /*iLightmapWidth - m_iMargin * 2*/; y++)
	{	
		for(x=0; x<width /*iLightmapWidth - m_iMargin * 2*/; x++)
		{
//			Vector3& rvPoint = TexelAt( x + m_iMargin, y + m_iMargin ) ];
			Vector3& rvPoint = GetPoint( x, y );

			fMinDist = 99999;
			for(i=0; i<iNumGroupedFaces; i++)
			{
				CMapFace& rFace = rvecFace[ m_vecGroupedFaceIndex[i] ];
				rFace.IsInSweptVolume(rvPoint, fDist);
				if( fDist < fMinDist )
				{
					fMinDist = fDist;
					iMinFaceIndex = i;
				}
			}

			m_vecvNormal(x,y) = rvecFace[ m_vecGroupedFaceIndex[iMinFaceIndex] ].GetInterpolatedNormal( rvPoint );

			if( true /*m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE*/ )
			{	// use normal as default light direction
				// make vector length short so that the default direction will not have much influence
				m_vecvLightDir(x,y) = m_vecvNormal(x,y) * 0.0001f;
			}
		}
	}
}


void CLightmap::TransformLightDirectionToLocalFaceCoord( vector<CMapFace>& rvecFace )
{

	// there should be only one face per lightmap when the light direction map is created
	// i.e. assert( m_vecGroupedFaceIndex.size() == 1 )
	
	CMapFace& rFace = rvecFace[ m_vecGroupedFaceIndex[0] ];

	// normalize the light direction vectors
	int width  = m_Rect.GetWidth();
	int height = m_Rect.GetHeight();
	int x, y;

	D3DXMATRIX matToLocal;
	rFace.GetTransformationToLocalFaceCoord_Tex0( matToLocal );

	Vector3 v;

	for( x=0; x<width; x++ )
	{
		for( y=0; y<height; y++ )
		{
			Vec3Normalize( v, m_vecvLightDir(x,y) );

			D3DXVec3TransformCoord( &m_vecvLightDir(x,y), &v, &matToLocal );
		}
	}

/*
	int i, iNumVectors = MAX_LIGHTMAP_WIDTH * MAX_LIGHTMAP_WIDTH;
	for( i=0; i<iNumVectors; i++ )
		Vec3Normalize( &rLightmap.avLightDir[i], &rLightmap.avLightDir[i] );


	D3DXMATRIX matToLocal;
	rFace.GetTransformationToLocalFaceCoord_Tex0( matToLocal );

	// transform light direction to the local space of face
	D3DXVec3TransformCoordArray( rLightmap.avLightDir, sizeof(Vector3),
		                         rLightmap.avLightDir, sizeof(Vector3),
								 &matToLocal, iNumVectors );*/
}


void CLightmap::ApplySmoothing()
{
	int i,j,k;
	SFloatRGBColor color;
//	int x[8] = { -1,  0,  1, -1,  1, -1,  0,  1};
//	int y[8] = { -1, -1, -1,  0,  0,  1,  1,  1};
	int _x[9] = { -1,  0,  1, -1,  0,  1, -1,  0,  1};
	int _y[9] = { -1, -1, -1,  0,  0,  0,  1,  1,  1};
	float weight[9] = { 0.06f, 0.09f, 0.06f,
		                0.09f, 0.40f, 0.09f,
						0.06f, 0.09f, 0.06f };

	int width  = m_Rect.GetWidth();
	int height = m_Rect.GetHeight();

	for( i=1; i<width-1; i++ )
	{
		for( j=1; j<height-1; j++ )
		{
/*			color = Texel(i,j) * fCenterWeight;
			for(k=0; k<8; k++)
				color += Texel(i+x[k], j+y[k]) * (1.0f - fCenterWeight) * 0.125f;
			Texel(i,j) = color;*/

			color = SFloatRGBColor( 0,0,0 );
			for( k=0; k<9; k++ )
				color += m_vecIntensity(i+_x[k], j+_y[k]) * weight[k];
			m_vecIntensity(i,j) = color;

		}
	}

	if( m_vecvLightDir.size_x() == 0 || m_vecvLightDir.size_y() == 0 )
		return;

//	if( !(m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE) )
//		return;

	// fill margin region of light direction texture
	Vector3 vLightDir;
	for( i=1; i<width-1; i++ )
	{
		for( j=1; j<height-1; j++ )
		{
/*			vLightDir = LightDirection(i,j) * fCenterWeight;	//Vector3(0,0,0);
			for( k=0; k<8; k++ )
				vLightDir += m_vecvLightDirMap(i+x[k], j+y[k]) * (1.0f - fCenterWeight) * 0.125f;
			Vec3Normalize( vLightDir, vLightDir );
			LightDirection(i,j) = vLightDir;*/

			vLightDir = Vector3( 0,0,0 );
			for( k=0; k<9; k++ )
				vLightDir += m_vecvLightDir(i+_x[k], j+_y[k]) * weight[k];
			m_vecvLightDir(i,j) = vLightDir;
		}
	}
}
