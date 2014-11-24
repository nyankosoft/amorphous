#ifndef  __INPUTHANDLER_H__
#define  __INPUTHANDLER_H__


#include <vector> // Used by InputHandler


namespace amorphous
{


enum eGeneralInputCode
{
	GIC_KBD_MIN = 0x20,

	GIC_ENTER = 128,	/// start from a number that comes after the ASCII codes to avoid conflicts
	GIC_SPACE,
	GIC_RSHIFT,
	GIC_LSHIFT,
	GIC_RCONTROL,
	GIC_LCONTROL,
	GIC_RALT,
	GIC_LALT,
    GIC_TAB,
	GIC_BACK,
	GIC_ESC,
	GIC_UP,
	GIC_DOWN,
	GIC_RIGHT,
	GIC_LEFT,
	GIC_NUMPAD0,
	GIC_NUMPAD1,
	GIC_NUMPAD2,
	GIC_NUMPAD3,
	GIC_NUMPAD4,
	GIC_NUMPAD5,
	GIC_NUMPAD6,
	GIC_NUMPAD7,
	GIC_NUMPAD8,
	GIC_NUMPAD9,
	GIC_MULTIPLY,
	GIC_DIVIDE,
	GIC_NUMPAD_PLUS,
	GIC_NUMPAD_MINUS,
	GIC_NUMPAD_DECIMAL,
	GIC_NUMPAD_ENTER,

	GIC_INSERT,
	GIC_DELETE,
	GIC_HOME,
	GIC_END,
	GIC_PAGE_UP,
	GIC_PAGE_DOWN,
	
	GIC_CONVERT,
	GIC_NOCONVERT,

	GIC_F1,
	GIC_F2,
	GIC_F3,
	GIC_F4,
	GIC_F5,
	GIC_F6,
	GIC_F7,
	GIC_F8,
	GIC_F9,
	GIC_F10,
	GIC_F11,
	GIC_F12,
	GIC_KBD_MAX = GIC_F12,

	GIC_MOUSE_MIN,
	GIC_MOUSE_BUTTON_L = GIC_MOUSE_MIN,
	GIC_MOUSE_BUTTON_R,
	GIC_MOUSE_BUTTON_M,
	GIC_MOUSE_AXIS_X,
	GIC_MOUSE_AXIS_Y,
	GIC_MOUSE_WHEEL_UP,
	GIC_MOUSE_WHEEL_DOWN,
	GIC_MOUSE_MAX = GIC_MOUSE_WHEEL_DOWN,

	GIC_GPD_MIN,
	GIC_GPD_UP = GIC_GPD_MIN,
	GIC_GPD_DOWN,
	GIC_GPD_RIGHT,
	GIC_GPD_LEFT,
	GIC_GPD_BUTTON_00,
	GIC_GPD_BUTTON_01,
	GIC_GPD_BUTTON_02,
	GIC_GPD_BUTTON_03,
	GIC_GPD_BUTTON_04,
	GIC_GPD_BUTTON_05,
	GIC_GPD_BUTTON_06,
	GIC_GPD_BUTTON_07,
	GIC_GPD_BUTTON_08,
	GIC_GPD_BUTTON_09,
	GIC_GPD_BUTTON_10,
	GIC_GPD_BUTTON_11,
	GIC_GPD_AXIS_X,
	GIC_GPD_AXIS_Y,
	GIC_GPD_AXIS_Z,
	GIC_GPD_ROTATION_X,
	GIC_GPD_ROTATION_Y,
	GIC_GPD_ROTATION_Z,
	GIC_GPD_AXIS_X_D,     ///< GIC_GPD_AXIS_X converted to on/off input
	GIC_GPD_AXIS_Y_D,     ///< GIC_GPD_AXIS_Y converted to on/off input
	GIC_GPD_AXIS_Z_D,     ///< GIC_GPD_AXIS_Z converted to on/off input
	GIC_GPD_ROTATION_X_D, ///< GIC_GPD_ROTATION_X converted to on/off input
	GIC_GPD_ROTATION_Y_D, ///< GIC_GPD_ROTATION_Y converted to on/off input
	GIC_GPD_ROTATION_Z_D, ///< GIC_GPD_ROTATION_Z converted to on/off input
	GIC_GPD_MAX = GIC_GPD_ROTATION_Z_D,

