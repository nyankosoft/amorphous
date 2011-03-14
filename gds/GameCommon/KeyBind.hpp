#ifndef  __KeyBind_H__
#define  __KeyBind_H__

#include "Input/InputHandler.hpp"

#include "Support/Serialization/Serialization.hpp"
using namespace GameLib1::Serialization;


/**
- An action code can have multiple GI codes
  - Needed to support multiple input devices
    e.g. keyboard up key, gamepad up key -> both mapped to the same action
  - Redundant when there is only one input device.

- A GI code can also have multiple action codes
  - Need to actions in different contexts
    e.g. the enter key used both as attack command and as OK command in an item selection menu
  - When a single GI code has multiple action codes, they must belong to different action types.

- Steps of finding an action code
  - 1. An input handler responsible for key mapping receives a GI code
  - 2. The input handler look up the action code the received GI code is mapped to.
    - 2-1. In doing this, the input handler gives keybind table an action type (context)
	       to determine a single action code for the GI code.
*/
class CKeyBind : public IArchiveObjectBase
{
	int m_aGICodeToActionCode[NUM_GENERAL_INPUT_CODES];

	int m_aGICodeToSecondaryActionCode[NUM_GENERAL_INPUT_CODES];

	int m_aGICodeToSystemMenuCode[NUM_GENERAL_INPUT_CODES];

private:

	/// Change the const version below when you change this.
	int *GetGICodeToActionCodeTable( int action_type )
	{
		switch( action_type )
		{
		case ACTION_TYPE_PRIMARY:   return m_aGICodeToActionCode;
		case ACTION_TYPE_SECONDARY: return m_aGICodeToSecondaryActionCode;
		case ACTION_TYPE_SYSTEM:    return m_aGICodeToSystemMenuCode;
		default: return m_aGICodeToActionCode;
		}

		return m_aGICodeToActionCode;
	}

	/// Change the non-const version above when you change this.
	const int *GetGICodeToActionCodeTable( int action_type ) const
	{
		switch( action_type )
		{
		case ACTION_TYPE_PRIMARY:   return m_aGICodeToActionCode;
		case ACTION_TYPE_SECONDARY: return m_aGICodeToSecondaryActionCode;
		case ACTION_TYPE_SYSTEM:    return m_aGICodeToSystemMenuCode;
		default: return m_aGICodeToActionCode;
		}

		return m_aGICodeToActionCode;
	}

public:

	enum InputDeviceType
	{
		ANY,
		ALL,
		KEYBOARD,
		MOUSE,
		GAMEPAD,
		FLIGHTSTICK,
		NUM_INPUT_TYPES
	};

	enum ActionType
	{
		ACTION_TYPE_PRIMARY,
		ACTION_TYPE_SECONDARY,
		ACTION_TYPE_SYSTEM,
		NUM_ACTION_TYPES
	};

	inline CKeyBind();

/*	/// \param action_code [in]
	/// \param gi_codes [out]
	void FindGeneralInputCodes( int action_code, InputDeviceType input_device_type, std::vector<int>& gi_codes )
	{
		int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);
		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
		}
	}*/

