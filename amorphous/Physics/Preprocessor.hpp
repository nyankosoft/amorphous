#ifndef  __Physics_PhysPreprocessor_H__
#define  __Physics_PhysPreprocessor_H__


#include <string>
#include "../base.hpp"
#include "3DMath/precision.h"
#include "Support/singleton.hpp"
#include "fwd.hpp"
#include "Stream.hpp"


namespace amorphous
{


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
	virtual Result::Name CreateTriangleMeshStream( CTriangleMeshDesc& desc, CStream& phys_stream ) = 0;

	virtual Result::Name CreateConvexMeshStream( CTriangleMeshDesc& desc, CStream& phys_stream ) = 0;

	virtual Result::Name CreateClothMeshStream( CClothMeshDesc& desc, CStream& phys_stream ) = 0;

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

	static singleton<CPreprocessor> m_obj;

public:

	static CPreprocessor* Get() { return m_obj.get(); }

public:

	CPreprocessor();

	virtual ~CPreprocessor() { Release(); }

	void Release();

	inline Result::Name CreateTriangleMeshStream( CTriangleMeshDesc& desc, CStream& phys_stream );

	inline Result::Name CreateConvexMeshStream( CTriangleMeshDesc& desc, CStream& phys_stream );

	inline Result::Name CreateClothMeshStream( CClothMeshDesc& desc, CStream& phys_stream );

//	void SetDefault();
};


inline Result::Name CPreprocessor::CreateTriangleMeshStream( CTriangleMeshDesc& desc,
													 CStream& phys_stream )
{
	return m_pImpl->CreateTriangleMeshStream( desc, phys_stream );
}


inline Result::Name CPreprocessor::CreateConvexMeshStream( CTriangleMeshDesc& desc,
													 CStream& phys_stream )
{
	return m_pImpl->CreateConvexMeshStream( desc, phys_stream );
}


inline Result::Name CPreprocessor::CreateClothMeshStream( CClothMeshDesc& desc,
												  CStream& phys_stream )
{
	return m_pImpl->CreateClothMeshStream( desc, phys_stream );
}


inline CPreprocessor& Preprocessor()
{
	return (*CPreprocessor::Get());
}


} // namespace physics

} // namespace amorphous



#endif		/*  __Physics_PhysPreprocessor_H__  */
