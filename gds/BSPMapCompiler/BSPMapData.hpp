#ifndef _BSPMAPDATA_H__
#define _BSPMAPDATA_H__

#include "fwd.hpp"

#include "Stage/bspstructs.hpp"
#include "Stage/BSPStaticGeometry.hpp"

#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
using namespace MeshModel;


class CBaseLight;
struct SFog;
class CIndexedTriangle;
class CTriangleMesh;


struct SPointLightDesc
{
	char acName[128];
	float fRed, fGreen, fBlue;
	float fIntensity;
	float fAttenuation[3];

	SPointLightDesc()
	{
		memset( this, 0, sizeof(SPointLightDesc) );
		fRed = fGreen = fBlue = 1.0f;
		fAttenuation[0] = 0.01f;
		fAttenuation[1] = 0.01f;
		fAttenuation[2] = 0.01f;
	}
};


class CBSPMapData
{
private:
	void SearchAdjacentLightSourceFaces_r( vector<CMapFace>* pvecAdjacentFace, CMapFace& rFace1, vector<CMapFace>* pvecSourceFace);

	SPointLightDesc FindPointLightDesc(char* pcLightName);

	/// set face type flags MAIN / INTERIOR
	/// mark skybox polygons as non-collidables
	void SetFaceProperties();

	void SetLightsFromLightSourceFaces();	// convert light source faces into point lights and store them in 'm_vecpLight'


	// used for triangle mesh construction
	void GetTriangleInfoFromFaceSet( vector<D3DXVECTOR3>& rvecVertex,
		                             vector<int>& rveciTriangleIndex,
									 vector<int>& rveciMaterialIndex,
								     vector<CMapFace>& rvecSrcFace );

	int GetMeshVertexIndex( D3DXVECTOR3& v, vector<D3DXVECTOR3>& rvecVertex );

	/// implement according to the file format of the modeler
	virtual int LoadSpecificMapDataFromFile( const char* pFilename ) = 0;

protected:

	vector<CMapFace> m_aMainFace;

	vector<CMapFace> m_aInteriorFace;

	vector<CMapFace> m_aSkyboxFace;

	CPlaneBuffer m_aPlane;

	vector<CBaseLight*> m_vecpLight;	// array of pointers that point to various kinds of light objects 

	vector<CSG_Surface> m_vecSurface;

	vector<CSGA_TextureFile> m_vecTextureFile;
//	vector<STextureFile> m_aTexFile;

	SFog* m_pFog;

	vector<SPointLightDesc> m_vecLightDesc;

	/// for occlusion calc in lightmap
	vector<C3DMeshModelArchive> m_vecMesh;

	C3DMeshModelArchive m_EnvLightMesh;

	/// load the material list file and holds only the name of the surface materials.
	/// map data needs only the name - index relations of surface materials
	/// and not interested in physics or sound properties
	class CSurfaceMaterialData
	{
	public:
		string strName;

		CSurfaceMaterialData() {}
	};

	vector<CSurfaceMaterialData> m_vecMaterial;


public:

	CBSPMapData() { m_pFog = NULL; }

	~CBSPMapData();

	/// loads map data
	/// returns non-zero value if succeeded
	/// returns zero if failed
	int LoadMapDataFromFile( const std::string& filename );

	void SetPlane();  // put the plane of each face in the plane buffer, and set the index to the plane in each face 

	virtual void SetTextureFilename() = 0;

	bool LoadLightPresetsFromFile( const char* pcFilename );

	int GetNumLights();

	CBaseLight* GetLight(int i);	// returns access to the i-th light

	bool LoadMaterialNames( const string& strMaterialListFile );

//	void SetLocalLightDirections();

	void CalculateTangentSpace();

	/// set transformation from world space to each face space
	/// transformations are represented by quaternions and stored into diffuse color of each vertex 
	void SetLocalTransformsToVertices();

	/// return info which represents a single triangle mesh of this map
	void GetTriangleMeshForCollision( vector<D3DXVECTOR3>& rvecvTriangleVertex,
		                              vector<int>& rveciTriangleIndex,
								      vector<int>& rveciMaterialIndex);

	void WriteFacesToFile( const std::string& filename );

	//virtual int WriteMapDataToFile() = 0;

	friend class CBSPMapCompiler;
};



#endif  /*  _BSPMAPDATA_H__  */
