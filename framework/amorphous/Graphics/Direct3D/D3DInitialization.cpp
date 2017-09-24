#include "D3DInitialization.hpp"
#include "2DPrimitive/2DPrimitiveRenderer_D3D.hpp"
#include "2DPrimitive/2DRectSetImpl_D3D.hpp"
#include "PrimitiveRenderer_D3D.hpp"
#include "D3DGraphicsResources.hpp"
#include "D3DTextureRenderTarget.hpp"
#include "D3DCubeTextureRenderTarget.hpp"
#include "Mesh/D3DXSMeshObject.hpp" // Used by CD3DMeshImplFactory
#include "Mesh/D3DCustomMeshRenderer.hpp"
#include "Shader/D3DFixedFunctionPipelineManager.hpp"
#include "Shader/Embedded/EmbeddedGenericHLSL.hpp"
#include "Shader/Embedded/EmbeddedPostProcessEffectHLSLShader.hpp"
#include "../GraphicsResources.hpp"
#include "../GraphicsResourceCacheManager.hpp"
#include "../Mesh/CustomMesh.hpp"
#include "../Mesh/MeshFactory.hpp"
#include "../Shader/FixedFunctionPipelineManager.hpp"


namespace amorphous
{


using std::shared_ptr;


//=======================================================================================
// C2DPrimitiveFactoryImpl_D3D
//=======================================================================================

class C2DPrimitiveFactoryImpl_D3D : public C2DPrimitiveFactoryImpl
{
public:

	std::shared_ptr<C2DRectSetImpl> Create2DRectSetImpl()
	{
		std::shared_ptr<C2DRectSetImpl> pImpl( new C2DRectSetImpl_D3D );
		return pImpl;
	}
};



//=======================================================================================
// CD3DGraphicsResourceFactoryImpl
//=======================================================================================

class CD3DGraphicsResourceFactoryImpl : public GraphicsResourceFactoryImpl
{
public:

//	CreateGraphicsResource( GraphicsResourceDesc &desc );

	std::shared_ptr<TextureResource> CreateTextureResource( const TextureResourceDesc& desc );
	std::shared_ptr<MeshResource>    CreateMeshResource( const MeshResourceDesc& desc );
	std::shared_ptr<ShaderResource>  CreateShaderResource( const ShaderResourceDesc& desc );
};


//=======================================================================================
// CD3DMeshImplFactory
//=======================================================================================

class CD3DMeshImplFactory : public MeshImplFactory
{
public:

//	MeshImpl* CreateMeshImpl( MeshType::Name mesh_type )
//	{
//		switch( mesh_type )
//		{
//		case MeshType::BASIC:
//			return new CD3DXMeshObject();
//		case MeshType::PROGRESSIVE:
//			return new CD3DXPMeshObject();
//		case MeshType::SKELETAL:
//			return new CD3DXSMeshObject();
//		default:
//			return NULL;
//		}
//
//		return NULL;
//	}

	MeshImpl* CreateBasicMeshImpl()       { return new CD3DXMeshObject(); }
	MeshImpl* CreateProgressiveMeshImpl() { return new CD3DXPMeshObject(); }
	MeshImpl* CreateSkeletalMeshImpl()    { return new CD3DXSMeshObject(); }
};


shared_ptr<TextureResource> CD3DGraphicsResourceFactoryImpl::CreateTextureResource( const TextureResourceDesc& desc )
{
//	return shared_ptr<CD3DTextureResource>( new CD3DTextureResource(&desc) );

	shared_ptr<CD3DTextureResource> pTexResource;

	if( desc.TypeFlags & TextureTypeFlag::CUBE_MAP )
		pTexResource.reset( new CD3DCubeTextureResource(&desc) );
	else
		pTexResource.reset( new CD3DTextureResource(&desc) );

	return pTexResource;
}

shared_ptr<MeshResource> CD3DGraphicsResourceFactoryImpl::CreateMeshResource( const MeshResourceDesc& desc )
{
	return shared_ptr<MeshResource>( new MeshResource(&desc) );
}

shared_ptr<ShaderResource> CD3DGraphicsResourceFactoryImpl::CreateShaderResource( const ShaderResourceDesc& desc )
{
	return shared_ptr<ShaderResource>( new CD3DShaderResource(&desc) );
}


Result::Name InitializeDirect3DClasses()
{
	CGraphicsDeviceHolder::Get()->SetDevice( &DIRECT3D9 );
	Ref2DPrimitiveRendererPtr() = &PrimitiveRenderer_D3D();
	RefPrimitiveRendererPtr() = &GetPrimitiveRenderer_D3D();
	GetGraphicsResourceFactory().Init( new CD3DGraphicsResourceFactoryImpl() );
	Ref2DPrimitiveFactory().Init( new C2DPrimitiveFactoryImpl_D3D );
	GetMeshImplFactory().reset( new CD3DMeshImplFactory );
	CFixedFunctionPipelineManagerHolder::Get()->Init( &D3DFixedFunctionPipelineManager() );
	TextureRenderTarget::SetInstanceCreationFunction( CD3DTextureRenderTarget::Create );
	CubeTextureRenderTarget::SetInstanceCreationFunction( D3DCubeTextureRenderTarget::Create );
	CustomMeshRenderer::ms_pInstance = &(CD3DCustomMeshRenderer::ms_Instance);
	std::shared_ptr<EmbeddedGenericShader> pEmbeddedGenericHLSL( new EmbeddedGenericHLSL );
	SetEmbeddedGenericShader( pEmbeddedGenericHLSL );
	GetEmbeddedPostProcessEffectShader().reset( new CEmbeddedPostProcessEffectHLSLShader );

	// For Direct3D, Use ARGB32 as the default vertex diffuse color format to support
	// fixed function pipeline (FFP). On some graphics cards, the application crashes
	// when floating point RGBA is used for vertex diffuse color in the FFP mode.
	CustomMesh::SetDefaultVertexDiffuseColorFormat( CustomMesh::VCF_ARGB32 );

	return Result::SUCCESS;
}


} // namespace amorphous
