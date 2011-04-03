#ifndef __D3DXMESHOBJECT_H__
#define __D3DXMESHOBJECT_H__


#include "gds/base.hpp"
#include "gds/Graphics/Direct3D/Direct3D9.hpp"
#include "D3DXMeshObjectBase.hpp"


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

	virtual bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, std::vector<D3DVERTEXELEMENT9>& vecVertexElement );

	virtual bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, U32 vertex_format_flags );

	virtual LPD3DXBASEMESH GetBaseMesh() { return m_pMesh; }

	virtual const LPD3DXBASEMESH GetBaseMesh() const { return m_pMesh; }

	virtual LPD3DXMESH GetMesh() { return m_pMesh; }

	virtual CMeshType::Name GetMeshType() const { return CMeshType::BASIC; }

};


#endif	/*  __D3DXMESHOBJECT_H__  */
