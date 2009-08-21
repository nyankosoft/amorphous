#ifndef  __EntityHandle_H__
#define  __EntityHandle_H__


#include "3DMath/Matrix34.hpp"
#include <boost/weak_ptr.hpp>

class CCopyEntity;


/// Used to access the private members of CCopyEntity from CEntityHandle
class CEntityHandleBase
{
public:

	inline bool IsEntityInUse( CCopyEntity *pEntity );
};


template<class T = CCopyEntity>
class CEntityHandle : public CEntityHandleBase
{
	boost::weak_ptr<T> m_pEntity;

	/// - m_StockID == -2 : m_pEntity is a valid entity that does not use shared_prealloc_pool
	/// - m_StockID == -1 : No entity is set to m_pEntity. The handle is empty
	/// - m_StockID >= 0 : m_pEntity is a valid entity that uses shared_prealloc_pool
	int m_StockID;

public:

	CEntityHandle() : m_StockID(-1) {}

	/// Defined in CopyEntity.inl
	CEntityHandle( boost::weak_ptr<T> pEntity );

	/// Defined in CopyEntity.inl
	CEntityHandle( boost::shared_ptr<T> pEntity );

	virtual ~CEntityHandle() {}

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

//	virtual void OnCopyEntityReceivedMessage( CCopyEntity* pEntity, const SGameMessage& msg );

//	virtual void OnCopyEntityHitByAnother( CCopyEntity* pSelf, CCopyEntity* pOther );

};


#endif  /*  __EntityHandle_H__  */
