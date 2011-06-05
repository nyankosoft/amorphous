#ifndef  __GraphicsComponentCollector_H__
#define  __GraphicsComponentCollector_H__


#include <vector>

#include "fwd.hpp"
#include "Rect.hpp"
using namespace Graphics;

#include "../Support/Singleton.hpp"
using namespace NS_KGL;


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
	bool m_RegisteredToGraphicsResourceManager;

public:

	enum Params
	{
		REFERENCE_SCREEN_WIDTH = 1600
	};

	CGraphicsComponent();
	virtual ~CGraphicsComponent();

	virtual void ReleaseGraphicsResources() = 0;
	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam ) = 0;

	static const CGraphicsParameters& GetCurrentGraphicsParams();

	static int GetScreenWidth() { return GetCurrentGraphicsParams().ScreenWidth; }
	static int GetScreenHeight() { return GetCurrentGraphicsParams().ScreenHeight; }

	static float GetAspectRatio() { return (float)GetScreenWidth() / (float)GetScreenHeight(); }

	/// Returns fixed value
	static int GetReferenceScreenWidth() { return REFERENCE_SCREEN_WIDTH; }

	/// Varies according to aspect ratio
	/// - range: [900,1280]
	static int GetReferenceScreenHeight() { return REFERENCE_SCREEN_WIDTH * GetScreenHeight() / GetScreenWidth(); }

	SRect ReferenceScreenRect() const { return RectLTWH( 0, 0, GetReferenceScreenWidth(), GetReferenceScreenHeight() ); }

	static SRect RectAtCenter( int width, int height )
	{
		return RectCWH( GetReferenceScreenWidth() / 2, GetReferenceScreenHeight() / 2, width, height );
	}


	static SRect RectAtLeftTop( int width, int height, int left_margin, int top_margin )
	{
		return RectLTWH( left_margin, top_margin, width, height );
	}


	static SRect RectAtCenterTop( int width, int height, int top_margin )
	{
		return RectLTWH( ( GetReferenceScreenWidth() - width ) / 2, top_margin, width, height );
	}


	static SRect RectAtRightTop( int width, int height, int right_margin, int top_margin )
	{
		return RectLTWH( GetReferenceScreenWidth() - width - right_margin, top_margin, width, height );
	}


	static SRect RectAtLeftCenter( int width, int height, int left_margin )
	{
		return RectLTWH( left_margin, ( GetReferenceScreenHeight() - height ) / 2, width, height );
	}


	static SRect RectAtRightCenter( int width, int height, int right_margin )
	{
		return RectLTWH( GetReferenceScreenWidth() - width - right_margin, ( GetReferenceScreenHeight() - height ) / 2, width, height );
	}


	static SRect RectAtLeftBottom( int width, int height, int left_margin, int bottom_margin )
	{
		return RectLTWH( left_margin, GetReferenceScreenHeight() - height - bottom_margin, width, height );
	}


	static SRect RectAtCenterBottom( int width, int height, int bottom_margin )
	{
		return RectLTWH( ( GetReferenceScreenWidth() - width ) / 2, GetReferenceScreenHeight() - height - bottom_margin, width, height );
	}


	static SRect RectAtRightBottom( int width, int height, int right_margin, int bottom_margin )
	{
		return RectLTWH( GetReferenceScreenWidth() - width - right_margin, GetReferenceScreenHeight() - height - bottom_margin, width, height );
	}

	friend class CGraphicsComponentCollector;
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
	bool RemoveComponent( CGraphicsComponent* pComponent );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void SetGraphicsPargams( const CGraphicsParameters& rParam ) { m_GraphicsParam = rParam; }

	const CGraphicsParameters& GetGraphicsParams() const { return m_GraphicsParam; }
};





#endif		/*  __GraphicsComponentCollector_H__  */