	void FindGeneralInputCodesOfAllInputDevices( int action_code, int action_type /*= CKeyBind::ACTION_TYPE_PRIMARY*/, std::vector<int>& gi_codes ) const
	{
		const int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		gi_codes.resize( 0 );
		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			if( action_code == pGICodeToActionCode[i] )
				gi_codes.push_back( i );
		}
	}

	void FindGeneralInputCodes( int action_code, int action_type, bool(*IsGICodeOfSpecifiedInputDevice)(int), std::vector<int>& gi_codes ) const
	{
		const int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		gi_codes.resize( 0 );
		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			if( action_code == pGICodeToActionCode[i] 
			 && IsGICodeOfSpecifiedInputDevice( i ) )
				gi_codes.push_back( i );
		}
	}

	void FindKeyboardInputCodes( int action_code, int action_type /*= CKeyBind::ACTION_TYPE_PRIMARY*/, std::vector<int>& gi_codes ) const { FindGeneralInputCodes( action_code, action_type, IsKeyboardInputCode, gi_codes ); }
	void FindMouseInputCodes(    int action_code, int action_type /*= CKeyBind::ACTION_TYPE_PRIMARY*/, std::vector<int>& gi_codes ) const { FindGeneralInputCodes( action_code, action_type, IsMouseInputCode,    gi_codes ); }
	void FindGamepadInputCodes(  int action_code, int action_type /*= CKeyBind::ACTION_TYPE_PRIMARY*/, std::vector<int>& gi_codes ) const { FindGeneralInputCodes( action_code, action_type, IsGamepadInputCode,  gi_codes ); }

	/// \param action_code [in]
	/// \param gi_codes [out]
	/// \brief Returns general input codes that are mapped to the given action code of the given action type
	void FindGeneralInputCodes( int action_code, int action_type, InputDeviceType input_device_type, std::vector<int>& gi_codes ) const
	{
		switch(input_device_type)
		{
		case ALL:      FindGeneralInputCodesOfAllInputDevices( action_code, action_type, gi_codes ); break;
		case KEYBOARD: FindKeyboardInputCodes( action_code, action_type, gi_codes ); break;
		case MOUSE:    FindMouseInputCodes(    action_code, action_type, gi_codes ); break;
		case GAMEPAD:  FindGamepadInputCodes(  action_code, action_type, gi_codes ); break;
//		case FLIGHTSTICK:
		default:
			break;
		}
	}


	int FindGeneralInputCode( int action_code, int action_type /*= CKeyBind::ACTION_TYPE_PRIMARY*/, bool(*IsGICodeOfSpecifiedInputDevice)(int) ) const
	{
		const int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			if( action_code == pGICodeToActionCode[i] 
			 && IsGICodeOfSpecifiedInputDevice( i ) )
				return i;
		}
		return -1;
	}

	int FindGeneralInputCodeOfAnyInputDevice( int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY ) const
	{
		const int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			if( action_code == pGICodeToActionCode[i] )
				return i;
		}
		return -1;
	}

	int FindKeyboardInputCode( int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY ) const { return FindGeneralInputCode( action_code, action_type, IsKeyboardInputCode ); }
	int FindMouseInputCode(    int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY ) const { return FindGeneralInputCode( action_code, action_type, IsMouseInputCode    ); }
	int FindGamepadInputCode(  int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY ) const { return FindGeneralInputCode( action_code, action_type, IsGamepadInputCode  ); }

	/// \return a general input code mapped to the given action
	/// \return -1 if corresponding general input code was not found
	/// Returns only one input 
	int FindGeneralInputCode( int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY, InputDeviceType input_device_type = ANY ) const
	{
		switch(input_device_type)
		{
		case ANY:      return FindGeneralInputCodeOfAnyInputDevice( action_code, action_type );
		case KEYBOARD: return FindKeyboardInputCode( action_code, action_type );
		case MOUSE:    return FindMouseInputCode(    action_code, action_type );
		case GAMEPAD:  return FindGamepadInputCode(  action_code, action_type );
//		case FLIGHTSTICK:
		default:		return -1;
		}
		return -1;
	}


	int GetActionCode( int gi_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY ) const
	{
		const int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		if( gi_code < 0 || NUM_GENERAL_INPUT_CODES <= gi_code )
			return -1;

		return pGICodeToActionCode[gi_code];
	}

	bool Assign( int gi_code, int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY )
	{
		int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		if( gi_code < 0 || NUM_GENERAL_INPUT_CODES <= gi_code )
			return false;

		pGICodeToActionCode[gi_code] = action_code;
		return true;
	}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
			ar & m_aGICodeToActionCode[i];

		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
			ar & m_aGICodeToSecondaryActionCode[i];

		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
			ar & m_aGICodeToSystemMenuCode[i];
	}
};


inline CKeyBind::CKeyBind()
{
	for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
	{
		m_aGICodeToActionCode[i]          = -1;
		m_aGICodeToSecondaryActionCode[i] = -1;
		m_aGICodeToSystemMenuCode[i]      = -1;
	}
}


#endif		/*  __KeyBind_H__  */
