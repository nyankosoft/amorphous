#ifndef  __MeshViewer_HPP__
#define  __MeshViewer_HPP__


#include "amorphous/App/GraphicsApplicationBase.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

using namespace amorphous;


class MeshViewer : public GraphicsApplicationBase
{
	std::vector<MeshHandle> m_vecMesh;

	MeshHandle m_Mesh;

	std::vector<std::string> m_vecMeshFilepath;

	int m_CurrentFileIndex;

	ShaderTechniqueHandle m_MeshTechnique;
	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	std::vector<ShaderHandle> m_Shaders;

	std::vector<ShaderTechniqueHandle> m_Techniques;

	int m_CurrentShaderIndex;

	bool m_UseSingleDiffuseColorShader;

	/// default: -1 (do not use normal map textures)
	int m_NormalMapTextureIndex;

	std::vector<ShaderHandle> m_SingleDiffuseColorShaders;

	std::vector<ShaderTechniqueHandle> m_SingleDiffuseColorShaderTechniques;

	int m_CurrentSDCShaderIndex;

	bool m_RenderSubsetsInformation;

	bool m_RenderZSorted;

//	CUnitSphere m_LightPosIndicator;

	Vector3 m_vLightPosition;

	boost::shared_ptr<FontBase> m_pFont;

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

	/// Rendered for debugging
	/// 7 length options: 0.002, 0.004, 0.008, 0.016, 0.032, 0.064, 0.128[m]
	int m_NormalLengthFactor;

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

	MeshViewer();

	~MeshViewer();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void RenderScene();

//	void UpdateViewTransform( const Matrix44& matView );
//	void UpdateProjectionTransform( const Matrix44& matProj );

	void HandleInput( const InputData& input );
};


#endif /* __MeshViewer_HPP__ */
