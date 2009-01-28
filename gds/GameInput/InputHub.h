#ifndef __INPUTHUB_H__
#define __INPUTHUB_H__

#include <vector>
#include "fwd.h"
#include "InputHandler.h"

#include "../base.h"
#include "Support/SafeDelete.h"
#include "Support/FixedVector.h"


/**
  Holds stacks of input handlers
  - There are total of NUM_MAX_INPUT_HANDLERS stacks.
    - Developer can simultaneously register input handlers up to the number of NUM_MAX_INPUT_HANDLERS
	- Developer can use stacks to proecess different inputs
	    - Input handler on stack 0 - for processing game inputs (player movement, etc.)
		- Input handler on stack 1 - debug input (turning on/off log screen, etc.)
    - All the registered input handlers at the top of each stack receive the same input data
  - Input handlers can either be managed by stack (PushInputHandler/PopInputHandler) or by single container (SetInputHandler)
    - Be careful when you use stack: all the input handlers in the stack must not be released until they are popped
 */
class CInputHub
{
	enum params
	{
		NUM_MAX_INPUT_HANDLERS = 8,

		NUM_MAX_SIMULTANEOUS_PRESSES  = 4,
		AUTO_REPEAT_INTERVAL_MS       = 150,
	};

//	CInputHandler *m_vecpInputHandler[NUM_MAX_INPUT_HANDLERS];

	/// stacks of input handelers
	/// - borrowed reference?
	std::vector<CInputHandler *> m_vecpInputHandler[NUM_MAX_INPUT_HANDLERS];

	CInputState m_aInputState[NUM_GENERAL_INPUT_CODES];

	TCFixedVector<int,NUM_MAX_SIMULTANEOUS_PRESSES> m_PressedKeyList;

//	int m_NumInputHandlers;

	static CInputHub ms_InputHub_Instance_;		// singleton instance

private:

	/// Used by input device classes
//	inline void SetInputState( int gi_code, CInputState::Name state ) { m_aInputState[gi_code].m_State = state; }

protected:

	CInputHub();

public:

	~CInputHub();

	static inline CInputHub& GetInstance() { return ms_InputHub_Instance_; }

	/// releases all input handlers
//	void ReleaseInputHandlers();

//	inline void ReleaseInputtHandler( int index );

	/// sets an input handler in the slot 0
	inline void SetInputHandler( CInputHandler *pInputHandler );// { m_vecpInputHandler[0] = pInputHandler; }
	inline void SetInputHandler( int index, CInputHandler *pInputHandler );

	/// returns a borrowed reference to the input handler currently at the top of the stack
	/// - Never release the returned pointer
	inline const CInputHandler *GetInputHandler() const { return GetInputHandler(0); }
	inline const CInputHandler *GetInputHandler( int index ) const;

	inline void PushInputHandler( CInputHandler *pInputHandler );
	inline void PushInputHandler( int index, CInputHandler *pInputHandler );

	inline CInputHandler *PopInputHandler();
	inline CInputHandler *PopInputHandler( int index );

	inline void UpdateInput( SInputData& input );

	inline void SendAutoRepeatInputToInputHandlers( SInputData& input );

	CInputState::Name GetInputState( int gi_code ) const { return m_aInputState[gi_code].m_State; }

	void SendAutoRepeat();

	friend class CInputDevice;
};


inline void CInputHub::UpdateInput( SInputData& input )
{
	for( int i=0; i<NUM_MAX_INPUT_HANDLERS; i++ )
	{
		if( m_vecpInputHandler[i].size() == 0 )
			continue;

		if( m_vecpInputHandler[i].back() )
            m_vecpInputHandler[i].back()->ProcessInput( input );
	}
}


inline void CInputHub::SendAutoRepeatInputToInputHandlers( SInputData& input )
{
	for( int i=0; i<NUM_MAX_INPUT_HANDLERS; i++ )
	{
		if( m_vecpInputHandler[i].size() == 0 )
			continue;

		if( m_vecpInputHandler[i].back()
		 && m_vecpInputHandler[i].back()->IsAutoRepeatEnabled() )
            m_vecpInputHandler[i].back()->ProcessInput( input );
	}
}


inline void CInputHub::SetInputHandler( CInputHandler *pInputHandler )
{
	SetInputHandler( 0, pInputHandler );
}


inline void CInputHub::SetInputHandler( int index, CInputHandler *pInputHandler )
{
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return;

	// release the previous input handler
//	SafeDelete( m_vecpInputHandler[index] );
	if( m_vecpInputHandler[index].size() == 0 )
		m_vecpInputHandler[index].push_back( NULL );

	// overwrite the input handler currently at the top of the stack
	m_vecpInputHandler[index].back() = pInputHandler;
}

/*
inline void CInputHub::ReleaseInputHandler( int index )
{
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return;

	SafeDelete( m_vecpInputHandler[index] );
}
*/


inline const CInputHandler *CInputHub::GetInputHandler( int index ) const
{
	if( m_vecpInputHandler[index].size() == 0 )
		return NULL;
	else
		return m_vecpInputHandler[index].back();
}


inline void CInputHub::PushInputHandler( CInputHandler *pInputHandler )
{
	m_vecpInputHandler[0].push_back( pInputHandler );
}


inline void CInputHub::PushInputHandler( int index, CInputHandler *pInputHandler )
{
	m_vecpInputHandler[index].push_back( pInputHandler );
}


inline CInputHandler *CInputHub::PopInputHandler()
{
	return PopInputHandler( 0 );
}


inline CInputHandler *CInputHub::PopInputHandler( int index )
{
	if( m_vecpInputHandler[index].size() == 0 )
		return NULL;	// no input handler in the stack

	CInputHandler* pTop = m_vecpInputHandler[index].back();

	m_vecpInputHandler[index].pop_back();

	return pTop;

}


inline CInputHub& InputHub()
{
//	return (*CInputHub::Get());
	return CInputHub::GetInstance();
}



#endif  /*  __INPUTHUB_H__  */
