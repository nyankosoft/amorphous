#ifndef  __MotionPrimitiveViewer_H__
#define  __MotionPrimitiveViewer_H__


#include <vector>
#include <string>

#include "GameInput/fwd.h"
#include "3DCommon/all.h"
#include "UI/ui_fwd.h"
#include "MotionSynthesis/fwd.h"
#include "MotionSynthesis/SkeletonRenderer.h"


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


private:

	void RenderFloor();

	void RenderPoles();

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

	void OnItemSelected( const CGM_ListBoxItem& item );

	void LoadMotionPrimitivesFromDatabase( const std::string& filename, const std::string& motion_table_name = "std" );
};


#endif		/*  __MotionPrimitiveViewer_H__  */
