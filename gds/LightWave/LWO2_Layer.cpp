#include "LWO2_Layer.hpp"
#include "LWO2_Object.hpp"
#include "gds/3DMath/Vector2.hpp"
#include "gds/Support/Log/DefaultLog.hpp"

#include "assert.h"
#include <algorithm>

using namespace std;


//================================================================================
// SVertexColor_LWO2::Methods()                               - SVertexColor_LWO2
//================================================================================

// return value is the same format as D3DCOLOR
UINT4 SVertexColor_LWO2::GetColor_UINT4_ARGB()
{
	unsigned char a, r, g, b;
	
	a = (unsigned char)( fAlpha * 255.0f );
	r = (unsigned char)( fRed   * 255.0f );
	g = (unsigned char)( fGreen * 255.0f );
	b = (unsigned char)( fBlue  * 255.0f );

	return ((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff));

//	return D3DCOLOR_ARGB(a,r,g,b);
}



//================================================================================
// CLWO2_VertexColorMap::Methods()                         - CLWO2_VertexColorMap
//================================================================================

CLWO2_VertexColorMap::CLWO2_VertexColorMap(const CLWO2_VertexColorMap& vc_map)
:
iNumIndices(0),
paVertexColor(NULL)
{
	strName = vc_map.strName;

	if( 0 < iNumIndices )
	{
		Release();
	}

	if( 0 < vc_map.iNumIndices )
	{
		paVertexColor = new SVertexColor_LWO2 [vc_map.iNumIndices];
		memcpy( paVertexColor, vc_map.paVertexColor, sizeof(SVertexColor_LWO2) * vc_map.iNumIndices );
	}

	iNumIndices = vc_map.iNumIndices;
}


CLWO2_VertexColorMap CLWO2_VertexColorMap::operator=(CLWO2_VertexColorMap vc_map)
{
	strName = vc_map.strName;

	if( 0 < iNumIndices )
	{
		Release();
	}

	if( 0 < vc_map.iNumIndices )
	{
		paVertexColor = new SVertexColor_LWO2 [vc_map.iNumIndices];
		memcpy( paVertexColor, vc_map.paVertexColor, sizeof(SVertexColor_LWO2) * vc_map.iNumIndices );
	}

	iNumIndices = vc_map.iNumIndices;

	return *this;
}


//================================================================================
// CLWO2_Layer::Methods()                                           - CLWO2_Layer
//================================================================================

void CLWO2_Layer::ReadLayerChunk(UINT4& chunksize, FILE* fp)
{
	m_iLayerIndex = ReadBE2BytesIntoLE(fp);
	UINT2 wFlag = ReadBE2BytesIntoLE(fp);
	UINT4 uiRead, bytesread = 4;
	char temp[512];
	int i;

	// read through the pivot of this layer
	for(i=0; i<3; i++)
		uiRead = ReadBE4BytesIntoLE(fp);

	// load the name of this layer
	bytesread += 12;
	bytesread += ReadName(temp, fp);
	m_strLayerName = temp;

	// what about a parent layer?
	if(bytesread == chunksize)
		return;			// the optional parent index is omitted.

	UINT2 wParentIndex = ReadBE2BytesIntoLE(fp);
	bytesread += 2;

	if(bytesread != chunksize)
		int error = 1;
//		MessageBox(NULL, "chunksize and bytesread mismatches", "lwo2 loading error: in ReadLayerChunk()", MB_OK);
}


void CLWO2_Layer::ReadVertices(UINT4& chunksize, FILE* fp)
{
	UINT4 uiReads[3];
	int i, iNumVertices;
	int j;

	iNumVertices = chunksize / sizeof(float[3]);

	// iNumVertices indicates the total number of the vertices
	// included in this PNTS chunk.

	Vector3 v;

	m_vecPoint.reserve( iNumVertices );
	
	for(i=0; i<iNumVertices; i++)
	{
		for(j=0; j<3; j++)
			uiReads[j] = ReadBE4BytesIntoLE(fp);

		memcpy(&v, uiReads, sizeof(float) * 3); 

		if( fabs(v.x) < 0.00001 ) v.x = 0.0f;
		if( fabs(v.y) < 0.00001 ) v.y = 0.0f;
		if( fabs(v.z) < 0.00001 ) v.z = 0.0f;

		m_vecPoint.push_back( v );
	}
}



