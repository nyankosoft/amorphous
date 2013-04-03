#ifndef  __InputHandler_StageDebug_H__
#define  __InputHandler_StageDebug_H__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "../../base.hpp"
#include "Stage/fwd.hpp"
#include "Input/InputHandler.hpp"


namespace amorphous
{


class InputHandler_StageDebug : public InputHandler
{
	boost::weak_ptr<CStage> m_pStage;

	ulong m_EntityTreeFileLastOutputTime;

private:

	void WriteEntityTreeToFile( boost::shared_ptr<CStage> pStage );

public:

	InputHandler_StageDebug( boost::weak_ptr<CStage> pStage )
		:
	m_pStage(pStage),
	m_EntityTreeFileLastOutputTime(0)
	{}

	~InputHandler_StageDebug() {}

	void ProcessInput( InputData& input );

};

} // namespace amorphous



#endif		/*  __InputHandler_StageDebug_H__  */