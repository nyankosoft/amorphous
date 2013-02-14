#ifndef  __Physics_PhysicsEngine_H__
#define  __Physics_PhysicsEngine_H__


#include <string>
#include "../3DMath/precision.h"
#include "../Support/singleton.hpp"
#include "fwd.hpp"


namespace amorphous
{


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

	virtual CConvexMesh *CreateConvexMesh( CStream& phys_stream ) = 0;

	virtual void ReleaseConvexMesh( CConvexMesh*& pConvexMesh ) = 0;

	virtual CTriangleMesh *CreateTriangleMesh( CStream& phys_stream ) = 0;

	virtual void ReleaseTriangleMesh( CTriangleMesh*& pTriangleMesh ) = 0;

	/// Creates a cloth mesh from a cooked cloth mesh stored in a stream. 
	virtual CTriangleMesh *CreateClothMesh ( CStream& phys_stream ) = 0;
 
	/// Deletes the specified cloth mesh. The cloth mesh must be in this scene. 
	virtual void  ReleaseClothMesh ( CTriangleMesh*& pCloth ) = 0;
};



/**
  singleton


*/
class CPhysicsEngine
{
	std::string m_Name;

	CPhysicsEngineImpl *m_pEngine;

protected:

	static singleton<CPhysicsEngine> m_obj;

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

	inline CConvexMesh *CreateConvexMesh( CStream& phys_stream ) { return m_pEngine->CreateConvexMesh( phys_stream ); }

	inline void ReleaseConvexMesh( CConvexMesh*& pConvexMesh ) { m_pEngine->ReleaseConvexMesh( pConvexMesh ); }

	inline CTriangleMesh *CreateTriangleMesh( CStream& phys_stream ) { return m_pEngine->CreateTriangleMesh( phys_stream ); }

	inline void ReleaseTriangleMesh( CTriangleMesh*& pTriangleMesh ) { m_pEngine->ReleaseTriangleMesh( pTriangleMesh ); }

	/// Creates a cloth mesh from a cooked cloth mesh stored in a stream. 
	inline CTriangleMesh *CreateClothMesh ( CStream& phys_stream ) { return m_pEngine->CreateClothMesh( phys_stream ); } 
 
	/// Deletes the specified cloth mesh. The cloth mesh must be in this scene. 
	inline void  ReleaseClothMesh ( CTriangleMesh*& pCloth ) { m_pEngine->ReleaseClothMesh( pCloth ); }
};


inline CPhysicsEngine& PhysicsEngine()
{
	return (*CPhysicsEngine::Get());
}



} // namespace physics

} // namespace amorphous



#endif		/*  __Physics_PhysicsEngine_H__  */
