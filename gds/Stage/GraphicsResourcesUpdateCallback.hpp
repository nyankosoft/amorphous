#ifndef __GraphicsResourcesUpdateCallback_HPP__
#define __GraphicsResourcesUpdateCallback_HPP__


class CGraphicsResourcesUpdateCallback
{
public:
	CGraphicsResourcesUpdateCallback(){}
	virtual ~CGraphicsResourcesUpdateCallback(){}

	virtual void UpdateGraphics() = 0;
};


/*
/// Used when each set of billboards has its own mesh.
/// Not used if the sets of billboards share a mesh.
class CBillboardsUpdateCallback : class CGraphicsResourcesUpdateCallback
{
public:

	CBillboardsUpdateCallback(){}
	virtual ~CBillboardsUpdateCallback(){}

	void UpdateGraphics()
	{
	}
}
*/



#endif /* __GraphicsResourcesUpdateCallback_HPP__ */
