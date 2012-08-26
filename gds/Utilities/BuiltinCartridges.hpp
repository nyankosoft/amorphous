#ifndef __BuiltinCartridges_HPP__
#define __BuiltinCartridges_HPP__


#include "CartridgeMaker.hpp"
#include "../GameCommon/Caliber.hpp"
#include <utility>


namespace firearm
{


bool GetCaseDesc( Caliber::Name caliber, CaseDesc& dest );


} // firearm


#endif /* BuiltinCartridges */
