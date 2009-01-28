#ifndef __LWO2_OBJECT_H__
#define __LWO2_OBJECT_H__


#include "LWO2_Surface.h"
#include "LWO2_Layer.h"


//=========================================================================================
// CLWO2_TAGChunk
//=========================================================================================

class CLWO2_TAGChunk
{
public:
	char* pTAGStrings;
	int tagchunk_size;
	int* piIndex;
	int iNumTAGs;  //how many string tags in this .lwo file

	CLWO2_TAGChunk(const CLWO2_TAGChunk& tagchunk);
	CLWO2_TAGChunk(){ memset(this, 0, sizeof(CLWO2_TAGChunk)); }
	~CLWO2_TAGChunk(){ SafeDeleteArray(pTAGStrings); SafeDeleteArray(piIndex); }

	void AllocateTAGStrings(UINT4 tagchunksize, FILE* fp);

	bool operator==(CLWO2_TAGChunk& tag){ return (this->iNumTAGs == tag.iNumTAGs && memcmp(this->pTAGStrings, tag.pTAGStrings, tagchunk_size) == 0); }
	bool operator<(CLWO2_TAGChunk& tag){return (this->iNumTAGs < tag.iNumTAGs); }

};



//=========================================================================================
// CLWO2_StillClip
//=========================================================================================

class CLWO2_StillClip
{
public:
	UINT4 uiClipIndex;
	std::string strName;

	CLWO2_StillClip() : uiClipIndex(0) {}

//	bool operator==(CLWO2_StillClip& clip){ return (this->uiClipIndex == clip.uiClipIndex); }
//	bool operator<(CLWO2_StillClip& clip){return (this->uiClipIndex < clip.uiClipIndex); }

	void Read(UINT4 clipsize, FILE* fp);

	UINT4 GetClipIndex() { return uiClipIndex; }
//	const char *GetName() { return strName.c_str(); }
	string& GetName() { return strName; }
};


class CLWO2_NameMatchCond
{
public:
	enum type
	{
		EQUAL,		///< keyword == input
		START_WITH,	///< keyword is a sub-string of input and it occurs at the first part of input
		SUBSTRING,	///< keyword is a sub-string of input
		NUM_MATCH_CONDITIONS
	};

	static bool meet_cond( int cond, const string& keyword, const string& input_str )
	{
		switch( cond )
		{
		case EQUAL:
			return (input_str == keyword );
		case START_WITH:
			return input_str.find( keyword ) == 0;
		case SUBSTRING:
			return input_str.find( keyword ) != string::npos;
		default:
			return false;
		}
	}
};


/**
 *   holds the content of light wave object file with LWO2 format
 */
class CLWO2_Object
{
	std::string m_strFilename;

	std::list<CLWO2_Layer> m_lstLayer;

	std::vector<CLWO2_Surface> m_vecSurface;

	std::vector<CLWO2_StillClip> m_vecStillClip;

	CLWO2_TAGChunk m_tag;

public:

	CLWO2_Object(){}
//	CLWO2_Object(const CLWO2_Object& lwo2data);
	~CLWO2_Object() {}

	bool LoadLWO2Object( const std::string& object_filename );

	std::string& GetFilename() { return m_strFilename; }

	list<CLWO2_Layer>& GetLayer() { return m_lstLayer; }

	vector<CLWO2_Surface>& GetSurface() { return m_vecSurface; }

	CLWO2_Surface& FindSurfaceFromTAG(UINT2 wSurfIndex);

	/// when a surface tag stored in polygon is handed,
	/// returns the index of the surface in the array
	int GetSurfaceIndexFromSurfaceTAG( const UINT2 wSurfTagIndex );

	/// find a uv-mapping associated with the surface
//	CLWO2_TextureUVMap* FindTextureUVMapFromSurface(CLWO2_Surface& rSurf, CLWO2_Layer& rLayer);

	CLWO2_TextureUVMap* FindTextureUVMapFromSurface( CLWO2_Surface& rSurf,
		                                             const UINT4 channel_id,
													 CLWO2_Layer& rLayer);

	CLWO2_TextureUVMap* FindTextureUVMapByName(const char *pcTexUVMapName, CLWO2_Layer& rLayer);

	CLWO2_VertexColorMap* FindVertexColorMapFromSurface(CLWO2_Surface& rSurf, CLWO2_Layer& rLayer);

	CLWO2_StillClip& GetStillClip(int i) { return m_vecStillClip[i]; }

	vector<CLWO2_StillClip>& GetStillClip() { return m_vecStillClip; }

	void ComputeFaceNormals();

	void ComputeVertexNormals();

	CLWO2_Layer *GetLayerWithKeyword( const std::string& keyword,
		                              int match_condition = CLWO2_NameMatchCond::EQUAL );

	std::vector<CLWO2_Layer *> GetLayersWithKeyword( const std::string& keyword,
		                                             int match_condition = CLWO2_NameMatchCond::EQUAL );

	std::vector<CLWO2_Layer *> GetLayersWithKeywords( const std::vector<std::string> keyword,
		                                              int match_condition = CLWO2_NameMatchCond::EQUAL );

	void WriteDebug( const std::string& filename );

	int GetNumTagStrings();

	char *GetTagString( int i );

	int GetBoneIndexForWeightMap( CLWO2_WeightMap& rWeightMap, CLWO2_Layer& rLayer );
};



#endif  /*  __LWO2_OBJECT_H__  */
