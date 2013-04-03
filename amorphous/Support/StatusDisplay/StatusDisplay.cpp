
#include "StatusDisplay.hpp"
#include "StatusDisplayRenderer.hpp"


namespace amorphous
{



singleton<CStatusDisplay> CStatusDisplay::m_obj;


CStatusDisplay::STask::STask()
{
	fStaus = 0;
}



CStatusDisplay::CStatusDisplay()
{
	m_pRenderer = NULL;

//	m_Position = SPoint(0,0);
	m_Position = SPoint(20,16);
	m_StatusRect = SRect( 0, 0, 120, 16 );
}


void CStatusDisplay::RegisterTask( string &strTaskName )
{
	m_vecTask.push_back( STask() );

	m_vecTask.back().strName = strTaskName;
}


void CStatusDisplay::RegisterTask( const char *pcTaskName )
{
	m_vecTask.push_back( STask() );

	m_vecTask.back().strName = pcTaskName;
}


void CStatusDisplay::UpdateProgress( const char *pcTaskName, float fNewStatus )
{
	string str = pcTaskName;
	UpdateProgress( str, fNewStatus );
}


void CStatusDisplay::UpdateProgress( string &strTaskName, float fNewStatus )
{
	int i, iNumTasks = m_vecTask.size();
	for( i=0; i<iNumTasks; i++ )
	{
		if( m_vecTask[i].strName == strTaskName )
			m_vecTask[i].fStaus = fNewStatus;
	}
}

} // amorphous
