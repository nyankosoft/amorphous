#ifndef  __SoundEntity_HPP__
#define  __SoundEntity_HPP__


#include "Stage/fwd.hpp"
#include "Stage/CopyEntity.hpp"


namespace amorphous
{
//#include "Support/shared_prealloc_pool.hpp"


//class SoundEntityDesc;


class SoundEntity : public CCopyEntity
{
	float m_fMaxRadius;

	float m_fCurrentRadius;

public:

	SoundEntity();

	virtual ~SoundEntity();

//	virtual void Init( CCopyEntityDesc& desc );

//	void Init( SoundEntityDesc& desc );

	virtual void Update( float dt );

//	virtual void UpdatePhysics( float dt );

	void Draw();

	virtual void HandleMessage( GameMessage& msg );

	virtual void TerminateDerived();

	void SetMaxRadius( float max_radius ) { m_fMaxRadius = max_radius; }
};

} // namespace amorphous



#endif		/*  __SoundEntity_HPP__  */
