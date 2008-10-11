#ifndef  __Physics_PhysicsEngine_H__
#define  __Physics_PhysicsEngine_H__


#include <string>
#include "3DMath/precision.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Singleton.h"
using namespace NS_KGL;

#include "fwd.h"


namespace physics
{


class CPhysicsEngineImpl
{
public:

	CPhysicsEngineImpl() {}

	virtual ~CPhysicsEngineImpl() {}

	virtual bool Init() = 0;

	virtual CScene *CreateScene( CSceneDesc& desc ) = 0;

	virtual void ReleaseScene( CScene*& pScene ) = 0;

	virtual CTriangleMesh *CreateTriangleMesh( CStream& phys_stream ) = 0;

	virtual void ReleaseTriangleMesh( CTriangleMesh*& pTriangleMesh ) = 0;
};



/**
  singleton


*/
class CPhysicsEngine
{
	std::string m_Name;

	CPhysicsEngineImpl *m_pEngine;

protected:

	static CSingleton<CPhysicsEngine> m_obj;

public:

	static CPhysicsEngine* Get() { return m_obj.get(); }

	CPhysicsEngine() : m_pEngine(NULL) {}

	~CPhysicsEngine() { Release(); }

	void Release();

	/// returns true on success
	bool Init( const std::string& physics_engine = "AgeiaPhysX" );

	const std::string& GetName() const { return m_Name; }

	inline CScene *CreateScene( CSceneDesc& desc ) { return m_pEngine->CreateScene( desc ); }

	inline void ReleaseScene( CScene*& pScene ) { m_pEngine->ReleaseScene( pScene ); }

	inline CTriangleMesh *CreateTriangleMesh( CStream& phys_stream ) { return m_pEngine->CreateTriangleMesh( phys_stream ); }

	inline void ReleaseTriangleMesh( CTriangleMesh*& pTriangleMesh ) { m_pEngine->ReleaseTriangleMesh( pTriangleMesh ); }

	/// Creates a cloth mesh from a cooked cloth mesh stored in a stream. 
//	virtual CClothMesh *CreateClothMesh ( CStream& phys_stream ) { return m_pEngine->CreateClothMesh(  ); } 
 
	/// Deletes the specified cloth mesh. The cloth mesh must be in this scene. 
//	virtual void  ReleaseClothMesh ( CClothMesh*& pCloth ) { m_pEngine->ReleaseClothMesh(); }
};


inline CPhysicsEngine& PhysicsEngine()
{
	return (*CPhysicsEngine::Get());
}



} // namespace physics


#endif		/*  __Physics_PhysicsEngine_H__  */
