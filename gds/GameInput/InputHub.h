#ifndef __INPUTHUB_H__
#define __INPUTHUB_H__

#include <vector>
#include "InputHandler.h"

#include "../Support/SafeDelete.h"


#define INPUTHUB ( CInputHub::GetInstance() )


/**
 * holds one or more input handlers
 * - input handlers can either be managed by stack (PushInputHandler/PopInputHandler) or by single container (SetInputHandler)
 *   - Be careful when you use stack: all the input handlers in the stack must not be released until they are popped
 * - user can register input handlers up to the number of NUM_MAX_INPUT_HANDLERS
 *   - all the registered input handlers receive input data
 */
class CInputHub
{
	enum params
	{
		NUM_MAX_INPUT_HANDLERS = 8
	};

//	CInputHandler *m_vecpInputHandler[NUM_MAX_INPUT_HANDLERS];

	/// stacks of input handelers
	/// - borrowed reference?
	std::vector<CInputHandler *> m_vecpInputHandler[NUM_MAX_INPUT_HANDLERS];

//	int m_NumInputHandlers;

	static CInputHub ms_InputHub_Instance_;		// singleton instance

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



#endif  /*  __INPUTHUB_H__  */
