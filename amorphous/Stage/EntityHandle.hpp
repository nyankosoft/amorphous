#ifndef  __EntityHandle_H__
#define  __EntityHandle_H__


#include "gds/3DMath/Matrix34.hpp"
#include <boost/weak_ptr.hpp>


namespace amorphous
{

class CCopyEntity;


/// Used to access the private members of CCopyEntity from EntityHandle
class EntityHandleBase
{
public:

	virtual ~EntityHandleBase() {}

	inline bool IsEntityInUse( CCopyEntity *pEntity );
};


template<class T = CCopyEntity>
class EntityHandle : public EntityHandleBase
{
	boost::weak_ptr<T> m_pEntity;

	/// - m_StockID == -2 : m_pEntity is a valid entity that does not use shared_prealloc_pool
	/// - m_StockID == -1 : No entity is set to m_pEntity. The handle is empty
	/// - m_StockID >= 0 : m_pEntity is a valid entity that uses shared_prealloc_pool
	int m_StockID;

public:

	EntityHandle() : m_StockID(-1) {}

	/// Defined in CopyEntity.inl
	EntityHandle( boost::weak_ptr<T> pEntity );

	/// Defined in CopyEntity.inl
	EntityHandle( boost::shared_ptr<T> pEntity );

	virtual ~EntityHandle() {}

	/// Defined in CopyEntity.inl
	inline boost::shared_ptr<T> Get();

	T *GetRawPtr()
	{
		boost::shared_ptr<T> pEntity = Get();
		if( pEntity )
			return pEntity.get();
		else
			return NULL;
	}

	bool IsAlive()
	{
		if( Get() )
			return true;
		else
			return false;
	}

	void Reset()
	{
		m_pEntity = boost::weak_ptr<T>();
		m_StockID = -1;
	}

	inline Vector3 GetWorldPosition();

	inline Matrix34 GetWorldPose();

	inline void SetWorldPosition( const Vector3& vPos );

	inline void SetWorldPose( const Matrix34& pose );

//	virtual void OnCopyEntityReceivedMessage( CCopyEntity* pEntity, const GameMessage& msg );

//	virtual void OnCopyEntityHitByAnother( CCopyEntity* pSelf, CCopyEntity* pOther );

};

} // namespace amorphous



#endif  /*  __EntityHandle_H__  */
