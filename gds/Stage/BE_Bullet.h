
#ifndef	__BASEENTITYBULLET_H__
#define __BASEENTITYBULLET_H__

#include "BaseEntity.h"
#include "Stage.h"
#include "BaseEntityHandle.h"

#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/TextureHandle.h"


class CBE_Bullet : public CBaseEntity
{
	std::string m_BillboardTextureFilepath;
	CTextureHandle m_BillboardTexture;

	float m_fBillboardRadius;
	TEXTUREVERTEX m_avBillboardRect[4];

	/// distance which the bullet can cover
	float m_fMaxRange;

	float m_fBulletPower;

	float m_fPenetration;

	float m_fSpeedAttenuAtReflection;

	float m_fNumMaxReflections;

//	float m_fMuzzleSpeed;

	CBaseEntityHandle m_Spark;
	CBaseEntityHandle m_Light;

private:

	void BulletMove(CCopyEntity* pCopyEnt);

	void PenetrationMove(CCopyEntity* pCopyEnt);

	void PenetrationIteration( CCopyEntity* pCopyEnt, float fFrameTime_Left );

	void ReflectiveMove(CCopyEntity* pCopyEnt);

	void Move( CCopyEntity* pCopyEnt );

public:

	CBE_Bullet();
	~CBE_Bullet() {}
	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	void Act(CCopyEntity* pCopyEnt);
	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	//void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

    void Draw3DObjectBullet(CCopyEntity* pCopyEnt);

	/// create decal/spark/smoke entities and make a hit sound
	/// according to the type of the surface material
	void OnBulletHit( CCopyEntity* pCopyEnt, STrace& trace );

	void Draw(CCopyEntity* pCopyEnt);
	void DrawBillboradTexture(CCopyEntity* pCopyEnt);

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_BULLET; }

	virtual void Serialize( IArchive& ar, const unsigned int version );


//	inline void CreateBulletHoleDecal(D3DXVECTOR3& rvPosition, D3DXVECTOR3& rvNormal);
};


// ================================ inline implementations ================================ 

/*
inline void CBE_Bullet::CreateBulletHoleDecal(D3DXVECTOR3& rvPosition, D3DXVECTOR3& rvNormal)
{
	// When a bullet hit a polygon of the map, we put the decal to make a bullet hole.
	// Bullet holes are made on only static geometry. When it hit other entities,
	// no holes are made on them because they can move and disappear.
	this->m_pStage->CreateEntity( m_BulletHole, rvPosition, D3DXVECTOR3(0,0,0), rvNormal );

	// generate particles to show spark
//	this->m_pStage->CreateEntity( "sprk", rvPosition, rvNormal * 1.8f, D3DXVECTOR3(0,0,0) );
	// generate a smoke that shoot out from the bullet hole
///	this->m_pStage->CreateEntity( "htsmk", rvPosition,	D3DXVECTOR3(0,0,0), rvNormal );
}
*/


#endif /*  __BASEENTITYBULLET_H__  */
