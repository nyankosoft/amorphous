#ifndef __MAPCOMPILER_LIGHTMAPTEXTURE_H__
#define __MAPCOMPILER_LIGHTMAPTEXTURE_H__


#include "amorphous/Support/array2d.hpp"
#include "amorphous/Graphics/FloatRGBColor.hpp"
#include "amorphous/Graphics/LightStructs.hpp"
#include "amorphous/Graphics/IndexedPolygon.hpp"
#include "amorphous/Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "amorphous/Graphics/Rect.hpp"
#include "amorphous/Graphics/RectTree.hpp"
#include "fwd.hpp"
#include "Lightmap.hpp"


namespace amorphous
{


//==========================================================================================
// CLightmapTexture
//==========================================================================================

class CLightmapTexture
{
	/// borrowed reference to the lightmap buffer
	std::vector<CLightmap> *m_pvecLightmap;

	/// holds indices to lightmaps
	/// lightmaps are stored in 'CLightmapBuilder'
	std::vector<int> m_vecLightmapIndex;

	/// lightmap texture, the output of the CreateLightmapTexture
	array2d<SFloatRGBColor> m_vecTexel;

	/// record the state of texels (whether it has been filled or not).
	/// This information is used during margin filling
	array2d<char> m_vecTexelState;

	/// experiment with light direction map texture
	/// records average directions to the lights that reach each lightmap surface
	array2d<Vector3> m_vecvLightDirMap;

	array2d<char> m_vecTexelState_LightDirMap;

	/// holds rectangular lightmap images in the tree
	RectTree m_LightmapTree;

	/// keyname in the database
	std::string m_KeyName;

	/// image filename used to save the lightmap texture to disk
	std::string m_ImageFilepath;

public:

	CLightmapTexture() : m_pvecLightmap(NULL) {}

	CLightmapTexture( std::vector<CLightmap> *pvecLightmap ) : m_pvecLightmap(pvecLightmap) {}

	~CLightmapTexture() {}

	void Resize( int width, int height );

	enum eTexelStateFlag
	{
		LMP_TEXEL_UNFILLED		= (1 << 0),
		LMP_TEXEL_FILLED		= (1 << 1),
		LMP_TEXEL_PREV_FILLED	= (1 << 2)
	};

	SFloatRGBColor GetTexel( int x, int y ) { return m_vecTexel(x,y); }
	inline SFloatRGBColor& Texel( int x, int y ) { return m_vecTexel(x,y); }

	inline Vector3& LightDirection( int x, int y ) { return m_vecvLightDirMap(x,y); }

	bool AddLightmap( CLightmap& rLightmap, int index );

	int GetNumLightmaps() { return (int)m_vecLightmapIndex.size(); }

	const CLightmap& GetLightmap( int index ) { return (*m_pvecLightmap)[m_vecLightmapIndex[index]]; }

//	void SetLightmapTextureIndexToFaces( int index, vector<CLightmap>& rvecLightmap/*, vector<CMapFace>& rvecFace*/ );

	void SetTextureUV( std::vector<CLightmap>& rvecLightmap, int tex_coord_index );

	void UpdateTexture();

	void UpdateMaterials(
		 std::vector<CMMA_Material>& src_material_buffer,
		 std::vector<CMMA_Material>& new_material_buffer,
		 int texture_archive_index,
		 const std::string& db_filepath
	);

	void ExpandTexels( SRect& rect );

	void FillMarginRegions();

	/// apply smoothing filter over the entire lightmap texture
	void ApplySmoothing( float fCenterWeight );

	/// apply smoothing filter to the specified rectangular region
	/// in the lightmap texture
	void ApplySmoothing( SRect& rect );

	void SetKeyName( const std::string& keyname ) { m_KeyName = keyname; }

	bool AddTexturesToDatabase( CBinaryDatabase<std::string>& db );

	/// Set image filepath for texture filename field of mesh material
	void SetImageFilepath( const std::string& filepath ) { m_ImageFilepath = filepath; }

	/// save the image file with the filepath m_ImageFilepath
	bool SaveTextureImageToFile();

	/// Save image to disk for visual confirmation
	/// \param [in] filepath
	bool SaveTextureImageToFile( const std::string& filepath );
};

} // amorphous



#endif  /*  __MAPCOMPILER_LIGHTMAPTEXTURE_H__  */
