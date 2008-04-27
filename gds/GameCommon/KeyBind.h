#ifndef  __KeyBind_H__
#define  __KeyBind_H__

#include "GameInput/InputHandler.h"

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


class CKeyBind : public IArchiveObjectBase
{
	int m_aGICodeToActionCode[NUM_GENERAL_INPUT_CODES];

	int m_aGICodeToSecondaryActionCode[NUM_GENERAL_INPUT_CODES];

private:

	int *GetGICodeToActionCodeTable( int action_code )
	{
		switch( action_code )
		{
		case ACTION_TYPE_PRIMARY: return m_aGICodeToActionCode;
		case ACTION_TYPE_SECONDARY: return m_aGICodeToSecondaryActionCode;
		default: return m_aGICodeToActionCode;
		}

		return m_aGICodeToActionCode;
	}

public:

	enum eInputType
	{
		ANY,
		KEYBOARD,
		MOUSE,
		GAMEPAD,
		FLIGHTSTICK,
		NUM_INPUT_TYPES
	};

	enum eActionType
	{
		ACTION_TYPE_PRIMARY,
		ACTION_TYPE_SECONDARY,
		NUM_ACTION_TYPES
	};

	inline CKeyBind();

	/// \return general input code mapped to the given action
	/// \return -1 if corresponding general input code was not found
	int FindInputCode( int action_code, int input_type = ANY, int action_type = CKeyBind::ACTION_TYPE_PRIMARY )
	{
		switch(input_type)
		{
		case ANY:      return FindAnyInputCode( action_code, action_type );
		case KEYBOARD: return FindKeyboardInputCode( action_code, action_type );
		case MOUSE:    return FindMouseInputCode( action_code, action_type );
		case GAMEPAD:  return FindGamepadInputCode( action_code, action_type );
//		case FLIGHTSTICK:
		default:		return -1;
		}
		return -1;
	}

	int FindAnyInputCode( int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY )
	{
		int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			if( action_code == pGICodeToActionCode[i] )
			{
				return i;
			}
		}
		return -1;
	}

	int FindKeyboardInputCode( int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY )
	{
		int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			if( action_code == pGICodeToActionCode[i] 
			 && IsKeyboardInputCode( i ) )
				return i;
		}
		return -1;
	}

	int FindMouseInputCode( int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY )
	{
		int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			if( action_code == pGICodeToActionCode[i] 
			 && IsMouseInputCode( i ) )
				return i;
		}
		return -1;
	}

	int FindGamepadInputCode( int action_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY )
	{
		int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			if( action_code == pGICodeToActionCode[i] 
			 && IsGamepadInputCode( i ) )
				return i;
		}
		return -1;
	}

	int GetActionCode( int gi_code, int action_type = CKeyBind::ACTION_TYPE_PRIMARY )
	{
		int *pGICodeToActionCode = GetGICodeToActionCodeTable(action_type);

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
	}
};


inline CKeyBind::CKeyBind()
{
	for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
	{
		m_aGICodeToActionCode[i]          = -1;
		m_aGICodeToSecondaryActionCode[i] = -1;
	}
}


#endif		/*  __KeyBind_H__  */
