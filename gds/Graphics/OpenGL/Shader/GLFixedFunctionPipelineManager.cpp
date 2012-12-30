#include "GLFixedFunctionPipelineManager.hpp"
#include "GLFixedPipelineLightManager.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


CGLFixedFunctionPipelineManager::CGLFixedFunctionPipelineManager()
:
m_matWorld(Matrix44Identity()),
m_matView(Matrix44Identity()),
m_matProjection(Matrix44Identity())
{
	// register the instance to the shader manager hub
//	ShaderManagerHub.RegisterShaderManager( this );

	m_pFFPLightManager.reset( new CGLFixedPipelineLightManager );
}


CGLFixedFunctionPipelineManager::~CGLFixedFunctionPipelineManager()
{
	// release from the list in shader manager hub
//	ShaderManagerHub.ReleaseShaderManager( this );

	Release();
}


void CGLFixedFunctionPipelineManager::Release()
{
}


void CGLFixedFunctionPipelineManager::Reload()
{
}


boost::shared_ptr<CShaderLightManager> CGLFixedFunctionPipelineManager::GetShaderLightManager()
{
	return m_pFFPLightManager;
}


bool CGLFixedFunctionPipelineManager::Init()
{
	return true;
}


bool CGLFixedFunctionPipelineManager::LoadShaderFromFile( const string& filename )
{
	return false;
}


bool CGLFixedFunctionPipelineManager::LoadShaderFromText( const stream_buffer& buffer )
{
	return false;
}


void CGLFixedFunctionPipelineManager::SetParam( CShaderParameter< std::vector<float> >& float_param )
{
}


} // namespace amorphous
