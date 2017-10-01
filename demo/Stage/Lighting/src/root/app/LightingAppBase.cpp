#include "LightingAppBase.hpp"

#include "amorphous/Graphics/GraphicsElementManager.hpp"
#include "amorphous/Graphics/GraphicsEffectManager.hpp"
#include "amorphous/Graphics/3DtoScreenSpaceConversions.hpp"
#include "amorphous/Support.hpp"
#include "amorphous/Input.hpp"
#include "amorphous/Stage.hpp"
#include "amorphous/Task.hpp"
#include "amorphous/Script.hpp"

using namespace std;


static string sg_TestStageScriptToLoad = "./Script/hs_lights.bin";


extern ApplicationBase *amorphous::CreateApplicationInstance() { return new LightingAppBase(); }


LightingAppTask::LightingAppTask()
{
	ScriptManager::ms_UseBoostPythonModules = true;

	StageLoader stg_loader;
//	m_pStage = stg_loader.LoadStage( "shadow_for_directional_light.bin" );
	m_pStage = stg_loader.LoadStage( sg_TestStageScriptToLoad );

	GetCameraController()->SetPose( Matrix34( Vector3(0,20,-15), Matrix33Identity() ) );
}


void LightingAppTask::DisplayEntityPositions( GraphicsElementAnimationManager& animated_graphics_manager )
{
	AABB3 aabb;
	aabb.vMin = Vector3(1,1,1) * (-100.0f);
	aabb.vMax = Vector3(1,1,1) * (100.0f);
	vector<CCopyEntity *> pEntities;
	OverlapTestAABB aabb_test( aabb, &pEntities );
	m_pStage->GetEntitySet()->GetOverlappingEntities( aabb_test );

	shared_ptr<GraphicsElementManager> pElementMgr = animated_graphics_manager.GetGraphicsElementManager();

	static vector< shared_ptr<FrameRectElement> > pFrameRects;
	const size_t num_max_frame_rects = 128;
	const int frame_width = 4;
	int rect_edge_length = 50;
	int layer = 0;
	if( pFrameRects.empty() )
	{
		pFrameRects.resize( num_max_frame_rects );
		for( size_t i=0; i<pFrameRects.size(); i++ )
			pFrameRects[i] = pElementMgr->CreateFrameRect( SRect(0,0,rect_edge_length,rect_edge_length), SFloatRGBAColor(0,0,0,0), frame_width, layer );
	}

	for( size_t i=0; i<pFrameRects.size(); i++ )
	{
		pFrameRects[i]->SetLocalTopLeftPos( Vector2(500,20) );
		pFrameRects[i]->SetColor( 0, SFloatRGBAColor(0,0,0,0) );
	}
	
	static int s_layer = 10;
	for( size_t i=0; i<pFrameRects.size(); i++ )
		pFrameRects[i]->SetLayer( s_layer );

	size_t num_displayed_entities = 0;
	const size_t num_entities = pEntities.size();
	for( size_t i=0; i<num_entities; i++ )
	{

		if( num_displayed_entities == pFrameRects.size() )
			break;

		if( !pEntities[i] )
			continue;

		bool entity_is_in_camera = GetCamera().ViewFrustumIntersectsWith( Sphere( pEntities[i]->GetWorldPosition(), 0.1f ) );
		if( !entity_is_in_camera )
			continue;

		bool is_light_entity = false;
		const uint archive_id = pEntities[i]->GetBaseEntity()->GetArchiveObjectID();
		if( archive_id == BaseEntity::BE_POINTLIGHT
		 || archive_id == BaseEntity::BE_DIRECTIONALLIGHT )
		{
			is_light_entity = true;
		}
		else if( pEntities[i]->bNoClip )
			continue;
		else
			is_light_entity = false;

		Vector2 pos = CalculateScreenCoordsFromWorldPosition( GetCamera(), pEntities[i]->GetWorldPosition() );
//		Vector2 pos = Vector2(0,0);
//		clamp( pos.x, 0.0f, 800.0f );
//		clamp( pos.y, 0.0f, 600.0f );

		if( 0 <= pos.x && pos.x <= GraphicsComponent::GetReferenceScreenWidth()
		 && 0 <= pos.y && pos.y <= GraphicsComponent::GetReferenceScreenHeight() )
		{
			// display the entity position
			Vector2 top_left_pos = pos - Vector2((float)rect_edge_length,(float)rect_edge_length) * 0.5f;
			pFrameRects[num_displayed_entities]->SetLocalTopLeftPos( top_left_pos );

			SFloatRGBAColor color = is_light_entity ? SFloatRGBAColor(1.0f,0.6f,0.6f,0.7f) : SFloatRGBAColor(0.6f,1.0f,0.6f,0.7f);
			pFrameRects[num_displayed_entities]->SetColor( 0, color );
			num_displayed_entities++;
		}
	}
}


int LightingAppTask::FrameMove( float dt )
{
	int ret = StageViewerGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( GetAnimatedGraphicsManager() )
	{
		DisplayEntityPositions( *GetAnimatedGraphicsManager() );
	}

	return ID_INVALID;
}



//========================================================================================
// LightingAppBase
//========================================================================================

LightingAppBase::LightingAppBase()
{
}


LightingAppBase::~LightingAppBase()
{
//	Release();
}


int LightingAppBase::GetStartTaskID() const
{
//	return GameTask::ID_STAGE_VIEWER_TASK;
	return GAMETASK_ID_LIGHTING;
}
