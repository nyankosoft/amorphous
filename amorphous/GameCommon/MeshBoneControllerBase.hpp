#ifndef  __MeshBoneControllerBase_H__
#define  __MeshBoneControllerBase_H__


#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include "amorphous/3DMath/Transform.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/XML/fwd.hpp"

#include "amorphous/Support/Serialization/Serialization.hpp"


namespace amorphous
{
using namespace serialization;


class MeshBoneControllerBase : public IArchiveObjectBase
{
protected:

	class CBoneControlParam : public IArchiveObjectBase
	{
		static CBoneControlParam ms_NullObject;

	public:

		std::string Name;

		Vector3 vRotationAxis;
//		MeshBone* pBone;

		// RotationAngleFunction

//		RangedSet<float> m_Range;

		Matrix34 LocalTransform;

		int MatrixIndex;

	public:

		CBoneControlParam() : vRotationAxis(Vector3(1,0,0)), /*pBone(NULL),*/ MatrixIndex(-1) {}

		void LoadFromXMLNode( CXMLNodeReader& reader );

		virtual void Serialize( IArchive& ar, const unsigned int version )
		{
			ar & Name;
			// ar & vRotationAxis & MatrixIndex;	/// values for these vars are taken from mesh
		}

		static const CBoneControlParam& NullObject() { return ms_NullObject; }
	};

	std::vector<CBoneControlParam> m_vecBoneControlParam;


	/// borrowed reference
//	boost::shared_ptr<SkeletalMesh> m_pTargetMesh;

//	std::vector<std::string> m_vecTargetBoneName;

public:

	enum type_id
	{
		ID_AIRCRAFT_FLAP,
		ID_AIRCRAFT_VFLAP,
		ID_AIRCRAFT_ROTOR,
		ID_AIRCRAFT_COVER,
		ID_AIRCRAFT_SHAFT,
		ID_AIRCRAFT_GEAR_UNIT,
		NUM_IDS
	};

	MeshBoneControllerBase( boost::shared_ptr<SkeletalMesh> pTargetMesh = boost::shared_ptr<SkeletalMesh>() ){}
//		:
//	m_pTargetMesh(pTargetMesh) {}

	virtual ~MeshBoneControllerBase() {}

	virtual void Init( const SkeletalMesh& target_skeletal_mesh ) {}

	virtual void UpdateTransforms() = 0;

//	virtual void SetTargetMesh( boost::shared_ptr<SkeletalMesh> pTargetMesh ) { m_pTargetMesh = pTargetMesh; }

//	void UpdateTargetMeshTransforms();

	void UpdateMeshBoneLocalTransforms( std::vector<Transform>& mesh_bone_local_transforms );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	inline virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_vecBoneControlParam;
	}

	friend class CItemDatabaseBuilder;
};

} // namespace amorphous



#endif		/*  __MeshBoneControllerBase_H__  */
