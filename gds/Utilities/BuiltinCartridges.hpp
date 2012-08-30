#ifndef __BuiltinCartridges_HPP__
#define __BuiltinCartridges_HPP__


#include "CartridgeMaker.hpp"
#include "../GameCommon/Caliber.hpp"
#include <utility>


namespace firearm
{


//class CartridgeIandDescPair
//{
//public:
//	const Caliber::Name caliber;
//	const char *name;
//	CartridgeDesc desc;
//};
//
//
//const CartridgeDesc[] =
//{
//	{ Caliber::_9MM, "FMJ 9mm", { BulletDesc(), } },
//};


bool GetFMJBulletDesc( Caliber::Name caliber, BulletDesc& dest );


bool GetCaseDesc( Caliber::Name caliber, CaseDesc& dest );


} // firearm


#endif /* BuiltinCartridges */
