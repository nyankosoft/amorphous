#include "GLInitialization.hpp"
#include "2DPrimitive/2DPrimitiveRenderer_GL.hpp"
#include "2DPrimitive/2DRectSetImpl_GL.hpp"
#include "GLGraphicsDevice.hpp"
#include "PrimitiveRenderer_GL.hpp"
#include "GLGraphicsResources.hpp"
#include "GLTextureRenderTarget.hpp"
#include "Mesh/GL2BasicMeshImpl.hpp"
#include "Mesh/GLCustomMeshRenderer.hpp"
#include "Shader/GLFixedFunctionPipelineManager.hpp"
#include "Shader/Embedded/EmbeddedGenericGLSLShader.hpp"
#include "../GraphicsResources.hpp"
#include "../GraphicsResourceCacheManager.hpp"
#include "../Mesh/CustomMesh.hpp"
#include "../Shader/FixedFunctionPipelineManager.hpp"


namespace amorphous
{


using boost::shared_ptr;


//=======================================================================================
// C2DPrimitiveFactoryImpl_GL
//=======================================================================================

class C2DPrimitiveFactoryImpl_GL : public C2DPrimitiveFactoryImpl
{
public:

	boost::shared_ptr<C2DRectSetImpl> Create2DRectSetImpl()
	{
		boost::shared_ptr<C2DRectSetImpl> pImpl( new C2DRectSetImpl_GL );
		return pImpl;
	}
};



//=======================================================================================
// CGLGraphicsResourceFactoryImpl
//=======================================================================================

class CGLGraphicsResourceFactoryImpl : public GraphicsResourceFactoryImpl
{
public:

//	CreateGraphicsResource( GraphicsResourceDesc &desc );

	boost::shared_ptr<TextureResource> CreateTextureResource( const TextureResourceDesc& desc );
	boost::shared_ptr<MeshResource>    CreateMeshResource( const MeshResourceDesc& desc );
	boost::shared_ptr<ShaderResource>  CreateShaderResource( const ShaderResourceDesc& desc );
};


shared_ptr<TextureResource> CGLGraphicsResourceFactoryImpl::CreateTextureResource( const TextureResourceDesc& desc )
{
	shared_ptr<GLTextureResourceBase> pTexResource;

	if( desc.TypeFlags & TextureTypeFlag::CUBE_MAP )
		pTexResource.reset( new CGLCubeTextureResource(&desc) );
	else
		pTexResource.reset( new CGLTextureResource(&desc) );

	return pTexResource;
}

shared_ptr<MeshResource> CGLGraphicsResourceFactoryImpl::CreateMeshResource( const MeshResourceDesc& desc )
{
	return shared_ptr<MeshResource>( new MeshResource(&desc) );
}

shared_ptr<ShaderResource> CGLGraphicsResourceFactoryImpl::CreateShaderResource( const ShaderResourceDesc& desc )
{
	return shared_ptr<ShaderResource>( new CGLShaderResource(&desc) );
}


Result::Name InitializeOpenGLClasses()
{
	CGraphicsDeviceHolder::Get()->SetDevice( &GLGraphicsDevice() );
	Ref2DPrimitiveRendererPtr() = &PrimitiveRenderer_GL();
	RefPrimitiveRendererPtr() = &GetPrimitiveRenderer_GL();
	GetGraphicsResourceFactory().Init( new CGLGraphicsResourceFactoryImpl() );
	Ref2DPrimitiveFactory().Init( new C2DPrimitiveFactoryImpl_GL );
	GetMeshImplFactory().reset( new CGLMeshImplFactory );
	CFixedFunctionPipelineManagerHolder::Get()->Init( &GLFixedFunctionPipelineManager() );
	TextureRenderTarget::SetInstanceCreationFunction( CGLTextureRenderTarget::Create );
	CustomMeshRenderer::ms_pInstance = &(CGLCustomMeshRenderer::ms_Instance);
	CustomMesh::SetDefaultVertexDiffuseColorFormat( CustomMesh::VCF_FRGBA );
	boost::shared_ptr<EmbeddedGenericShader> pEmbeddedGenericGLSLShader( new EmbeddedGenericGLSLShader );
	SetEmbeddedGenericShader( pEmbeddedGenericGLSLShader );

	return Result::SUCCESS;
}


} // namespace amorphous
