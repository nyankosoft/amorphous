#ifndef __InputHub_H__
#define __InputHub_H__

#include <vector>
#include "fwd.hpp"
#include "InputHandler.hpp"

#include "../base.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/FixedVector.hpp"


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
public:

	enum Params
	{
		NUM_MAX_INPUT_HANDLERS      = 8,
		AUTO_REPEAT_INTERVAL_MS     = 150,
	};

private:

//	CInputHandler *m_vecpInputHandler[NUM_MAX_INPUT_HANDLERS];

	/// stacks of input handelers
	/// - borrowed reference?
	std::vector<CInputHandler *> m_vecpInputHandler[NUM_MAX_INPUT_HANDLERS];

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
	inline void SetInputHandler( int index, CInputHandler *pInputHandler );

	/// sets an input handler in the slot 0
	inline void SetInputHandler( CInputHandler *pInputHandler );// { m_vecpInputHandler[0] = pInputHandler; }


	/// returns a borrowed reference to the input handler currently at the top of the stack
	/// - Never release the returned pointer
	inline const CInputHandler *GetInputHandler( int index ) const;

	inline const CInputHandler *GetInputHandler() const { return GetInputHandler(0); }


	inline void PushInputHandler( int index, CInputHandler *pInputHandler );

	inline void PushInputHandler( CInputHandler *pInputHandler );


	inline CInputHandler *PopInputHandler( int index );

	inline CInputHandler *PopInputHandler();

	/// Removes an input handler from the stack.
	/// Does not release it from the memory.
	inline void RemoveInputHandler( int index, CInputHandler *pInputHandler );

	inline void RemoveInputHandler( CInputHandler *pInputHandler ) { RemoveInputHandler( 0, pInputHandler ); }


	inline void UpdateInput( SInputData& input );

	inline void SendAutoRepeatInputToInputHandlers( SInputData& input );

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
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return NULL;

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
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return NULL;

	if( m_vecpInputHandler[index].size() == 0 )
		return NULL;	// no input handler in the stack

	CInputHandler* pTop = m_vecpInputHandler[index].back();

	m_vecpInputHandler[index].pop_back();

	return pTop;

}


inline void CInputHub::RemoveInputHandler( int index, CInputHandler *pInputHandler )
{
	if( index < 0 || NUM_MAX_INPUT_HANDLERS <= index )
		return;

	for( size_t i=0; i<m_vecpInputHandler[index].size(); i++ )
	{
		if( m_vecpInputHandler[index][i] == pInputHandler )
		{
			m_vecpInputHandler[index].erase(  m_vecpInputHandler[index].begin() + i );
			return;
		}
	}
}


inline CInputHub& InputHub()
{
//	return (*CInputHub::Get());
	return CInputHub::GetInstance();
}



#endif  /*  __InputHub_H__  */
