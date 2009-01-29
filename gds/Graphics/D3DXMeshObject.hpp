#ifndef __D3DXMESHOBJECT_H__
#define __D3DXMESHOBJECT_H__


#include "../base.h"
#include "Graphics/Direct3D9.h"
#include "D3DXMeshObjectBase.h"


class CD3DXMeshObject : public CD3DXMeshObjectBase
{
private:

	LPD3DXMESH m_pMesh;

protected:

	virtual HRESULT LoadFromXFile( const std::string& filename );

public:

	CD3DXMeshObject();

	CD3DXMeshObject( const std::string& filename );

	virtual ~CD3DXMeshObject();

	void Release();

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

	virtual LPD3DXBASEMESH GetBaseMesh() { return m_pMesh; }

	virtual LPD3DXMESH GetMesh() { return m_pMesh; }

	/// render object by using the fixed function pipeline
	void Render();

	virtual CMeshType::Name GetMeshType() const { return CMeshType::BASIC; }

};


#endif	/*  __D3DXMESHOBJECT_H__  */
