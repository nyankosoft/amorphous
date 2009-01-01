
#include "MeshBoneController_Aircraft.h"
#include "Graphics/D3DXSMeshObject.h"
#include "PseudoAircraftSimulator.h"


void CMeshBoneController_Flap::Init()
{
	if( !m_pTargetMesh )
		return;

	if( m_vecBoneControlParam.size() != 2 )
		return;

//	m_vecBoneControlParam.resize( 2 );

//	string bone_name[2] = { "FlapR", "FlapL" };
	int i, num_bones = 2;
	for( i=0; i<num_bones; i++ )
	{
//		const CMM_Bone& bone = m_pTargetMesh->GetBone( bone_name[i] );
		const CMM_Bone& bone = m_pTargetMesh->GetBone( m_vecBoneControlParam[i].Name );
		if( bone != CMM_Bone::NullBone() )
		{
			m_vecBoneControlParam[i].MatrixIndex = bone.GetMatrixIndex();
			m_vecBoneControlParam[i].vRotationAxis = Vec3GetNormalized( bone.GetLocalOffset() );
		}
	}

}


void CMeshBoneController_Flap::UpdateTransforms()
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



void CMeshBoneController_VFlap::Init()
{
	if( !m_pTargetMesh )
		return;

	vector<string> bone_name;
	size_t i, num_bones = m_vecBoneControlParam.size();

/*	if( m_Type == TYPE_SINGLE )
	{
		bone_name.push_back( "VFlap" );
	}
	else
	{
		bone_name.push_back( "VFlapR" );
		bone_name.push_back( "VFlapL" );
	}*/

//	int i, num_bones = bone_name.size();
//	m_vecBoneControlParam.resize( num_bones );
	for( i=0; i<num_bones; i++ )
	{
//		const CMM_Bone& bone = m_pTargetMesh->GetBone( bone_name[i] );
		const CMM_Bone& bone = m_pTargetMesh->GetBone( m_vecBoneControlParam[i].Name );
		if( bone != CMM_Bone::NullBone() )
		{
			m_vecBoneControlParam[i].MatrixIndex = bone.GetMatrixIndex();
			m_vecBoneControlParam[i].vRotationAxis = Vec3GetNormalized( bone.GetLocalOffset() );
		}
	}

}


void CMeshBoneController_VFlap::UpdateTransforms()
{
	const float current_yaw_accel = m_pSimulator->GetYawAccel();

	const float factor = m_fAnglePerAccel;
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

/*
void CMeshBoneController_Rotor::Update( float dt )
{
	m_fRotorAngle = rotor_speed * dt;
}
*/


void CMeshBoneController_Rotor::Init()
{
	if( !m_pTargetMesh )
		return;

	if( m_vecBoneControlParam.size() != 1 )
		return;

	const CMM_Bone& bone = m_pTargetMesh->GetBone( m_vecBoneControlParam[0].Name );
	if( bone != CMM_Bone::NullBone() )
	{
		m_vecBoneControlParam[0].MatrixIndex = bone.GetMatrixIndex();
		m_vecBoneControlParam[0].vRotationAxis = Vec3GetNormalized( bone.GetLocalOffset() );
	}

}


void CMeshBoneController_Rotor::UpdateTransforms()
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
