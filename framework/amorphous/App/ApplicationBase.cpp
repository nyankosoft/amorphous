#include "ApplicationBase.hpp"


namespace amorphous
{


ApplicationBase *ApplicationBase::ms_pInstance = nullptr;
std::vector<std::string> ApplicationBase::ms_CommandLineArguments;


} // namespace amorphous
