#ifndef __GM_Keybind_HPP__
#define __GM_Keybind_HPP__


#include "Input/InputHandler.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"
using namespace GameLib1::Serialization;

#include "GM_Input.hpp"


class CGM_Keybind : public IArchiveObjectBase
{
public:
	CGM_Keybind() {}
	virtual ~CGM_Keybind() {}

	virtual Result::Name Assign( int gi_code, CGM_Input::InputCode gui_input_code ) = 0;//{ return Result::UNKNOWN_ERROR; }

	virtual int GetInputCode( int gi_code ) const = 0;
};


/// faster lookup, more memory usage
class CGM_CompleteKeybind : public CGM_Keybind
{
	int m_aGICodeToGMInputCode[NUM_GENERAL_INPUT_CODES];

public:

	CGM_CompleteKeybind();

	void SetDefault();

	Result::Name Assign( int gi_code, CGM_Input::InputCode gui_input_code )
	{
		if( gi_code < 0 || NUM_GENERAL_INPUT_CODES <= gi_code )
			return Result::INVALID_ARGS;

		m_aGICodeToGMInputCode[gi_code] = gui_input_code;

		return Result::SUCCESS;
	}

	int GetInputCode( int gi_code ) const
	{
		if( gi_code < 0 || NUM_GENERAL_INPUT_CODES <= gi_code )
			return -1;

		return m_aGICodeToGMInputCode[gi_code];
		
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		{
			ar & m_aGICodeToGMInputCode[i];
		}

	}
};


/// slower lookup, less memory usage
class CGM_SmallKeybind : public CGM_Keybind
{
	std::map<int,int> m_mapGICodeToGMInputCode;

public:

	Result::Name Assign( int gi_code, CGM_Input::InputCode gui_input_code )
	{
		if( gi_code < 0 || NUM_GENERAL_INPUT_CODES <= gi_code )
			return Result::INVALID_ARGS;

		m_mapGICodeToGMInputCode[gi_code] = gui_input_code;

		return Result::SUCCESS;
	}

	int GetInputCode( int gi_code ) const
	{
		if( gi_code < 0 || NUM_GENERAL_INPUT_CODES <= gi_code )
			return -1;

		std::map<int,int>::const_iterator itr
			= m_mapGICodeToGMInputCode.find( gi_code );

		if( itr != m_mapGICodeToGMInputCode.end() )
			return itr->second;
		else
			return -1;
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_mapGICodeToGMInputCode;
	}
};



#endif /* __GM_Keybind_HPP__ */
