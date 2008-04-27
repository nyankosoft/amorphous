#ifndef  __D3DXDMESHOBJECT_H__
#define  __D3DXDMESHOBJECT_H__


#include "3DCommon/D3DXMeshObjectBase.h"

//#include "3DCommon/MeshModel/MeshBone.h"
using namespace MeshModel;


class CPackedFile;

class CD3DXDMeshObject : public CD3DXMeshObjectBase
{
	/// borrowed reference ?
	CPackedFile *m_pPackFile;

//	CD3DXDMeshObjectBase *m_pMesh;
	LPD3DXMESH              m_pMesh;			// Our mesh object in sysmem

	std::vector<std::string> m_DRTexFilename;
	std::vector<std::string> m_DRNormalTexFilename;

	std::vector<int> m_TexState;

	LPDIRECT3DTEXTURE9*     m_ppDRTextures;
	LPDIRECT3DTEXTURE9*     m_ppDRNormalMapTextures;

	enum TextureState
	{
		TEX_NOT_LOADED,
		TEX_LOADING,
		TEX_LOADED,
		NUM_TEX_STATES
	};

protected:

public:

	CD3DXDMeshObject();

	~CD3DXDMeshObject();

	virtual void Release();

	virtual LPD3DXBASEMESH GetBaseMesh() { return m_pMesh; }

	virtual unsigned int GetMeshType() { return TYPE_DMESH; }

	/// \param packed_file packed file for texture resources
	virtual HRESULT LoadFromFile( const std::string& filename, CPackedFile& packed_file );

	virtual LPDIRECT3DTEXTURE9 GetTexture( int i );

	void UpdateResources();


	LPD3DXMESH m_pMeshForTest;

	virtual unsigned int GetMeshType() const { return TYPE_DMESH; }

//	virtual LPD3DXBASEMESH GetBaseMesh() { return m_pMeshForTest; }
};


#endif  /*  __D3DXDMESHOBJECT_H__  */
