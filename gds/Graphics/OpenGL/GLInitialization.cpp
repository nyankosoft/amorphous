#include "GLInitialization.hpp"
#include "2DPrimitive/2DPrimitiveRenderer_GL.hpp"
#include "2DPrimitive/2DRectSetImpl_GL.hpp"
#include "PrimitiveRenderer_GL.hpp"
#include "GLGraphicsResources.hpp"
#include "GLTextureRenderTarget.hpp"
#include "Mesh/GLBasicMeshImpl.hpp"
#include "Mesh/GLCustomMeshRenderer.hpp"
#include "Shader/GLFixedFunctionPipelineManager.hpp"
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

class CGLGraphicsResourceFactoryImpl : public CGraphicsResourceFactoryImpl
{
public:

//	CreateGraphicsResource( CGraphicsResourceDesc &desc );

	boost::shared_ptr<CTextureResource> CreateTextureResource( const CTextureResourceDesc& desc );
	boost::shared_ptr<CMeshResource>    CreateMeshResource( const CMeshResourceDesc& desc );
	boost::shared_ptr<CShaderResource>  CreateShaderResource( const CShaderResourceDesc& desc );
};


shared_ptr<CTextureResource> CGLGraphicsResourceFactoryImpl::CreateTextureResource( const CTextureResourceDesc& desc )
{
	return shared_ptr<CGLTextureResource>( new CGLTextureResource(&desc) );
}

shared_ptr<CMeshResource> CGLGraphicsResourceFactoryImpl::CreateMeshResource( const CMeshResourceDesc& desc )
{
	return shared_ptr<CMeshResource>( new CMeshResource(&desc) );
}

shared_ptr<CShaderResource> CGLGraphicsResourceFactoryImpl::CreateShaderResource( const CShaderResourceDesc& desc )
{
	return shared_ptr<CShaderResource>( new CGLShaderResource(&desc) );
}


Result::Name InitializeOpenGLClasses()
{
	PrimitiveRendererPtr() = &PrimitiveRenderer_GL();
	RefPrimitiveRendererPtr() = &GetPrimitiveRenderer_GL();
	GraphicsResourceFactory().Init( new CGLGraphicsResourceFactoryImpl() );
	Ref2DPrimitiveFactory().Init( new C2DPrimitiveFactoryImpl_GL );
	GetMeshImplFactory().reset( new CGLMeshImplFactory );
	CFixedFunctionPipelineManagerHolder::Get()->Init( &GLFixedFunctionPipelineManager() );
	CTextureRenderTarget::SetInstanceCreationFunction( CGLTextureRenderTarget::Create );
	CustomMeshRenderer::ms_pInstance = &(CGLCustomMeshRenderer::ms_Instance);
	CustomMesh::SetDefaultVertexDiffuseColorFormat( CustomMesh::VCF_FRGBA );

	return Result::SUCCESS;
}


} // namespace amorphous
