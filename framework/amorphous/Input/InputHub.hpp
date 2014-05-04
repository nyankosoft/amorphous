#ifndef __InputHub_H__
#define __InputHub_H__

#include <vector>
#include "fwd.hpp"
#include "InputHandler.hpp"

#include "../base.hpp"
#include "../Support/SafeDelete.hpp"


namespace amorphous
{


/**
  Holds stacks of input handlers
  - This is a singleton class.
  About Input Handlers and Input Hub
  - Each input handler is placed in a stack identified by an index.
  - There are total of NUM_MAX_INPUT_HANDLERS stacks.
    - Developer can simultaneously register input handlers up to the number of NUM_MAX_USER_INPUT_HANDLERS
    - Index of input handlers registered to the input hub by developer must be in the range [MIN_USER_INPUT_HANDLER_INDEX,MAX_USER_INPUT_HANDLER_INDEX]
	- Developer can use stacks to proecess different inputs
	    - Input handler on stack A - for processing game inputs (player movement, etc.)
		- Input handler on stack B - debug input (turning on/off system info as overlay, etc.)
    - All the registered input handlers at the top of each stack receive the same input data.
	- Input handlers not on top of their stack do not receive input data.
  - The input hub holds input handlers as borrowed reference. Developer is responsible for releasing them
    after removing the borrowed reference from the input hub.
  - The developer has two ways of registering and unregistering input handlers to the input hub.
    - 1. Set and remove input handlers to and from the input hub.
      - Use InputHandler::SetInputHandler() and InputHandler::RemoveInputHandler()
    - 2. Push and pop input handlers to and from the input hub.
      - Use InputHandler::PushInputHandler() and InputHandler::PopInputHandler()
      - You can pop a pushed input handler with InputHandler::RemoveInputHandler().
	    This is usually safer because InputHandler::RemoveInputHandler() removes
        the input handler even if another input handler is on top of the stack.
    - Be careful when you use stack: all the input handlers in the stack must not be released until they are popped or removed.
 */
class InputHub
{
public:

	enum Params
	{
		NUM_MAX_INPUT_HANDLERS       = 12,
		MIN_USER_INPUT_HANDLER_INDEX = 2,
		MAX_USER_INPUT_HANDLER_INDEX = NUM_MAX_INPUT_HANDLERS - 1,
		NUM_MAX_USER_INPUT_HANDLERS  = MAX_USER_INPUT_HANDLER_INDEX - MIN_USER_INPUT_HANDLER_INDEX + 1,
		AUTO_REPEAT_INTERVAL_MS      = 150,
	};

private:

//	InputHandler *m_vecpInputHandler[NUM_MAX_INPUT_HANDLERS];

	/// stacks of input handelers
	/// - borrowed reference?
	std::vector<InputHandler *> m_vecpInputHandler[NUM_MAX_INPUT_HANDLERS];

	int m_CurrentMaxIndex;

	static InputHub ms_InputHub_Instance_;		// singleton instance

private:

	/// Used by input device classes
//	inline void SetInputState( int gi_code, CInputState::Name state ) { m_aInputState[gi_code].m_State = state; }

	inline void UpdateMaxIndex();

protected:

	InputHub();

public:

	~InputHub();

	static inline InputHub& GetInstance() { return ms_InputHub_Instance_; }

	/// releases all input handlers
//	void ReleaseInputHandlers();

//	inline void ReleaseInputHandler( int index );

	/// Sets an input handler in the slot 0.
	/// Calling this will cause memory leak if an input handler already exists in the slot with the specified index.
	inline void SetInputHandler( int index, InputHandler *pInputHandler );

	/// sets an input handler in the slot 0
	inline void SetInputHandler( InputHandler *pInputHandler );// { m_vecpInputHandler[0] = pInputHandler; }


	/// returns a borrowed reference to the root input handler currently at the top of the stack
	/// - Does not pop the input handler from that stack.
	/// - Never release the returned pointer before removing it from the stack with RemoveInputHandler()
	inline InputHandler *GetInputHandler( int index );

	inline const InputHandler *GetInputHandler( int index ) const;


	inline void PushInputHandler( int index, InputHandler *pInputHandler );

	inline void PushInputHandler( InputHandler *pInputHandler );


	// Removes the input handler currently on top of the stack specified with the index.
	inline InputHandler *PopInputHandler( int index );

	/// Removes an input handler from the stack.
	/// The input handler does not have to be at the top of the stack. The function search the stack and removes the input handler from the stack.
	/// Does not release it from the memory.
	/// \return Result::SUCCESS The specified input handler was found and removed.
	/// \return Result::INVALID_ARGS The specified input handler was not found. The content of the stack was not changed.
	inline Result::Name RemoveInputHandler( int index, InputHandler *pInputHandler );

