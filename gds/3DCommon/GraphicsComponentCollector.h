#ifndef  __GRAPHICSCOMPONENTCOLLECTOR_H__
#define  __GRAPHICSCOMPONENTCOLLECTOR_H__


#include <vector>

#include "Support/Singleton.h"
using namespace NS_KGL;

#include <stdlib.h>


//===============================================================================
// CGraphicsParameters
//===============================================================================

class CGraphicsParameters
{
public:

	int ScreenWidth;
	int ScreenHeight;
	bool bWindowed;

public:

	CGraphicsParameters()
		:
	ScreenWidth(800),
	ScreenHeight(600),
	bWindowed(true)
	{}

/*
public:
	int GetScreenWidth() const { return m_iScreenWidth }
	int GetScreenWidth() const { return m_iScreenWidth }
	bool WindowedMode() const { return m_bWindowed };*/
};



//===============================================================================
// CGraphicsComponent
//===============================================================================

class CGraphicsComponent
{
public:

	enum Params
	{
		REFERENCE_SCREEN_WIDTH = 1600
	};

	CGraphicsComponent();
	virtual ~CGraphicsComponent();

	virtual void ReleaseGraphicsResources() = 0;
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam ) = 0;

	const CGraphicsParameters& GetCurrentGraphicsParams() const;

	int GetScreenWidth() const { return GetCurrentGraphicsParams().ScreenWidth; }
	int GetScreenHeight() const { return GetCurrentGraphicsParams().ScreenHeight; }

	/// Returns fixed value
	int GetReferenceScreenWidth() const { return REFERENCE_SCREEN_WIDTH; }

	/// Varies according to aspect ratio
	/// - range: [900,1280]
	int GetReferenceScreenHeight() const { return REFERENCE_SCREEN_WIDTH * GetScreenHeight() / GetScreenWidth(); }
};



//===============================================================================
// CGraphicsComponentCollector
//===============================================================================

class CGraphicsComponentCollector
{
	std::vector<CGraphicsComponent *> m_vecpGraphicsComponent;

	CGraphicsParameters m_GraphicsParam;

public:

	static CSingleton<CGraphicsComponentCollector> m_obj;

//	static CShader* Get() { return m_obj->get(); }
	static CGraphicsComponentCollector* Get() { return m_obj.get(); }

	CGraphicsComponentCollector();
	~CGraphicsComponentCollector();

	void AddComponent( CGraphicsComponent* pComponent );
	bool DeleteComponent( CGraphicsComponent* pComponent );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void SetGraphicsPargams( const CGraphicsParameters& rParam ) { m_GraphicsParam = rParam; }

	const CGraphicsParameters& GetGraphicsParams() const { return m_GraphicsParam; }
};



#endif		/*  __GRAPHICSCOMPONENTCOLLECTOR_H__  */
