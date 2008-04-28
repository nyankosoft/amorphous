#ifndef  __BUMPMAPTEXTUREMAKER2_LWO2_H__
#define  __BUMPMAPTEXTUREMAKER2_LWO2_H__


#include "3DMath/Vector3.h"
#include "3DMath/Matrix33.h"
#include "3DCommon/TextureHandle.h"

#include <string>
using namespace std;


class CD3DXMeshObject;
class CD3DXSMeshObject;
class CShaderManager;

enum eBumpmapTextureMaker_RenderMode
{
	BTM_RENDERMODE_NORMALMAP = 0,
	BTM_RENDERMODE_FLAT_TEXTURED_SURFACE,
	BTM_RENDERMODE_BUMPY_TEXTURED_SURFACE,
	BTM_RENDERMODE_PREVIEW, /// not implemented yet
	BTM_NUM_RENDERMODES
};


class CBumpmapTextureMaker2_LWO2
{


	CD3DXMeshObject *m_p3DModel;

	CShaderManager *m_pShaderManager;

	LPDIRECT3DTEXTURE9 m_pDefaultTexture;

	CTextureHandle m_PreviewTexture;

	int m_TechniqueID;
	
	float m_fViewVolumeWidth;

	string m_strBaseFilename;

	class CDirectionalLight
	{
	public:
		Vector3 m_vDir;	// the directional light
		float m_Intensity;		// the directional light intensity
		float m_AngleX;	// light direction rotation angle around x-axis [rad]
		float m_AngleY;
	};

	CDirectionalLight m_DirLight;

	void UpdateDirectionalLight();

	void RenderBackground();

	void RenderPreview();

	void RenderTexture();

public:

	CBumpmapTextureMaker2_LWO2();

	~CBumpmapTextureMaker2_LWO2();

	void Release();

	bool LoadShader( const std::string& shader_filename );

	/// load a model file, compile and load the mesh object
	bool LoadModel( const char *pFilename );

	/// load a mesh object file
	bool LoadMeshObject( const char *pFilename );

	void Render();

	void SetRenderMode( unsigned int render_mode );

	void SetViewWidth( float fViewVolumeWidth );

	inline float GetViewWidth() const { return m_fViewVolumeWidth; }

	/// set the direction of the directional light
	void SetDirectionalLightAngleX( float angle_x ) { m_DirLight.m_AngleX = angle_x; }
	void SetDirectionalLightAngleY( float angle_y ) { m_DirLight.m_AngleY = angle_y; }

	/// save the rendering results as image files
	void SaveImages( int width = 512, int height = 512 );
};



#endif		/*  __BUMPMAPTEXTUREMAKER2_LWO2_H__  */
