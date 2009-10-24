#ifndef  __2DRECTSET_H__
#define  __2DRECTSET_H__


#include "../../base.hpp"
#include "3DMath/Vector2.hpp"
#include "3DMath/Vector3.hpp"
#include "Graphics/FVF_TLVertex.h"
#include "Graphics/RectTriListIndex.hpp"
#include "Graphics/2DPrimitive/2DPrimitive.hpp"
#include "Graphics/2DPrimitive/2DRectSet.hpp"
#include "Graphics/TextureHandle.hpp"


//=============================================================================
// C2DRectSetImpl
//=============================================================================

class C2DRectSetImpl
{
public:

	/// position

	virtual void SetRectMinMax( int rect_index, const Vector2& vMin, const Vector2& vMax ) = 0;

//	virtual void SetRectMinMax( int rect_index, const Vector3& vMin, const Vector3& vMax ) = 0;

	virtual void SetRectMinMax( int rect_index, float min_x, float min_y, float max_x, float max_y ) = 0;

	virtual void SetRectVertexPosition( int rect_index, int vert_index, const Vector2& vPos ) = 0;

	virtual void SetRectVertexPosition( int rect_index, int vert_index, const Vector3& vPos ) = 0;

	virtual void SetRectVertexPosition( int rect_index, int vert_index, const float x, const float y ) = 0;

	virtual void SetVertexPosition( int vert_index, const float x, const float y ) = 0;


	/// texture coord

	virtual void SetTextureCoordMinMax( int rect_index, float u_min, float v_min, float u_max, float v_max ) = 0;

	virtual void SetTextureCoordMinMax( int rect_index, const TEXCOORD2& vMin, const TEXCOORD2& vMax ) = 0;

	virtual void SetRectVertexTextureCoord( int rect_index, int vert_index, float u, float v ) = 0;


	/// color

	virtual void SetColor( U32 color ) = 0;

	virtual void SetRectColor( int rect_index, U32 color ) = 0;

	virtual void SetRectVertexColor( int rect_index, int vert_index, const SFloatRGBAColor& color ) = 0;


	/// FVF vertex

	//	 virtual void SetRectVertex( int rect_index, int vert_index, const TLVERTEX& vSrc ) = 0;

	//	 virtual void SetVertex( int vert_index, const TLVERTEX& vSrc ) = 0;


	virtual int GetNumRects() = 0;

	virtual void SetNumRects( int num_rects ) = 0;

	virtual void AddRects( int num_rects ) = 0;

	virtual void ClearRects() = 0;

//	virtual int unsigned GetPrimitiveType() const { return C2DPrimitive::TYPE_RECTSET; }

	 virtual void draw( int start_rect_index, int num_rects ) = 0;

	/// renders all the rects with the current render states
//	 virtual void draw() { draw( 0, (int)m_vecRectVertex.size() / 4 ) = 0; }

	 virtual void Draw( int start_rect_index, int num_rects ) = 0;

	/// renders all the rects
//	 virtual void Draw() { Draw( 0, (int)m_vecRectVertex.size() / 4 ) = 0; }

	 virtual void Draw( int start_rect_index, int num_rects, const CTextureHandle& texture ) = 0;

//	 virtual void Draw( const CTextureHandle& texture ) { Draw( 0, (int)m_vecRectVertex.size() / 4, texture ); }
};



//=============================================================================
// C2DRectSet
//=============================================================================

class C2DRectSet : public C2DPrimitive
{
	boost::shared_ptr<C2DRectSetImpl> m_pImpl;

public:

	C2DRectSet();

	/// position

	inline void SetRectMinMax( int rect_index, const Vector2& vMin, const Vector2& vMax );

	inline void SetRectMinMax( int rect_index, const Vector3& vMin, const Vector3& vMax );

	inline void SetRectMinMax( int rect_index, float min_x, float min_y, float max_x, float max_y );

	inline void SetRectVertexPosition( int rect_index, int vert_index, const Vector2& vPos );

	inline void SetRectVertexPosition( int rect_index, int vert_index, const Vector3& vPos );

	inline void SetRectVertexPosition( int rect_index, int vert_index, const float x, const float y );

	inline void SetVertexPosition( int vert_index, const float x, const float y );


	/// texture coord

	inline void SetTextureCoordMinMax( int rect_index, float u_min, float v_min, float u_max, float v_max );

	inline void SetTextureCoordMinMax( int rect_index, const TEXCOORD2& vMin, const TEXCOORD2& vMax );

	inline void SetRectVertexTextureCoord( int rect_index, int vert_index, float u, float v );


	/// color

	inline void SetColor( U32 color );

	inline void SetRectColor( int rect_index, U32 color );

	inline void SetRectVertexColor( int rect_index, int vert_index, const SFloatRGBAColor& color );


	/// FVF vertex

	inline void SetRectVertex( int rect_index, int vert_index, const TLVERTEX& vSrc );

	inline void SetVertex( int vert_index, const TLVERTEX& vSrc );


	inline int GetNumRects() { return m_pImpl->GetNumRects(); }

	inline void SetNumRects( int num_rects );

	inline void AddRects( int num_rects );

	inline void ClearRects() { m_pImpl->ClearRects(); }

	virtual int unsigned GetPrimitiveType() const { return C2DPrimitive::TYPE_RECTSET; }

	inline void draw( int start_rect_index, int num_rects ) { m_pImpl->draw( start_rect_index, num_rects ); }

	/// renders all the rects with the current render states
	inline void draw() { m_pImpl->draw( 0, GetNumRects() ); }

	inline void Draw( int start_rect_index, int num_rects );

