#include "GraphicsComponentCollector.hpp"

using namespace std;


//=====================================================================================
// CGraphicsComponent
//=====================================================================================

CGraphicsComponent::CGraphicsComponent()
:
m_RegisteredToGraphicsResourceManager(false)
{
	CGraphicsComponentCollector::Get()->AddComponent( this );

	m_RegisteredToGraphicsResourceManager = true;
}

CGraphicsComponent::~CGraphicsComponent()
{
	if( m_RegisteredToGraphicsResourceManager )
	{
		CGraphicsComponentCollector::Get()->RemoveComponent( this );

		m_RegisteredToGraphicsResourceManager = false;
	}
}


const CGraphicsParameters& CGraphicsComponent::GetCurrentGraphicsParams()
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
	// Remove all the graphics components to avoid instanciating the singleton instance
	// of this class in the dtor of derived classes of CGraphicsComponent
	size_t i, num_components = m_vecpGraphicsComponent.size();
	for( i=0; i<num_components; i++ )
	{
		m_vecpGraphicsComponent[i]->m_RegisteredToGraphicsResourceManager = false;
	}

	m_vecpGraphicsComponent.clear();
}


void CGraphicsComponentCollector::AddComponent( CGraphicsComponent* pComponent )
{
	m_vecpGraphicsComponent.push_back( pComponent );
}


bool CGraphicsComponentCollector::RemoveComponent( CGraphicsComponent* pComponent )
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
