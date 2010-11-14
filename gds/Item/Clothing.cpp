#include "Clothing.hpp"

#include "3DMath/MathMisc.hpp"
#include "Graphics/3DGameMath.hpp"
#include "GameCommon/3DActionCode.hpp"
#include "Support/MTRand.hpp"
#include "XML/XMLNodeReader.hpp"
#include "Input/InputHandler.hpp"

#include "Stage/Stage.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"

using namespace std;


CClothing::CClothing()
:
m_ApplyClothSimulation(false),
m_fProjectionMatrixOffset(0)
{
}

void CClothing::Update( float dt )
{
}


bool CClothing::HandleInput( int input_code, int input_type, float fParam )
{
	return false;
}


void CClothing::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );
}


void CClothing::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGameItem::LoadFromXMLNode( reader );

	reader.GetAttributeValue( "cloth_simulation", m_ApplyClothSimulation );
}
