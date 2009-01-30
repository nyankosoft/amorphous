#ifndef  __InputHandler_StageDebug_H__
#define  __InputHandler_StageDebug_H__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "../../base.hpp"
#include "Stage/fwd.hpp"
#include "GameInput/InputHandler.hpp"


class CInputHandler_StageDebug : public CInputHandler
{
	boost::weak_ptr<CStage> m_pStage;

	ulong m_EntityTreeFileLastOutputTime;

private:

	void WriteEntityTreeToFile( boost::shared_ptr<CStage> pStage );

public:

	CInputHandler_StageDebug( boost::weak_ptr<CStage> pStage )
		:
	m_pStage(pStage),
	m_EntityTreeFileLastOutputTime(0)
	{}

	~CInputHandler_StageDebug() {}

	void ProcessInput( SInputData& input );

};


#endif		/*  __InputHandler_StageDebug_H__  */