#include "GLInitialization.hpp"
#include "2DPrimitive/2DPrimitiveRenderer_GL.hpp"
#include "2DPrimitive/2DRectSetImpl_GL.hpp"
#include "GLGraphicsDevice.hpp"
#include "PrimitiveRenderer_GL.hpp"
#include "GLGraphicsResources.hpp"
#include "GLTextureRenderTarget.hpp"
#include "GLCubeTextureRenderTarget.hpp"
#include "Mesh/GLSkeletalMeshImpl.hpp"	  // Used by GLMeshImplFactory
#include "Mesh/GLCustomMeshRenderer.hpp"
#include "Shader/GLFixedFunctionPipelineManager.hpp"
#include "Shader/Embedded/EmbeddedGenericGLSLShader.hpp"
#include "Shader/Embedded/EmbeddedPostProcessEffectGLSLShader.hpp"
#include "amorphous/Graphics/GraphicsResources.hpp"
#include "amorphous/Graphics/GraphicsResourceCacheManager.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"
#include "amorphous/Graphics/Mesh/MeshFactory.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"


namespace amorphous
{


using std::shared_ptr;


//=======================================================================================
// C2DPrimitiveFactoryImpl_GL
//=======================================================================================

class C2DPrimitiveFactoryImpl_GL : public C2DPrimitiveFactoryImpl
{
public:

	std::shared_ptr<C2DRectSetImpl> Create2DRectSetImpl()
	{
		std::shared_ptr<C2DRectSetImpl> pImpl( new C2DRectSetImpl_GL );
		return pImpl;
	}
};



//=======================================================================================
// GLGraphicsResourceFactoryImpl
//=======================================================================================

class GLGraphicsResourceFactoryImpl : public GraphicsResourceFactoryImpl
{
public:

//	CreateGraphicsResource( GraphicsResourceDesc &desc );

	std::shared_ptr<TextureResource> CreateTextureResource( const TextureResourceDesc& desc );
	std::shared_ptr<MeshResource>    CreateMeshResource( const MeshResourceDesc& desc );
	std::shared_ptr<ShaderResource>  CreateShaderResource( const ShaderResourceDesc& desc );
};


//=======================================================================================
// GLGraphicsResourceFactoryImpl
//=======================================================================================

class GLMeshImplFactory : public MeshImplFactory
{
public:

//	MeshImpl* CreateBasicMeshImpl()       { return new CGL2BasicMeshImpl; }
	MeshImpl* CreateBasicMeshImpl()       { return new CustomMesh; }
	MeshImpl* CreateProgressiveMeshImpl() { return new GLProgressiveMeshImpl; }
	MeshImpl* CreateSkeletalMeshImpl()    { return new GLSkeletalMeshImpl; }
};


shared_ptr<TextureResource> GLGraphicsResourceFactoryImpl::CreateTextureResource( const TextureResourceDesc& desc )
{
	shared_ptr<GLTextureResourceBase> pTexResource;

	if( desc.TypeFlags & TextureTypeFlag::CUBE_MAP )
		pTexResource.reset( new CGLCubeTextureResource(&desc) );
	else
		pTexResource.reset( new CGLTextureResource(&desc) );

	return pTexResource;
}

shared_ptr<MeshResource> GLGraphicsResourceFactoryImpl::CreateMeshResource( const MeshResourceDesc& desc )
{
	return shared_ptr<MeshResource>( new MeshResource(&desc) );
}

shared_ptr<ShaderResource> GLGraphicsResourceFactoryImpl::CreateShaderResource( const ShaderResourceDesc& desc )
{
	return shared_ptr<ShaderResource>( new CGLShaderResource(&desc) );
}


Result::Name InitializeOpenGLClasses()
{
	CGraphicsDeviceHolder::Get()->SetDevice( &GLGraphicsDevice() );
	Ref2DPrimitiveRendererPtr() = &PrimitiveRenderer_GL();
	RefPrimitiveRendererPtr() = &GetPrimitiveRenderer_GL();
	GetGraphicsResourceFactory().Init( new GLGraphicsResourceFactoryImpl() );
	Ref2DPrimitiveFactory().Init( new C2DPrimitiveFactoryImpl_GL );
	GetMeshImplFactory().reset( new GLMeshImplFactory );
	CFixedFunctionPipelineManagerHolder::Get()->Init( &GLFixedFunctionPipelineManager() );
	TextureRenderTarget::SetInstanceCreationFunction( GLTextureRenderTarget::Create );
	CubeTextureRenderTarget::SetInstanceCreationFunction( GLCubeTextureRenderTarget::Create );
	CustomMeshRenderer::ms_pInstance = &(GLCustomMeshRenderer::ms_Instance);
	CustomMesh::SetDefaultVertexDiffuseColorFormat( CustomMesh::VCF_FRGBA );
	std::shared_ptr<EmbeddedGenericShader> pEmbeddedGenericGLSLShader( new EmbeddedGenericGLSLShader );
	SetEmbeddedGenericShader( pEmbeddedGenericGLSLShader );
	GetEmbeddedPostProcessEffectShader().reset( new EmbeddedPostProcessEffectGLSLShader );

	return Result::SUCCESS;
}


} // namespace amorphous
