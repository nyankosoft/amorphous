#include "GraphicsComponentCollector.hpp"


namespace amorphous
{

using namespace std;


//=====================================================================================
// GraphicsComponent
//=====================================================================================

GraphicsComponent::GraphicsComponent()
:
m_RegisteredToGraphicsResourceManager(false)
{
	GraphicsComponentCollector::Get()->AddComponent( this );

	m_RegisteredToGraphicsResourceManager = true;
}

GraphicsComponent::~GraphicsComponent()
{
	if( m_RegisteredToGraphicsResourceManager )
	{
		GraphicsComponentCollector::Get()->RemoveComponent( this );

		m_RegisteredToGraphicsResourceManager = false;
	}
}


const GraphicsParameters& GraphicsComponent::GetCurrentGraphicsParams()
{
	return GraphicsComponentCollector::Get()->GetGraphicsParams();
}


//=====================================================================================
// GraphicsComponentCollector
//=====================================================================================

// define singleton instance
singleton<GraphicsComponentCollector> GraphicsComponentCollector::m_obj;

GraphicsComponentCollector::GraphicsComponentCollector()
{
	GraphicsParameters params;
	params.ScreenWidth  = 800;
	params.ScreenHeight = 600;
	params.bWindowed    = true;

	SetGraphicsPargams( params );
}


GraphicsComponentCollector::~GraphicsComponentCollector()
{
	// Remove all the graphics components to avoid instanciating the singleton instance
	// of this class in the dtor of derived classes of GraphicsComponent
	size_t i, num_components = m_vecpGraphicsComponent.size();
	for( i=0; i<num_components; i++ )
	{
		m_vecpGraphicsComponent[i]->m_RegisteredToGraphicsResourceManager = false;
	}

	m_vecpGraphicsComponent.clear();
}


void GraphicsComponentCollector::AddComponent( GraphicsComponent* pComponent )
{
	m_vecpGraphicsComponent.push_back( pComponent );
}


bool GraphicsComponentCollector::RemoveComponent( GraphicsComponent* pComponent )
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


void GraphicsComponentCollector::ReleaseGraphicsResources()
{
	size_t i, num_components = m_vecpGraphicsComponent.size();
	for( i=0; i<num_components; i++ )
	{
		m_vecpGraphicsComponent[i]->ReleaseGraphicsResources();
	}
}


void GraphicsComponentCollector::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	m_GraphicsParam = rParam;

	size_t i, num_components = m_vecpGraphicsComponent.size();
	for( i=0; i<num_components; i++ )
	{
		m_vecpGraphicsComponent[i]->LoadGraphicsResources( rParam );
	}
}


} // namespace amorphous
