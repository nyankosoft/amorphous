#include "ShaderManager.hpp"
#include "ShaderManagerHub.hpp"

#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


CShaderManager::CShaderManager()
{
	// register the instance to the shader manager hub
	ShaderManagerHub.RegisterShaderManager( this );

	m_RegisteredToHub = true;
}


CShaderManager::~CShaderManager()
{
	// release from the list in shader manager hub
	ShaderManagerHub.ReleaseShaderManager( this );

	m_RegisteredToHub = false;
}


} // namespace amorphous
