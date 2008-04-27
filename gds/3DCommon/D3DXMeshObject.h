#ifndef __D3DXMESHOBJECT_H__
#define __D3DXMESHOBJECT_H__


#include "3DCommon/Direct3D9.h"
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

	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename );

	virtual LPD3DXBASEMESH GetBaseMesh() { return m_pMesh; }

	LPD3DXMESH GetMesh() { return m_pMesh; }

	/// render object by using the fixed function pipeline
	void Render();

	virtual unsigned int GetMeshType() const { return TYPE_MESH; }

};


#endif	/*  __D3DXMESHOBJECT_H__  */