	/// renders all the rects
	inline void Draw() { SetBasicRenderStates(); m_pImpl->Draw( 0, GetNumRects() ); }

	inline void Draw( int start_rect_index, int num_rects, const CTextureHandle& texture );

	inline void Draw( const CTextureHandle& texture ) { m_pImpl->Draw( 0, GetNumRects(), texture ); }
};


//================================/ inline implementations /================================//


inline void C2DRectSet::SetRectMinMax( int rect_index,
									   float min_x, float min_y,
									   float max_x, float max_y )
{
	m_pImpl->SetRectMinMax( rect_index, min_x, min_y, max_x, max_y );
}


inline void C2DRectSet::SetRectMinMax( int rect_index,
									   const Vector2& vMin,
									   const Vector2& vMax )
{
	SetRectMinMax( rect_index, vMin.x, vMin.y, vMax.x, vMax.y );
}


inline void C2DRectSet::SetRectVertexPosition( int rect_index, int vert_index, const Vector2& vPos )
{
	m_pImpl->SetRectVertexPosition( rect_index, vert_index, vPos );
}


inline void C2DRectSet::SetRectVertexPosition( int rect_index, int vert_index, const Vector3& vPos )
{
	m_pImpl->SetRectVertexPosition( rect_index, vert_index, vPos );
}


inline void C2DRectSet::SetRectVertexPosition( int rect_index, int vert_index, const float x, const float y )
{
	m_pImpl->SetRectVertexPosition( rect_index, vert_index, x, y );
}


inline void C2DRectSet::SetVertexPosition( int vert_index, const float x, const float y )
{
	m_pImpl->SetVertexPosition( vert_index, x, y );
}


inline void C2DRectSet::SetTextureCoordMinMax( int rect_index, float u_min, float v_min, float u_max, float v_max )
{
	m_pImpl->SetTextureCoordMinMax( rect_index, u_min, v_min, u_max, v_max );
}


inline void C2DRectSet::SetTextureCoordMinMax( int rect_index, const TEXCOORD2& vMin, const TEXCOORD2& vMax )
{
	SetTextureCoordMinMax( rect_index, vMin.u, vMin.v, vMax.u, vMax.v );
}


inline void C2DRectSet::SetColor( U32 color )
{
	m_pImpl->SetColor( color );
}


inline void C2DRectSet::SetRectColor( int rect_index, U32 color )
{
	m_pImpl->SetRectColor( rect_index, color );
}


inline void C2DRectSet::SetNumRects( int num_rects )
{
	m_pImpl->SetNumRects( num_rects );
}


inline void C2DRectSet::AddRects( int num_rects )
{
	m_pImpl->AddRects( num_rects );
}


inline void C2DRectSet::Draw( int start_rect_index, int num_rects )
{
	SetBasicRenderStates();
	m_pImpl->Draw( start_rect_index, num_rects );
}


inline void C2DRectSet::Draw( int start_rect_index, int num_rects, const CTextureHandle& texture )
{
	SetBasicRenderStates();
	m_pImpl->Draw( start_rect_index, num_rects, texture );
}



//=============================================================================
// C2DPrimitiveFactoryImpl
//=============================================================================

class C2DPrimitiveFactoryImpl
{
public:

	virtual boost::shared_ptr<C2DRectSetImpl> Create2DRectSetImpl() = 0;
};


#include "Graphics/Direct3D/2DPrimitive/2DRectSetImpl_D3D.hpp"

class C2DPrimitiveFactoryImpl_D3D : public C2DPrimitiveFactoryImpl
{
public:

	boost::shared_ptr<C2DRectSetImpl> Create2DRectSetImpl()
	{
		boost::shared_ptr<C2DRectSetImpl> pImpl( new C2DRectSetImpl_D3D );
		return pImpl;
	}
};



#include "Graphics/OpenGL/2DPrimitive/2DRectSetImpl_GL.hpp"

class C2DPrimitiveFactoryImpl_GL : public C2DPrimitiveFactoryImpl
{
public:

	boost::shared_ptr<C2DRectSetImpl> Create2DRectSetImpl()
	{
		boost::shared_ptr<C2DRectSetImpl> pImpl( new C2DRectSetImpl_GL );
		return pImpl;
	}
};



//=============================================================================
// C2DPrimitiveFactory
//=============================================================================

class C2DPrimitiveFactory
{
	boost::shared_ptr<C2DPrimitiveFactoryImpl> m_pImpl;

public:

	void Init( C2DPrimitiveFactoryImpl *pImpl ) { m_pImpl = boost::shared_ptr<C2DPrimitiveFactoryImpl>( pImpl ); }

	boost::shared_ptr<C2DRectSetImpl> Create2DRectSetImpl() { return m_pImpl->Create2DRectSetImpl(); }
};


inline C2DPrimitiveFactory& Ref2DPrimitiveFactory()
{
	static C2DPrimitiveFactory s_Factory;
	return s_Factory;
}



/*
//=============================================================================
// C2DRectFSet
//=============================================================================

template<int NumRects>
class C2DRectFSet
{

	TLVERTEX m_aRectVertex[NumRects];

public:

	inline void Draw( int start_rect_index, int num_rects = NumRects );
};


inline void C2DRectSet::Draw( int start_rect_index, int num_rects )
{
	DIRECT3D9.GetDevice()->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST,
									0,
									num_rects * 4,
									num_rects * 2,
									s_RectTriListIndex,
									D3DFMT_INDEX16,
									m_aRectVertex + start_rect_index * 4,
									sizeof(TLVERTEX) );

}*/







#endif  /*  __2DRECTSET_H__  */