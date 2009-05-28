#ifndef  __GAMEITEM_H__
#define  __GAMEITEM_H__


#include <string>

#include "Item/ItemEntity.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/MeshObjectContainer.hpp"
#include "GameCommon/LangID.hpp"
#include "Stage/fwd.hpp"
#include "XML/fwd.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"
using namespace GameLib1::Serialization;

#include "fwd.hpp"


/**
 * item description
 *
 */
class ItemDesc : public IArchiveObjectBase
{
public:

	std::string text[Lang::NumLangs];

	inline void Serialize( IArchive& ar, const unsigned int version )
	{
		for( int i=0; i<Lang::NumLangs; i++ )
			ar & text[i];
	}

	void LoadFromXMLNode( CXMLNodeReader& reader );
};


/**
 * base class for items
 *
 */
class CGameItem : public IArchiveObjectBase
{
private:

	void SetWeakPtr( boost::weak_ptr<CGameItem> pMyself ) { m_pMyself = pMyself; }

protected:

	std::string m_strName;

	/// holds mesh related properties
	/// - mesh object handle
	/// - 2d array of shader technique handles
	CMeshContainerNode m_MeshContainerRootNode;

	/// holds how many(much) units the owner can carry
	int m_iMaxQuantity;

	/// holds how many(much) units the owner is carrying
	int m_iCurrentQuantity;

	int m_Price;

	unsigned int m_TypeFlag;

	ItemDesc m_Desc;

	CStageWeakPtr m_pStage;

	CEntityHandle<CItemEntity> m_Entity;

	boost::weak_ptr<CGameItem> m_pMyself;

public:

	CGameItem();
	virtual ~CGameItem();

	const std::string& GetName() const { return m_strName; }

	void SetName( const std::string& item_name ) { m_strName = item_name; }

	//
	// mesh object
	//

//	const CMeshObjectContainer& GetMeshObjectContainer() const { return m_MeshObjectContainer; }
 
//	const std::string& GetMeshObjectFilename() const { return m_MeshObjectContainer.m_MeshDesc.ResourcePath; }

//	const CMeshObjectHandle& GetMeshObjectHandle() const { return m_MeshObjectContainer.m_MeshObjectHandle; }
/*	const CMeshObjectHandle& GetMeshObjectHandle() const
	{
		if( 0 < m_MeshContainerRootNode.GetNumChildren() )
			return m_MeshContainerRootNode.GetChild(0)->m_MeshObjectHandle;
		else
			return CMeshObjectHandle::Null();
	}
*/

	const CMeshContainerNode& GetMeshContainerRootNode() const { return m_MeshContainerRootNode; }

	CMeshContainerNode& MeshContainerRootNode() { return m_MeshContainerRootNode; }

	bool LoadMeshObject();

//	void RenderMeshObject();

	//
	// functions that change / update item states
	//

	/// Called right after the item is loaded (serialized in input mode) from the DB.
	/// Added for init routines that need 'm_pMyself'.
	/// See CItemDatabaseManager::GetItem() for details.
	virtual Result::Name OnLoadedFromDatabase() { return Result::SUCCESS; }

	virtual void OnSelected() {}

	virtual bool HandleInput( int input_code, int input_type, float fParam ) { return false; }

	/// Call this when the item exists in a stage
	virtual void Update( float dt ) {}

	virtual void Render();


	/// item staus expressed with a text format
	virtual void GetStatus( std::string& dest_buffer ) {}

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	inline int GetCurrentQuantity() const { return m_iCurrentQuantity; }
	inline int GetMaxQuantity() const { return m_iMaxQuantity; }
	inline void AddQuantity( int i ) { m_iCurrentQuantity += i; }
	inline void ReduceQuantity( int i ) { m_iCurrentQuantity -= i; }
	inline void SetZeroQuantity() { m_iCurrentQuantity = 0; }
	inline void SetMaxQuantity() { m_iCurrentQuantity = m_iMaxQuantity; }
	inline int GetPrice() const { return m_Price; }

