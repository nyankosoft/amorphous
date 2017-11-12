#include "InputHub.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/StringAux.hpp"


namespace amorphous
{


//===================================================================
// InputHub
//===================================================================

InputHub InputHub::ms_InputHub_Instance_;

InputHub::InputHub()
:
m_CurrentMaxIndex(-1)
{
	// all the stacks have NULL at the bottom of the stack
//	int i, num = NUM_MAX_INPUT_HANDLERS;
//	for( i=0; i<num; i++ )
//		m_vecpInputHandler[i].push_back( NULL );

//	m_NumInputHandlers = 0;

//	m_pInputHandler = NULL;

	//m_KeyStates.fill(0);
}


InputHub::~InputHub()
{
//	ReleaseInputHandlers();
}


void InputHub::PrintInputHandler( InputHandler& input_handler, const std::string& indent, std::string& dest )
{
	dest += indent + typeid(input_handler).name();
	dest += ( input_handler.IsActive() ? " (active)" : " (inactive)" );
	dest += "\n";

	const std::vector<InputHandler *> children = input_handler.GetChildren();
	const int num_children = (int)children.size();
	for( int i=0; i<num_children; i++ )
	{
		if( children[i] )
			PrintInputHandler( *children[i], indent + "  ", dest );
	}
}


void InputHub::PrintInputHandlers( std::string& dest )
{
	for( int i=0; i<=m_CurrentMaxIndex; i++ )
	{
		const int num_input_handlers = m_vecpInputHandler[i].size();
		for( int j=0; j<num_input_handlers; j++ )
		{
			dest += fmt_string("[%d][%d]\n",i,j);
			if( m_vecpInputHandler[i][j] )
				PrintInputHandler( *m_vecpInputHandler[i][j], "", dest );
		}
	}
}

/*
void InputHub::ReleaseInputHandlers()
{
	for( int i=0; i<NUM_MAX_INPUT_HANDLERS; i++ )
		SafeDelete( m_apInputHandler[i] );
}
*/


} // namespace amorphous
