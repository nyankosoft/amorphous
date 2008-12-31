
#include "GameTextSet.h"

#include "3DCommon/Font/Font.h"


//===========================================================================
// CGameTextComponent
//===========================================================================

void CGameTextComponent::SetDefault()
{
	m_strText = "";
	m_Type = 0;
	m_TextColor = 0xFFFFFFFF;
//	pNextString = NULL;
}


void CGameTextComponent::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_Type;
	ar & m_strText;
	ar & m_TextColor;
}


void CGameTextComponent::DrawLine( D3DXVECTOR2& rvPos, int iNumChars, CFont *pFont )
{
	int iStrLength = strlen(m_strText.c_str());
	if( iStrLength <= iNumChars )
	{
		float fStrWidth = 0;
		/*fStrWidth = */pFont->DrawText( m_strText.c_str(), rvPos, m_TextColor );
/*		if( pNextString )
		{
			D3DXVECTOR2 vNextPos = rvPos + D3DXVECTOR2(fStrWidth,0);
			pNextString->DrawLine( vNextPos, iNumChars - iStrLength, pFont );
		}*/
	}
	else
	{
		char temp_char = m_strText[iNumChars];
		m_strText[iNumChars] = '\0';
		pFont->DrawText( m_strText.c_str(), rvPos, m_TextColor );
		m_strText[iNumChars] = temp_char;
	}
}



//===========================================================================
// CGameTextSet
//===========================================================================

void CGameTextSet::Release()
{
//	SafeDeleteArray( m_paTextComponent );
//	m_iNumComponents = 0;
}


void CGameTextSet::SetTextComponents( CGameTextComponent* paTextComponent, int iNumTextComponents )
{
	int i;
	for( i=0; i<iNumTextComponents; i++ )
	{
		m_vecTextComponent.push_back( paTextComponent[i] );
	}
}

// TODO: support CGameTextComponent::m_NextComponent
void CGameTextSet::AddTextComponent( CGameTextComponent& rComponent )
{
	m_vecTextComponent.push_back( rComponent );
}


void CGameTextSet::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecTextComponent;
}