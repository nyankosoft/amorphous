#ifndef  __NxPhysOutputStream_H__
#define  __NxPhysOutputStream_H__


#include "NxPhysics.h"

#include "Support/Log/DefaultLog.h"


namespace physics
{


class CNxPhysOutputStream : public NxUserOutputStream
{
	void reportError (NxErrorCode code, const char *message, const char *file, int line)
	{
		// this should be routed to the application specific error handling. If this gets hit
		// then you are in most cases using the SDK wrong and you need to debug your code!
		// however, code may just be a warning or information.
		if ( code < NXE_DB_INFO )
			g_Log.Print( WL_ERROR, "AGEIA PhysX Error: [%s(%d)] %s", file, line, message );

//		exit(1);
	}

	NxAssertResponse reportAssertViolation (const char *message, const char *file, int line)
	{
		//this should not get hit by
		// a properly debugged SDK!
		assert(0);
		return NX_AR_CONTINUE;
	}

	void print (const char *message)
	{
//		printf(gSDK says: %s\nh, message);
//		MessageBox( NULL, "SDK Error", message, MB_OK|MB_ICONWARNING);
	}

};


} // namespace physics


#endif  /*  __NxPhysOutputStream_H__  */
