#ifndef  __NamedResourceHandle_H__
#define  __NamedResourceHandle_H__


#include "amorphous/Support/Serialization/Serialization.hpp"
#include <string>


namespace amorphous
{

using namespace serialization;


class CNamedResourceHandle : public IArchiveObjectBase
{
protected:

	/// name of the resource
	std::string m_Name;

	/// index to the resource
	int m_iIndex;

public:

	CNamedResourceHandle() : m_iIndex(INVALID_INDEX) {}

	CNamedResourceHandle( const std::string& name ) : m_Name(name), m_iIndex(UNINITIALIZED) {}

//	inline int GetIndex() { return m_iIndex; }

	inline const std::string &GetName() const { return m_Name; }

	inline void SetName( const std::string& name )
	{
		m_Name = name;
		m_iIndex = UNINITIALIZED;	// clear the index when a new name is set
	}

	inline virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		std::string name;
		if( ar.GetMode() == IArchive::MODE_INPUT )
		{
			ar & name;
			SetName( name );
		}
		else
		{
			name = GetName();
			ar & name;
		}
	}

	enum eSpecialIndexVal { INVALID_INDEX = -1, UNINITIALIZED = -2 };
};


} // amorphous



#endif		/*  __NamedResourceHandleR_H__  */
