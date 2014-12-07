#ifndef __DemoFactory_HPP__
#define __DemoFactory_HPP__


class CGraphicsTestBase;


class DemoFactory
{
public:
	DemoFactory(){}
	virtual ~DemoFactory(){}

	virtual unsigned int GetNumDemos() = 0;

	virtual const char **GetDemoNames() = 0;

	virtual CGraphicsTestBase *CreateDemoInstance( unsigned int index ) = 0;
};


DemoFactory *CreateDemoFactory();


#endif /* __DemoFactory_HPP__ */
