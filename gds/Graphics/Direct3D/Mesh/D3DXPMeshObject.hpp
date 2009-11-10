#ifndef  __D3DXPMESHOBJECT_H__
#define  __D3DXPMESHOBJECT_H__


#include "Support/FixedVector.hpp"

//#include "Graphics/GraphicsResourceHandles.h"
#include "D3DXMeshObjectBase.hpp"


class CD3DXPMeshObject : public CD3DXMeshObjectBase
{
protected:

	enum eParam
	{
		NUM_DEFAULT_PMESHES = 2,
		NUM_MAX_PMESHES = 16,
	};

	TCFixedVector<LPD3DXPMESH, NUM_MAX_PMESHES> m_vecpPMesh;

	DWORD                   m_iPMeshCur;

//	D3DXMATRIXA16           m_mWorldCenter;

//	BOOL                    m_bShowOptimized = true;


	HRESULT CreatePMeshFromMesh( LPD3DXMESH pMesh,
		                         LPD3DXBUFFER pAdjacencyBuffer,
								 int num_mesh_divisions );

	virtual HRESULT LoadFromXFile( const std::string& filename );

	virtual HRESULT LoadFromXFile( const std::string& filename, int num_meshes );

public:

	CD3DXPMeshObject();

	CD3DXPMeshObject( const std::string& filename, int num_pmeshes = NUM_DEFAULT_PMESHES );

	virtual ~CD3DXPMeshObject();

	void Release();

	bool LoadFromFile( const std::string& filename, int num_pmeshes );

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes );

	virtual LPD3DXBASEMESH GetBaseMesh() { return GetPMesh(); }

	virtual LPD3DXMESH GetMesh() { return NULL; }

	void SetNumVertices( DWORD dwNumVertices );

	/// set the level of detail of the mesh
	/// from 0.0(loweset level of detail) to 1.0(maximum level of detail)
	inline void SetDetailLevel( float fLevel );

	inline LPD3DXPMESH GetPMesh() { if( 0 < m_vecpPMesh.size() ) return m_vecpPMesh[m_iPMeshCur]; else return NULL; }

	virtual CMeshType::Name GetMeshType() const { return CMeshType::PROGRESSIVE; }
};


//============================= inline implementations =============================


inline void CD3DXPMeshObject::SetDetailLevel( float fLevel )
{
	DWORD num_min_verts = m_vecpPMesh[0]->GetMinVertices();
	DWORD num_max_verts = m_vecpPMesh.back()->GetMaxVertices();

	DWORD num_target_verts = num_min_verts + (DWORD)( (float)(num_max_verts - num_min_verts) * fLevel );

	SetNumVertices( num_target_verts );
}


#endif		/*  __D3DXPMESHOBJECT_H__  */
