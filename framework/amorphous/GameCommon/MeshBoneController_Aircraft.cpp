#include "MeshBoneController_Aircraft.hpp"
#include "MeshBoneControllerFactory.hpp"
#include "amorphous/Graphics/Mesh/SkeletalMesh.hpp"
#include "amorphous/PseudoAircraftSimulator.hpp"
#include "amorphous/XML/XMLNode.hpp"


namespace amorphous
{

using namespace std;

/*
/// uniformly-accelarated variable
/// variable for uniformly-accelarated motion
template<typename T>
class uav
{
public:

	// [out]
	T vel;
	T x;

	// [in]
	T accel;
	T min_vel;
	T max_vel;
	T min_x;
	T max_x;

	void Update( float dt )
	{
		vel += accel * dt;
		Limit( vel, min_vel, max_vel );
		x   += vel * dt;
		Limit( x, min_x, max_x );
	}
};
*/


void MeshBoneController_AircraftBase::Init( const SkeletalMesh& target_mesh )
{
//	if( !m_pTargetMesh )
//		return;

	size_t i, num_bones = m_vecBoneControlParam.size();

	for( i=0; i<num_bones; i++ )
	{
//		const MeshBone& bone = m_pTargetMesh->GetBone( m_vecBoneControlParam[i].Name );
		const MeshBone& bone = target_mesh.GetBone( m_vecBoneControlParam[i].Name );
		if( bone == MeshBone::NullBone() )
		{
			LOG_PRINT_WARNING( fmt_string(" The bone named '%s' was not found in the skeleton of the target mesh.",m_vecBoneControlParam[i].Name.c_str()) );
			continue;
		}

		m_vecBoneControlParam[i].MatrixIndex = bone.GetMatrixIndex();
		m_vecBoneControlParam[i].vRotationAxis = Vec3GetNormalized( bone.GetLocalOffset() );
	}
}



//=====================================================================================
// MeshBoneController_VFlap
//=====================================================================================

/**
 bone names are usually, "FlapR" and "FlapL"


*/
void MeshBoneController_Flap::Init( const SkeletalMesh& target_skeletal_mesh )
{
	MeshBoneController_AircraftBase::Init( target_skeletal_mesh );
}


void MeshBoneController_Flap::UpdateTransforms()
{
/*
	float pitch_factor = current_pitch_accel / ( current_pitch_accel > 0 ) ? pitch_accel.max ? pitch_accel.min;
	float roll_factor = current_roll_accel / ( current_roll_accel > 0 ) ? roll_accel.max ? roll_accel.min;

	float factor_R = GetLimited( -pitch_factor + roll_factor, -1.0f, 1.0f );
	float factor_L = GetLimited( -pitch_factor + roll_factor, -1.0f, 1.0f );
	angle_R *= factor_R > 0 ? param.angle.max : param.angle.min;
	angle_L *= factor_L > 0 ? param.angle.max : param.angle.min;
*/

	const float current_pitch_accel = m_pSimulator->GetPitchAccel();
	const float current_roll_accel  = m_pSimulator->GetRollAccel();

	const float pitch_factor = m_fAnglePerPitchAccel;
	const float roll_factor  = m_fAnglePerRollAccel;
	const float angle_R = -current_pitch_accel * pitch_factor + current_roll_accel * roll_factor;
	const float angle_L =  current_pitch_accel * pitch_factor + current_roll_accel * roll_factor;

	CBoneControlParam& rParamR = m_vecBoneControlParam[FLAP_R];
	CBoneControlParam& rParamL = m_vecBoneControlParam[FLAP_L];

/*	if( rParamL.pBone )
		rParamL.LocalTransform.matOrient = Matrix33RotationAxis( rParamL.vRotationAxis, angle_L );
	if( rParamR.pBone )
		rParamR.LocalTransform.matOrient = Matrix33RotationAxis( rParamR.vRotationAxis, angle_R );
*/
	Matrix33 matRot;
	if( 0 <= rParamR.MatrixIndex )
	{
		matRot = Matrix33RotationAxis( angle_R, rParamR.vRotationAxis );
//		m_pTargetMesh->SetLocalTransformToCache( rParamR.MatrixIndex, Matrix34( Vector3(0,0,0), matRot ) );
		rParamR.LocalTransform = Matrix34( Vector3(0,0,0), matRot );
	}

	if( 0 <= rParamL.MatrixIndex )
	{
		matRot = Matrix33RotationAxis( angle_L, rParamL.vRotationAxis );
//		m_pTargetMesh->SetLocalTransformToCache( rParamL.MatrixIndex, Matrix34( Vector3(0,0,0), matRot ) );
		rParamL.LocalTransform = Matrix34( Vector3(0,0,0), matRot );
	}
}


void MeshBoneController_Flap::LoadFromXMLNode( XMLNode& reader )
{
	MeshBoneControllerBase::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "AnglePerPitchAccel", m_fAnglePerPitchAccel );
	reader.GetChildElementTextContent( "AnglePerRollAccel",  m_fAnglePerRollAccel );

