#include "LWO2_Surface.hpp"


namespace amorphous
{

using namespace std;


//================================================================================
// LWO2_Surface::Methods()                                       - LWO2_Surface
//================================================================================


LWO2_Surface::LWO2_Surface()
{
	Clear();
}


void LWO2_Surface::Clear()
{
	m_strName = "";
	m_strComment = "";
//	m_strUVMapName = "";
//	imagetag = 0;
	m_fMaxSmoothingAngle = 3.14159265f / 2.0f;	// 90 [deg] in default

	m_vecSurfaceBlock.clear();

	int i;
	for( i=0; i<NUM_BASE_SHADING_VALUES; i++ )
	{
		m_afBaseShadingValue[i] = 0.0f;
	}
}



void LWO2_Surface::ReadSurfaceBlock(UINT2 wBlockSize, FILE* fp)
{
	m_vecSurfaceBlock.push_back( LWO2_SurfaceBlock() );
	LWO2_SurfaceBlock& rBlock = m_vecSurfaceBlock.back();

	UINT4 uiSubchunk;//, uiImageNum;
	UINT2 wSize;//, wImageNum;
	UINT4 bytesread = 0;
	char temp[1024];

	UINT2 uiBlockHeaderSize, header_bytesread = 0;
	UINT4 uiBlockHeaderSubchunkID;
	UINT2 sub_size;

	int size;

	while(bytesread < wBlockSize)
	{
		uiSubchunk = ReadBE4BytesIntoLE(fp);  //read a sub-subchunk within the BLOK subchunk
		wSize = ReadBE2BytesIntoLE(fp);		  //the size of the sub-subchunk
		bytesread += 6;

		switch(uiSubchunk)
		{
		case ID_IMAP:
		case ID_PROC:
		case ID_GRAD:
		case ID_SHDR:
			// found the block header
			uiBlockHeaderSize = wSize;
			header_bytesread = ReadName( temp, fp );	// read an ordinal string
			rBlock.m_strOrdinalString = temp;

			while( header_bytesread < uiBlockHeaderSize )
			{
				uiBlockHeaderSubchunkID = ReadBE4BytesIntoLE(fp);	// read a sub-subchunk ID
				sub_size = ReadBE2BytesIntoLE(fp);					// the size of the sub-subchunk
				header_bytesread += 6;

				switch(uiBlockHeaderSubchunkID)
				{
				case ID_CHAN:
					rBlock.m_Channel = ReadBE4BytesIntoLE(fp);
					header_bytesread += 4;
					break;

				case ID_ENAB:
				case ID_OPAC:
				case ID_AXIS:
				default:
					AdvanceFP(fp, sub_size);
					header_bytesread += sub_size;
					break;
				}
			}
			bytesread += wSize;
			break;

		case ID_IMAG:
			rBlock.m_ImageTag = ReadVLIndex( fp, &size );
			bytesread += wSize;
			break;

			/*
			if(wSize == 2){
				wImageNum = ReadBE2BytesIntoLE(fp);
				this->imagetag = (UINT4)wImageNum;
			}
			else{
				uiImageNum = ReadBE4BytesIntoLE(fp);
				if((uiImageNum & 0xFF000000) != 0xFF000000)
					int error = 1;
//					MessageBox(NULL, "Unexpected description found at IMAG in SURF", "Error", MB_OK|MB_ICONWARNING);
				uiImageNum &= 0x00FFFFFF;
				this->imagetag = uiImageNum;
			}
			bytesread += wSize;*/

		case ID_VMAP:
			fread(temp, sizeof(char), wSize, fp);
			rBlock.m_strUVMapName = temp;
			bytesread += wSize;
			break;

		default:
			AdvanceFP(fp, wSize);
			bytesread += wSize;
			break;
		}

	}
}


// process "one SURF / one call"
void LWO2_Surface::ReadOneSurface(UINT4 chunksize, FILE* fp)
{
	UINT4 uiRead;
	UINT4 bytesread = 0;
	UINT2 wSize;
	int iVLIndexSize;
	char temp[512];

	// initialization
	Clear();

	bytesread += ReadName(temp, fp);  // the name of this surface
	m_strName = temp;

	bytesread += ReadName(temp, fp);	// parent name, ( not supported )

	while(bytesread < chunksize) // read one subchunk per loop
	{
		uiRead = ReadBE4BytesIntoLE(fp);  // ID of a subchunk in this surface
		wSize = ReadBE2BytesIntoLE(fp);   // the size of this subchunk
		bytesread += 6;

		switch(uiRead)
		{
		case ID_BLOK:
			ReadSurfaceBlock(wSize, fp);
			bytesread += wSize;
			break;

		case ID_VCOL:	// vertex color mapping
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &this->VMap.fIntensity, &uiRead, sizeof(float) );
			this->VMap.iEnvelope = (int)ReadVLIndex(fp, &iVLIndexSize);
			this->VMap.uiVMapType = ReadBE4BytesIntoLE(fp);
			ReadName( temp, fp );
			VMap.strName = temp;
			bytesread += wSize;
			break;

		case ID_SMAN:
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &m_fMaxSmoothingAngle, &uiRead, sizeof(float) );
			bytesread += wSize;
			break;

		case ID_CMNT:
			bytesread += ReadName( temp, fp );
			m_strComment.reserve( strlen(temp) );
			m_strComment = temp;
			break;

		case ID_COLR:
			// the base color of the surface.  TODO: save properly
			AdvanceFP(fp, wSize);
			bytesread += wSize;
			break;

		// base shading values

		case ID_DIFF:
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &m_afBaseShadingValue[SHADE_DIFFUSE], &uiRead, sizeof(float) );
			ReadVLIndex(fp, &iVLIndexSize);			// TODO: save envelope value
			bytesread += wSize;
			break;

