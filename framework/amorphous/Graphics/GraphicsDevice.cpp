#include "GraphicsDevice.hpp"


namespace amorphous
{


singleton<CGraphicsDeviceHolder> CGraphicsDeviceHolder::m_obj;


// TODO: make this a member of CGraphicsDevice
bool g_IsReadyToLoadGraphicsResources = false;


} // namespace amorphous
