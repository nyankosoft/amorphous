#ifndef  __GRAPHICSCOMPONENTCOLLECTOR_H__
#define  __GRAPHICSCOMPONENTCOLLECTOR_H__


#include <vector>

#include "Graphics/Rect.h"
using namespace Graphics;

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

	SRect ReferenceScreenRect() const { return RectLTWH( 0, 0, GetReferenceScreenWidth(), GetReferenceScreenHeight() ); }

	SRect RectAtCenter( int width, int height )
	{
		return RectCWH( GetReferenceScreenWidth() / 2, GetReferenceScreenHeight() / 2, width, height );
	}


	SRect RectAtLeftTop( int width, int height, int left_margin, int top_margin )
	{
		return RectLTWH( left_margin, top_margin, width, height );
	}


	SRect RectAtCenterTop( int width, int height, int top_margin )
	{
		return RectLTWH( ( GetReferenceScreenWidth() - width ) / 2, top_margin, width, height );
	}


	SRect RectAtRightTop( int width, int height, int right_margin, int top_margin )
	{
		return RectLTWH( GetReferenceScreenWidth() - width - right_margin, top_margin, width, height );
	}


	SRect RectAtLeftCenter( int width, int height, int left_margin )
	{
		return RectLTWH( left_margin, ( GetReferenceScreenHeight() - height ) / 2, width, height );
	}


	SRect RectAtRightCenter( int width, int height, int right_margin )
	{
		return RectLTWH( GetReferenceScreenWidth() - width - right_margin, ( GetReferenceScreenHeight() - height ) / 2, width, height );
	}


	SRect RectAtLeftBottom( int width, int height, int left_margin, int bottom_margin )
	{
		return RectLTWH( left_margin, GetReferenceScreenHeight() - height - bottom_margin, width, height );
	}


	SRect RectAtCenterBottom( int width, int height, int bottom_margin )
	{
		return RectLTWH( ( GetReferenceScreenWidth() - width ) / 2, GetReferenceScreenHeight() - height - bottom_margin, width, height );
	}


	SRect RectAtRightBottom( int width, int height, int right_margin, int bottom_margin )
	{
		return RectLTWH( GetReferenceScreenWidth() - width - right_margin, GetReferenceScreenHeight() - height - bottom_margin, width, height );
	}
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
