#include "StageUtility.hpp"
#include "Graphics/D3DXMeshObjectBase.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Physics/ActorDesc.hpp"
#include "Physics/BoxShapeDesc.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"
#include "Stage/Stage.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Support/Log/DefaultLog.hpp"

using namespace std;
using namespace boost;
using namespace physics;


CEntityHandle<> CStageUtility::CreateNamedEntity( const std::string& entity_name,
								const std::string& base_name,
								const Matrix34& pose,
								const Vector3& vel,
								CActorDesc *pPhysActorDesc )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return CEntityHandle<>();

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_name.c_str() );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = entity_name;
	desc.WorldPose = pose;
	desc.pPhysActorDesc = pPhysActorDesc;
//	desc.SetWorldOrient( CreateOrientFromFwdDir( dir ) );

	desc.vVelocity = vel;
	desc.fSpeed = Vec3Length(vel);

	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	return pEntity ? CEntityHandle<>( pEntity->Self() ) : CEntityHandle<>();
}



//========================================================================================
// CreateCameraController
//========================================================================================

CEntityHandle<> CStageCameraUtility::CreateCameraController( const std::string& camera_controller_name,
													         int cutscene_input_handler_index )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return CEntityHandle<>();

	char *base_entity_name = NULL;
	if( 0 <= cutscene_input_handler_index )
		base_entity_name = "CutsceneCameraController";
	else
		base_entity_name = "CameraController";

//	int result = PyArg_ParseTuple( args, "s|s", &camera_controller_name, &base_entity_name );

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_entity_name );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = camera_controller_name;
	desc.WorldPose.Identity();

	CCopyEntity* pEntity = pStage->CreateEntity( desc );
	if( !pEntity )
	{
		LOG_PRINT_WARNING( fmt_string("Failed to create camera controller: '%s'", camera_controller_name.c_str()) );
		return CEntityHandle<>();
	}

	return CEntityHandle<>( pEntity->Self() );
}


CScriptedCameraEntity *CStageCameraUtility::CreateScriptedCamera( const std::string& camera_name,
														          const std::string& camera_controller_name,
															      CameraParam default_camera_param )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return NULL;

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( "ScriptedCamera" );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.TypeID = CCopyEntityTypeID::SCRIPTED_CAMERA_ENTITY;
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = camera_name;
	desc.WorldPose.vPosition = Vector3(0,0,0);

	CCopyEntity *pCameraController = pStage->GetEntitySet()->GetEntityByName( camera_controller_name.c_str() );
	if( !IsValidEntity(pCameraController) )
	{
		LOG_PRINT_ERROR( fmt_string(" Cannot find camera controller entity: '%s'", camera_controller_name.c_str()) );
		return NULL;
	}

	desc.pParent = pCameraController;

	CCopyEntity* pCameraEntity = pStage->CreateEntity( desc );
	if( !pCameraEntity )
	{
		LOG_PRINT_ERROR( fmt_string(" Cannot create camera entity: '%s'", camera_name.c_str()) );
		return NULL;
	}

	// set default camera params
	SGameMessage msg( GM_SET_DEFAULT_CAMERA_PARAM );
	msg.pUserData = &default_camera_param;
	SendGameMessageTo( msg, pCameraEntity );

//	return CEntityHandle<CScriptedCameraEntity>( pCameraEntity->??? )
	return dynamic_cast<CScriptedCameraEntity *>(pCameraEntity);
}



//========================================================================================
// CStageMiscUtility
//========================================================================================

CEntityHandle<> CStageMiscUtility::CreateBoxEntity( CMeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass )
{

	CMeshObjectHandle mesh_handle;
	bool mesh_loaded = mesh_handle.Load( mesh_desc );
	if( !mesh_loaded )
		return CEntityHandle<>();

	shared_ptr<CD3DXMeshObjectBase> pMesh = mesh_handle.GetMesh();
	if( !pMesh || pMesh->GetNumMaterials() == 0 )
		return CEntityHandle<>();

	// assumes that the box mesh consists of just one material
	AABB3 aabb;
	aabb.Nullify();
	for( int i=0; i<pMesh->GetNumMaterials(); i++ )
		aabb.MergeAABB( pMesh->GetAABB(i) );

	// actor and shape descriptors

	CBoxShapeDesc box_desc;
	box_desc.vSideLength = aabb.vMax;

	CActorDesc actor_desc;
	actor_desc.vecpShapeDesc.push_back( &box_desc );

	CEntityHandle<> entity = CreateNamedEntity( entity_name, entity_attributes_name, pose, vel, &actor_desc );
	CCopyEntity *pEntityRawPtr = entity.GetRawPtr();
	if( !pEntityRawPtr )
		return CEntityHandle<>();

	pEntityRawPtr->m_MeshHandle = mesh_handle;
	pEntityRawPtr->local_aabb = aabb;
	pEntityRawPtr->world_aabb.TransformCoord( aabb, pEntityRawPtr->GetWorldPose().vPosition );
}


CEntityHandle<> CStageMiscUtility::CreateBox( Vector3 edge_lengths,
											  SFloatRGBAColor diffuse_color,
											  const Matrix34& pose,
											  const float mass,
					                          const std::string& entity_name,
					                          const std::string& entity_attributes_name
					                          )
{
//	printf( "Creating box at %s - size: %s, color: %s\n",
//		to_string(pos).c_str(), to_string(edge_lengths).c_str(), to_string(diffuse_color).c_str() );
/*
	Matrix34 pose;
	pose.vPosition = pos;
	pose.matOrient
		= Matrix33RotationY( deg_to_rad(heading) )
		* Matrix33RotationX( deg_to_rad(pitch) )
		* Matrix33RotationZ( deg_to_rad(bank) );
*/

	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__BoxFromDimension__";

	shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( edge_lengths );
	CMeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;

	Vector3 vel = Vector3(0,0,0);

	return CreateBoxEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, mass );
}


CEntityHandle<> CStageMiscUtility::CreateBoxFromMesh( const char *mesh_resource_name,//const std::string& mesh_resource_path,
					    const Matrix34& pose,
						const float mass,
					    const std::string& entity_name,
					    const std::string& entity_attributes_name
						)
{
//	printf( "Creating box from mesh '%s' at %s\n", mesh_resource_path/*.c_str()*/,
//		to_string(pose.vPosition).c_str() );

	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__BoxFromMesh__";

	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = mesh_resource_name;

	Vector3 vel = Vector3(0,0,0);

	return CreateBoxEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, mass );
}



//========================================================================================
// CStageLightUtility
//========================================================================================

void CStageLightUtility::CreateHSPointLight( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, Vector3& pos, float attenu0, float attenu1, float attenu2 )
{
}


void CStageLightUtility::CreateHSDirectionalLight( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& dir )
{
}
