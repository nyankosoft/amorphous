#ifndef  __StaticGeometry_H__
#define  __StaticGeometry_H__


#include "StaticGeometryBase.hpp"
#include "StaticGeometryArchiveFG.hpp"

#include "3DMath/3DStructs.hpp"
#include "3DMath/AABTree.hpp"
#include "Graphics/FloatRGBColor.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "Graphics/Shader/ShaderParameter.hpp"
#include "Support/StringAux.hpp"


class CStaticGeometryDBKey
{
public:

	static const char *Main;
	static const char *CollisionGeometryStream;
	static const char *GraphicsMeshArchive;
//	static const char *Shaders;
//	static const char *MeshSubsetTree;
};


inline std::string GetGraphicsMeshArchiveKey( int index )
{
	return std::string(CStaticGeometryDBKey::GraphicsMeshArchive) + fmt_string( "[%d]", index );
}


class CShaderContainer : public IArchiveObjectBase
{
public:

	CShaderResourceDesc m_Desc;

	std::vector<CShaderTechniqueHandle> m_vecTechniqueHandle;

	/// used during runtime
	/// - Not serialized.
	CShaderHandle m_ShaderHandle;

	CShaderParameterGroup m_ParamGroup;

public:

	void Serialize( IArchive& ar, const unsigned int version )
	{
//		ar & m_ShaderHandle;
		ar & m_Desc;
		ar & m_vecTechniqueHandle;
		ar & m_ParamGroup;
	}

	/// - load the shader from the desc
	/// - load the textures from m_ParamGroup
	bool Load();

	void SetParams();
};


class CStaticGeometryMeshHolder : public IArchiveObjectBase
{
public:

	CMeshResourceDesc m_Desc;

	/// bounding volumes of the mesh
	AABB3 m_AABB;
//	OBB obb;
//	Sphere Sphere;

	// key string for the database
//	std::string Key;

	/// used during runtime
	/// - Not serialized.
	CMeshObjectHandle m_Mesh;

public:

	CStaticGeometryMeshHolder()
	{
		m_AABB.Nullify();
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_Desc;
		ar & m_AABB; // & obb & Sphere
//		ar & Key
	}

	void Load()
	{
		m_Mesh.Load( m_Desc );
	}
};


class CStaticGeometryArchive : public IArchiveObjectBase
{

public:

	std::vector<CShaderContainer> m_vecShaderContainer;

	std::vector<CStaticGeometryMeshHolder> m_vecMesh;

	CNonLeafyAABTree<CMeshSubset> m_MeshSubsetTree;

	SFloatRGBAColor m_AmbientColor;

	SFloatRGBAColor m_FogColor;

	float m_FogStartDist;

	float m_FarClipDist;

	CShaderParameterGroup m_GlobalShaderParameter;

public:

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_vecShaderContainer;
		ar & m_vecMesh; // & obb & Sphere
		ar & m_MeshSubsetTree;

		ar & m_AmbientColor;
		ar & m_FogColor;
		ar & m_FogStartDist;
		ar & m_FarClipDist;
	}
};


class CStaticGeometry : public CStaticGeometryBase
{
	CStaticGeometryArchive m_Archive;

	int m_PrevShaderIndex;
	int m_PrevShaderTechinqueIndex;

	physics::CTriangleMesh *m_pTriangleMesh;

	physics::CActor *m_pTriangleMeshActor;

	/// used in UpdateResources()
	std::vector<int> m_vecNodesToCheckRU;

	/// used in UpdateResources_NonHierarchical()
	int m_MeshSubsetToCheckNext;

	/// Temporarily hold nodes to check for rendering in Render()
	std::vector<int> m_vecNodesToCheck;

private:

	void UpdateResources( const CCamera& rCam );

	void UpdateResources_NonHierarchical( const CCamera& rCam );

	void UpdateMeshSubsetResources( CMeshSubset& subset, const CCamera& rCam, const Sphere& cam_sphere );

public:

	CStaticGeometry( CStage *pStage );

	~CStaticGeometry();

	virtual int GetType() const { return CStaticGeometryBase::TYPE_GENERAL; }

	virtual bool Render( const CCamera& rCam, const unsigned int EffectFlag );

	virtual bool LoadFromFile( const std::string& filename, bool bLoadGraphicsOnly = false );

	/// collision detection
	virtual int ClipTrace(STrace& tr);

	virtual short CheckPosition(STrace& tr) { return 0; }

	virtual char IsCurrentlyVisibleCell( short sCellIndex ) { return 0; }

	void SetAmbientColor( const SFloatRGBAColor& ambient_color );

	void SetFogColor( const SFloatRGBAColor& color );

	void SetFogStartDist( float dist );

	void SetFogEndDist( float dist );

	virtual void SetDynamicLightManager( CEntitySet* pEntitySet ) {}

	/// used for dynamic lighting
	virtual void GetIntersectingPolygons( Vector3& vCenter,
		                                  float fRadius,
										  AABB3& aabb,
										  std::vector<int>& veciLitPolygonIndex ) {}

	/// \param [in] dest scene
	virtual physics::CActor *CreateCollisionGeometry( physics::CScene& physics_scene );
};



#endif		/*  __StaticGeometry_H__  */