	m_vecBoneControlParam.resize( 2 );
	reader.GetChildElementTextContent( "Names/R", m_vecBoneControlParam[0].Name );
	reader.GetChildElementTextContent( "Names/L", m_vecBoneControlParam[1].Name );
}



//=====================================================================================
// MeshBoneController_VFlap
//=====================================================================================

/**
 bone names are usually,
 - m_Type == TYPE_SINGLE: "VFlap"
 - m_Type == TYPE_TWIN:   "VFlapR", "VFlapL"

*/
void MeshBoneController_VFlap::Init( const SkeletalMesh& target_skeletal_mesh )
{
	MeshBoneController_AircraftBase::Init( target_skeletal_mesh );
}


void MeshBoneController_VFlap::UpdateTransforms()
{
	const float current_yaw_accel = m_pSimulator->GetYawAccel();

	const float factor = m_fAnglePerYawAccel;
	const float angle = - current_yaw_accel * factor;

	size_t i, num_bones = m_vecBoneControlParam.size();
	Matrix33 matRot;
	for( i=0; i<num_bones; i++ )
	{
		CBoneControlParam& rParam = m_vecBoneControlParam[i];
		if( 0 <= rParam.MatrixIndex )
		{
			matRot = Matrix33RotationAxis( angle, rParam.vRotationAxis );
//			m_pTargetMesh->SetLocalTransformToCache( rParam.MatrixIndex, Matrix34( Vector3(0,0,0), matRot ) );
			rParam.LocalTransform = Matrix34( Vector3(0,0,0), matRot );
		}
	}
}


void MeshBoneController_VFlap::LoadFromXMLNode( XMLNode& reader )
{
	MeshBoneControllerBase::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "AnglePerYawAccel",  m_fAnglePerYawAccel );

	string vflap_type;
	reader.GetChildElementTextContent( "Type", vflap_type );
	if( vflap_type == "Single" )
	{
		m_vecBoneControlParam.resize( 1 );
		reader.GetChildElementTextContent( "Name", m_vecBoneControlParam[0].Name );
	}
	else if( vflap_type == "Twin" )
	{
		m_vecBoneControlParam.resize( 2 );
		reader.GetChildElementTextContent( "Names/R", m_vecBoneControlParam[0].Name );
		reader.GetChildElementTextContent( "Names/L", m_vecBoneControlParam[1].Name );
	}
	else
	{
		LOG_PRINT_ERROR( " An invalid vertical flap type: " + vflap_type );
	}
}



//=====================================================================================
// MeshBoneController_Rotor
//=====================================================================================

/*
void MeshBoneController_Rotor::Update( float dt )
{
	m_fRotorAngle = rotor_speed * dt;
}
*/


void MeshBoneController_Rotor::Init( const SkeletalMesh& target_skeletal_mesh )
{
	if( m_vecBoneControlParam.size() != 1 )
	{
		LOG_PRINT_WARNING( " m_vecBoneControlParam.size() != 1" );
		return;
	}

	MeshBoneController_AircraftBase::Init( target_skeletal_mesh );
}


void MeshBoneController_Rotor::Update( float dt )
{
	m_fAngle += m_fRotationSpeed * dt;
}


