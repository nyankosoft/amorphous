#include "PhysicsDemoFactory.hpp"
#include "ClothTest.hpp"
#include "ConvexTest.hpp"
#include "MultiShapeActorsTest.hpp"
#include "amorphous/Support/ParamLoader.hpp"


DemoFactory *CreateDemoFactory() { return new PhysicsDemoFactory; }


static const char *sg_demos[] =
{
	"ClothTest",
	"ConvexTest",
	"MultiShapeActorsTest"
};


unsigned int PhysicsDemoFactory::GetNumDemos()
{
	return sizeof(sg_demos) / sizeof(sg_demos[0]);
}


const char **PhysicsDemoFactory::GetDemoNames()
{
	return sg_demos;
}


CGraphicsTestBase *PhysicsDemoFactory::CreateDemoInstance( const std::string& demo_name )
{
	if( demo_name == "" )
		return NULL;
	else if( demo_name == "ClothTest" )            return new CClothTest;
	else if( demo_name == "ConvexTest" )           return new CConvexTest;
	else if( demo_name == "MultiShapeActorsTest" ) return new CMultiShapeActorsTest;
	else
		return NULL;
}


CGraphicsTestBase *PhysicsDemoFactory::CreateDemoInstance( unsigned int index )
{
	return CreateDemoInstance( GetDemoNames()[index] );
}