	inline Result::Name RemoveInputHandler( InputHandler *pInputHandler );


	inline void UpdateInput( InputData& input );

	inline void SendAutoRepeatInputToInputHandlers( InputData& input );

	friend class InputDevice;
};

//=============================== inline implementations ===============================

inline void InputHub::UpdateMaxIndex()
{
	for( int i=NUM_MAX_INPUT_HANDLERS-1; 0<=i; i-- )
	{
		if( 0 < m_vecpInputHandler[i].size() )
		{
			m_CurrentMaxIndex = i;
			return;
		}
	}

	m_CurrentMaxIndex = -1;
}


inline void InputHub::UpdateInput( InputData& input )
{
	for( int i=0; i<=m_CurrentMaxIndex; i++ )
	{
		if( m_vecpInputHandler[i].size() == 0 )
			continue;

		if( m_vecpInputHandler[i].back() )
			m_vecpInputHandler[i].back()->ProcessInputBase( input );
	}
}


inline void InputHub::SendAutoRepeatInputToInputHandlers( InputData& input )
{
	for( int i=0; i<=m_CurrentMaxIndex; i++ )
	{
		if( m_vecpInputHandler[i].size() == 0 )
			continue;

		if( m_vecpInputHandler[i].back()
		 && m_vecpInputHandler[i].back()->IsAutoRepeatEnabled() )
            m_vecpInputHandler[i].back()->ProcessInput( input );
	}
}


inline void InputHub::SetInputHandler( InputHandler *pInputHandler )
{
	SetInputHandler( 0, pInputHandler );
}


inline void InputHub::SetInputHandler( int index, InputHandler *pInputHandler )
{
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return;

	if( m_vecpInputHandler[index].size() == 0 )
		m_vecpInputHandler[index].push_back( NULL );

	// overwrite the input handler currently at the top of the stack
	m_vecpInputHandler[index].back() = pInputHandler;

	if( m_CurrentMaxIndex < index )
		m_CurrentMaxIndex = index;
}

/*
inline void InputHub::ReleaseInputHandler( int index )
{
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return;

	SafeDelete( m_vecpInputHandler[index] );
}
*/


inline InputHandler *InputHub::GetInputHandler( int index )
{
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return NULL;

	if( m_vecpInputHandler[index].size() == 0 )
		return NULL;
	else
		return m_vecpInputHandler[index].back();
}


inline const InputHandler *InputHub::GetInputHandler( int index ) const
{
	const InputHub& const_ref = (*this);
	return const_ref.GetInputHandler(index);
}


inline void InputHub::PushInputHandler( InputHandler *pInputHandler )
{
	m_vecpInputHandler[0].push_back( pInputHandler );
}


inline void InputHub::PushInputHandler( int index, InputHandler *pInputHandler )
{
	m_vecpInputHandler[index].push_back( pInputHandler );

	if( m_CurrentMaxIndex < index )
		m_CurrentMaxIndex = index;
}


inline InputHandler *InputHub::PopInputHandler( int index )
{
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return NULL;

	if( m_vecpInputHandler[index].size() == 0 )
		return NULL;	// no input handler in the stack

	InputHandler* pTop = m_vecpInputHandler[index].back();

	m_vecpInputHandler[index].pop_back();

	return pTop;

}


inline Result::Name InputHub::RemoveInputHandler( int index, InputHandler *pInputHandler )
{
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return Result::INVALID_ARGS;

	for( size_t i=0; i<m_vecpInputHandler[index].size(); i++ )
	{
		if( m_vecpInputHandler[index][i] == pInputHandler )
		{
			m_vecpInputHandler[index].erase(  m_vecpInputHandler[index].begin() + i );
			UpdateMaxIndex();
			return Result::SUCCESS;
		}
		else
		{
			bool removed = m_vecpInputHandler[index][i]->RemoveChild( pInputHandler );
			if( removed )
			{
				UpdateMaxIndex();
				return Result::SUCCESS;
			}
		}
	}

	return Result::INVALID_ARGS;
}


inline Result::Name InputHub::RemoveInputHandler( InputHandler *pInputHandler )
{
	for( int i=0; i<NUM_MAX_INPUT_HANDLERS; i++ )
	{
		Result::Name res = RemoveInputHandler( i, pInputHandler );
		if( res == Result::SUCCESS )
			return Result::SUCCESS;
	}

	return Result::INVALID_ARGS;
}


inline InputHub& GetInputHub()
{
//	return (*InputHub::Get());
	return InputHub::GetInstance();
}


} // namespace amorphous



#endif  /*  __InputHub_H__  */