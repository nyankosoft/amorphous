#ifndef __LoadFromXMLNode_3DMath_H__
#define __LoadFromXMLNode_3DMath_H__


#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/XML/XMLNode.hpp"


namespace amorphous
{


/*
goal
<Pose>
	<Position>0.0 2.0 1.0</Position>
	<Rotation>
		<Heading>30</Heading>
		<Pitch>20</Pitch>
		<Bank>0</Bank>
	</Rotation>
</Pose>
*/
inline void LoadFromXMLNode( XMLNode& reader, Vector3& dest )
{
	std::string pos_str;
	dest = Vector3(0,0,0);
	reader.GetChildElementTextContent( "Position", pos_str );
	sscanf( pos_str.c_str(), "%f %f %f", &dest.x, &dest.y, &dest.z );
}


// TODO: support rotation orders
inline void LoadFromXMLNode( XMLNode& reader, Matrix33& dest )
{
	float heading = 0, pitch = 0, bank = 0;
	Matrix33 matRotation = Matrix33Identity();

	if( reader.GetChildElementTextContent( "Heading", heading ) )
		matRotation = Matrix33RotationY( deg_to_rad(heading) );

	if( reader.GetChildElementTextContent( "Pitch", pitch ) )
		matRotation = Matrix33RotationX( deg_to_rad(pitch) );

	if( reader.GetChildElementTextContent( "Bank", bank ) )
		matRotation = Matrix33RotationZ( deg_to_rad(bank) );

	dest = matRotation;
}


inline void LoadFromXMLNode( XMLNode& reader, Matrix34& dest )
{
	LoadFromXMLNode( reader, dest.vPosition );
	LoadFromXMLNode( reader.GetChild( "Rotation" ), dest.matOrient );
}

} // amorphous



#endif /* __LoadFromXMLNode_3DMath_H__ */
