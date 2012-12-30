#include "PyModule_Camera.hpp"
#include "PyModule_Stage.hpp"

#include "3DMath/Vector3.hpp"
#include "3DMath/Matrix34.hpp"

#include "BE_ScriptedCamera.hpp"
#include "EntityMotionPathRequest.hpp"
#include "GameMessage.hpp"
#include "Stage.hpp"
#include "EntitySet.hpp"

#include "CopyEntityDesc.hpp"

#include "Support/Macro.h"
#include "Support/Vec3_StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "../base.hpp"


namespace amorphous
{

using namespace std;


inline static CStage *GetStage() { return GetStageForScriptCallback(); }


using namespace gsf::py::cam;

/*
struct NamedEntityCache
{
	string name;
	CCopyEntity *pEntity;
};

#define NAMED_ENTITY_CACHE_SIZE	4
static NamedEntityCache g_NamedEntityCache[NAMED_ENTITY_CACHE_SIZE];
*/


static CCopyEntity *GetEntityByName( const char* entity_name )
{
	if( GetStage() )
        return GetStage()->GetEntitySet()->GetEntityByName(entity_name);
	else
        return NULL;
}

static CCopyEntity *CreateEntity( CCopyEntityDesc& desc )
{
	if( GetStage() )
        return GetStage()->CreateEntity( desc );
	else
        return NULL;
}

//CCopyEntity *g_pMotionPathTargetEntity = NULL;



static EntityMotionPathRequest g_EntityMotionPathRequest;
static CCopyEntity* GetCurrentTargetCameraEntity() { return g_EntityMotionPathRequest.pTargetEntity; }

static CScriptCameraKeyFrames gs_ScriptCameraKeyFrames;


PyObject* CreateCameraController( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	char *camera_controller_name;
	char *base_entity_name = "CutsceneCameraController"; // TODO: check whether this is safe or not

	int result = PyArg_ParseTuple( args, "s|s", &camera_controller_name, &base_entity_name );

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_entity_name );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = camera_controller_name;
	desc.WorldPose.Identity();

	CCopyEntity* pEntity = CreateEntity( desc );
	if( !pEntity )
	{
		LOG_PRINT_WARNING( "Failed to create camera controller: " + string(camera_controller_name) );
	}

//	if( !pEntity )
//		MsgBoxFmt( "" );

	return Py_None;
}


PyObject* CreateCamera( PyObject* self, PyObject* args )
{
	LOG_FUNCTION_SCOPE();

	Py_INCREF( Py_None );

	char *camera_entity_name = NULL;
	char *camera_controller_name = NULL;
	CameraParam param;

	float nearclip = 0.05f, farclip = 500.0f, fov = 3.141592f / 3.0f;
	float aspectratio = CGraphicsComponent::GetAspectRatio();// 4.0f / 3.0f;

	int result = PyArg_ParseTuple( args, "ss|ffff",
		&camera_entity_name, &camera_controller_name,
		&param.nearclip, &param.farclip, &param.fov, &param.aspect_ratio );

	if( !GetStage() )
		return Py_None;

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( "ScriptedCamera" );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.TypeID = CCopyEntityTypeID::SCRIPTED_CAMERA_ENTITY;
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = camera_entity_name;
	desc.WorldPose.vPosition = Vector3(0,0,0);

	CCopyEntity *pCameraController = GetEntityByName( camera_controller_name );
	if( !IsValidEntity(pCameraController) )
	{
		g_Log.Print( "CreateCamera() - cannot find camera controller entity: '%s'", camera_controller_name );
		return Py_None;
	}

	desc.pParent = pCameraController;

	CCopyEntity* pEntity = CreateEntity( desc );
	if( !pEntity )
	{
		g_Log.Print( "CreateCamera() - cannot create camera entity: '%s'", camera_entity_name );
		return Py_None;
	}

	// set default camera params
	SGameMessage msg( GM_SET_DEFAULT_CAMERA_PARAM );
	msg.pUserData = &param;
	SendGameMessageTo( msg, pEntity );

	return Py_None;
}

