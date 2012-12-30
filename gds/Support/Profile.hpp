/* Copyright (C) Steve Rabin, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steve Rabin, 2000"
 */

#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <vector>
#include <string>


namespace amorphous
{


class CProfileTimer
{

public:

	CProfileTimer() {}

	virtual ~CProfileTimer() {}

	virtual double GetExactTime() = 0;

	virtual double GetFrameTime() = 0;

	virtual void UpdateFrameTime() = 0;
};


void ProfileInit( void );

void ProfileBegin( const char* name );

void ProfileEnd( const char* name );

void ProfileDumpOutputToBuffer( void );

void ProfileEnable( bool enable );

void StoreProfileInHistory( char* name, float percent );

void GetProfileFromHistory( char* name, float* ave, float* min, float* max );

const std::vector<std::string>& GetProfileText();

int GetNumProfileTextRows();


/// for profiling a scope
class CScopeProfile
{
	const char *m_pProfileName;
	const bool m_Enabled;

public:

	CScopeProfile( const char *pProfileName, bool enabled = true )
	:
	m_pProfileName(pProfileName),
	m_Enabled(enabled)
	{
		if( m_Enabled )
			ProfileBegin( m_pProfileName );
	}

	~CScopeProfile()
	{
		if( m_Enabled )
			ProfileEnd( m_pProfileName );
	}
};


/// put this at the start of a function or a loop to profile the scope
#define PROFILE_FUNCTION()                      CScopeProfile scope_profile( __FUNCTION__, true )
#define PROFILE_FUNCTION_IF( enabled )          CScopeProfile scope_profile( __FUNCTION__, enabled )
#define PROFILE_SCOPE( scope_name )             CScopeProfile scope_profile( scope_name, true )
#define PROFILE_SCOPE_IF( scope_name, enabled ) CScopeProfile scope_profile( scope_name, enabled )

} // namespace amorphous



#endif /* __PROFILE_H__ */
