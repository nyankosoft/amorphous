
#include "AmbientOcclusionLightmapBuilder.h"
#include "lightmapbuilder.h"

#include "3DCommon/Camera.h"
#include "3DCommon/TextureRenderTarget.h"
#include "3DCommon/D3DXMeshObject.h"
#include "3DCommon/Shader/ShaderManager.h"

#include "3DMath/Vector3.h"
#include "3DMath/Plane.h"
#include "Support/SafeDeleteVector.h"
#include "Support/macro.h"
#include "Support/BMPImageExporter.h"


AmbientOcclusionLightmapBuilder::~AmbientOcclusionLightmapBuilder()
{
	Release();
}


void AmbientOcclusionLightmapBuilder::Release()
{
	SafeDeleteVector( m_vecpMesh );
	SafeDelete( m_pEnvLightMesh );
	SafeDelete( m_pShaderMgr );
}


//template <class T>
//bool AmbientOcclusionLightmapBuilder<T>::Calculate( vector<CLightmap>& rvecLightmap, CPolygonMesh<T>& rMesh )
//bool AmbientOcclusionLightmapBuilder::Calculate( AOLightmapDesc& desc )
bool AmbientOcclusionLightmapBuilder::Calculate( vector<CLightmap>& rvecLightmap, LightmapDesc& desc )
{
	AOLightmapDesc& ao_desc = desc.AOLightmap;

	CTextureRenderTarget tex_render_target;
	tex_render_target.Init( ao_desc.RenderTargetTexWidth, ao_desc.RenderTargetTexHeight );

	m_NumRenderTargetTexels = ao_desc.RenderTargetTexWidth * ao_desc.RenderTargetTexHeight;

	// load shader
	m_pShaderMgr = new CShaderManager();
	if( !m_pShaderMgr->LoadShaderFromFile( "D:\\R&D\\Project\\Project3\\BSPMapCompiler\\BSPMapCompiler\\Shader\\AmbientOcclusionLightmap.fx" ) )
	{
		return false;
	}
	m_pShaderMgr->RegisterTechnique( TECH_ENVIRONMENT_LIGHT,"EnvLight" );
	m_pShaderMgr->RegisterTechnique( TECH_OCCULUSION_MESH,	"OcclusionGeometry" );

	// load meshes
	size_t i, num_meshes = ao_desc.m_MeshArchive.size();
	m_vecpMesh.resize(num_meshes);
	for( i=0; i<num_meshes; i++ )
	{
		CD3DXMeshObject* pMesh = new CD3DXMeshObject();
		pMesh->LoadMeshFromArchive( ao_desc.m_MeshArchive[i], "dummy_filename_for_mesh" );
		m_vecpMesh[i] = pMesh;
	}

	CD3DXMeshObject* pEnvLightMesh = new CD3DXMeshObject();
	pEnvLightMesh->LoadMeshFromArchive( ao_desc.m_EnvLightMesh, "dummy_name_for_envlight_mesh" );
	m_pEnvLightMesh = pEnvLightMesh;

	// set world & projection matrix
	D3DXMATRIX matWorld, matProj;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixPerspectiveFovLH( &matProj, 3.141592f - 0.1f, 1.0f, 0.05f, 100.0f );
	m_pShaderMgr->SetWorldTransform( matWorld );
	m_pShaderMgr->SetProjectionTransform( matProj );


	size_t num_lightmaps = rvecLightmap.size();

	// calc the total number of lightmap texels
	// used to report progress
	int num_total_lightmap_texels = 0;
	for( i=0; i<num_lightmaps; i++ )
	{
		num_total_lightmap_texels += rvecLightmap[i].GetNumPoints();
	}


	m_NumTotalLightmapTexels = num_total_lightmap_texels;
	m_NumProcessedLightmapTexels = 0;

//	CStatusDisplay::Get()->RegisterTask( "Lightmap Raytrace" );

	for( i=0; i<num_lightmaps; i++ )
	{
		CalculateLightmapTexelIntensity( rvecLightmap[i], tex_render_target );

//		CStatusDisplay::Get()->UpdateProgress( "Lightmap Raytrace", (float)(i+1) / (float)num_lightmaps );
	}

//	ScaleIntensityAndAddAmbientLight(rvecLightmap);

	return true;
}


//template <class T>
void AmbientOcclusionLightmapBuilder::RenderScene()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    // clear the backbuffer to a blue color
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

    // begin the scene
    pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	D3DXHANDLE pTexHandle = m_pShaderMgr->GetEffect()->GetParameterByName( NULL, "Texture0" );

	m_pShaderMgr->SetTechnique( TECH_ENVIRONMENT_LIGHT );

	// render skybox
//	m_pEnvLightMesh->Render( m_pShaderMgr->GetEffect(), pTexHandle, NULL );


	HRESULT	hr = m_pShaderMgr->SetTechnique( TECH_OCCULUSION_MESH );

	size_t i, num_meshes = m_vecpMesh.size();
	for( i=0; i<num_meshes; i++ )
	{
		// render mesh
//		m_vecpMesh[i]->Render( m_pShaderMgr->GetEffect(), pTexHandle, NULL );
		m_vecpMesh[i]->Render( *m_pShaderMgr );
	}


    // end the scene
    pd3dDevice->EndScene();

    // present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


