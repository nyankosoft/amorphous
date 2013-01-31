#ifndef  __MeshViewer_HPP__
#define  __MeshViewer_HPP__


#include "gds/App/GraphicsApplicationBase.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

using namespace amorphous;


class CMeshViewer : public CGraphicsApplicationBase
{
	std::vector<MeshHandle> m_vecMesh;

	MeshHandle m_Mesh;

	std::vector<std::string> m_vecMeshFilepath;

	int m_CurrentFileIndex;

	CShaderTechniqueHandle m_MeshTechnique;
	CShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	std::vector<ShaderHandle> m_Shaders;

	std::vector<CShaderTechniqueHandle> m_Techniques;

	int m_CurrentShaderIndex;

	bool m_UseSingleDiffuseColorShader;

	/// default: -1 (do not use normal map textures)
	int m_NormalMapTextureIndex;

	std::vector<ShaderHandle> m_SingleDiffuseColorShaders;

	std::vector<CShaderTechniqueHandle> m_SingleDiffuseColorShaderTechniques;

	int m_CurrentSDCShaderIndex;

	bool m_RenderSubsetsInformation;

	bool m_RenderZSorted;

//	CUnitSphere m_LightPosIndicator;

	Vector3 m_vLightPosition;

	CFontSharedPtr m_pFont;

	int m_CurrentMouseX;
	int m_CurrentMouseY;
	int m_LastRMouseClickX;
	int m_LastRMouseClickY;

	float m_fHeading;
	float m_fPitch;


	float m_fInitCamShift;

	int m_ScalingFactor;

	Matrix34 m_MeshWorldPose;

	bool m_Lighting;

	const std::string GetApplicationTitle() { return "MeshViewer"; }

protected:

	void RefreshFileList( const std::string& directory_path );

	void RenderMeshes();

	void UpdateShaderParams();

	bool LoadModel( const std::string& mesh_filepath );

	void LoadShaders();

	void SetLights( ShaderManager& shader_mgr );

	void LoadBlankTextures( BasicMesh& mesh );

	void RenderSubsetsInformation( BasicMesh& mesh );

public:

	CMeshViewer();

	~CMeshViewer();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void RenderScene();

//	void UpdateViewTransform( const Matrix44& matView );
//	void UpdateProjectionTransform( const Matrix44& matProj );

	void HandleInput( const SInputData& input );
};


#endif /* __MeshViewer_HPP__ */