#define DAFAULT_MAX_UV 512


void CLWO2_Layer::ReadVertexMap(UINT4& chunksize, FILE* fp)
{
	UINT4 uiRead, dwType, bytesread;
	UINT2 wDimension;
	char acVMapName[512];
	int i = 0;
	int iVLIndexSize;	// size of variable length index (either 2 bytes or 4 bytes)
	UINT4 pnt_index;

	// Prepare for the TXUV subchunk
//	CLWO2_TextureUVMap* pUVMap = NULL;

	// Prepare for the RGBA subchunk of vertex color maps
	CLWO2_VertexColorMap new_vcmap;
	CLWO2_VertexColorMap *pNewVertexColorMap = NULL;
	vector<SVertexColor_LWO2> vecTempVertexColor;
	SVertexColor_LWO2 vertex_color;
	int iNumVertexColorIndices;

	dwType = ReadBE4BytesIntoLE(fp); //type of this VMAP chunk
	bytesread = 4;
	wDimension = ReadBE2BytesIntoLE(fp); // dimension of the mapped vertex
	bytesread += 2;
	bytesread += ReadName(acVMapName, fp);

	switch(dwType)
	{
	case ID_TXUV:
	{
		if(wDimension != 2) //if dimension is not 2.
			int error = 1;
//			MessageBox(NULL, "Cannot handle this: The dimension must be 2 for texture mapping", "Error", MB_OK|MB_ICONWARNING);

		// texture uv map set
		this->m_vecTexuvmap.push_back( CLWO2_TextureUVMap() );
		CLWO2_TextureUVMap& rUVMap = m_vecTexuvmap.back();

		rUVMap.strName = acVMapName; // set the name of this texture mapping

		//( index + u + v ) = either ( 4 + 4 + 4 )bytes or ( 2 + 4 + 4 )bytes
		SIndexAndUV iuv;
//		uvbuf.reserve(DAFAULT_MAX_UV);
		rUVMap.vecIndexUV.reserve( 1024 );
		while(bytesread < chunksize)
		{
			iuv.i = (int)ReadVLIndex(fp, &iVLIndexSize);  //index to a vertex in the most recent PNTS chunk
			uiRead = ReadBE4BytesIntoLE(fp);  //Read u (4-byte float)
			memcpy(&iuv.u, &uiRead, sizeof(float));
			uiRead = ReadBE4BytesIntoLE(fp);  //Read v (4-byte float)
			memcpy(&iuv.v, &uiRead, sizeof(float));

//			iuv.v *= ( -1 );	//----------------------------------   v values have to be reversed
			iuv.v = 1.0f - iuv.v;	//----------------------   v values have to be reversed
			// now 'iuv' holds an index, u and v

			rUVMap.vecIndexUV.push_back( iuv );

			bytesread += iVLIndexSize + 4 * 2;
		}

		// sort uv map based on the vertex index
		std::sort( rUVMap.vecIndexUV.begin(), rUVMap.vecIndexUV.end() );

		break;
	}

	case ID_RGBA:	// vertex color map with a dimension of 4
		if( wDimension != 4 )
			return;
		m_vecVertexColorMap.reserve(1024);
		this->m_vecVertexColorMap.push_back( new_vcmap );
		pNewVertexColorMap = &m_vecVertexColorMap.back();

		pNewVertexColorMap->strName = acVMapName;	// copy the name of this vertex color map

		vecTempVertexColor.clear();
		vecTempVertexColor.reserve(2048);
		while(bytesread < chunksize)
		{
			vertex_color.iIndex = (int)ReadVLIndex(fp, &iVLIndexSize);  //index to a vertex in the most recent PNTS chunk
			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fRed,   &uiRead, sizeof(float));
			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fGreen, &uiRead, sizeof(float));
			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fBlue,  &uiRead, sizeof(float));