		case ID_SPEC:
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &m_afBaseShadingValue[SHADE_SPECULAR], &uiRead, sizeof(float) );
			ReadVLIndex(fp, &iVLIndexSize);			// TODO: save envelope value
			bytesread += wSize;
			break;

		case ID_GLOS:
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &m_afBaseShadingValue[SHADE_GLOSSINESS], &uiRead, sizeof(float) );
			ReadVLIndex(fp, &iVLIndexSize);			// TODO: save envelope value
			bytesread += wSize;
			break;

		case ID_LUMI:
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &m_afBaseShadingValue[SHADE_LUMINOSITY], &uiRead, sizeof(float) );
			ReadVLIndex(fp, &iVLIndexSize);			// TODO: save envelope value
			bytesread += wSize;
			break;

		case ID_REFL:
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &m_afBaseShadingValue[SHADE_REFLECTION], &uiRead, sizeof(float) );
			ReadVLIndex(fp, &iVLIndexSize);			// TODO: save envelope value
			bytesread += wSize;
			break;

		case ID_TRAN:
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &m_afBaseShadingValue[SHADE_TRANSPARENCY], &uiRead, sizeof(float) );
			ReadVLIndex(fp, &iVLIndexSize);			// TODO: save envelope value
			bytesread += wSize;
			break;

		case ID_TRNL:
			uiRead = ReadBE4BytesIntoLE(fp);
			memcpy( &m_afBaseShadingValue[SHADE_TRANSLUCENCY], &uiRead, sizeof(float) );
			ReadVLIndex(fp, &iVLIndexSize);			// TODO: save envelope value
			bytesread += wSize;
			break;

		case ID_BUMP:
			AdvanceFP(fp, wSize);
			bytesread += wSize;
			break;

		default:
			AdvanceFP(fp, wSize);
			bytesread += wSize;
			break;
		}

	}

	return;
}


const LWO2_SurfaceBlock *LWO2_Surface::GetSurfaceBlockByChannel( UINT4 uiChannelID ) const
{
	int i, iNumBlocks = m_vecSurfaceBlock.size();
	for( i=0; i<iNumBlocks; i++ )
	{
		if( m_vecSurfaceBlock[i].m_Channel == uiChannelID )
			return &m_vecSurfaceBlock[i];
	}

	return NULL;	// a surface block with the specified channel was not found
}


} // amorphous