void MeshBoneController_Rotor::UpdateTransforms()
{
//	if( m_pSimulator->GetNumRotors
//	const float current_angle = m_pSimulator->GetRotor(0);
	const float current_angle = m_fCurrentRotationAngle;
/*
	float dt = 0.01f;
	float m_fCurrentAngle = 0;
	float rotor_speed = 100.0f;
	m_fCurrentAngle += rotor_speed * dt * ( m_RotationDirection == DIR_CW ? 1.0f : -1.0f );
*/

	Matrix33 matRot;

	CBoneControlParam& rParam = m_vecBoneControlParam[0];
	if( 0 <= rParam.MatrixIndex )
	{
		matRot = Matrix33RotationAxis( current_angle, rParam.vRotationAxis );
//		m_pTargetMesh->SetLocalTransformToCache( rParam.MatrixIndex, Matrix34( Vector3(0,0,0), matRot ) );
		rParam.LocalTransform = Matrix34( Vector3(0,0,0), matRot );
	}
}


void MeshBoneController_Rotor::LoadFromXMLNode( XMLNode& reader )
{
	MeshBoneControllerBase::LoadFromXMLNode( reader );

	string rot_dir;
	reader.GetChildElementTextContent( "RotationDirection", rot_dir );
	if( rot_dir == "CW" )       m_RotationDirection = DIR_CW;
	else if( rot_dir == "CCW" ) m_RotationDirection = DIR_CCW;
	else m_RotationDirection = DIR_CW;

	m_vecBoneControlParam.resize( 1 );
	reader.GetChildElementTextContent( "Name", m_vecBoneControlParam[0].Name );
}


//=====================================================================================

void CConstraintComponent::LoadFromXMLNode( XMLNode& reader )
{
	reader.GetChildElementTextContent( "Name", Name );
	reader.GetChildElementTextContent( "Angle/AllowedRange/Min", AllowedAngleRange.min );
	reader.GetChildElementTextContent( "Angle/AllowedRange/Max", AllowedAngleRange.max );
}


//=====================================================================================

/*
void MeshBoneController_Cover::Init( std::vector<MeshBoneController_Cover>& vecComponent )
{
	for( size_t i=0; i<m_vecConstraint.size(); i++ )
	{
		for( size_t j=0; j<vecComponent.size(); j++ )
		{
//			if( m_vecConstraint[i].Name == vecComponent[j]
		}
	}
}
*/

void MeshBoneController_Cover::Open()
{
	m_Angle.target = m_fOpenAngle;
	m_State = CAircraftComponentState::OPENING;
}


void MeshBoneController_Cover::Close()
{
	m_Angle.target = 0;
	m_State = CAircraftComponentState::CLOSING;
}


void MeshBoneController_Cover::Init( const SkeletalMesh& target_skeletal_mesh )
{
	MeshBoneController_AircraftBase::Init( target_skeletal_mesh );

	if( m_pParent )
	{
		for( size_t i=0; i<m_vecConstraint.size(); i++ )
		{
			m_vecConstraint[i].m_pComponent
				= m_pParent->GetComponent( m_vecConstraint[i].Name );
		}
	}
}


void MeshBoneController_Cover::UpdatedFromRequestedState( CAircraftComponentState::Name requested_state )
{
	switch( requested_state )
	{
	case CAircraftComponentState::OPEN:
	case CAircraftComponentState::OPENING:
		switch( m_State )
		{
		case CAircraftComponentState::OPEN:
		default:
			break;
		case CAircraftComponentState::OPENING:
			if( fabsf(m_Angle.target - m_Angle.current) < 0.001f )
				m_State = CAircraftComponentState::OPEN;
			break;
		case CAircraftComponentState::CLOSED:
		case CAircraftComponentState::CLOSING:
			if( SatisfyConstraints() )
			{
				// rotate the shaft / cover around the axis (open)
				Open();
			}
			break;
		}
	case CAircraftComponentState::CLOSED:
	case CAircraftComponentState::CLOSING:
		switch( m_State )
		{
		case CAircraftComponentState::OPEN:
		case CAircraftComponentState::OPENING:
			if( SatisfyConstraints() )
			{
				// rotate the shaft / cover around the axis (close)
				Close();
			}
			break;
		case CAircraftComponentState::CLOSED:
		default:
			break;
		case CAircraftComponentState::CLOSING:
			if( fabsf(m_Angle.target - m_Angle.current) < 0.001f )
				m_State = CAircraftComponentState::CLOSED;
			break;
		}
		break;
	default:
		break;
	}
}


