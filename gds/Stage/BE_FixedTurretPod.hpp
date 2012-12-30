#ifndef __BE_FIXEDTURRETPOD_H__
#define __BE_FIXEDTURRETPOD_H__

#include "BaseEntity.hpp"
#include "BaseEntityHandle.hpp"


namespace amorphous
{


class CBE_FixedTurretPod : public CBaseEntity
{
private:

	/// base entity for the gun which is mounted on this pod
	CBaseEntityHandle m_TurretGun;

	Vector3 m_vTurretGunLocalOffset;

	enum eMeshType
	{
		MESH_POD,
		MESH_TURNTABLE,
		NUM_TURRET_MESHES
	};

    CBE_MeshObjectProperty m_aMesh[NUM_TURRET_MESHES];

public:

	CBE_FixedTurretPod();
	~CBE_FixedTurretPod();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	// behavior in in one frame
	void Draw(CCopyEntity* pCopyEnt); 
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	// void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) {}

//	void AdaptToNewScreenSize() {}
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
	void ReleaseGraphicsResources();

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_FIXEDTURRETPOD; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};
} // namespace amorphous



#endif  /*  __BE_FIXEDTURRETPOD_H__  */
