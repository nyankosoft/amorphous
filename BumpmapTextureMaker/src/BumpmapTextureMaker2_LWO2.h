#ifndef  __BUMPMAPTEXTUREMAKER2_LWO2_H__
#define  __BUMPMAPTEXTUREMAKER2_LWO2_H__


#include "3DMath/Vector3.hpp"
#include "3DMath/Matrix33.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"


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
	CMeshObjectHandle m_3DModel;

	CShaderHandle m_Shader;

	LPDIRECT3DTEXTURE9 m_pDefaultTexture;

	CTextureHandle m_PreviewTexture;

	std::string m_PreviewTextureFilepath;

	CShaderTechniqueHandle m_aShaderTechnique[BTM_NUM_RENDERMODES];

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
