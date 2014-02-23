#include "ShaderManager.hpp"
#include "ShaderManagerHub.hpp"

#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


ShaderManager::ShaderManager()
{
	// register the instance to the shader manager hub
	GetShaderManagerHub().RegisterShaderManager( this );

	m_RegisteredToHub = true;
}


ShaderManager::~ShaderManager()
{
	// release from the list in shader manager hub
	GetShaderManagerHub().ReleaseShaderManager( this );

	m_RegisteredToHub = false;
}


} // namespace amorphous
