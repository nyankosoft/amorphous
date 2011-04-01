#ifndef  __GAMETEXTSYSTEM_H__
#define  __GAMETEXTSYSTEM_H__


#include <string>
#include "GameTextSet.hpp"
#include "Support/Serialization/Serialization.hpp"


//----------------------------------------------------------------
// SGameTextComponent
// CGameTextSet
// CGameTextManager
//----------------------------------------------------------------

#define GTC_NUM_MAXLETTERS_PER_LINE 64


#define TEXTSET_INVALID_INDEX -1

class CGameTextManager
{
	int m_iNumGameTextSets;
	CGameTextSet *m_paGameTextSet;

public:
	CGameTextManager();
	~CGameTextManager();
	void Release();
	bool LoadTextDataFromFile( char* pcFilename );

	int GetNumTextSets() { return m_iNumGameTextSets; }
	int GetIndexForGameTextSet( char* pcTag );

	CGameTextSet *GetTextSet( int iIndex )
	{
		if( iIndex < 0 || m_iNumGameTextSets <= iIndex )
			return NULL;
		
		return &m_paGameTextSet[iIndex];
	}
};

#endif		/*  __GAMETEXTSYSTEM_H__  */
