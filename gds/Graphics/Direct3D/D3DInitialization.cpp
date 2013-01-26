#include "D3DInitialization.hpp"
#include "2DPrimitive/2DPrimitiveRenderer_D3D.hpp"
#include "2DPrimitive/2DRectSetImpl_D3D.hpp"
#include "PrimitiveRenderer_D3D.hpp"
#include "D3DGraphicsResources.hpp"
#include "D3DTextureRenderTarget.hpp"
#include "Mesh/D3DXMeshObjectBase.hpp"
#include "Mesh/D3DCustomMeshRenderer.hpp"
#include "Shader/D3DFixedFunctionPipelineManager.hpp"
#include "../GraphicsResources.hpp"
#include "../GraphicsResourceCacheManager.hpp"
#include "../Mesh/CustomMesh.hpp"
#include "../Shader/FixedFunctionPipelineManager.hpp"


namespace amorphous
{


using boost::shared_ptr;


//=======================================================================================
// C2DPrimitiveFactoryImpl_D3D
//=======================================================================================

class C2DPrimitiveFactoryImpl_D3D : public C2DPrimitiveFactoryImpl
{
public:

	boost::shared_ptr<C2DRectSetImpl> Create2DRectSetImpl()
	{
		boost::shared_ptr<C2DRectSetImpl> pImpl( new C2DRectSetImpl_D3D );
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

	boost::shared_ptr<TextureResource> CreateTextureResource( const TextureResourceDesc& desc );
	boost::shared_ptr<MeshResource>    CreateMeshResource( const MeshResourceDesc& desc );
	boost::shared_ptr<ShaderResource>  CreateShaderResource( const ShaderResourceDesc& desc );
};


shared_ptr<TextureResource> CD3DGraphicsResourceFactoryImpl::CreateTextureResource( const TextureResourceDesc& desc )
{
	return shared_ptr<CD3DTextureResource>( new CD3DTextureResource(&desc) );
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
	PrimitiveRendererPtr() = &PrimitiveRenderer_D3D();
	RefPrimitiveRendererPtr() = &GetPrimitiveRenderer_D3D();
	GetGraphicsResourceFactory().Init( new CD3DGraphicsResourceFactoryImpl() );
	Ref2DPrimitiveFactory().Init( new C2DPrimitiveFactoryImpl_D3D );
	GetMeshImplFactory().reset( new CD3DMeshImplFactory );
	CFixedFunctionPipelineManagerHolder::Get()->Init( &D3DFixedFunctionPipelineManager() );
	CTextureRenderTarget::SetInstanceCreationFunction( CD3DTextureRenderTarget::Create );
	CustomMeshRenderer::ms_pInstance = &(CD3DCustomMeshRenderer::ms_Instance);

	// For Direct3D, Use ARGB32 as the default vertex diffuse color format to support
	// fixed function pipeline (FFP). On some graphics cards, the application crashes
	// when floating point RGBA is used for vertex diffuse color in the FFP mode.
	CustomMesh::SetDefaultVertexDiffuseColorFormat( CustomMesh::VCF_ARGB32 );

	return Result::SUCCESS;
}


} // namespace amorphous
