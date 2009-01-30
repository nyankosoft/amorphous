
#include "LWO2_Object.hpp"


//================================================================================
// CLWO2_TAGChunk::Methods()                                     - CLWO2_TAGChunk
//================================================================================

CLWO2_TAGChunk::CLWO2_TAGChunk(const CLWO2_TAGChunk& tagchunk)
{
	this->tagchunk_size = tagchunk.tagchunk_size;
	this->pTAGStrings = new char [tagchunk_size];
	memcpy(this->pTAGStrings, tagchunk.pTAGStrings, tagchunk_size);
	this->iNumTAGs = tagchunk.iNumTAGs;
	this->piIndex = new int [iNumTAGs];
	memcpy(this->piIndex, tagchunk.piIndex, iNumTAGs * sizeof(int));
}


#include "Support/FixedVector.hpp"


void CLWO2_TAGChunk::AllocateTAGStrings(UINT4 tagchunksize, FILE* fp)
{
	char *p;
	int temp_indices[1024];

	this->tagchunk_size = tagchunksize;

	TCFixedVector<string, 64> m_vecstrTag;

	// Copy the content of the TAGS chunk
	pTAGStrings = new char [tagchunksize];
	fread(pTAGStrings, sizeof(char), tagchunksize, fp);
	p = pTAGStrings;
	int i = 0;
	UINT4 n = 0;

	// Set Indices to each tag
	while(n < tagchunksize)
	{

		m_vecstrTag.push_back( string() );
		m_vecstrTag.back() = p + n;


		temp_indices[i] = n;
		n += strlen(p+n);		//pTAGStrings[n] indicates the terminating NULL character
		if(n % 2 == 0)  //We got the double NULL at the end of this tag
			n++;
		n++;
		i++;

	}
	this->iNumTAGs = i;
	piIndex = new int [i];
	memcpy(piIndex, temp_indices, sizeof(int) * i);
	return;
}


//================================================================================
// CLWO2_StillClip::Methods()                                   - CLWO2_StillClip
//================================================================================

// only supports STIL subchunk
void CLWO2_StillClip::Read(UINT4 clipsize, FILE* fp)
{
	UINT4 uiType, uiClipID;
	UINT4 bytesread, bytesleft;
	UINT2 wRead;
	char temp[512];
/*
	uiClipID = ReadBE4BytesIntoLE(fp);  //The index that identifies this clip uniquely
	uiType = ReadBE4BytesIntoLE(fp);  //type of this CLIP chunk
	this->uiClipIndex = uiClipID;
	bytesread = 8;
	bytesleft = clipsize - bytesread;

	switch(uiType)
	{
	case ID_STIL:
		wRead = ReadBE2BytesIntoLE(fp);
		bytesread +=2;
		fread(temp ,sizeof(char), wRead, fp);
		strName = temp;
		return;

	default:
		AdvanceFP(fp, bytesleft);
		break;
	}
*/
	uiClipID = ReadBE4BytesIntoLE(fp);  // The index that identifies this clip uniquely
	this->uiClipIndex = uiClipID;
	bytesread = 4;
	bytesleft = clipsize - 4;
	while( 0 < bytesleft )
	{
		uiType = ReadBE4BytesIntoLE(fp);  // a CLIP sub-chunk
		bytesleft -= 4;

		switch(uiType)
		{
		case ID_STIL:
			wRead = ReadBE2BytesIntoLE(fp);
			bytesread +=2;
			fread(temp ,sizeof(char), wRead, fp);
			strName = temp;
			bytesleft -= (2+wRead);
			break;

		case ID_FLAG:
			AdvanceFP(fp, 6);
			bytesleft -= 6;
			break;

		default:
			AdvanceFP(fp, bytesleft);
			break;
		}
	}
}



//================================================================================
// CLWO2_Object::Methods()                                         - CLWO2_Object
//================================================================================