void MeshBoneController_Cover::Update( float dt )
{
	if( m_pParent )
		UpdatedFromRequestedState( m_pParent->GetRequestedState() );

	// update rotation angle of the component
	m_Angle.Update( dt );
}


void MeshBoneController_Cover::UpdateTransforms()
{
	float angle = GetCurrentAngle();
	Matrix33 matRot;
	size_t i, num_bones = m_vecBoneControlParam.size();
	for( i=0; i<num_bones; i++ )
	{
		CBoneControlParam& rParam = m_vecBoneControlParam[i];
		if( 0 <= rParam.MatrixIndex )
		{
			matRot = Matrix33RotationAxis( angle, rParam.vRotationAxis );
//			m_pTargetMesh->SetLocalTransformToCache( rParam.MatrixIndex, Matrix34( Vector3(0,0,0), matRot ) );
			rParam.LocalTransform = Matrix34( Vector3(0,0,0), matRot );
		}
	}
}


bool MeshBoneController_Cover::SatisfyConstraints()
{
	float current_angle = GetCurrentAngle();
	for( size_t i=0; i<m_vecConstraint.size(); i++ )
	{
		if( !m_vecConstraint[i].m_pComponent )
			continue;

		// check if the rotation angle of constraint component is in a certain range
//		if( !m_vecConstraint[i].AllowedAngleRange.IsInRange( vecComponent[m_vecConstraint[i].Index].GetCurrentAngle() ) )
		if( !m_vecConstraint[i].AllowedAngleRange.IsInRange( m_vecConstraint[i].m_pComponent->GetCurrentAngle() ) )
			return false;
	}

	return true;
}


const string MeshBoneController_Cover::GetName()
{
	if( 1 <= m_vecBoneControlParam.size() )
		return m_vecBoneControlParam[0].Name;
	else
		return "";
}


void MeshBoneController_Cover::LoadFromXMLNode( XMLNode& reader )
{
	MeshBoneController_AircraftBase::LoadFromXMLNode( reader );

	float open_angle_deg = 0;
	reader.GetChildElementTextContent( "OpenAngle", open_angle_deg );
	m_fOpenAngle = deg_to_rad( open_angle_deg );

	vector<XMLNode> vecReader = reader.GetImmediateChildren( "Constraint" );
	m_vecConstraint.resize( vecReader.size() );
	for( size_t i=0; i<vecReader.size(); i++ )
	{
		m_vecConstraint[i].LoadFromXMLNode( vecReader[i] );
	}
}

/*
<GearUnit>
	<Shaft>
		<Name>FrontShaft</Name>
		<OpenAngle>100</OpenAngle>
		<Tire>
			<Name>FrontTire</Name>
		</Tire>
		<!-- start moving the shaft only if the front gear cover is rotated by between 90 and 100 -->
		<Constraint>
			<Name>FrontGearCoverF</Name>
			<Angle>
				<AllowedRange>
					<Min>90</Min>
					<Max>100</Max>
				</AllowedRange>
			</Angle>
		</Constraint>
		<Constraint>
			<Name>FrontGearCoverR</Name>
			<AngleLessThan>20</AngleLessThan>
			</AllowedAngleRange>
		</Constraint>
	</Shaft>
	<Cover>
		<Name>FrontGearCoverF</Name>
		<OpenAngle>100</OpenAngle>
	</Cover>
	<Cover>
		<Name>FrontGearCoverR</Name>
		<OpenAngle>100</OpenAngle>
	</Cover>
</GearUnit>
*/
//=====================================================================================
// MeshBoneController_GearUnit
//=====================================================================================
void MeshBoneController_GearUnit::Init( const SkeletalMesh& target_skeletal_mesh )
{
	for( size_t i=0; i<m_vecpComponent.size(); i++ )
	{
		m_vecpComponent[i]->SetParent( this );
		m_vecpComponent[i]->Init( target_skeletal_mesh );
	}
}


