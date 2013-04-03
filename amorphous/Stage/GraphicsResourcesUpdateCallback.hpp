#ifndef __GraphicsResourcesUpdateCallback_HPP__
#define __GraphicsResourcesUpdateCallback_HPP__


class CGraphicsResourcesUpdateCallback
{
public:
	CGraphicsResourcesUpdateCallback(){}
	virtual ~CGraphicsResourcesUpdateCallback(){}

	virtual void UpdateGraphics() = 0;
};



template<class T>
class CGraphicsResourcesUpdateDelegate : public CGraphicsResourcesUpdateCallback
{
	T *m_pTarget;

public:

	CGraphicsResourcesUpdateDelegate( T *pTarget = NULL )
		:
	m_pTarget(pTarget)
	{}

	void SetTarget( T *pTarget ) { m_pTarget = pTarget; }

	virtual void UpdateGraphics()
	{
		if( m_pTarget )
			m_pTarget->UpdateGraphics();
	}
};


/*
/// Used when each set of billboards has its own mesh.
/// Not used if the sets of billboards share a mesh.
class CBillboardsUpdateCallback : public CGraphicsResourcesUpdateCallback
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