	inline unsigned int GetTypeFlag() const { return m_TypeFlag; }

	inline const std::string& GetDesc( int lang_id ) const { return m_Desc.text[lang_id]; }

	void SetStageWeakPtr( CStageWeakPtr pStage ) { m_pStage = pStage; }

	CEntityHandle<CItemEntity> GetItemEntity() { return m_Entity; }

	void SetItemEntity( CEntityHandle<CItemEntity>& entity ) { m_Entity = entity; }

	inline void SetWorldPose( const Matrix34& rSrcWorldPose )
	{
		boost::shared_ptr<CItemEntity> pEntity = m_Entity.Get();
		if( pEntity )
			pEntity->SetWorldPose( rSrcWorldPose );
	}

	void SetWorldPosition( const Vector3& vPosition ) { SetWorldPose( Matrix34( vPosition, Matrix33Identity() ) ); }

	inline void SetLinearVelocity( const Vector3& vLinearVelocity )
	{
		boost::shared_ptr<CItemEntity> pEntity = m_Entity.Get();
		if( pEntity )
			pEntity->SetVelocity( vLinearVelocity );
	}


	enum eTypeFlag
	{
		TYPE_WEAPON		= (1 << 0),
		TYPE_AMMO		= (1 << 1),
		TYPE_SUPPLY		= (1 << 2),
		TYPE_KEY		= (1 << 3),
		TYPE_UTILITY	= (1 << 4),
		TYPE_VEHICLE	= (1 << 5),
/*		TYPE_			= (1 << 5),
		TYPE_			= (1 << 6),*/

///		TYPE_STOCKABLE	= (1 << 3),
	};

	enum eID
	{
		ID_AMMUNITION = 0,
		ID_FIREARMS,
		ID_BINOCULAR,
		ID_NIGHT_VISION,
		ID_GRAVITY_GUN,
		ID_CAMFLOUGE_DEVICE,
		ID_SUPPRESSOR,
		ID_KEY,
//		ID_AMMO_SUPPLY,
//		ID_LIFE_SUPPLY,
		ID_MISSILELAUNCHER,
		ID_AIRCRAFT,
		ID_RADAR,
		ID_ROTATABLE_TURRET,
		ID_LAND_VEHICLE,
		ID_ARMED_VEHICLE,
		NUM_IDS
	};

	friend class CItemDatabaseBuilder;
	friend class CGameItemInfo;
	friend class CItemDatabaseManager;
};



//===========================================================================
// CGI_Battery
//===========================================================================


class CGI_Battery
{
public:
	bool m_bEnabled;

	float m_fBatteryLeft;
	float m_fMaxBatteryLife;
	float m_fChargeSpeed;

public:

	CGI_Battery() : m_bEnabled(false), m_fBatteryLeft(0), m_fMaxBatteryLife(1), m_fChargeSpeed(1) {}

	inline void Update( float dt );
	inline bool IsBatteryFull() const { return (m_fBatteryLeft == m_fMaxBatteryLife); }
};


inline void CGI_Battery::Update( float dt )
{
	if( m_bEnabled )
	{
		m_fBatteryLeft -= dt;
		if( m_fBatteryLeft <= 0 )
		{
			m_fBatteryLeft = 0;
			m_bEnabled = false;
		}
	}
	else
	{
		if( m_fBatteryLeft < m_fMaxBatteryLife )
		{
			m_fBatteryLeft += dt * m_fChargeSpeed;
		}

		if( m_fMaxBatteryLife <= m_fBatteryLeft )
		{
			m_fBatteryLeft = m_fMaxBatteryLife;
			return;
		}
	}
}


//===========================================================================
// CGI_Binocular
//===========================================================================

class CGI_Binocular : public CGameItem
{
	float m_fCurrentZoom;
	float m_fZoomSpeed;
	float m_fTargetZoom;

	float m_fMaxZoomSpeed;
	float m_fMaxZoom;

