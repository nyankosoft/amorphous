#include "ClothTest.hpp"
#include "ConvexTest.hpp"
#include "MultiShapeActorsTest.hpp"
#include "amorphous/Support/ParamLoader.hpp"


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


CGraphicsTestBase *CreateTestInstance()
{
	std::string demo_name;
	LoadParamFromFile( "params.txt", "demo", demo_name );
	return CreateTestInstance( demo_name );
}
