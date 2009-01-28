#ifndef  __Physics_PhysPreprocessor_H__
#define  __Physics_PhysPreprocessor_H__


#include <string>
#include "3DMath/precision.h"
#include "Support/Singleton.h"
using namespace NS_KGL;

#include "fwd.h"
#include "Stream.h"


namespace physics
{


class CPreprocessorImpl
{
protected:

	inline void SetPhysicsEngineName( CStream& phys_stream );

public:

	CPreprocessorImpl() {}

	virtual ~CPreprocessorImpl() {}

	virtual bool Init() = 0;

	/// [in] desc
	/// [out] 
	virtual void CreateTriangleMeshStream( CTriangleMeshDesc& desc, CStream& phys_stream ) = 0;

	virtual const char *GetPhysicsEngineName() const = 0;
};


//=========================== inline implementations ===========================

inline void CPreprocessorImpl::SetPhysicsEngineName( CStream& phys_stream )
{
	phys_stream.m_PhysicsEngine = GetPhysicsEngineName();
}


/**
  singleton
  - Call physics::PhysicsEngine().Init( physics_engine_name ) in advance
    to access the preprocessor singleton of a particular physics engine.
  - TODO: support different preprocessors of different physics engine during runtime

*/
class CPreprocessor
{
	CPreprocessorImpl *m_pImpl;

private:

	/// returns true on success
	/// - called in ctor
	bool Init();

protected:

	static CSingleton<CPreprocessor> m_obj;

public:

	static CPreprocessor* Get() { return m_obj.get(); }

public:

	CPreprocessor();

	virtual ~CPreprocessor() { Release(); }

	void Release();

	inline void CreateTriangleMeshStream( CTriangleMeshDesc& desc, CStream& phys_stream );

//	void SetDefault();
};


inline void CPreprocessor::CreateTriangleMeshStream( CTriangleMeshDesc& desc,
													 CStream& phys_stream )
{
	m_pImpl->CreateTriangleMeshStream( desc, phys_stream );
}


inline CPreprocessor& Preprocessor()
{
	return (*CPreprocessor::Get());
}


} // namespace physics


#endif		/*  __Physics_PhysPreprocessor_H__  */
