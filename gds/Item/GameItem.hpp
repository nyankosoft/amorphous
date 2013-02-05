#ifndef  __GAMEITEM_H__
#define  __GAMEITEM_H__


#include "gds/Item/ItemEntity.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectContainer.hpp"
#include "gds/GameCommon/LangID.hpp"
#include "gds/Stage/fwd.hpp"
#include "gds/XML/fwd.hpp"
#include "gds/Support/Serialization/Serialization.hpp"
#include "fwd.hpp"


namespace amorphous
{

using namespace serialization;


template<class T>
inline boost::shared_ptr<T> CreateGameItem()
{
	boost::shared_ptr<T> pItem( new T );
	pItem->SetWeakPtr( pItem );
	return pItem;
}


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
class GameItem : public IArchiveObjectBase
{
private:

	void SetWeakPtr( boost::weak_ptr<GameItem> pMyself ) { m_pMyself = pMyself; }

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

	EntityHandle<ItemEntity> m_Entity;

	boost::weak_ptr<GameItem> m_pMyself;

public:

	GameItem();
	virtual ~GameItem();

	const std::string& GetName() const { return m_strName; }

	void SetName( const std::string& item_name ) { m_strName = item_name; }

	//
	// mesh object
	//

//	const CMeshObjectContainer& GetMeshObjectContainer() const { return m_MeshObjectContainer; }
 
//	const std::string& GetMeshObjectFilename() const { return m_MeshObjectContainer.m_MeshDesc.ResourcePath; }

//	const MeshHandle& GetMeshObjectHandle() const { return m_MeshObjectContainer.m_MeshObjectHandle; }
/*	const MeshHandle& GetMeshObjectHandle() const
	{
		if( 0 < m_MeshContainerRootNode.GetNumChildren() )
			return m_MeshContainerRootNode.GetChild(0)->m_MeshObjectHandle;
		else
			return MeshHandle::Null();
	}
*/

	const CMeshContainerNode& GetMeshContainerRootNode() const { return m_MeshContainerRootNode; }

	CMeshContainerNode& MeshContainerRootNode() { return m_MeshContainerRootNode; }

	virtual bool LoadMeshObject();

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

	virtual void GetStatusForDebugging( std::string& dest_buffer ) const;

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

	EntityHandle<ItemEntity> GetItemEntity() { return m_Entity; }

	void SetItemEntity( EntityHandle<ItemEntity>& entity ) { m_Entity = entity; }

	inline void SetWorldPose( const Matrix34& rSrcWorldPose )
	{
		boost::shared_ptr<ItemEntity> pEntity = m_Entity.Get();
		if( pEntity )
			pEntity->SetWorldPose( rSrcWorldPose );
	}

	void SetWorldPosition( const Vector3& vPosition ) { SetWorldPose( Matrix34( vPosition, Matrix33Identity() ) ); }

	inline void SetLinearVelocity( const Vector3& vLinearVelocity )
	{
		boost::shared_ptr<ItemEntity> pEntity = m_Entity.Get();
		if( pEntity )
			pEntity->SetVelocity( vLinearVelocity );
	}

	virtual void OnPhysicsTrigger( physics::CShape& my_shape, CCopyEntity& other_entity, physics::CShape& other_shape, U32 trigger_flags ) {}

	virtual void OnPhysicsContact( physics::CContactPair& pair, CCopyEntity& other_entity ) {}

	void SetGraphicsUpdateCallbackForSkeletalMesh();

	virtual void UpdateGraphics() {}

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
		ID_CARTRIDGE,
		ID_MAGAZINE,
		ID_WEAPON,
		ID_FIREARM,
		ID_CLOTHING,
		ID_SKELETAL_CHARACTER,
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
	friend class GameItemInfo;
	friend class CItemDatabaseManager;
	template<class T> friend inline boost::shared_ptr<T> CreateGameItem();

};


} // namespace amorphous



#endif		/*  __GAMEITEM_H__  */