bool CLWO2_Object::LoadLWO2Object( const std::string& object_filename )
{
	UINT4 chunksize, bytesread = 0, datasize = 0;
	UINT4 uiRead;
	UINT4 uiTypeLastPOLS;
	UINT4 uiPrevType;

	CLWO2_Surface surf;
	CLWO2_StillClip clip;
	CLWO2_Layer new_layer;

	Vector3 vMin, vMax;

	FILE* fp = fopen( object_filename.c_str(), "rb" );  //binary-reading mode
	if(fp==NULL)
	{
///		MessageBox(NULL, "The specified LWO file was not found.", "Error", MB_OK|MB_ICONWARNING);
		return false;
	}

	// The first 4 bytes of the "*.lwo" file
	uiRead = ReadBE4BytesIntoLE(fp);
	if(uiRead != ID_FORM)
	{
///		MessageBox(NULL, "Not an IFF file (Missing FORM tag)", "Error", MB_OK|MB_ICONWARNING);
		return false;
	}

	// Next 4 bytes indicate the datasize of the LWO file.
	// Here, the datasize means the size of the file excepting the first 8 bytes, FORM and datasize itself
	uiRead = ReadBE4BytesIntoLE(fp);
	datasize = uiRead;

	// Next 4 bytes, from 8 to 11 indicates the format.
	// This 4 bytes have to be LWO2
	uiRead = ReadBE4BytesIntoLE(fp);
	bytesread += 4;
	if(uiRead != ID_LWO2)
	{
///		MessageBox(NULL, "Not LWO2 format (Missing LWO2 tag)", "Error", MB_OK|MB_ICONWARNING);
		return false;
	}

	// save the original filename
	m_strFilename = object_filename;
	
	while(bytesread < datasize)
	{
		// read a primary chunk ID
		uiRead = ReadBE4BytesIntoLE(fp);

		// next, read the size of this chunk
		chunksize = ReadBE4BytesIntoLE(fp);

		// these 2 tags accounts for 4 * 2 = 8 bytes
		bytesread += 8;

		switch(uiRead)
		{
		case ID_TAGS:
			this->m_tag.AllocateTAGStrings(chunksize, fp);
			break;

		case ID_LAYR:
			this->m_lstLayer.push_back(new_layer);
			this->m_lstLayer.back().ReadLayerChunk(chunksize, fp);
			break;

		case ID_PNTS:
			m_lstLayer.back().ReadVertices(chunksize, fp);
			break;

		case ID_POLS:
			uiTypeLastPOLS = m_lstLayer.back().ReadPols(chunksize, fp);
			break;

		case ID_PTAG:
			m_lstLayer.back().ReadPTAG(chunksize, *this, fp);	// set surface indices to faces
			break;

		case ID_VMAP:  //There are as many VMAP chunks as the number of surfaces in the LWO file
			//Store the indices to vertices & the indices to uvs in UVMap[]
			//and store the actual uv data in pTextureUV[].
			m_lstLayer.back().ReadVertexMap(chunksize, fp);
			break;

		case ID_VMAD:
//			m_lstLayer.back().ReadVMAD(chunksize, PolUVMap[numduvmaps++], pFaces, pTextureUV, numTextureUVs, fp);
			m_lstLayer.back().ReadVMADChunk(chunksize, fp);
			break;

		case ID_SURF:  //There are as many SURF chunks as the number of the surfaces in the LWO file
//			surf.ReadOneSurface(chunksize, fp);
//			this->m_vecSurface.push_back(surf);
			m_vecSurface.push_back( CLWO2_Surface() );
			m_vecSurface.back().ReadOneSurface( chunksize, fp );
			break;

		case ID_CLIP:
			clip.Read(chunksize, fp);
			this->m_vecStillClip.push_back(clip);
			break;

		case ID_BBOX:
			fread( &vMin, sizeof(Vector3), 1, fp );
			fread( &vMax, sizeof(Vector3), 1, fp );
//			AdvanceFP(fp, chunksize);
			break;

		default:
			AdvanceFP(fp, chunksize);
			break;
		}
		bytesread += chunksize;

		uiPrevType = uiRead;
	}

	fclose(fp);


	// compute normals on each face(polygon) in the LightWave object 
	ComputeFaceNormals();
	ComputeVertexNormals();

	return true;

}