//			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fBlue,  &uiRead, sizeof(float));
//			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fGreen, &uiRead, sizeof(float));
			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fAlpha, &uiRead, sizeof(float));

			// now, 'vertex_color' holds an index & a vertex color
			vecTempVertexColor.push_back( vertex_color );
			bytesread += iVLIndexSize + 4 * 4;
		}

		// sort by vertex index for binary search
		std::sort( vecTempVertexColor.begin(), vecTempVertexColor.end() );

		iNumVertexColorIndices = vecTempVertexColor.size();
		pNewVertexColorMap->iNumIndices = iNumVertexColorIndices;
		pNewVertexColorMap->paVertexColor = new SVertexColor_LWO2 [iNumVertexColorIndices];
		for(i=0; i<iNumVertexColorIndices; i++)
			pNewVertexColorMap->paVertexColor[i] = vecTempVertexColor[i];
		break;

	case ID_PICK:	// point selection set
	{
		if( wDimension != 0 )
		{
			assert(0);
			AdvanceFP(fp, chunksize - bytesread);
			break;
		}

		m_vecPointSelectionSet.push_back( CLWO2_PointSelectionSet() );
		CLWO2_PointSelectionSet& rPointSet = m_vecPointSelectionSet.back();
		rPointSet.SetName( acVMapName );
		while( bytesread < chunksize )
		{
			pnt_index = ReadVLIndex(fp,&iVLIndexSize);
			bytesread += iVLIndexSize;

			rPointSet.GetPointIndex().push_back( pnt_index );
		}
	}
		break;

	case ID_WGHT:	// weight maps
	{
		if( wDimension != 1 )
		{	assert(0); AdvanceFP(fp, chunksize - bytesread); break; }

		float fWeight;

		m_vecVertexWeightMap.push_back( CLWO2_WeightMap() );
		m_vecVertexWeightMap.back().m_strName = acVMapName;

		while( bytesread < chunksize )
		{
			pnt_index = ReadVLIndex(fp,&iVLIndexSize);
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy(&fWeight, &uiRead, sizeof(float));

			bytesread += iVLIndexSize + 4;

			m_vecVertexWeightMap.back().m_vecPntIndex.push_back( pnt_index );
			m_vecVertexWeightMap.back().m_vecfWeight.push_back( fWeight );

		}
	}
		break;

	default:
//		MessageBox(NULL, "unsupported VMAP chunk", "Error", MB_OK|MB_ICONWARNING);
		AdvanceFP(fp, chunksize - bytesread);
		break;
	}
}


void CLWO2_Layer::ReadVMADChunk(UINT4& chunksize, FILE* fp)
{
	UINT4 uiRead, dwType, bytesread;
	UINT2 wDimension;
	char acVMapName[256];
	int i = 0;
	int iVLIndexSize1 = 0, iVLIndexSize2 = 0; // size of variable length indices (either 2 bytes or 4 bytes)

	vector<Vector2> txuv;

	// Prepare for the RGBA subchunk of vertex color maps
	CLWO2_VertexColorMap new_vcmap;
	CLWO2_VertexColorMap *pNewVertexColorMap = NULL;
	vector<SVertexColor_LWO2> vecTempVertexColor;
	SVertexColor_LWO2 vertex_color;
	int iNumVertexColorIndices;

	dwType = ReadBE4BytesIntoLE(fp); //type of this VMAP chunk
	bytesread = 4;
	wDimension = ReadBE2BytesIntoLE(fp); // dimension of the mapped vertex
	bytesread += 2;
	bytesread += ReadName(acVMapName, fp);

	switch(dwType)
	{
	case ID_TXUV:
//		AdvanceFP(fp, chunksize - bytesread);

		if( wDimension != 2 )
			return;

		txuv.reserve( 1024 );
		while(bytesread < chunksize)
		{
			vertex_color.iIndex        = (int)ReadVLIndex(fp, &iVLIndexSize1);  //index to a vertex in the most recent PNTS chunk
			vertex_color.iPolygonIndex = (int)ReadVLIndex(fp, &iVLIndexSize2);  //index to a polygon in the most recent POLS chunk? Is this right?

			float u=0,v=0;
			uiRead = ReadBE4BytesIntoLE(fp); memcpy(&u, &uiRead, sizeof(float));
			uiRead = ReadBE4BytesIntoLE(fp); memcpy(&v, &uiRead, sizeof(float));
			txuv.push_back( Vector2(u,v) );
			bytesread += iVLIndexSize1 +iVLIndexSize2 + 4 * 2;
		}
		break;

	case ID_RGBA:	// vertex color map with a dimension of 4
		if( wDimension != 4 )
			return;

		this->m_vecVertexColorMap.push_back( new_vcmap );
		pNewVertexColorMap = &m_vecVertexColorMap.back();

		pNewVertexColorMap->strName = acVMapName;	// copy the name of this vertex color map

		vecTempVertexColor.clear();
		vecTempVertexColor.reserve(2048);
		while(bytesread < chunksize)
		{
			vertex_color.iIndex        = (int)ReadVLIndex(fp, &iVLIndexSize1);  //index to a vertex in the most recent PNTS chunk
			vertex_color.iPolygonIndex = (int)ReadVLIndex(fp, &iVLIndexSize2);  //index to a polygon in the most recent POLS chunk? Is this right?
			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fRed,   &uiRead, sizeof(float));
//			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fGreen, &uiRead, sizeof(float));
//			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fBlue,  &uiRead, sizeof(float));
			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fBlue,  &uiRead, sizeof(float));
			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fGreen, &uiRead, sizeof(float));
			uiRead = ReadBE4BytesIntoLE(fp);	memcpy(&vertex_color.fAlpha, &uiRead, sizeof(float));

			// now, 'vertex_color' holds an index & a vertex color
			vecTempVertexColor.push_back( vertex_color );
			bytesread += iVLIndexSize1 +iVLIndexSize2 + 4 * 4;
		}
		iNumVertexColorIndices = vecTempVertexColor.size();
		pNewVertexColorMap->iNumIndices = iNumVertexColorIndices;
		pNewVertexColorMap->paVertexColor = new SVertexColor_LWO2 [iNumVertexColorIndices];
		for(i=0; i<iNumVertexColorIndices; i++)
			pNewVertexColorMap->paVertexColor[i] = vecTempVertexColor[i];
		break;

	default:
