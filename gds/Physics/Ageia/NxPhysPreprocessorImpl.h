#ifndef  __NxPhysPreprocessor_H__
#define  __NxPhysPreprocessor_H__


#include "3DMath/precision.h"
#include "Support/Log/DefaultLog.h"

#include "fwd.h"
#include "../CPreprocessor.h"


namespace physics
{


class CNxPhysPreprocessorImpl : public CPreprocessorImpl
{
	NxCookingInterface *m_pCooking;

public:

	CNxPhysPreprocessorImpl() : m_pCooking(NULL) {}

	virtual ~CNxPhysPreprocessorImpl();

	virtual bool Init();

	/// crates a stream of triangle mesh
	/// \param desc [in] desc for triangle mesh
	/// \param desc [out] stream of triangle mesh
	virtual void CreateTriangleMeshStream( CTriangleMeshDesc& desc,
		                                   CStream& phys_stream );

	virtual const char *GetPhysicsEngineName() const { return "AgeiaPhysX"; }
};


} // namespace physics


#endif /* __NxPhysPreprocessor_H__ */