void CLWO2_Object::WriteDebug( const std::string& filename )
{
	FILE* fp;
	fp = fopen(filename.c_str(), "w");
	char acStr[32];
	char acStr2[256];

	size_t i, num_tags = m_tag.iNumTAGs;
	for(i=0; i<num_tags; i++)
	{
		fputs(m_tag.pTAGStrings + m_tag.piIndex[i], fp);
		fputs("\n", fp);
	}

	_itoa(m_lstLayer.size(), acStr, 10);
	sprintf(acStr2 ,"\n----- %s Layers -----\n", acStr);
	fputs(acStr2, fp);
	list<CLWO2_Layer>::iterator p;
	for(p = m_lstLayer.begin(); p!=m_lstLayer.end(); p++)
	{
		fputs("[", fp);
		_itoa(p->GetLayerIndex(), acStr, 10);
		fputs(acStr, fp);
		fputs("] ", fp);
		fputs(p->GetName().c_str(), fp);
		fputs("\n", fp);

		_itoa(p->GetVertex().size(), acStr, 10);
		fputs(acStr, fp);
		fputs(" points\n", fp);
		_itoa(p->GetFace().size(), acStr, 10);
		fputs(acStr, fp);
		fputs(" faces\n", fp);
		
	}


	_itoa(m_vecSurface.size(), acStr, 10);
	sprintf(acStr2, "\n----- %s Surfaces -----\n", acStr);
	fputs(acStr2, fp);

	size_t iNumSurfs = m_vecSurface.size();
	for( i=0; i<iNumSurfs; i++ )
	{
		CLWO2_Surface& rSurf = m_vecSurface[i];

		fputs("name: ", fp);
		fputs(rSurf.GetName().c_str(), fp);
		fputs("\n", fp);
		fputs("texture uv: ", fp);
//		fputs(rSurf.GetUVMapName().c_str(), fp);
		fputs("\n", fp);

	}

	fputs("\nCLIP - STIL\n", fp);

	size_t iNumClips = m_vecStillClip.size();
	for( i=0; i<iNumClips; i++ )
	{
		CLWO2_StillClip& clip = m_vecStillClip[i];
		fputs(" [", fp);
		_itoa(clip.uiClipIndex, acStr, 10);
		fputs(acStr, fp);
		fputs("] ", fp);
		fputs(clip.strName.c_str(), fp);
		fputs("\n", fp);
	}
	fclose(fp);

}

/*
CLWO2_Object::CLWO2_Object(const CLWO2_Object& lwo2data){
	this->m_lstLayer.assign(lwo2data.m_lstLayer.begin(), lwo2data.m_lstLayer.end());
	this->m_vecSurface.assign(lwo2data.m_vecSurface.begin(), lwo2data.m_vecSurface.end());
	this->m_vecStillClip.assign(lwo2data.m_vecStillClip.begin(), lwo2data.m_vecStillClip.end());
}

CLWO2_Object::~CLWO2_Object(){
	m_lstLayer.clear();
	m_vecSurface.clear();
	m_vecStillClip.clear();
}*/


CLWO2_Surface& CLWO2_Object::FindSurfaceFromTAG( UINT2 wSurfIndex )
{
	char* pcSurfaceName = m_tag.pTAGStrings + m_tag.piIndex[wSurfIndex];

	size_t i, iNumSurfs = m_vecSurface.size();
	for(i=0; i<iNumSurfs; i++)
	{
		CLWO2_Surface& rSurf = m_vecSurface.at(i);
		if( strcmp(pcSurfaceName, rSurf.GetName().c_str()) == 0 )
			return rSurf;
	}

//	MessageBox(NULL, "The requested surface was not found in the .lwo file", "Error", MB_OK|MB_ICONWARNING);

	return m_vecSurface.at(0);
}


int CLWO2_Object::GetSurfaceIndexFromSurfaceTAG( const UINT2 wSurfTagIndex )
{
	char* pcSurfaceName = m_tag.pTAGStrings + m_tag.piIndex[wSurfTagIndex];

	size_t i, num_surfs = m_vecSurface.size();
	for(i=0; i<num_surfs; i++)
	{
		CLWO2_Surface& rSurf = m_vecSurface[i];
		if( strcmp(pcSurfaceName, rSurf.GetName().c_str()) == 0 )
			return (int)i;
	}

	return -1;	// surface nost found
}