//		MessageBox(NULL, "unsupported VMAP chunk", "Error", MB_OK|MB_ICONWARNING);
		AdvanceFP(fp, chunksize - bytesread);
		break;
	}
}


//#include "../Support/FixedVector.hpp"



UINT4 CLWO2_Layer::ReadPols(UINT4& chunksize, FILE* fp)
{
	UINT4 numpols = 0;
	UINT4 uiRead, dwType, bytesread = 0;
	UINT2 j, wRead, vertexcount, wIndex;

	// The tag which represents the type of the pols comes just after the chunksize.
	dwType = ReadBE4BytesIntoLE(fp);
	bytesread +=4;

//	vector<CLWO2_Bone> m_vecTempBone;
//	TCFixedVector<CLWO2_Bone, 128> m_vecTempBone;

	switch( dwType )
	{
	case ID_FACE:
		// safety measure - allocate enough memory in advance (crucial in release mode) ///
		m_vecFace.reserve( 2048 );

		while( bytesread < chunksize )
		{
			vertexcount = ReadBE2BytesIntoLE(fp); bytesread += 2;
			vertexcount &= 1023;	// Mask the high 6 bits  "1023 = 0000001111111111(2)"

			CLWO2_Face new_face;
			this->m_vecFace.push_back(new_face);
			CLWO2_Face& face = this->m_vecFace.back();

//			face.wNumPnts = vertexcount;
			face.m_vecPointIndex.resize( vertexcount );	// = new UINT4 [vertexcount];
			for(j=0; j<vertexcount; j++)
			{
				wIndex = ReadBE2BytesIntoLE(fp);
				bytesread += 2;
				if((wIndex & 0xFF00) != 0xFF00)  //if the high(first) 1 byte is not 0xFF
				{//then, the index is 2 bytes
					face.m_vecPointIndex[j] = (UINT4)wIndex;
				}
				else
				{	//the index is 4 bytes
					uiRead = 0;
					uiRead = (UINT4)wIndex;
					uiRead = uiRead << 16;
					wRead = ReadBE2BytesIntoLE(fp);  bytesread += 2; //Read the rest of 2 bytes
					uiRead |= (UINT4)wRead;
					face.m_vecPointIndex[j] = (uiRead & 0x00FFFFFF);
				}
			}
			numpols++;
		}
		break;

	case ID_BONE:
		while( bytesread < chunksize )
		{
			m_vecBone.push_back( CLWO2_Bone() );

			vertexcount = ReadBE2BytesIntoLE(fp); bytesread += 2;
			vertexcount &= 1023;	// Mask the high 6 bits  "1023 = 0000001111111111(2)"

			if(  vertexcount != 2 )
				int iUnexpected = 1;

			int size;
			for(j=0; j<vertexcount; j++)
			{
				uiRead = ReadVLIndex( fp, &size );

				m_vecBone.back().m_aiPointIndex[j] = uiRead;

				bytesread += size;
			}
		}
		break;

	default:
		AdvanceFP( fp, chunksize - bytesread );
		break;
	}

	return dwType;

}


