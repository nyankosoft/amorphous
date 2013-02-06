#ifndef  __BVHViewer_HPP__
#define  __BVHViewer_HPP__


#include <boost/shared_ptr.hpp>
#include "gds/App/GraphicsApplicationBase.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/Input.hpp"
#include "gds/BVH/BVHPlayer.hpp"

using namespace amorphous;


class CBVHViewer : public CGraphicsApplicationBase
{
	MeshHandle m_BoxForGround;

	MeshHandle m_Mesh;

	std::vector<std::string> m_Filepaths;

	int m_CurrentFileIndex;

	ShaderTechniqueHandle m_MeshTechnique;
	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	std::vector<ShaderHandle> m_Shaders;

	std::vector<ShaderTechniqueHandle> m_Techniques;

	int m_CurrentShaderIndex;

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

	Matrix34 m_MeshWorldPose;

	bool m_Lighting;

	BVHPlayer m_BVHPlayer;

	bool m_Playing;

	float m_fPlayTime;

	const std::string GetApplicationTitle() { return "BVHViewer"; }

protected:

	void RefreshFileList( const std::string& directory_path );

	void RenderBVHSkeleton();

	void UpdateShaderParams();

	bool LoadModel( const std::string& mesh_filepath );

	void LoadShaders();

	void SetLights( ShaderManager& shader_mgr );

	void LoadBlankTextures( BasicMesh& mesh );

public:

	CBVHViewer();

	~CBVHViewer();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void RenderScene();

//	void UpdateViewTransform( const Matrix44& matView );
//	void UpdateProjectionTransform( const Matrix44& matProj );

	void HandleInput( const SInputData& input );
};


#endif /* __BVHViewer_HPP__ */
