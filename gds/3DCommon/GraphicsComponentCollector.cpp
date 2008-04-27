
#include "GraphicsComponentCollector.h"

using namespace std;


//=====================================================================================
// CGraphicsComponent
//=====================================================================================

CGraphicsComponent::CGraphicsComponent()
{
	CGraphicsComponentCollector::Get()->AddComponent( this );
}

CGraphicsComponent::~CGraphicsComponent()
{
	CGraphicsComponentCollector::Get()->DeleteComponent( this );
}


const CGraphicsParameters& CGraphicsComponent::GetCurrentGraphicsParams() const
{
	return CGraphicsComponentCollector::Get()->GetGraphicsParams();
}


//=====================================================================================
// CGraphicsComponentCollector
//=====================================================================================

// define singleton instance
CSingleton<CGraphicsComponentCollector> CGraphicsComponentCollector::m_obj;

CGraphicsComponentCollector::CGraphicsComponentCollector()
{
	CGraphicsParameters params;
	params.ScreenWidth  = 800;
	params.ScreenHeight = 600;
	params.bWindowed    = true;

	SetGraphicsPargams( params );
}


CGraphicsComponentCollector::~CGraphicsComponentCollector()
{
}


void CGraphicsComponentCollector::AddComponent( CGraphicsComponent* pComponent )
{
	m_vecpGraphicsComponent.push_back( pComponent );
}


bool CGraphicsComponentCollector::DeleteComponent( CGraphicsComponent* pComponent )
{
	size_t i, num_components = m_vecpGraphicsComponent.size();
	for( i=0; i<num_components; i++ )
	{
		if( m_vecpGraphicsComponent[i] == pComponent )
		{
			m_vecpGraphicsComponent.erase( m_vecpGraphicsComponent.begin() + i );
			return true;
		}
	}

	return false;	// the requested component was not found
}


void CGraphicsComponentCollector::ReleaseGraphicsResources()
{
	size_t i, num_components = m_vecpGraphicsComponent.size();
	for( i=0; i<num_components; i++ )
	{
		m_vecpGraphicsComponent[i]->ReleaseGraphicsResources();
	}
}


void CGraphicsComponentCollector::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	m_GraphicsParam = rParam;

	size_t i, num_components = m_vecpGraphicsComponent.size();
	for( i=0; i<num_components; i++ )
	{
		m_vecpGraphicsComponent[i]->LoadGraphicsResources( rParam );
	}
}