//If the type of the PTAG is SURF, link the face to its surface
void CLWO2_Layer::ReadPTAG(UINT4& ptagsize, CLWO2_Object& rObject, FILE* fp)
{
	UINT4 i, uiType, uiRead = 0, numfaces = 0;
	UINT4 bytesread = 0;
	UINT4 face_index, bone_index;
	UINT2 index;
	UINT2 surf_index, part_index;
	int index_size;

//	UINT2 bone_indices[256];
	UINT2 num_bone_indices = 0;

	uiType = ReadBE4BytesIntoLE(fp);  //The 4 bytes after the chunksize of PTAG indicate the type of this PTAG
	bytesread +=4;
	
	switch(uiType)  // Type of the PTAG chunk
	{
	case ID_SURF:

		// TODO: support the case where the corresponding POLS chunk has not appeared at this point
		// or exists in the previous layer object
		if( m_vecFace.size() == 0 )
		{
			AdvanceFP(fp, ptagsize - bytesread);
			break;
		}

		numfaces = this->m_vecFace.size();  // how many faces in this layer
		for(i=0; bytesread < ptagsize && i<numfaces; i++)
		{
			// TODO: polygon index is a variable length index
//			face_index = ReadBE2BytesIntoLE(fp);
			face_index = ReadVLIndex( fp, &index_size );
			surf_index = ReadBE2BytesIntoLE(fp);
			this->m_vecFace[face_index].m_iSurfaceIndex = surf_index;

			bytesread += ( index_size + 2 );
		}
		break;

	case ID_BONE:
		while( bytesread < ptagsize )
		{
			bone_index = ReadVLIndex( fp, &index_size );
			index = ReadBE2BytesIntoLE(fp);

			m_vecBone[bone_index].m_strName = rObject.GetTagString( index );

			bytesread += ( index_size + 2 );

//			bone_indices[num_bone_indices++] = index;
		}
		break;

	case ID_PART:

		// TODO: support polygon groups for skeletons
/*		if( m_vecFace.size() == 0 )
		{
			AdvanceFP(fp, ptagsize - bytesread);
			break;
		}
*/
		m_vecPolygonGroup.push_back( CLWO2_PolygonGroup() );
		while( bytesread < ptagsize )
		{
			face_index = ReadVLIndex( fp, &index_size );
			part_index = ReadBE2BytesIntoLE(fp);

			// 4/22/2008 commented out
			// stored in a separate array 'm_vecPolygonIndex'
			// rationale: that chunk could come before the polygons are loaded (true?)
//			m_vecFace[face_index].m_iPartIndex = part_index;

			m_vecPolygonGroup.back().m_vecPolygonIndex.push_back( face_index );
			m_vecPolygonGroup.back().m_vecTag.push_back( part_index );

			bytesread += ( index_size + 2 );
		}
		break;

	case ID_BNWT:
		while( bytesread < ptagsize )
		{
			m_vecBoneWeightMap.push_back( CLWO2_BoneWeightMap() );
			m_vecBoneWeightMap.back().iBoneIndex = ReadVLIndex( fp, &index_size );
			m_vecBoneWeightMap.back().iWeightMapTagIndex = ReadBE2BytesIntoLE(fp);
			bytesread += ( index_size + 2 );
		}
		break;

	case ID_SMGP:	// smoothing group
	case ID_BNUP:
		AdvanceFP(fp, ptagsize - bytesread);
		break;

	default:
		// ignore any other subchunks
		AdvanceFP(fp, ptagsize - bytesread);
		break;
		
	}
}


