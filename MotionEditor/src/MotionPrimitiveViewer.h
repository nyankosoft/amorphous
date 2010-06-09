#ifndef  __MotionPrimitiveViewer_H__
#define  __MotionPrimitiveViewer_H__


#include "gds/Input/fwd.hpp"
#include "gds/Graphics.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/MotionSynthesis/fwd.hpp"
#include "gds/MotionSynthesis/SkeletonRenderer.hpp"


class CSkeletalMeshMotionViewer
{
	CMeshObjectHandle m_SkeletalMesh;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_Technique;

	Matrix34 m_ViewerPose;

//	boost::shared_ptr<msynth::CSkeleton> m_pSkeleton;
	boost::weak_ptr<msynth::CSkeleton> m_pSkeleton;

	bool m_UseQuaternionForBoneTransformation;

	void Update_r( const msynth::CBone& bone,
                                          const msynth::CTransformNode& node,
										  boost::shared_ptr<CSkeletalMesh>& pMesh );

	void UpdateVertexBlendTransforms( CShaderManager& shader_mgr, CSkeletalMesh& skeletal_mesh );

	void UpdateVertexBlendMatrices( CShaderManager& shader_mgr, CSkeletalMesh& skeletal_mesh );

public:

	CSkeletalMeshMotionViewer();

	void Init();
	void LoadSkeletalMesh( const std::string& mesh_path );
	void Render();
	void Update( const msynth::CKeyframe& keyframe );

	void SetSkeleton( boost::weak_ptr<msynth::CSkeleton> pSkeleton ) { m_pSkeleton = pSkeleton; }

	void SetViewerPose( const Matrix34& viewer_pose ) { m_ViewerPose = viewer_pose; }
};


class CMotionPrimitiveViewer
{
	CGM_DialogManagerSharedPtr m_pDialogManager;

	boost::shared_ptr<CInputHandler> m_pGUIInputHandler;

	boost::shared_ptr<CInputHandler> m_pInputHandler;

	std::vector< boost::shared_ptr<msynth::CMotionPrimitive> > m_vecpMotionPrimitive;

	CGM_ListBox *m_pMotionPrimitiveListBox;

	boost::shared_ptr<msynth::CMotionPrimitive> m_pCurrentMotion;

	float m_fCurrentPlayTime;

	msynth::CSkeletonRenderer m_SkeletonRenderer;

	boost::shared_ptr<CUnitCube> m_pUnitCube;

	CLineStrip m_MotionTrace;

	CLineList m_KeyframeCoords;

	CLineList m_DirectionGuide;

	bool m_Playing;

	bool m_RenderMesh;

	CSkeletalMeshMotionViewer m_MeshViewer;

	float m_fPlaySpeedFactor;

	bool m_DisplaySkeletalMesh;

	CGM_Static *m_pPlaytimeText;

private:

	void RenderFloor();

	void RenderPoles();

	void UpdateLinestrip();

public:

	enum UIID
	{
		ROOT_DIALOG,
		LBX_MOTION_PRIMITIVES,
		STC_PLAYTIME,
		NUM_UIIDS
	};

	CMotionPrimitiveViewer();
	virtual ~CMotionPrimitiveViewer();

	void Init();

	void Update( float dt );

	void Render();

	void OnItemSelected( const CGM_ListBoxItem& item, int item_index );

	int LoadMotionPrimitivesFromDatabase( const std::string& mdb_filepath, const std::string& motion_table_name = "std" );

	void SetRenderMesh( bool render ) { m_RenderMesh = render; }

	void SetViewerPose( const Matrix34& viewer_pose ) { m_MeshViewer.SetViewerPose( viewer_pose ); }

	void ToggleDisplaySkeletalMesh() { m_DisplaySkeletalMesh = !m_DisplaySkeletalMesh; }

	void UpdatePlayTime( float new_playtime );

	void HandleInput( const SInputData& input );
};


#endif		/*  __MotionPrimitiveViewer_H__  */