// TODO: support MT. this scheme will not allow
// paths for multiple entities to be set at the same time
PyObject* StartCameraScript( PyObject* self, PyObject* args )
{
	LOG_FUNCTION_SCOPE();

	Py_INCREF( Py_None );

	char *camera_entity_name = NULL;
	int path_track_mode = EntityMotionPathRequest::SET_POSITION;
	int result = PyArg_ParseTuple( args, "s|i", &camera_entity_name, &path_track_mode );

	g_Log.Print( "StartCameraScript() - acquiring target camera entity: %s", camera_entity_name );

	CCopyEntity *pCameraEntity = GetEntityByName( camera_entity_name );

	if( !pCameraEntity )
	{
		return Py_None;
	}

	g_Log.Print( "StartCameraScript() - acquired target camera entity: %s", camera_entity_name );

	g_EntityMotionPathRequest.Clear();
	g_EntityMotionPathRequest.pTargetEntity = pCameraEntity;
	g_EntityMotionPathRequest.mode = path_track_mode;

	gs_ScriptCameraKeyFrames.Reset();

//	MsgBoxFmt( "starting to set motion path for entity: %s", entity_name );

	return Py_None;
}


PyObject* SetScreenColorC32( PyObject* self, PyObject* args )
{
	float time;
	U32 color;	// 32-bit ARGB color (8 bits for each channel)
	int result = PyArg_ParseTuple( args, "fk", &time, &color );

	CCopyEntity *pCameraEntity = GetCurrentTargetCameraEntity();

	if( !pCameraEntity )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

//	MsgBoxFmt( "starting to set motion path for entity: %s", entity_name );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetBlurStrength( PyObject* self, PyObject* args )
{
	float time;
	Vector2 vBlurStrength;	// 32-bit ARGB color (8 bits for each channel)
	int result = PyArg_ParseTuple( args, "fff", &time, &vBlurStrength.x, &vBlurStrength.y );

	gs_ScriptCameraKeyFrames.Effect.Blur.AddKeyFrame( time, vBlurStrength );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetGlareThreshold( PyObject* self, PyObject* args )
{
	float time;
	float glare_threshold;	// 32-bit ARGB color (8 bits for each channel)
	int result = PyArg_ParseTuple( args, "ff", &time, &glare_threshold );

	gs_ScriptCameraKeyFrames.Effect.GlareThreshold.AddKeyFrame( time, glare_threshold );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetMotionBlurWeight( PyObject* self, PyObject* args )
{
	float time;
	float motion_blur;
	int result = PyArg_ParseTuple( args, "ff", &time, &motion_blur );

	gs_ScriptCameraKeyFrames.Effect.MotionBlurStrength.AddKeyFrame( time, motion_blur );

    Py_INCREF( Py_None );
	return Py_None;
}


/*
PyObject* FadeOutTo_C32( PyObject* self, PyObject* args )
{
	float start_time, end_time;
	U32 fade_color;
	int result = PyArg_ParseTuple( args, "ffk", &start_time, &end_time, &fade_color );

	CCopyEntity *pCameraEntity = GetCurrentTargetCameraEntity( camera_entity_name );

	if( !pCameraEntity )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

//	MsgBoxFmt( "starting to set motion path for entity: %s", entity_name );

    Py_INCREF( Py_None );
	return Py_None;
}
*/

PyObject* EndCameraScript( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	char *camera_entity_name;
	int result = PyArg_ParseTuple( args, "s", &camera_entity_name );

	// set motion path (pos & orientation)
	SGameMessage msg( GM_SET_MOTION_PATH );
	msg.pUserData = &g_EntityMotionPathRequest;

	g_Log.Print( "EndCameraScript() - sending motion path to camera entity (%d key poses)", (int)g_EntityMotionPathRequest.vecKeyPose.size() );

	SendGameMessageTo( msg, g_EntityMotionPathRequest.pTargetEntity );

	// set camera-related effects
	// camera property: fov, nearclip, farclip, etc.
	// effect property: blur, glare, etc.
	msg = SGameMessage( GM_SET_SCRIPTCAMERAKEYFRAMES );
	msg.pUserData = &gs_ScriptCameraKeyFrames;

	g_Log.Print( "EndCameraScript() - sending camera effects to script camera entity" );

	SendGameMessageTo( msg, g_EntityMotionPathRequest.pTargetEntity );

	return Py_None;
}


PyObject* gsf::py::cam::SetPose( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	if( !GetStage() )
		return Py_None;

	if( !g_EntityMotionPathRequest.pTargetEntity )
		return Py_None;

	float time;
	Vector3 pos;
	float heading = 0, pitch = 0, bank = 0;	// rotation angles measured in degree

	int result = PyArg_ParseTuple( args, "ffff|fff", &time, &pos.x, &pos.y, &pos.z, &heading, &pitch, &bank );

	Matrix34 pose;
	pose.vPosition = pos;
	pose.matOrient = Matrix33RotationZ(deg_to_rad(bank)) * Matrix33RotationX(deg_to_rad(pitch)) * Matrix33RotationY(deg_to_rad(heading));
    g_EntityMotionPathRequest.vecKeyPose.push_back( KeyPose(time,pose) );

	return Py_None;
}


// PyObject* SetTimeOffset( PyObject* self, PyObject* args )
// link error from multiple definition (conflict with the one defined in PyModule_GraphicsElement.cpp)
// The linker cannot tell the difference.

PyObject* gsf::py::cam::SetTimeOffset( PyObject* self, PyObject* args )
{
	if( !GetStage() )
		return Py_None;

//	float time_offset;
//	int result = PyArg_ParseTuple( args, "f", &time_offset );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetTarget( PyObject* self, PyObject* args )
{
	g_Log.Print( "cam.SetTarget() - called" );

	Py_INCREF( Py_None );

	if( !GetStage() || !g_EntityMotionPathRequest.pTargetEntity )
	{
		return Py_None;
	}

	float time = 0;
	char *focus_target_entity_name = NULL;

	int result = PyArg_ParseTuple( args, "fs", &time, &focus_target_entity_name );

	CCopyEntity* pFocusTarget = GetEntityByName( focus_target_entity_name );

	// TODO: what if the focus target entity is created later
	if( pFocusTarget )
	{
		gs_ScriptCameraKeyFrames.Camera.FocusTarget.AddKeyFrame( time, CameraTargetHolder(pFocusTarget) );
	}
	else
	{
		gs_ScriptCameraKeyFrames.Camera.FocusTarget.AddKeyFrame( time, CameraTargetHolder(focus_target_entity_name) );
//		g_Log.Print( "cam.SetTarget() - unable to find the target entity: %s", focus_target_entity_name );
//		Py_INCREF( Py_None );
//		return Py_None;
	}


	return Py_None;
}


PyObject* SetBlendWeight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );

	if( !GetStage() || !g_EntityMotionPathRequest.pTargetEntity )
		return Py_None;

	float time, blend_weight;

	int result = PyArg_ParseTuple( args, "ff", &time, &blend_weight );

//	g_EntityMotionPathRequest.vecKeyPose.push_back( KeyPose(time,pose) );

	return Py_None;
}



PyMethodDef gsf::py::cam::g_PyModuleCameraMethod[] =
{
	{ "CreateCameraController",	CreateCameraController,	METH_VARARGS, "" },
	{ "CreateCamera",			CreateCamera,			METH_VARARGS, "" },
	{ "StartCameraScript",		StartCameraScript,		METH_VARARGS, "" },
	{ "EndCameraScript",		EndCameraScript,		METH_VARARGS, "" },
	{ "SetScreenColorC32",		SetScreenColorC32,		METH_VARARGS, "" },
	{ "SetPose",				gsf::py::cam::SetPose,	METH_VARARGS, "" },
//	{ "SetPosition",			SetPosition,	B		METH_VARARGS, "" },
//	{ "SetOrientation",			SetOrientation,			METH_VARARGS, "" },
	{ "SetTarget",				SetTarget,				METH_VARARGS, "" },
//	{ "SetTargetPosition",		SetTargetPosition,		METH_VARARGS, "" },
	{ "SetBlurStrength",		SetBlurStrength,		METH_VARARGS, "" },
	{ "SetMotionBlurWeight",	SetMotionBlurWeight,	METH_VARARGS, "" },
	{ "SetGlareThreshold",		SetGlareThreshold,		METH_VARARGS, "" },
	{ "SetBlendWeight",			SetBlendWeight,			METH_VARARGS, "" },
//	{ "SetBlendOrder",			SetBlendOrder,			METH_VARARGS, "" },
	{ "SetTimeOffset",			gsf::py::cam::SetTimeOffset,		METH_VARARGS, "" },
	{NULL, NULL}
};


} // namespace amorphous