void CLWO2_Layer::ComputeFaceNormals()
{
	LOG_FUNCTION_SCOPE();

	int i, iFace, iNumPoints;
	int iNumFaces = m_vecFace.size();
	Vector3 vEdge0, vEdge1, vNormal;

	for( iFace=0; iFace < iNumFaces; iFace++ )
	{
		CLWO2_Face& rFace = m_vecFace[iFace];

		iNumPoints = rFace.m_vecPointIndex.size();
		for( i=1; i <= iNumPoints-2; i++ )
		{
			vEdge0 = m_vecPoint[rFace.m_vecPointIndex[i-1]] - m_vecPoint[rFace.m_vecPointIndex[i]];
			vEdge1 = m_vecPoint[rFace.m_vecPointIndex[i+1]] - m_vecPoint[rFace.m_vecPointIndex[i]];

			Vec3Cross( vNormal, vEdge1, vEdge0 );
			if( Vec3LengthSq(vNormal) < 0.0000000000000000001f )
				continue;	// edges are parallel

			Vec3Normalize( vNormal, vNormal );
			rFace.m_vFaceNormal = vNormal;
			break;
		}

		if( Vec3LengthSq(vNormal) < 0.0000000000000000001f )
			int iUnexpected = 1;	// unable to create face normal
	}
}


void CLWO2_Layer::ComputeVertexNormals()
{
	LOG_FUNCTION_SCOPE();

/*	CLWO2_Face temp;

	Vector3 vNormal;

	int i, iNumVertices = m_vecPoint.size();

	m_vecVertexNormal.clear();
	m_vecVertexNormal.resize( iNumVertices );

	for( i=0; i<iNumVertices; i++ )
	{
		vNormal = GetInterpolatedNormal( temp, i );

		m_vecVertexNormal[i] = vNormal;
	}
*/
	const size_t num_faces = m_vecFace.size();
	const size_t num_vertices = m_vecPoint.size();
	size_t i, j, num_verts = 0;
	Vector3 vNormal;
	vector<Vector3> vecNormalSum;
	vector<int> vecNumShares;
	UINT4 vert_index;

	vecNormalSum.resize( num_vertices, Vector3(0,0,0) );
	vecNumShares.resize( num_vertices, 0 );

	for(i=0; i<num_faces; i++)
	{
		num_verts = m_vecFace[i].m_vecPointIndex.size();
		vNormal = m_vecFace[i].GetFaceNormal();
		for(j=0; j<num_verts; j++)
		{
			vert_index = m_vecFace[i].m_vecPointIndex[j];
			vecNormalSum[vert_index] += vNormal;
			vecNumShares[vert_index] += 1;
		}
	}

    for(j=0; j<num_verts; j++)
	{
		vecNormalSum[j] /= (float)vecNumShares[j];
	}

	m_vecVertexNormal.assign( vecNormalSum.begin(), vecNormalSum.end() );
}


// TODO: support multiple PNTS / POLS chunks
// When there are multiple POLS chunks in a layer, a point index('iPntIndex') alone
// will not be the enough information to determine which faces are sharing the point.
Vector3 CLWO2_Layer::GetInterpolatedNormal( CLWO2_Face& rFace, int iPntIndex )
{
	int i, j, num_verts;
	int iNumFaces = m_vecFace.size();
	Vector3 vNormal, vNormalSum = Vector3(0,0,0);
	int iNumShares = 0;

	for(i=0; i<iNumFaces; i++)
	{
		num_verts = m_vecFace[i].m_vecPointIndex.size();
		for(j=0; j<num_verts; j++)
		{
			if( m_vecFace[i].m_vecPointIndex[j] == iPntIndex )
			{
				vNormalSum += m_vecFace[i].GetFaceNormal();
				iNumShares++;
			}
		}
	}

	if( 2 <= iNumShares )
		int iIsSharedPoint = 1;

	Vec3Normalize( vNormal, vNormalSum );
	return vNormal;
}


/**
 * \return true if the texture uv is found for the vertex of the specified index
 * \return false if the texture uv is not found for the vertex of the specified index
 * \param [out] uv texture u & v
 */
