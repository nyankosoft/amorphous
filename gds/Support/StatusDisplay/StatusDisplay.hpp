
#ifndef __STATUSDISPLAY_SINGLETON_H__
#define __STATUSDISPLAY_SINGLETON_H__


#include "Support/Singleton.hpp"

#include <vector>
#include <string>
using namespace std;

#include "Graphics/Point.hpp"
#include "Graphics/Rect.hpp"
using namespace Graphics;


#include "StatusDisplayRenderer.hpp"
//class CStatusDisplayRenderer;


class CStatusDisplay
{
public:

	struct STask
	{
		/// unique name for the task
		string strName;

		/// current progress of the task 0.0(start) to 1.0(complete)
		float fStaus;

		STask();
	};

//	static CStatusDisplay* Get() { return m_obj->get(); }
	static CStatusDisplay* Get() { return m_obj.get(); }

	CStatusDisplay();

	vector<STask>& GetTask() { return m_vecTask; }

	SPoint& GetPosition() { return m_Position; }

	SRect& GetStatusRect() { return m_StatusRect; }

	void RegisterTask( string &strTaskName );

	void RegisterTask( const char *pcTaskName );

	void UpdateProgress( string &strTaskName, float fNewStatus );

	void UpdateProgress( const char *pcTaskName, float fNewStatus );
	
	void Render() { m_pRenderer->Render(this); }

	void SetStatusDisplayRenderer( CStatusDisplayRenderer *pRenderer ) { m_pRenderer = pRenderer; }

//	friend class CStatusDisplayRenderer;


private:

	/// singleton instance
	static NS_KGL::CSingleton<CStatusDisplay> m_obj;

	/// top left corner of the task display
	SPoint m_Position;
	SRect m_StatusRect;

	vector<STask> m_vecTask;

	CStatusDisplayRenderer *m_pRenderer;

};


#endif  /*  __STATUSDISPLAY_SINGLETON_H__  */