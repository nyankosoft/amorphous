#include "SkyboxMisc.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/Direct3D/Shader/D3DFixedFunctionPipelineManager.hpp"

using namespace std;
using namespace boost;


boost::shared_ptr<CBoxMeshGenerator> CreateSkyboxMeshGenerator( const std::string& texture_filepath )
{
	boost::shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator
		= shared_ptr<CBoxMeshGenerator>( new CBoxMeshGenerator );

	pBoxMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetTexCoordStyleFlags( TexCoordStyle::LINEAR_SHIFT_INV_Y );
	pBoxMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
	pBoxMeshGenerator->SetTexturePath( texture_filepath );

	return pBoxMeshGenerator;
}


CMeshObjectHandle CreateSkyboxMesh( const std::string& texture_filepath )
{
	CMeshResourceDesc skybox_mesh_desc;
	skybox_mesh_desc.pMeshGenerator = CreateSkyboxMeshGenerator( texture_filepath );

	CMeshObjectHandle skybox_mesh;
	bool loaded = skybox_mesh.Load( skybox_mesh_desc );

	return skybox_mesh;
}


void RenderAsSkybox( CMeshObjectHandle& mesh, const Vector3& vCamPos )
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;

	hr = pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	hr = pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	hr = pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	hr = pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	hr = pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	hr = pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	hr = pd3dDevice->SetVertexShader( NULL );
	hr = pd3dDevice->SetPixelShader( NULL );

	shared_ptr<CBasicMesh> pMesh = mesh.GetMesh();

	CD3DFixedFunctionPipelineManager ffp_mgr;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
//	const Vector3 vCamPos = g_CameraController.GetPosition();
	matWorld._41 = vCamPos.x;
	matWorld._42 = vCamPos.y;
	matWorld._43 = vCamPos.z;
	ffp_mgr.SetWorldTransform( matWorld );

	if( pMesh )
		pMesh->Render();

	hr = pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	hr = pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}
