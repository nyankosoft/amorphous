#include "Clothing.hpp"

#include "amorphous/3DMath/MathMisc.hpp"
#include "amorphous/3DMath/3DGameMath.hpp"
#include "amorphous/GameCommon/3DActionCode.hpp"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/XML/XMLNode.hpp"
#include "amorphous/Input/InputHandler.hpp"

#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/CopyEntity.hpp"
#include "amorphous/Stage/CopyEntityDesc.hpp"
#include "amorphous/Stage/GameMessage.hpp"


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