void MeshBoneController_GearUnit::Update( float dt )
{
//	m_ShaftAngle.Update( dt );

	for( size_t i=0; i<m_vecpComponent.size(); i++ )
	{
		m_vecpComponent[i]->Update( dt );
	}
/*
	for( size_t i=0; i<m_vecComponent.size(); i++ )
	{
		switch( m_vecComponent[i].m_State )
		{
		case CAircraftComponentState::OPEN:
			break;
		case CAircraftComponentState::OPENING:
			if( m_vecComponent[i].CanOpen( m_vecComponent ) )
			{
				// rotate the shaft / cover around the axis
			}
			break;
		case CAircraftComponentState::CLOSED:
			break;
		case CAircraftComponentState::CLOSING:
			break;
		}
	}*/
}


void MeshBoneController_GearUnit::UpdateTransforms()
{
	for( size_t i=0; i<m_vecpComponent.size(); i++ )
		m_vecpComponent[i]->UpdateTransforms();
}


void MeshBoneController_GearUnit::SetTargetMesh( std::shared_ptr<SkeletalMesh> pTargetMesh )
{
//	MeshBoneControllerBase::SetTargetMesh( pTargetMesh );

//	for( size_t i=0; i<m_vecpComponent.size(); i++ )
//		m_vecpComponent[i]->SetTargetMesh( pTargetMesh );
}


void MeshBoneController_GearUnit::Open()
{
//	m_OpenStartTime = m_CurrentTime;
//	m_ShaftAngle.target = m_fShaftAngleOpen;

	m_RequestedState = CAircraftComponentState::OPEN;
}


void MeshBoneController_GearUnit::Close()
{
//	m_CloseStartTime = m_CurrentTime;
//	m_ShaftAngle.target = m_fShaftAngleClosed;

	m_RequestedState = CAircraftComponentState::CLOSED;
}


shared_ptr<MeshBoneController_Cover> MeshBoneController_GearUnit::GetComponent( const std::string& component_name )
{
	for( size_t i=0; i<m_vecpComponent.size(); i++ )
	{
		if( m_vecpComponent[i]->GetName() == component_name )
		{
			return m_vecpComponent[i];
		}
	}

	return shared_ptr<MeshBoneController_Cover>();
}


void MeshBoneController_GearUnit::LoadFromXMLNode( XMLNode& reader )
{
	MeshBoneControllerBase::LoadFromXMLNode( reader );

	vector<XMLNode> vecCover = reader.GetImmediateChildren( "Cover" );
	vector<XMLNode> vecShaft = reader.GetImmediateChildren( "Shaft" );
	m_vecpComponent.resize( vecCover.size() + vecShaft.size() );

	for( size_t i=0; i<vecCover.size(); i++ )
	{
		m_vecpComponent[i] = shared_ptr<MeshBoneController_Cover>( new MeshBoneController_Cover() );
		m_vecpComponent[i]->LoadFromXMLNode( vecCover[i] );
	}

	for( size_t i=0; i<vecShaft.size(); i++ )
	{
		size_t index = vecCover.size() + i;
		m_vecpComponent[index] = shared_ptr<MeshBoneController_Cover>( new MeshBoneController_Shaft() );
		m_vecpComponent[index]->LoadFromXMLNode( vecShaft[i] );
	}
}


void MeshBoneController_GearUnit::Serialize( IArchive& ar, const unsigned int version )
{
	MeshBoneController_AircraftBase::Serialize( ar, version );

	MeshBoneControllerFactory factory;
	ar.Polymorphic( m_vecpComponent, factory );
//	ar & m_TirePose & m_ShaftPose & m_vecCoverPose;
//	ar & m_fTireAngle & m_fShaftOpenAngle;
}


} // namespace amorphous
