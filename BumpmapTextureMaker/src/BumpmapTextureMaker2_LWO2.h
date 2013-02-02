#ifndef  __BUMPMAPTEXTUREMAKER2_LWO2_H__
#define  __BUMPMAPTEXTUREMAKER2_LWO2_H__


#include "gds/3DMath/Vector3.hpp"
#include "gds/3DMath/Matrix33.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"

using namespace amorphous;


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
	MeshHandle m_3DModel;

	ShaderHandle m_Shader;

//	LPDIRECT3DTEXTURE9 m_pDefaultTexture;

	TextureHandle m_PreviewTexture;

	std::string m_PreviewTextureFilepath;

	ShaderTechniqueHandle m_aShaderTechnique[BTM_NUM_RENDERMODES];

	int m_TechniqueID;
	
	float m_fViewVolumeWidth;

	std::string m_strBaseFilename;

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
