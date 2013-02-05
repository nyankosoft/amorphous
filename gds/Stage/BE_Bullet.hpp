#ifndef	__BASEENTITYBULLET_H__
#define __BASEENTITYBULLET_H__


#include "BaseEntity.hpp"
#include "Stage.hpp"
#include "BaseEntityHandle.hpp"

#include "Graphics/TextureHandle.hpp"
#include "Graphics/Mesh/RectSetMesh.hpp"


namespace amorphous
{


class CBE_Bullet : public BaseEntity
{
	std::string m_BillboardTextureFilepath;
	TextureHandle m_BillboardTexture;

	float m_fBillboardRadius;

	/// Holds a single billboard mesh
	RectSetMesh m_BillboardRectMesh;

	/// distance which the bullet can cover
	float m_fMaxRange;

	float m_fBulletPower;

	float m_fPenetration;

	float m_fSpeedAttenuAtReflection;

	float m_fNumMaxReflections;

//	float m_fMuzzleSpeed;

	BaseEntityHandle m_Spark;
	BaseEntityHandle m_Light;

	// For visual debugging
	std::vector< boost::shared_ptr< std::vector<Vector3> > > m_pTrajectoryPoints;

	int m_RenderTrajectory;

private:

	void BulletMove(CCopyEntity* pCopyEnt);

	void PenetrationMove(CCopyEntity* pCopyEnt);

	void PenetrationIteration( CCopyEntity* pCopyEnt, float fFrameTime_Left );

	void ReflectiveMove(CCopyEntity* pCopyEnt);

	void Move( CCopyEntity* pCopyEnt );

	void DrawBillboradTexture(CCopyEntity* pCopyEnt);

    void Draw3DObjectBullet(CCopyEntity* pCopyEnt);

	// Return the buffer to store trajectory points. Used for visual debugging
	boost::shared_ptr< std::vector<Vector3> > GetTrajectoryPoints( CCopyEntity& bullet );

	void ReleaseTrajectoryPoints( CCopyEntity& bullet );

public:

	CBE_Bullet();
	~CBE_Bullet() {}
	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	void Act(CCopyEntity* pCopyEnt);
	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	//void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	/// create decal/spark/smoke entities and make a hit sound
	/// according to the type of the surface material
	void OnBulletHit( CCopyEntity* pCopyEnt, STrace& trace );

	void Draw(CCopyEntity* pCopyEnt);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_BULLET; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	static float& Power(CCopyEntity* pCopyEnt) { return pCopyEnt->f1; }
	static float& Power(boost::shared_ptr<CCopyEntity> pEntity) { return pEntity->f1; }

	static float& TraveledDist(CCopyEntity* pCopyEnt) { return pCopyEnt->f2; }

	enum DamageFactorFlag
	{
		DFF_SPEED         = (1 << 0), ///< mutually exclusive with DFF_SQUARED_SPEED
		DFF_SQUARED_SPEED = (1 << 1), ///< mutually exclusive with DFF_SPEED
	};


//	inline void CreateBulletHoleDecal(Vector3& rvPosition, Vector3& rvNormal);
};


// ================================ inline implementations ================================ 

/*
inline void CBE_Bullet::CreateBulletHoleDecal(Vector3& rvPosition, Vector3& rvNormal)
{
	// When a bullet hit a polygon of the map, we put the decal to make a bullet hole.
	// Bullet holes are made on only static geometry. When it hit other entities,
	// no holes are made on them because they can move and disappear.
	this->m_pStage->CreateEntity( m_BulletHole, rvPosition, Vector3(0,0,0), rvNormal );

	// generate particles to show spark
//	this->m_pStage->CreateEntity( "sprk", rvPosition, rvNormal * 1.8f, Vector3(0,0,0) );
	// generate a smoke that shoot out from the bullet hole
///	this->m_pStage->CreateEntity( "htsmk", rvPosition,	Vector3(0,0,0), rvNormal );
}
*/

} // namespace amorphous



#endif /*  __BASEENTITYBULLET_H__  */
