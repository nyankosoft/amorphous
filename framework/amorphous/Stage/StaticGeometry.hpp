#ifndef  __StaticGeometry_H__
#define  __StaticGeometry_H__


#include "StaticGeometryBase.hpp"
#include "StaticGeometryArchiveFG.hpp"

#include "amorphous/3DMath/3DStructs.hpp"
#include "amorphous/3DMath/AABTree.hpp"
#include "amorphous/Graphics/FloatRGBColor.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderParameter.hpp"
#include "amorphous/Support/StringAux.hpp"


namespace amorphous
{


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

	ShaderResourceDesc m_Desc;

	std::vector<ShaderTechniqueHandle> m_vecTechniqueHandle;

	/// used during runtime
	/// - Not serialized.
	ShaderHandle m_ShaderHandle;

	ShaderParameterGroup m_ParamGroup;

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

	MeshResourceDesc m_Desc;

	/// bounding volumes of the mesh
	AABB3 m_AABB;
//	OBB obb;
//	Sphere Sphere;

	// key string for the database
//	std::string Key;

	/// used during runtime
	/// - Not serialized.
	MeshHandle m_Mesh;

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

	ShaderParameterGroup m_GlobalShaderParameter;

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

	void UpdateResources( const Camera& rCam );

	void UpdateResources_NonHierarchical( const Camera& rCam );

	void UpdateMeshSubsetResources( CMeshSubset& subset, const Camera& rCam, const Sphere& cam_sphere );

public:

	CStaticGeometry( CStage *pStage );

	~CStaticGeometry();

	virtual int GetType() const { return CStaticGeometryBase::TYPE_GENERAL; }

	virtual bool Render( const Camera& rCam, const unsigned int EffectFlag );

	virtual bool LoadFromFile( const std::string& filename, bool bLoadGraphicsOnly = false );

	/// collision detection
	virtual int ClipTrace(STrace& tr);

	virtual short CheckPosition(STrace& tr) { return 0; }

	virtual char IsCurrentlyVisibleCell( short sCellIndex ) { return 0; }

	void SetAmbientColor( const SFloatRGBAColor& ambient_color );

	void SetFogColor( const SFloatRGBAColor& color );

	void SetFogStartDist( float dist );

	void SetFogEndDist( float dist );

	void SetFloatShaderParam( ShaderParameter<float>& tex_param );

	void SetColorShaderParam( ShaderParameter<SFloatRGBAColor>& tex_param );

	void SetTextureShaderParam( ShaderParameter<TextureParam>& tex_param );

	virtual void SetDynamicLightManager( EntityManager* pEntitySet ) {}

	/// used for dynamic lighting
	virtual void GetIntersectingPolygons( Vector3& vCenter,
		                                  float fRadius,
										  AABB3& aabb,
										  std::vector<int>& veciLitPolygonIndex ) {}

	/// \param [in] dest scene
	virtual physics::CActor *CreateCollisionGeometry( physics::CScene& physics_scene );
};


} // namespace amorphous



#endif		/*  __StaticGeometry_H__  */
