#ifndef __FlightGameGlobalParams_H__
#define __FlightGameGlobalParams_H__


#include <stdio.h>
#include "Support/ParamLoader.h"


//>>>========================== default config file ==========================
/*
skip_briefing	no
//aircraft	aircraft_name
//stage		stage_name stage_script
*/
//<<<========================== default config file ==========================



class CFlightGameGlobalParams
{
public:

	bool SkipBriefing;

	std::string DefaultAircraftName;

	std::string StageName;
	std::string StageScriptFilename;

public:

	CFlightGameGlobalParams() : SkipBriefing(false) {}
/*
	void OutputConfigFile();
	{
		FILE fp = fopen( "config", "w" );
		if( !fp )
			return;

		fprintf( fp, "fullscreen	%s\n", (FullScreen ? "yes" : "no") );
		fprintf( fp, "resolution	%d	%d\n", ScreenWidth, ScreenHeight );
		fclose(fp);
	}
*/
	bool LoadFromFile( const std::string& filename )
	{
		CParamLoader loader( filename );

		if( !loader.IsReady() )
			return false;

		loader.LoadBoolParam( "skip_briefing", "yes/no", SkipBriefing );
		loader.LoadParam( "aircraft",	DefaultAircraftName );
		loader.LoadParam( "stage",		StageName, StageScriptFilename );

		return true;
	}
};


extern CFlightGameGlobalParams g_FlightGameParams;


#endif  /*  __FlightGameGlobalParams_H__  */