	NUM_GENERAL_INPUT_CODES
};


inline bool IsValidGeneralInputCode( int gi_code )
{
	if( 0 <= gi_code && gi_code < NUM_GENERAL_INPUT_CODES )
		return true;
	else
		return false;
}



inline bool IsKeyboardInputCode( int gi_code )
{
	if( GIC_ENTER <= gi_code && gi_code <= GIC_KBD_MAX || '0' <= gi_code && gi_code <= 'z' )
		return true;
	else
		return false;
}


inline bool IsMouseInputCode( int gi_code )
{
	if( GIC_MOUSE_MIN <= gi_code && gi_code <= GIC_MOUSE_MAX )
		return true;
	else
		return false;
}


inline bool IsGamepadInputCode( int gi_code )
{
	if( GIC_GPD_MIN <= gi_code && gi_code <= GIC_GPD_MAX )
		return true;
	else
		return false;
}


/// \return true if the given input code is of analog axis input from gamepad
inline bool IsGamepadAnalogAxisInputCode( int gi_code )
{
	if( GIC_GPD_AXIS_X <= gi_code && gi_code <= GIC_GPD_ROTATION_Z )
		return true;
	else
		return false;
}


/// \return true if the given input code is the mouse movement
inline bool IsMouseMoveInputCode( int gi_code )
{
	if( gi_code == GIC_MOUSE_AXIS_X || gi_code == GIC_MOUSE_AXIS_Y )
		return true;
	else
		return false;
}


// values of ASCII character codes for '0' through '9' and 'A' through 'Z' can be directly used
// as general input code (just like virtual key codes)

#define GIC_INVALID  -1

#define ITYPE_KEY_PRESSED  -1
#define ITYPE_KEY_RELEASED  1
#define ITYPE_VALUE_CHANGED  0 ///< mouse positions & gamepad axes


struct InputData
{
	/// general input code (GIC_)
	int iGICode;

	/// input type - ITYPE_KEY_PRESSED or ITYPE_KEY_RELEASED
	short iType;

	float fParam1;

	unsigned int uiParam;


	inline InputData() : iGICode(GIC_INVALID), iType(0), fParam1(0), uiParam(0) {}

	inline short GetParamH16() const { return (short)(0x0000FFFF & (uiParam >> 16)); }
	inline short GetParamL16() const { return (short)(0x0000FFFF & uiParam); }

	inline void SetParamH16( const short s ) { uiParam &= 0x0000FFFF; uiParam |= (s << 16); }
	inline void SetParamL16( const short s ) { uiParam &= 0xFFFF0000; uiParam |= s; }

	inline bool IsKeyboardInput() const;

	inline bool IsMouseInput() const;

	inline bool IsGamepadInput() const;
};


///////////////////////////// inline implementations /////////////////////////////

inline bool InputData::IsKeyboardInput() const
{
	return IsKeyboardInputCode( iGICode );
}


inline bool InputData::IsMouseInput() const
{
	return IsMouseInputCode( iGICode );
}


inline bool InputData::IsGamepadInput() const
{
	return IsGamepadInputCode( iGICode );
}



class InputHandler
{
	bool m_bActive;

	bool m_bAutoRepeat;

	/// borrowed references
	std::vector<InputHandler *> m_vecpChild;

public:

	InputHandler()
		:
	m_bActive(true),
	m_bAutoRepeat(false)
	{}

	virtual ~InputHandler() {}

	inline void ProcessInputBase(InputData& input);

	bool IsActive() const { return m_bActive; }

	bool IsAutoRepeatEnabled() const { return m_bAutoRepeat; }

	const std::vector<InputHandler *>& GetChildren() const { return m_vecpChild; }

	void SetActive( bool active ) { m_bActive = active; }

	void EnableAutoRepeat( bool enable_auto_repeat ) { m_bAutoRepeat = enable_auto_repeat; }

	void AddChild( InputHandler *pInputHandler ) { m_vecpChild.push_back( pInputHandler ); }

	/// Returns true if the input handler is found in the child list and removed.
	/// NOTE: detach the child and does not release the memory.
	inline bool RemoveChild( InputHandler *pInputHandler );

	virtual void ProcessInput(InputData& input) = 0;
};


inline void InputHandler::ProcessInputBase(InputData& input)
{
	if( m_bActive )
	{
		ProcessInput( input );

		for( size_t i=0; i<m_vecpChild.size(); i++ )
			m_vecpChild[i]->ProcessInputBase( input );
	}
}


inline bool InputHandler::RemoveChild( InputHandler *pInputHandler )
{
	for( size_t i=0; i<m_vecpChild.size(); i++ )
	{
		if( m_vecpChild[i] == pInputHandler )
		{
			m_vecpChild.erase( m_vecpChild.begin() + i );
			return true;
		}
	}

	return false;
}


/// Used as a parent of input handlers.
/// Does not process input for itself.
class CParentInputHandler : public InputHandler
{
public:

	void ProcessInput(InputData& input) {}
};



class CInputState
{
public:

	enum Name
	{
		PRESSED,
		RELEASED,
		NUM_STATES
	};

public:

//	unsigned long m_LastPressedTimeMS;
	unsigned long m_NextAutoRepeatTimeMS; ///< the absolute time when the next auto repeat event is scheduled to be sent

	Name m_State;

	float m_fPressedState;

public:

	CInputState()
		:
	m_NextAutoRepeatTimeMS(0),
	m_State(RELEASED),
	m_fPressedState(0)
	{}

	void Update( unsigned long current_time_ms )
	{
	}
};


template<class T>
class CInputDataDelegate : public InputHandler
{
	T *m_pTarget;
public:
	CInputDataDelegate(T *pTarget) : m_pTarget(pTarget) {}

	void ProcessInput( InputData& input )
	{
		m_pTarget->HandleInput( input );
	}
};


} // namespace amorphous



#endif		/*  __INPUTHANDLER_H__  */
