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

std::vector<std::string>& GetProfileText();

int GetNumProfileTextRows();


/// for profiling a scope
class CScopeProfile
{
	const char *m_pProfileName;

public:

	CScopeProfile( const char *pProfileName)
	:
	m_pProfileName(pProfileName)
	{
		ProfileBegin( m_pProfileName );
	}

	~CScopeProfile()
	{
		ProfileEnd( m_pProfileName );
	}
};


/// put this at the start of a function or a loop to profile the scope
#define PROFILE_FUNCTION()          CScopeProfile scope_profile(__FUNCTION__)
#define PROFILE_SCOPE( scope_name ) CScopeProfile scope_profile( scope_name )


#endif /* __PROFILE_H__ */
