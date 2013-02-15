#ifndef __LWO2_OBJECT_H__
#define __LWO2_OBJECT_H__


#include "LWO2_Surface.hpp"
#include "LWO2_Layer.hpp"
#include "gds/Support/progress_display.hpp"


namespace amorphous
{


//=========================================================================================
// LWO2_TAGChunk
//=========================================================================================

class LWO2_TAGChunk
{
public:
	char* pTAGStrings;
	int tagchunk_size;
	int* piIndex;
	int iNumTAGs;  //how many string tags in this .lwo file

	LWO2_TAGChunk(const LWO2_TAGChunk& tagchunk);
	LWO2_TAGChunk(){ memset(this, 0, sizeof(LWO2_TAGChunk)); }
	~LWO2_TAGChunk(){ SafeDeleteArray(pTAGStrings); SafeDeleteArray(piIndex); }

	void AllocateTAGStrings(UINT4 tagchunksize, FILE* fp);

	bool operator==(LWO2_TAGChunk& tag){ return (this->iNumTAGs == tag.iNumTAGs && memcmp(this->pTAGStrings, tag.pTAGStrings, tagchunk_size) == 0); }
	bool operator<(LWO2_TAGChunk& tag){return (this->iNumTAGs < tag.iNumTAGs); }

};



//=========================================================================================
// LWO2_StillClip
//=========================================================================================

class LWO2_StillClip
{
public:
	UINT4 uiClipIndex;
	std::string strName;
public:
	LWO2_StillClip() : uiClipIndex(0) {}
	void Read(UINT4 clipsize, FILE* fp);
	UINT4 GetClipIndex() const { return uiClipIndex; }
	const std::string& GetName() const { return strName; }
};


class LWO2_NameMatchCond
{
public:
	enum type
	{
		EQUAL,		///< keyword == input
		START_WITH,	///< keyword is a sub-string of input and it occurs at the first part of input
		SUBSTRING,	///< keyword is a sub-string of input
		NUM_MATCH_CONDITIONS
	};

	static bool meet_cond( int cond, const std::string& keyword, const std::string& input_str )
	{
		switch( cond )
		{
		case EQUAL:
			return (input_str == keyword );
		case START_WITH:
			return input_str.find( keyword ) == 0;
		case SUBSTRING:
			return input_str.find( keyword ) != std::string::npos;
		default:
			return false;
		}
	}
};


/**
 *   holds the content of light wave object file with LWO2 format
 */
class LWO2_Object
{
	std::string m_strFilename;

	std::list<LWO2_Layer> m_lstLayer;

	std::vector<LWO2_Surface> m_vecSurface;

	std::vector<LWO2_StillClip> m_vecStillClip;

	LWO2_TAGChunk m_tag;

	// should you boost::shared_ptr<>?
	progress_display m_ProgressDisplay;

public:

	LWO2_Object(){}
//	LWO2_Object(const LWO2_Object& lwo2data);
	~LWO2_Object() {}

	bool LoadLWO2Object( const std::string& object_filename );

	const std::string& GetFilename() const { return m_strFilename; }

	std::list<LWO2_Layer>& GetLayer() { return m_lstLayer; }

	std::vector<LWO2_Surface>& GetSurface() { return m_vecSurface; }

	const std::vector<LWO2_Surface>& GetSurface() const { return m_vecSurface; }

	LWO2_Surface *FindSurfaceFromTAG(UINT2 wSurfIndex);

	/// when a surface tag stored in polygon is handed,
	/// returns the index of the surface in the array
	int GetSurfaceIndexFromSurfaceTAG( const UINT2 wSurfTagIndex );

	/// find a uv-mapping associated with the surface
//	LWO2_TextureUVMap* FindTextureUVMapFromSurface(LWO2_Surface& rSurf, LWO2_Layer& rLayer);

	LWO2_TextureUVMap* FindTextureUVMapFromSurface( const LWO2_Surface& rSurf,
		                                             const UINT4 channel_id,
													 LWO2_Layer& rLayer);

	LWO2_TextureUVMap* FindTextureUVMapByName(const char *pcTexUVMapName, LWO2_Layer& rLayer);

	LWO2_VertexColorMap* FindVertexColorMapFromSurface(LWO2_Surface& rSurf, LWO2_Layer& rLayer);

	LWO2_StillClip& GetStillClip(int i) { return m_vecStillClip[i]; }

	std::vector<LWO2_StillClip>& GetStillClip() { return m_vecStillClip; }

	void ComputeFaceNormals();

	void ComputeVertexNormals();

	LWO2_Layer *GetLayerWithKeyword( const std::string& keyword,
		                              int match_condition = LWO2_NameMatchCond::EQUAL );

	std::vector<LWO2_Layer *> GetLayersWithKeyword( const std::string& keyword,
		                                             int match_condition = LWO2_NameMatchCond::EQUAL );

	std::vector<LWO2_Layer *> GetLayersWithKeywords( const std::vector<std::string> keyword,
		                                              int match_condition = LWO2_NameMatchCond::EQUAL );

	void WriteDebug( const std::string& filename ) const;

	int GetNumTagStrings() const;

	const char *GetTagString( int i ) const;

	int GetBoneIndexForWeightMap( LWO2_WeightMap& rWeightMap, LWO2_Layer& rLayer );

	const progress_display& GetProgressDisplay() const { return m_ProgressDisplay; }
};


} // amorphous



#endif  /*  __LWO2_OBJECT_H__  */
