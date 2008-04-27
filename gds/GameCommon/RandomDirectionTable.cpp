
#include "RandomDirectionTable.h"

// definition of the singleton instance
CRandomDirectionTable CRandomDirectionTable::ms_SingletonInstance_;


CRandomDirectionTable::CRandomDirectionTable()
{
	int i=0, h, v;
	D3DXMATRIX matRot;
	D3DXVECTOR3 vDir;
	float fPitch, fYaw;

	// store the random direction for smoke particles
	for(v=0; v<32; v++)
	{
		fPitch = ( ((float)v) / 32.0f - 0.5f ); // -0.5 <= fPitch < 0.5
		fPitch *= D3DX_PI;	// -PI/2 <= fPitch < PI/2
		fPitch *= 0.8f;	// avoid right angle

		for(h=0; h<32; h++)
		{
			fYaw = ((float)h) / 32.0f; // 0.0 <= fYaw < 1.0
			fYaw *= (2.0f * D3DX_PI);  // 0 <= fPitch < 2*PI

			D3DXMatrixRotationYawPitchRoll( &matRot, fYaw, fPitch, 0 );
			vDir = D3DXVECTOR3(0,0,1);
			D3DXVec3TransformCoord( &m_avRandomDirectionTable[i++], &vDir, &matRot );
		}
	}
}


//CRandomDirectionTable::~CRandomDirectionTable() {}
