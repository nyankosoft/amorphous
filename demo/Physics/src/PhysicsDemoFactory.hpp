#ifndef __PhysicsDemoFactory_HPP__
#define __PhysicsDemoFactory_HPP__


#include "../../_Common/DemoFactory.hpp"
#include <string>


class PhysicsDemoFactory : public DemoFactory
{
	CGraphicsTestBase *CreateDemoInstance( const std::string& demo_name );

public:

	PhysicsDemoFactory(){}
	~PhysicsDemoFactory(){}

	unsigned int GetNumDemos();

	const char **GetDemoNames();

	CGraphicsTestBase *CreateDemoInstance( unsigned int index );
};


#endif /* __PhysicsDemoFactory_HPP__ */
