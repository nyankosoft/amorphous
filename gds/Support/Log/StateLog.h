#ifndef  __STATELOG_H__
#define  __STATELOG_H__

#include <vector>
#include <string>

#include "../Singleton.h"
using namespace NS_KGL;


// for quick access to the singleton instance
#define StateLog ( *CStateLog::Get() )


//===================================================================================
// CStateLog (singleton)
//===================================================================================

/* 
 * takes logs of states which keep changing
 * the user is responsible for updating the state info
 * by calling StateLog.Update( index, text )
 * 
 */
class CStateLog
{
private:

	std::vector<std::string> m_LogText;

protected:

	static CSingleton<CStateLog> m_obj;

public:

	static CStateLog* Get() { return m_obj.get(); }

	CStateLog() {}

	~CStateLog() {}

	inline void Update( int index, const std::string& strLogText );

	inline const std::string& GetLog( int index ) const { return m_LogText[index]; }

	inline int GetNumLogs() const { return (int)m_LogText.size(); }
};


inline void CStateLog::Update( int index, const std::string& strLogText )
{
	int num = (int)m_LogText.size();
	while( num <= index )
	{
		m_LogText.push_back( std::string() );
		num++;
	}

	m_LogText[index] = strLogText;
}


#endif		/*  __STATELOG_H__  */
