#ifndef  __NxPhysOutputStream_H__
#define  __NxPhysOutputStream_H__


#include "NxPhysics.h"

#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{


namespace physics
{

static inline const char *NxErrorCodeString(NxErrorCode code)
{
	switch(code)
	{
	case NXE_NO_ERROR:           return "NXE_NO_ERROR";
	case NXE_INVALID_PARAMETER:  return "NXE_INVALID_PARAMETER";
	case NXE_INVALID_OPERATION:  return "NXE_INVALID_OPERATION";
	case NXE_OUT_OF_MEMORY:      return "NXE_OUT_OF_MEMORY";
	case NXE_INTERNAL_ERROR:	 return "NXE_INTERNAL_ERROR";
	case NXE_ASSERTION:			 return "NXE_ASSERTION";
	case NXE_DB_INFO:			 return "NXE_DB_INFO";	
	case NXE_DB_WARNING:		 return "NXE_DB_WARNING";
	case NXE_DB_PRINT:			 return "NXE_DB_PRINT";
	default: return "An unknown error code";
	}

	return "An unknown error code";
}

class CNxPhysOutputStream : public NxUserOutputStream
{
	void reportError (NxErrorCode code, const char *message, const char *file, int line)
	{
		// this should be routed to the application specific error handling. If this gets hit
		// then you are in most cases using the SDK wrong and you need to debug your code!
		// however, code may just be a warning or information.
		if ( code < NXE_DB_INFO )
			GlobalLog().Print( WL_ERROR, "AGEIA PhysX Error: [%s(%d)] %s (NxErrorCode: %s)", file, line, message, NxErrorCodeString(code) );

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
		GlobalLog().Print( WL_INFO, "AGEIA PhysX: %s", message );
//		printf(gSDK says: %s\nh, message);
//		MessageBox( NULL, "SDK Error", message, MB_OK|MB_ICONWARNING);
	}

};


/// singleton
inline CNxPhysOutputStream &NxPhysOutputStream()
{
	static CNxPhysOutputStream output_stream;
	return output_stream;
}


} // namespace physics

} // namespace amorphous



#endif  /*  __NxPhysOutputStream_H__  */
