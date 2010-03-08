#include "InputHub.hpp"
#include "Support/Timer.hpp"


//===================================================================
// CInputHub
//===================================================================

CInputHub CInputHub::ms_InputHub_Instance_;

CInputHub::CInputHub()
:
m_CurrentMaxIndex(-1)
{
	// all the stacks have NULL at the bottom of the stack
//	int i, num = NUM_MAX_INPUT_HANDLERS;
//	for( i=0; i<num; i++ )
//		m_vecpInputHandler[i].push_back( NULL );

//	m_NumInputHandlers = 0;

//	m_pInputHandler = NULL;
}


CInputHub::~CInputHub()
{
//	ReleaseInputHandlers();
}

/*
void CInputHub::ReleaseInputHandlers()
{
	for( int i=0; i<NUM_MAX_INPUT_HANDLERS; i++ )
		SafeDelete( m_apInputHandler[i] );
}
*/
