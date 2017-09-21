#ifndef __GraphicsEffectHandle_H__
#define __GraphicsEffectHandle_H__


#include "fwd.hpp"
#include "TextureCoord.hpp"
#include "FloatRGBAColor.hpp"
#include "amorphous/3DMath/Vector2.hpp"


namespace amorphous
{


class GraphicsElementAnimationHandle
{
	GraphicsElementAnimationManager *m_pManager;

	int m_EffectIndex;

	int m_EffectID;

	static const GraphicsElementAnimationHandle ms_NullHandle; /// definition in GraphicsEffectManager.cpp

	GraphicsElementAnimationHandle( GraphicsElementAnimationManager *pMgr, int index, int id )
		:
	m_pManager(pMgr), m_EffectIndex(index), m_EffectID(id)
	{}

public:

	GraphicsElementAnimationHandle()
		:
	m_pManager(NULL), m_EffectIndex(-1), m_EffectID(-1)
	{}

	static const GraphicsElementAnimationHandle Null() { return ms_NullHandle; }

	/// valid for non-linear translation effects
	void SetDestPosition( const Vector2& vDestPos );

	/// valid effect: non-linear color change effect
	void SetDestColor( const SFloatRGBAColor& dest_color );

	/// valid effect: non-linear tex coord change effect to a polygon element
	void ChangeDestVertexTexCoord( int vertex, const TEXCOORD2& tex_coord );

	bool operator==( const GraphicsElementAnimationHandle& rhs )
	{
		return m_pManager    == rhs.m_pManager
			&& m_EffectIndex == rhs.m_EffectIndex
			&& m_EffectID    == rhs.m_EffectID;
	}

	bool operator!=( const GraphicsElementAnimationHandle& rhs ) { return !(*this==rhs); }

	friend class GraphicsElementAnimationManager;
};


/*
class CLinearGraphicsEffectHandle : public GraphicsElementAnimationHandle
{

public:
};


class CNonLinearGraphicsEffectHandle : public GraphicsElementAnimationHandle
{
	/// Must be cheked by m_pManager->IsAlive(  )
	GraphicsElementNonLinearEffect *m_pEffect;

public:

	void SetDestPosition( const Vecotor2& vDestPos );

	void SetDestColor( const SFloatRGBAColor& dest_color );

	// smooth time for critical damping
	void SetSmoothTime( float smooth_time );

	// Don't return pointer to an effect
	// effects are automatically released when it's over and the caller cannot check if its still alive
	// from the raw pointer
	// Update() must not be called when this is called.
//	GraphicsElementNonLinearEffect *operator->() { if( m_pManager->IsAlive( *this ) return m_pEffect; else return NULL; }

};


void CNonLinearGraphicsEffectHandle::SetDestPos( const Vecotor2& vDestPos )
{
}
*/
} // namespace amorphous



#endif /* __GraphicsEffectHandle_H__ */
