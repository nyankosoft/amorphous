#include "Clothing.hpp"

#include "3DMath/MathMisc.hpp"
#include "3DMath/3DGameMath.hpp"
#include "GameCommon/3DActionCode.hpp"
#include "Support/MTRand.hpp"
#include "XML/XMLNode.hpp"
#include "Input/InputHandler.hpp"

#include "Stage/Stage.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"


namespace amorphous
{

using namespace std;


Clothing::Clothing()
:
m_ApplyClothSimulation(false),
m_fProjectionMatrixOffset(0)
{
}

void Clothing::Update( float dt )
{
}


bool Clothing::HandleInput( int input_code, int input_type, float fParam )
{
	return false;
}


void Clothing::Serialize( IArchive& ar, const unsigned int version )
{
	GameItem::Serialize( ar, version );
}


void Clothing::LoadFromXMLNode( XMLNode& reader )
{
	GameItem::LoadFromXMLNode( reader );

	reader.GetAttributeValue( "cloth_simulation", m_ApplyClothSimulation );
}


} // namespace amorphous
