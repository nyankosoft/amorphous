#ifndef  __SG_STATICGEOMETRY_H__
#define  __SG_STATICGEOMETRY_H__


#include "3DMath/3DStructs.h"
#include "3DMath/AABTree.h"
#include "3DCommon/fwd.h"
#include "3DCommon/FloatRGBColor.h"
#include "3DCommon/TextureHandle.h"
#include "3DCommon/MeshObjectHandle.h"
#include "3DCommon/ShaderHandle.h"
#include "3DCommon/Shader/Serialization_ShaderTechniqueHandle.h"
#include "3DCommon/Shader/ShaderParameter.h"
#include "Physics/fwd.h"
#include "Support/StringAux.h"
#include "fwd.h"
#include "StaticGeometryBase.h"
#include "StaticGeometryArchiveFG.h"


class CStaticGeometryDBKey
{
public:

	static const char *Main;
	static const char *CollisionGeometryStream;
	static const char *GraphicsMeshArchive;
//	static const char *Shaders;
//	static const char *MeshSubsetTree;
};


inline string GetGraphicsMeshArchiveKey( int index )
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
	}

	bool Load()
	{
		return m_ShaderHandle.Load( m_Desc );
	}

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


/// texture for additional effects
class CAuxiliaryTexture : public IArchiveObjectBase
{
public:

	/// shader that contains the techinque below
	int m_ShaderIndex;

	/// shader technique that needs this texture
	int m_ShaderTechniqueIndex;

	/// texture stage index
	int m_TextureStage;

	/// name of the texture in the shader
	/// Used if m_TextureStage is not specified
	std::string m_TextureName;

	std::string m_UsageDesc;

	CTextureResourceDesc m_Desc;

	/// used during runtime
	CTextureHandle m_Texture;

	/// float parameters set with the texture
	std::vector< CShaderParameter<float> > m_vecFloatParameter;

	/// integer parameters set with the texture
	std::vector< CShaderParameter<int> > m_vecIntParameter;

public:

	CAuxiliaryTexture();

	void Serialize( IArchive& ar, const unsigned int version );
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

//	std::vector<CAuxiliaryTexture> m_vecAuxTexture;

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
//	std::vector<boost::shared_ptr<CShaderManager>> m_vecpShaderManager;
//	CNonLeafyAABTree<CMeshSubset> m_MeshSubsetTree;

//	std::vector<CStaticGeometryMeshHolder> m_vecMesh;

//	std::vector<CShaderContainer> m_vecShaderContainer;

	CStaticGeometryArchive m_Archive;

	int m_PrevShaderIndex;
	int m_PrevShaderTechinqueIndex;

public:

	CStaticGeometry( CStage *pStage );

	virtual int GetType() const { return CStaticGeometryBase::TYPE_GENERAL; }

	virtual bool Render( const CCamera& rCam, const unsigned int EffectFlag );

	virtual bool LoadFromFile( const std::string& filename, bool bLoadGraphicsOnly = false );

	virtual void MakeEntityTree( CEntitySet* pEntitySet )
	{
		assert( !__FUNCTION__" - Not implemented!" );
	}

	/// collision detection
	virtual int ClipTrace(STrace& tr) { return 0; }

	virtual short CheckPosition(STrace& tr) { return 0; }

	virtual char IsCurrentlyVisibleCell( short sCellIndex ) { return 0; }

	virtual void SetDynamicLightManager( CEntitySet* pEntitySet ) {}

	/// used for dynamic lighting
	virtual void GetIntersectingPolygons( Vector3& vCenter,
		                                  float fRadius,
										  AABB3& aabb,
										  std::vector<int>& veciLitPolygonIndex ) {}

	/// \param [in] dest scene
	virtual physics::CActor *CreateCollisionGeometry( physics::CScene& physics_scene );

	virtual void ReleaseGraphicsResources() {}

	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam ) {}

};



#endif		/*  __SG_STATICGEOMETRY_H__  */
