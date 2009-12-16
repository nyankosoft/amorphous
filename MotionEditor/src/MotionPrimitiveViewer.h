#ifndef  __MotionPrimitiveViewer_H__
#define  __MotionPrimitiveViewer_H__


#include <vector>
#include <string>

#include <gds/Input/fwd.hpp>
#include <gds/Graphics.hpp>
#include <gds/GUI/fwd.hpp>
#include <gds/MotionSynthesis/fwd.hpp>
#include <gds/MotionSynthesis/SkeletonRenderer.hpp>


class CSkeletalMeshMotionViewer
{
	CMeshObjectHandle m_SkeletalMesh;

	CShaderHandle m_Shader;

public:

	void Init();
	void LoadSkeletalMesh( const std::string& mesh_path );
	void Render();
	void Update( const msynth::CKeyframe& keyframe );
};


class CMotionPrimitiveViewer
{
	CGM_DialogManagerSharedPtr m_pDialogManager;

	CInputHandlerSharedPtr m_pInputHandler;

	std::vector<msynth::CMotionPrimitiveSharedPtr> m_vecpMotionPrimitive;

	CGM_ListBox *m_pMotionPrimitiveListBox;

	msynth::CMotionPrimitiveSharedPtr m_pCurrentMotion;

	float m_fCurrentPlayTime;

	msynth::CSkeletonRenderer m_SkeletonRenderer;

	boost::shared_ptr<CUnitCube> m_pUnitCube;

	CLineStrip m_MotionTrace;

	CLineList m_KeyframeCoords;

	CLineList m_DirectionGuide;

	bool m_RenderMesh;

	CSkeletalMeshMotionViewer m_MeshViewer;

private:

	void RenderFloor();

	void RenderPoles();

	void UpdateLinestrip();

public:

	enum UIID
	{
		ROOT_DIALOG,
		LBX_MOTION_PRIMITIVES,
		NUM_UIIDS
	};

	CMotionPrimitiveViewer();
	virtual ~CMotionPrimitiveViewer();

	void Init();

	void Update( float dt );

	void Render();

	void OnItemSelected( const CGM_ListBoxItem& item, int item_index );

	void LoadMotionPrimitivesFromDatabase( const std::string& filename, const std::string& motion_table_name = "std" );

	void SetRenderMesh( bool render ) { m_RenderMesh = render; }
};


#endif		/*  __MotionPrimitiveViewer_H__  */
