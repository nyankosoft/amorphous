#ifndef  __NxPhysPreprocessor_H__
#define  __NxPhysPreprocessor_H__


// PhysX header(s)
#include "PhysXLoader.h"

// my library headers
#include "3DMath/precision.h"
#include "Support/Log/DefaultLog.hpp"

#include "fwd.hpp"
#include "NxPhysOutputStream.hpp"
#include "../Preprocessor.hpp"


#pragma comment( lib, "PhysXCooking.lib" )


namespace physics
{


class CNxPhysPreprocessorImpl : public CPreprocessorImpl
{
	NxCookingInterface *m_pCooking;

	CNxPhysOutputStream m_ErrorStream;

public:

	CNxPhysPreprocessorImpl() : m_pCooking(NULL) {}

	virtual ~CNxPhysPreprocessorImpl();

	virtual bool Init();

	/// crates a stream of triangle mesh
	/// \param desc [in] desc for triangle mesh
	/// \param desc [out] stream of triangle mesh
	virtual Result::Name CreateTriangleMeshStream( CTriangleMeshDesc& desc,
		                                           CStream& phys_stream );

	virtual Result::Name CreateConvexMeshStream( CTriangleMeshDesc& mesh_desc,
		                                         CStream& phys_stream );

	virtual Result::Name CreateClothMeshStream( CClothMeshDesc& desc,
                                                CStream& phys_stream );

	virtual const char *GetPhysicsEngineName() const { return "AgeiaPhysX"; }
};


} // namespace physics


#endif /* __NxPhysPreprocessor_H__ */