void CLWO2_Object::ComputeFaceNormals()
{
	list<CLWO2_Layer>::iterator itrLayer;

	for( itrLayer = m_lstLayer.begin(); itrLayer != m_lstLayer.end(); itrLayer++ )
	{
		itrLayer->ComputeFaceNormals();
	}
}


void CLWO2_Object::ComputeVertexNormals()
{
	list<CLWO2_Layer>::iterator itrLayer;

	for( itrLayer = m_lstLayer.begin(); itrLayer != m_lstLayer.end(); itrLayer++ )
	{
		itrLayer->ComputeVertexNormals();
	}
}


//find the uv-mapping used by the surface
CLWO2_TextureUVMap* CLWO2_Object::FindTextureUVMapByName(const char *pcTexUVMapName, CLWO2_Layer& rLayer)
{
	vector<CLWO2_TextureUVMap>& rvecTexUVMap = rLayer.GetTextureUVMap();

	size_t i, num = rvecTexUVMap.size();
	for(i=0; i<num ; i++)
	{
		CLWO2_TextureUVMap& rTexUVMap = rvecTexUVMap.at(i);
		if(strcmp(rTexUVMap.strName.c_str(), pcTexUVMapName) == 0)
			return &rTexUVMap;
	}
	//MessageBox(NULL, "The requested uv-mapping for texture was not found in this layer", "Error", MB_OK|MB_ICONWARNING);
	return NULL;
}

/*
//find the uv-mapping used by the surface
CLWO2_TextureUVMap* CLWO2_Object::FindTextureUVMapFromSurface(CLWO2_Surface& rSurf, CLWO2_Layer& rLayer)
{
	return FindTextureUVMapByName( rSurf.GetUVMapName().c_str(), rLayer);
}
*/

CLWO2_TextureUVMap* CLWO2_Object::FindTextureUVMapFromSurface( CLWO2_Surface& rSurf,
		                                                       const UINT4 channel_id,
													            CLWO2_Layer& rLayer)
{
	vector<CLWO2_SurfaceBlock>& rvecSurfBlock = rSurf.GetSurfaceBlock();
	int i, iNumSurfaceBlocks = rvecSurfBlock.size();
	int index;
	for( i=0; i<iNumSurfaceBlocks; i++ )
	{
		CLWO2_SurfaceBlock& rSurfBlock = rvecSurfBlock[i];

		if( rSurfBlock.m_Channel == channel_id )
		{
			index = i;
			break;
		}
	}

	if( i == iNumSurfaceBlocks )
		return NULL;

	return FindTextureUVMapByName( rvecSurfBlock[index].m_strUVMapName.c_str(), rLayer);

}


/*
//find the uv-mapping used by the surface
CLWO2_TextureUVMap* CLWO2_Object::FindTextureUVMapFromSurface(CLWO2_Surface& rSurf, CLWO2_Layer& rLayer)
{
	int i;
	for(i=0; i< rLayer.m_texuvmap.size(); i++)
	{
		CLWO2_TextureUVMap& rTexUVMap = rLayer.m_texuvmap.at(i);
		if(strcmp(rTexUVMap.pName, rSurf.pUVMapName) == 0)
			return &rTexUVMap;
	}
	//MessageBox(NULL, "The requested uv-mapping for texture was not found in this layer", "Error", MB_OK|MB_ICONWARNING);
	return NULL;
}*/


//find the uv-mapping used by the surface
CLWO2_VertexColorMap* CLWO2_Object::FindVertexColorMapFromSurface(CLWO2_Surface& rSurf, CLWO2_Layer& rLayer)
{
	size_t i, num = rLayer.GetVertexColorMap().size();
	for(i=0; i<num ; i++)
	{
		CLWO2_VertexColorMap& rVCMap = rLayer.GetVertexColorMap()[i];

		if( rVCMap.strName == rSurf.GetVertexColorMap().strName )
			return &rVCMap;
	}
	//MessageBox(NULL, "The requested vertex color mapping was not found in this layer", "Error", MB_OK|MB_ICONWARNING);
	return NULL;
}