bool CLWO2_Layer::GetUV( float& u, float& v, int iPointIndex, CLWO2_TextureUVMap *pTexUVMap )
{
	if( !pTexUVMap )
		return false;

	size_t iNumUVMaps = pTexUVMap->vecIndexUV.size();

	if( iNumUVMaps == 0 )
		return false;

	// start at the middle position
	int mid = -1, left = 0, right = iNumUVMaps;
	int prev_mid = -1;
	while(1)
	{
		prev_mid = mid;
		mid = ( left + right ) / 2;

		if( prev_mid == mid )
			return false;

		const SIndexAndUV& rIndexUV = pTexUVMap->vecIndexUV[mid];
		if( rIndexUV.i == iPointIndex )
		{
			u = rIndexUV.u;
			v = rIndexUV.v;
			return true;
		}
		else
		{
			if( iPointIndex < rIndexUV.i )
			{
				right = mid;
			}
			else
			{
				left = mid;
			}

		}
	}

	return false;
}

/*
bool CLWO2_Layer::GetUV( float& u, float& v, int iPointIndex, CLWO2_TextureUVMap *pTexUVMap )
{
	if( !pTexUVMap )
		return false;

	size_t i, iNumUVMaps = pTexUVMap->vecIndexUV.size();

	for(i=0; i<iNumUVMaps; i++)  // check the uv-mapped points in this uv map set
	{
		SIndexAndUV& rIndexUV = pTexUVMap->vecIndexUV[i];

		if( rIndexUV.i == iPointIndex )
		{
			u = rIndexUV.u;
			v = rIndexUV.v;
			return true;
		}
	}

	// uv was not found
	return false;
}
*/

bool CLWO2_Layer::GetVertexColor( SFloatRGBAColor& color,	 // [out] vertex color is stored here if a corresponding one is found
		  			              const int iPntIndex,		 // [in]  index to a point
					              const CLWO2_Surface& rSurf // [in]
					             )
{
	int i, iNumVertexColorMaps = m_vecVertexColorMap.size();
	int map_index = -1;

	for(i=0; i<iNumVertexColorMaps; i++)  // check the uv-mapped points in this uv map set
	{
		if( rSurf.GetVertexColorMap().strName == m_vecVertexColorMap[i].strName )
		{	// found the corresponding vertex color map
			map_index = i;
			break;
		}
	}

	if( map_index == -1 )
		return false;	// vertex color map was not found

	// search the texture uv for the specified vertex

#define LWO2LAYER_USE_BINARY_SEARCH
#ifdef LWO2LAYER_USE_BINARY_SEARCH

	// start at the middle position
	int iNumIndices = m_vecVertexColorMap[map_index].iNumIndices;
	int mid = -1, left = 0, right = iNumIndices;
	int prev_mid = -1;
	while(1)
	{
		prev_mid = mid;
		mid = ( left + right ) / 2;

		if( prev_mid == mid )
			return false;

		const SVertexColor_LWO2& rVertexColor = m_vecVertexColorMap[map_index].paVertexColor[mid];
		if( rVertexColor.iIndex == iPntIndex )
		{
			// found the matching index
			color.fAlpha = rVertexColor.fAlpha;
			color.fRed   = rVertexColor.fRed;
			color.fGreen = rVertexColor.fGreen;
			color.fBlue  = rVertexColor.fBlue;
			return true;
		}
		else
		{
			if( iPntIndex < rVertexColor.iIndex )
				right = mid;
			else
				left = mid;
		}
	}

	return false;

#else /* LWO2LAYER_USE_BINARY_SEARCH */

	int iNumIndices = m_vecVertexColorMap[map_index].iNumIndices;
	for( i=0; i<iNumIndices; i++ )
	{
		const SVertexColor_LWO2& rVertexColor = m_vecVertexColorMap[map_index].paVertexColor[i];
		if( rVertexColor.iIndex == iPntIndex )
		{
			// found the matching index
			color.fAlpha = rVertexColor.fAlpha;
			color.fRed   = rVertexColor.fRed;
			color.fGreen = rVertexColor.fGreen;
			color.fBlue  = rVertexColor.fBlue;

            return true;
		}
	}

	return false;

#endif /* LWO2LAYER_USE_BINARY_SEARCH */

}