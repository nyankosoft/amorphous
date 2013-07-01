#ifndef  __BuiltinMotionsTest_HPP__
#define  __BuiltinMotionsTest_HPP__


#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/MotionSynthesis/MotionPrimitive.hpp" // For m_KeyframeToRender

#include "../../../_Common/GraphicsTestBase.hpp"


class BuiltinMotionsTest : public CGraphicsTestBase
{
	MeshHandle m_SkyboxMesh;

	MeshHandle m_TerrainMesh;

	MeshHandle m_SkeletalCharacter;

	ShaderTechniqueHandle m_MeshTechnique;

	TextureHandle m_FloorTexture;

	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

	ShaderHandle m_SkeletalCharacterShader;

	std::vector<std::string> m_CharacterModelPathnames;

	boost::shared_ptr<msynth::CSkeleton> m_pSkeleton;

	std::vector< boost::shared_ptr<msynth::CMotionPrimitive> > m_pMotions;

	boost::shared_ptr<FontBase> m_pFont;

	unsigned int m_CurrentModelIndex;

	unsigned int m_CurrentKeyframeIndex;

	float m_fCurrentTime;

	bool m_IsPlayingMotion;

	msynth::CKeyframe m_KeyframeToRender;

protected:

	void RenderFloor();

	void InitBuiltinMotions();

	void SetLight();

	boost::shared_ptr<msynth::CMotionPrimitive> GetCurrentMotion();

	void LoadCharacterModel();

	bool GetKeyframeToRender( msynth::CKeyframe& dest );

	void ResetTimeIfPlayedToEnd();

public:

	BuiltinMotionsTest();

	~BuiltinMotionsTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const SInputData& input );

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __BuiltinMotionsTest_HPP__ */
