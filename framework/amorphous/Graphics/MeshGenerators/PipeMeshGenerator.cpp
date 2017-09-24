#include "PipeMeshGenerator.hpp"
#include "amorphous/3DMath/PrimitivePolygonModelMaker.hpp"
#include "amorphous/Graphics/MeshModel/General3DMesh.hpp"


namespace amorphous
{

using std::vector;
using std::shared_ptr;


Result::Name PipeMeshGenerator::Generate()
{
	vector<Vector3> positions, normals;
	vector< vector<int> > polygons;

	float length = m_Desc.height;
	float outer_radius = m_Desc.radii[0];
	float inner_radius = outer_radius - m_fWallThickness;
	CreatePipe(
		length, outer_radius, inner_radius,
		m_Desc.num_sides,
		m_Desc.num_divisions,
		true,
		PolygonModelStyle::EDGE_VERTICES_UNWELDED,
		true,
		PolygonModelStyle::EDGE_VERTICES_UNWELDED,
		positions, normals, polygons
		);

	// Copy vertices and polygons to a general 3D mesh.

	shared_ptr<General3DMesh> pMesh( new General3DMesh() );

	General3DMesh& mesh = *pMesh;

	mesh.SetVertexFormatFlags(
		 CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0 );

	vector<TEXCOORD2> tex_uvs;
	tex_uvs.resize( positions.size(), TEXCOORD2(0,0) );

	mesh.SetVertices( positions, normals, tex_uvs );

	mesh.SetPolygons( polygons );

	mesh.GetMaterialBuffer().resize( 1 );

	Result::Name res = amorphous::CreateArchiveFromGeneral3DMesh( pMesh, m_MeshArchive );

	SetMiscMeshAttributes(); // set texture filename if it is specified.

	return res;
}



} // namespace amorphous
