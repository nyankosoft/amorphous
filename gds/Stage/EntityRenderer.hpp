#ifndef __EntityRenderer_HPP__
#define __EntityRenderer_HPP__


#include "fwd.hpp"


class CEntityRenderer
{
public:
	CEntityRenderer() {}
	virtual ~CEntityRenderer() {}

	virtual bool ShouldRenderEntity( CCopyEntity& entity ) { return true; }

	virtual void RenderEntity( CCopyEntity& entity ) = 0;
};



#endif /* __EntityRenderer_HPP__ */
