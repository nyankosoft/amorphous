#ifndef __GTS_GAMETEXTSET_H__
#define __GTS_GAMETEXTSET_H__


#include <vector>
#include "../base.hpp"
#include "3DMath/Vector2.hpp"
//#include "Graphics/32BITCOLOR.h"
#include "Support/Serialization/Serialization.hpp"


namespace amorphous
{
using namespace serialization;


class CFontBase;
class CGameEventCompiler;
class CGameTextWindow;
class CGameTextSet;


enum eGameTextComponentType
{
	GTC_TEXTLINE = 0,
	GTC_IMAGE,
	GTC_CLEARTEXT,
	GTC_WAITUSERRESPONSE,
	GTC_TERMINATE
};


// sotres a line of texts
class CGameTextComponent : public IArchiveObjectBase
{
	char m_Type;

	std::string m_strText;

	U32 m_TextColor;

//	SGameTextComponent *pNextString;

public:

	CGameTextComponent() { SetDefault(); }

	void SetDefault();

	inline char GetType() { return m_Type; }

	inline int GetLineLength() { return m_strText.size(); }

	void DrawLine( Vector2& rvPos, int iNumChars, CFontBase *pFont );

	void Serialize( IArchive& ar, const unsigned int version );


	friend class CGameEventCompiler;
	friend class CGameTextSet;
//	friend class CGameTextWindow;

};


class CGameTextSet : public IArchiveObjectBase
{
	std::vector<CGameTextComponent> m_vecTextComponent;

//	int m_iNumComponents;
//	SGameTextComponent* m_paTextComponent;

public:
	CGameTextSet()
	{
//		m_iNumComponents = 0;
//		m_paTextComponent = NULL;
	}

	~CGameTextSet() { Release(); }
	void Release();

	inline CGameTextComponent* GetComponent(int iComponentIndex);// { return m_vecTextComponent[iComponentIndex]; }

	void AddTextComponent( CGameTextComponent& rComponent );

	void SetTextComponents( CGameTextComponent* paTextComponent, int iNumTextComponents );

	void Serialize( IArchive& ar, const unsigned int version );
};


//=========================== inline implementations ===========================

inline CGameTextComponent* CGameTextSet::GetComponent(int iComponent)
{
	return &m_vecTextComponent[iComponent];
}

} // namespace amorphous



#endif  /*  __GTS_GAMETEXTSET_H__  */