#ifndef __GraphicsDemoFactory_HPP__
#define __GraphicsDemoFactory_HPP__


#include "../../_Common/DemoFactory.hpp"
#include <string>


class GraphicsDemoFactory : public DemoFactory
{
	CGraphicsTestBase *CreateDemoInstance( const std::string& demo_name );

public:

	GraphicsDemoFactory(){}
	~GraphicsDemoFactory(){}

	unsigned int GetNumDemos();

	const char **GetDemoNames();

	CGraphicsTestBase *CreateDemoInstance( unsigned int index );
};


#endif /* __GraphicsDemoFactory_HPP__ */
