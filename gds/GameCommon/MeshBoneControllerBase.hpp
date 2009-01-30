#ifndef  __MeshBoneControllerBase_H__
#define  __MeshBoneControllerBase_H__


#include <vector>
#include <string>

#include "3DMath/Vector3.hpp"
#include "3DMath/Matrix34.hpp"
#include "Graphics/fwd.hpp"
#include "XML/fwd.hpp"

#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"
using namespace GameLib1::Serialization;


class CMeshBoneControllerBase : public IArchiveObjectBase
{
protected:

	class CBoneControlParam : public IArchiveObjectBase
	{
	public:

		std::string Name;

		Vector3 vRotationAxis;
//		CMM_Bone* pBone;

		// RotationAngleFunction

//		RangedSet<float> m_Range;

		Matrix34 LocalTransform;

		int MatrixIndex;

	public:

		CBoneControlParam() : vRotationAxis(Vector3(1,0,0)), /*pBone(NULL),*/ MatrixIndex(-1) {}

		void LoadFromXMLNode( CXMLNodeReader& reader );

		virtual void Serialize( IArchive& ar, const unsigned int version )
		{
			ar & Name;//& vRotationAxis & MatrixIndex;	/// values for these vars are taken from mesh
		}
	};

//	CBoneControlParam m_aBoneControlParam[NUM_CONTROL_BONES];
	std::vector<CBoneControlParam> m_vecBoneControlParam;


	/// borrowed reference
	CD3DXSMeshObject *m_pTargetMesh;

//	std::vector<std::string> m_vecTargetBoneName;

public:

	enum type_id
	{
		ID_AIRCRAFT_FLAP,
		ID_AIRCRAFT_VFLAP,
		ID_AIRCRAFT_ROTOR,
		NUM_IDS
	};

	CMeshBoneControllerBase( CD3DXSMeshObject *pTargetMesh = NULL ) : m_pTargetMesh(pTargetMesh) {}

	virtual ~CMeshBoneControllerBase() {}

	virtual void Init() {}

	virtual void UpdateTransforms() = 0;

	void SetTargetMesh( CD3DXSMeshObject *pTargetMesh ) { m_pTargetMesh = pTargetMesh; }

	void UpdateTargetMeshTransforms();

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	inline virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_vecBoneControlParam;
	}

	friend class CItemDatabaseBuilder;
};


#endif		/*  __MeshBoneControllerBase_H__  */
