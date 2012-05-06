#ifndef  __SoundEntity_HPP__
#define  __SoundEntity_HPP__


#include "Stage/fwd.hpp"
#include "Stage/CopyEntity.hpp"
//#include "Support/shared_prealloc_pool.hpp"


//class CSoundEntityDesc;


class CSoundEntity : public CCopyEntity
{
	float m_fMaxRadius;

	float m_fCurrentRadius;

public:

	CSoundEntity();

	virtual ~CSoundEntity();

//	virtual void Init( CCopyEntityDesc& desc );

//	void Init( CSoundEntityDesc& desc );

	virtual void Update( float dt );

//	virtual void UpdatePhysics( float dt );

	void Draw();

	virtual void HandleMessage( SGameMessage& msg );

	virtual void TerminateDerived();

	void SetMaxRadius( float max_radius ) { m_fMaxRadius = max_radius; }
};


#endif		/*  __SoundEntity_HPP__  */
