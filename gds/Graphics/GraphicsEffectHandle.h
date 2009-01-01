#ifndef __GraphicsEffectHandle_H__
#define __GraphicsEffectHandle_H__


#include "Graphics/fwd.h"
#include "Graphics/TextureCoord.h"


class CGraphicsEffectHandle
{
	CAnimatedGraphicsManager *m_pManager;

	int m_EffectIndex;

	int m_EffectID;

	static const CGraphicsEffectHandle ms_NullHandle; /// definition in GraphicsEffectManager.cpp

	CGraphicsEffectHandle( CAnimatedGraphicsManager *pMgr, int index, int id )
		:
	m_pManager(pMgr), m_EffectIndex(index), m_EffectID(id)
	{}

public:

	CGraphicsEffectHandle()
		:
	m_pManager(NULL), m_EffectIndex(-1), m_EffectID(-1)
	{}

	static const CGraphicsEffectHandle Null() { return ms_NullHandle; }

	/// valid for non-linear translation effects
	void SetDestPosition( const Vector2& vDestPos );

	/// valid effect: non-linear color change effect
	void SetDestColor( const SFloatRGBAColor& dest_color );

	/// valid effect: non-linear tex coord change effect to a polygon element
	void ChangeDestVertexTexCoord( int vertex, const TEXCOORD2& tex_coord );

	bool operator==( const CGraphicsEffectHandle& rhs )
	{
		return m_pManager    == rhs.m_pManager
			&& m_EffectIndex == rhs.m_EffectIndex
			&& m_EffectID    == rhs.m_EffectID;
	}

	bool operator!=( const CGraphicsEffectHandle& rhs ) { return !(*this==rhs); }

	friend class CAnimatedGraphicsManager;
};


/*
class CLinearGraphicsEffectHandle : public CGraphicsEffectHandle
{

public:
};


class CNonLinearGraphicsEffectHandle : public CGraphicsEffectHandle
{
	/// Must be cheked by m_pManager->IsAlive(  )
	CGraphicsElementNonLinearEffect *m_pEffect;

public:

	void SetDestPosition( const Vecotor2& vDestPos );

	void SetDestColor( const SFloatRGBAColor& dest_color );

	// smooth time for critical damping
	void SetSmoothTime( float smooth_time );

	// Don't return pointer to an effect
	// effects are automatically released when it's over and the caller cannot check if its still alive
	// from the raw pointer
	// Update() must not be called when this is called.
//	CGraphicsElementNonLinearEffect *operator->() { if( m_pManager->IsAlive( *this ) return m_pEffect; else return NULL; }

};


void CNonLinearGraphicsEffectHandle::SetDestPos( const Vecotor2& vDestPos )
{
}
*/

#endif /* __GraphicsEffectHandle_H__ */
