#ifndef  __InputHandler_StageDebug_H__
#define  __InputHandler_StageDebug_H__

#include <memory>
#include "amorphous/base.hpp"
#include "amorphous/Stage/fwd.hpp"
#include "amorphous/Input/InputHandler.hpp"


namespace amorphous
{


class InputHandler_StageDebug : public InputHandler
{
	std::weak_ptr<CStage> m_pStage;

	ulong m_EntityTreeFileLastOutputTime;

private:

	void WriteEntityTreeToFile( std::shared_ptr<CStage> pStage );

public:

	InputHandler_StageDebug( std::weak_ptr<CStage> pStage )
		:
	m_pStage(pStage),
	m_EntityTreeFileLastOutputTime(0)
	{}

	~InputHandler_StageDebug() {}

	void ProcessInput( InputData& input );

};

} // namespace amorphous



#endif		/*  __InputHandler_StageDebug_H__  */