	float m_fFocusDelay;

public:

	CGI_Binocular();

	unsigned int GetArchiveObjectID() const { return ID_BINOCULAR; }

	virtual void OnSelected();

	virtual bool HandleInput( int input_code, int input_type, float fParam );

	virtual void Update( float dt );

	/// render item status on HUD
	virtual void GetStatus( std::string& dest_buffer );

	inline void Serialize( IArchive& ar, const unsigned int version );

	friend class CItemDatabaseBuilder;
};


inline void CGI_Binocular::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_fMaxZoom;

//	ar & m_fCurrentZoom;
//	ar & m_fTargetZoom;
}



//===========================================================================
// CGI_NightVision
//===========================================================================

class CGI_NightVision : public CGameItem 
{
	/// indicates if the object is registered as an active item 
	bool m_bActive;

	bool m_bEnabled;

	float m_fBatteryLeft;
	float m_fMaxBatteryLife;
	float m_fChargeSpeed;

public:

	CGI_NightVision() { m_TypeFlag = TYPE_UTILITY; m_bActive = false; m_bEnabled = false; m_fMaxBatteryLife = 10.0f; m_fBatteryLeft = m_fMaxBatteryLife; m_fChargeSpeed = 1.6f; }

	unsigned int GetArchiveObjectID() const { return ID_NIGHT_VISION; }

	virtual void OnSelected();

	virtual void Update( float dt );

	/// render item status on HUD
	virtual void GetStatus( std::string& dest_buffer );

	inline void Serialize( IArchive& ar, const unsigned int version );

	friend class CItemDatabaseBuilder;
};


inline void CGI_NightVision::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_fMaxBatteryLife;
	ar & m_fChargeSpeed;
	ar & m_fBatteryLeft;
}


//===========================================================================
// CGI_CamouflageDevice
//===========================================================================

class CGI_CamouflageDevice : public CGameItem
{
	bool m_bEnabled;

	float m_fEffectiveTimeLeft;
	float m_fMaxEffectiveTime;

public:

	CGI_CamouflageDevice() { m_TypeFlag = TYPE_UTILITY; m_bEnabled = false; m_fMaxEffectiveTime = 10.0f; m_fEffectiveTimeLeft = m_fMaxEffectiveTime; }

	void OnSelected();

	unsigned int GetArchiveObjectID() const { return ID_CAMFLOUGE_DEVICE; }

	inline void Serialize( IArchive& ar, const unsigned int version );

	friend class CItemDatabaseBuilder;
};


inline void CGI_CamouflageDevice::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_bEnabled;

	ar & m_fMaxEffectiveTime;
	ar & m_fEffectiveTimeLeft;
}



//===========================================================================
// CGI_Suppressor
//===========================================================================

class CGI_Suppressor : public CGameItem
{
	bool m_bAttached;

	float m_fSuppressorPerformance;

public:

	CGI_Suppressor() { m_bAttached = false; }

	void OnSelected();

	unsigned int GetArchiveObjectID() const { return ID_SUPPRESSOR; }

	inline void Serialize( IArchive& ar, const unsigned int version );
};


inline void CGI_Suppressor::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_fSuppressorPerformance;
}



//===========================================================================
// CGI_Key
//===========================================================================

class CGI_Key : public CGameItem
{
	enum eKeyParam
	{
		KEY_CODE_LENGTH = 12,
	};

	char m_acKeyCode[KEY_CODE_LENGTH];

public:

	CGI_Key() { memset( m_acKeyCode, 0, sizeof(char) * KEY_CODE_LENGTH ); }

	void OnSelected();

	unsigned int GetArchiveObjectID() const { return ID_KEY; }

	inline void Serialize( IArchive& ar, const unsigned int version );
};


inline void CGI_Key::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	for( int i=0; i<KEY_CODE_LENGTH; i++ )
        ar & m_acKeyCode[i];
}



#endif		/*  __GAMEITEM_H__  */