void SaveSceneImage( DWORD* pTexel, int num_render_target_texels )
{
	static int s_Counter=0;

	int width = (int)sqrt( (double)num_render_target_texels );

	CBMPImageExporter image_exporter;
	image_exporter.OutputImage_24Bit(
		fmt_string("DebugInfoFile/images/ao_scene%03d.bmp", s_Counter).c_str(),
		width, width, pTexel );

	s_Counter++;
}


inline SFloatRGBColor AmbientOcclusionLightmapBuilder::GetEnvLightIntensity( CTextureRenderTarget& tex_render_target )
{
//	LPDIRECT3DTEXTURE9 pRenderTargetTexture = tex_render_target.GetRenderTargetTexture();

	tex_render_target.CopyRenderTarget();
	LPDIRECT3DTEXTURE9 pRenderTargetTexture = tex_render_target.GetRenderTargetCopyTexture();

	D3DLOCKED_RECT locked_rect;
	HRESULT hr = pRenderTargetTexture->LockRect( 0, &locked_rect, NULL, 0 );

	DWORD* pTexel = (DWORD *)locked_rect.pBits;

	SFloatRGBColor color, color_sum = SFloatRGBColor(0,0,0);
	int i, num_texels = m_NumRenderTargetTexels;
	for( i=0; i<num_texels; i++ )
	{
		color.SetARGB32( pTexel[i] );
		color_sum += color;
	}

	color_sum = color_sum / 256.0f;

	// debut output of the scene images
	PERIODICAL( 20, SaveSceneImage( pTexel, m_NumRenderTargetTexels ) );

	pRenderTargetTexture->UnlockRect(0);

	return color_sum;
}


void AmbientOcclusionLightmapBuilder::CalculateLightmapTexelIntensity( CLightmap& rLightmap,
																	   CTextureRenderTarget& tex_render_target )
{
	int i,x,y;

	int iLightmapWidth  = rLightmap.GetNumPoints_X();
	int iLightmapHeight = rLightmap.GetNumPoints_Y();

	Vector3 vLightmapPoint;
	Vector3 vNormal;
	Vector3 vLightDir;
	SFloatRGBColor color;
	SFloatRGBColor envlight_intensity(0,0,0);
	Matrix34 lightmap_pose = rLightmap.GetWorldPose();
	D3DXMATRIX matCamera;

	// lightmap plane normal * shift
	const Vector3 vShift = lightmap_pose.matOrient.GetColumn(2) * 0.01f;

	int num_processed_texels = m_NumProcessedLightmapTexels;
	const int num_total_texels = m_NumTotalLightmapTexels;

	SPlane& rPlane = rLightmap.GetPlane();

	for(y=0; y<iLightmapHeight; y++)
	{
		for(x=0; x<iLightmapWidth; x++)
		{
			vLightmapPoint = rLightmap.GetPoint(x,y) + vShift;

//			vToLight = pPointLight->vPosition - vLightmapPoint;
//			fRealDist = Vec3Length(&vToLight);
//			vToLight_n = vToLight / fRealDist;
//			ray.vGoal = vLightmapPoint + vToLight_n * m_fSurfaceErrorTolerance;

			CCamera cam;
			lightmap_pose.vPosition = vLightmapPoint;
			cam.SetPose( lightmap_pose );
			cam.GetCameraMatrix(matCamera);
			m_pShaderMgr->SetViewTransform( matCamera );

			// set texture render target
			tex_render_target.SetRenderTarget();

			RenderScene();

			// restore the original render target
			tex_render_target.ResetRenderTarget();

			envlight_intensity = GetEnvLightIntensity( tex_render_target );

			// ========================= normal pixel intensity calculation ===========================
			color = rLightmap.GetTexelColor(x,y);
			rLightmap.SetTexelColor(x,y, color + envlight_intensity );

			// ========= replace lightmap colors with normal directions (visual debugging) ============
//			Vector3& rvNormal = rLightmap.avNormal[ LMTexelAt(x,y) ];
//			rLightmap.intensity[ LMTexelAt(x,y) ].fRed   = (float)fabs(rvNormal.x);
//			rLightmap.intensity[ LMTexelAt(x,y) ].fGreen = (float)fabs(rvNormal.y);
//			rLightmap.intensity[ LMTexelAt(x,y) ].fBlue  = (float)fabs(rvNormal.z);

			if( false /*m_iLightmapCreationFlag & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE*/ )
			{
//				vLightDir = rLightmap.GetLightDirection(x,y);
//				vLightDir += vToLight_n * pPointLight->fIntensity * 100.0f;
//				rLightmap.SetLightDirection( x, y, vLightDir );
			}

			num_processed_texels++;
//			CStatusDisplay::Get()->UpdateProgress( "Lightmap Raytrace",
//			                                       (float)(num_processed_texels) / (float)num_total_texels );

			Sleep( 5 );

		}
	}
	
	m_NumProcessedLightmapTexels = num_processed_texels;

}