int CLWO2_Object::GetNumTagStrings()
{
	return m_tag.iNumTAGs;
}


char *CLWO2_Object::GetTagString( int i )
{
	if( m_tag.iNumTAGs <= i )
		return NULL;

	return m_tag.pTAGStrings + m_tag.piIndex[i];
}


int CLWO2_Object::GetBoneIndexForWeightMap( CLWO2_WeightMap& rWeightMap, CLWO2_Layer& rLayer )
{
	const string& strWeightMapName = rWeightMap.GetName();

	int i, num_tags = GetNumTagStrings();
	int tag_index;
	for( i=0; i<num_tags; i++ )
	{
		if( strWeightMapName == GetTagString(i) )
		{
			tag_index = i;
			break;
		}
	}

	if( i == num_tags )
		return -1;	// the corresponding tag string was not found

	list<CLWO2_Layer>& rlstLayer = m_lstLayer;
	list<CLWO2_Layer>::iterator itrLayer;

	// search all the layers to find the bone - weight maps because the skeleton may exist in
	// a layer different from 'rLayer'
	for(itrLayer = rlstLayer.begin();
		itrLayer != rlstLayer.end();
		itrLayer++)
	{
		vector<CLWO2_BoneWeightMap>& rBoneWeightMap = (*itrLayer).GetBoneWeightMap();
		for( i=0; i<rBoneWeightMap.size(); i++ )
		{
			if( rBoneWeightMap[i].iWeightMapTagIndex == tag_index )
				return rBoneWeightMap[i].iBoneIndex;
		}
	}
/*
	vector<CLWO2_BoneWeightMap>& rBoneWeightMap = rLayer.GetBoneWeightMap();
	for( i=0; i<rBoneWeightMap.size(); i++ )
	{
		if( rBoneWeightMap[i].iWeightMapTagIndex == tag_index )
			return rBoneWeightMap[i].iBoneIndex;
	}
*/
	return -1;	// no corresponding bone index was found
}


CLWO2_Layer *CLWO2_Object::GetLayerWithKeyword( const string& keyword, int match_condition )
{
	list<CLWO2_Layer>::iterator itrLayer;

	for(itrLayer = m_lstLayer.begin();
		itrLayer != m_lstLayer.end();
		itrLayer++)
	{
		if( CLWO2_NameMatchCond::meet_cond( match_condition, keyword, itrLayer->GetName() ) )
		{
			// the layer name matches the target layer name
			return &(*itrLayer);
		}
	}

	return NULL;
}


vector<CLWO2_Layer *> CLWO2_Object::GetLayersWithKeyword( const string& keyword, int match_condition)
{
	vector<CLWO2_Layer *> vecpTargetLayer;

	list<CLWO2_Layer>::iterator itrLayer;
	for(itrLayer = m_lstLayer.begin();
		itrLayer != m_lstLayer.end();
		itrLayer++)
	{
		if( CLWO2_NameMatchCond::meet_cond( match_condition, keyword, itrLayer->GetName() ) )
		{
			// the layer name matches the target layer name
			vecpTargetLayer.push_back( &(*itrLayer) );
		}
	}

	return vecpTargetLayer;
}


vector<CLWO2_Layer *> CLWO2_Object::GetLayersWithKeywords( const vector<string> keyword,
													       int match_condition)
{
	vector<CLWO2_Layer *> vecpTargetLayer;

	size_t i, num_keywords = keyword.size();
	list<CLWO2_Layer>::iterator itrLayer;

	// check each layer to see if it is a target
	for(itrLayer = m_lstLayer.begin();
		itrLayer != m_lstLayer.end();
		itrLayer++)
	{
		for( i=0; i<num_keywords; i++ )
		{
			if( CLWO2_NameMatchCond::meet_cond( match_condition, keyword[i], itrLayer->GetName() ) )
			{
				// the layer name matches one of the target layer names
				// add the layer to the buffer and leave the loop
				vecpTargetLayer.push_back( &(*itrLayer) );
				break;
			}
		}
	}

	return vecpTargetLayer;
}

