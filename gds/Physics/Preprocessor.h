#ifndef  __Physics_PhysPreprocessor_H__
#define  __Physics_PhysPreprocessor_H__


#include <string>
#include "3DMath/precision.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Singleton.h"
using namespace NS_KGL;

#include "fwd.h"
#include "Stream.h"


namespace physics
{


#define PhysPreprocessor ( (*CPreprocessor::Get()) )


class CPreprocessorImpl
{
protected:

	inline void SetPhysicsEngineName( CStream& phys_stream );

public:

	CPreprocessorImpl() {}

	virtual ~CPreprocessorImpl() {}

	virtual bool Init() = 0;

	virtual void CreateTriangleMeshStream( CTriangleMeshDesc& desc, CStream& phys_stream ) = 0;

	virtual const char *GetPhysicsEngineName() const = 0;
};

//=========================== inline implementations ===========================

inline void CPreprocessorImpl::SetPhysicsEngineName( CStream& phys_stream )
{
	phys_stream.m_PhysicsEngine = GetPhysicsEngineName();
}


class CPreprocessor
{
	CPreprocessorImpl *m_pImpl;

protected:

	static CSingleton<CPreprocessor> m_obj;

public:

	static CPreprocessor* Get() { return m_obj.get(); }

public:

	CPreprocessor() : m_pImpl(NULL) {}

	virtual ~CPreprocessor() { Release(); }

	void Release();

	/// returns true on success
	bool Init( const std::string& physics_engine );

	inline void CreateTriangleMeshStream( CTriangleMeshDesc& desc, CStream& phys_stream );

//	void SetDefault();
};


bool CPreprocessor::Init( const std::string& physics_engine )
{
	SafeDelete( m_pImpl );

	if( physics_engine == "AgeiaPhysX" )
	{
		m_pImpl = new CNxPhysicsPreprocessor();
	}
/*	else if( physics_engine == "JigLib" )
	{
		m_pImpl = new CJigLibPhysicsEngine();
	}*/
	else
	{
		LOG_PRINT_ERROR( "invalid physics engine name: " + physics_engine );
		return false;
	}

	m_pImpl->Init();

	return true;
}


void CPreprocessor::Release()
{
	SafeDelete( m_pImpl );
}


inline void CPreprocessor::CreateTriangleMeshStream( CTriangleMeshDesc& desc,
													     PhysStream& phys_stream )
{
	m_pImpl->CreateTriangleMeshStream( desc, phys_stream );
}


} // namespace physics


#endif		/*  __Physics_PhysPreprocessor_H__  */
