#include "ClothTest.hpp"
#include "ConvexTest.hpp"
#include "MultiShapeActorsTest.hpp"
#include "amorphous/Support/ParamLoader.hpp"


static const char *sg_demos[] =
{
	"ClothTest",
	"ConvexTest",
	"MultiShapeActorsTest"
};


unsigned int GetNumDemos()
{
	return sizeof(sg_demos) / sizeof(sg_demos[0]);
}


const char **GetDemoNames()
{
	return sg_demos;
}


CGraphicsTestBase *CreateTestInstance( const std::string& demo_name )
{
	if( demo_name == "" )
		return NULL;
	else if( demo_name == "ClothTest" )            return new CClothTest;
	else if( demo_name == "ConvexTest" )           return new CConvexTest;
	else if( demo_name == "MultiShapeActorsTest" ) return new CMultiShapeActorsTest;
	else
		return NULL;
}


CGraphicsTestBase *CreateDemoInstance( unsigned int index )
{
	return CreateTestInstance( GetDemoNames()[index] );
}
