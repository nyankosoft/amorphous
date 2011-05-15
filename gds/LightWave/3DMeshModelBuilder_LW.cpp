#include "LightWave/LWO2_Object.hpp"
#include "LightWave/LWO2_Layer.hpp"

#include "Support/lfs.hpp"
#include "Support/StringAux.hpp"
#include "Support/Macro.h"
#include "Support/Log/DefaultLog.hpp"

#include "3DMeshModelBuilder_LW.hpp"
using namespace MeshModel;

using std::string;
using std::vector;
using std::list;
using namespace boost;


// default vertex flags for mesh archives
// created from LightWave object files
static const unsigned int gs_DefaultVertexFlags_LW
= CMMA_VertexSet::VF_POSITION
| CMMA_VertexSet::VF_NORMAL
| CMMA_VertexSet::VF_DIFFUSE_COLOR;


C3DMeshModelBuilder_LW::C3DMeshModelBuilder_LW()
:
m_DefaultVertexFlags(gs_DefaultVertexFlags_LW),
m_UseBoneStartAsBoneLocalOrigin(false)
{
	m_pMesh = shared_ptr<CGeneral3DMesh>( new CGeneral3DMesh() );

//	CIndexedPolygon::SetVertexBuffer( &m_pMesh->GetVertexBuffer() );
}


C3DMeshModelBuilder_LW::C3DMeshModelBuilder_LW( boost::shared_ptr<CLWO2_Object> pSrcObject )
:
m_pSrcObject(pSrcObject),
m_DefaultVertexFlags(gs_DefaultVertexFlags_LW),
m_UseBoneStartAsBoneLocalOrigin(false)
{
	m_pMesh = shared_ptr<CGeneral3DMesh>( new CGeneral3DMesh() );

//	CIndexedPolygon::SetVertexBuffer( &m_pMesh->GetVertexBuffer() );
}


C3DMeshModelBuilder_LW::~C3DMeshModelBuilder_LW()
{
}

const std::string C3DMeshModelBuilder_LW::GetInputDirectoryPath() const
{
	return lfs::get_parent_path( m_pSrcObject->GetFilename() );
}


const std::string C3DMeshModelBuilder_LW::GetOutputFilePath() const
{
	return m_TargetLayerInfo.strOutputFilename;
}


bool C3DMeshModelBuilder_LW::BuildMeshModel()
{
	// set the output filename from the name of the target layer

	size_t pos;
	string strFileTag = "[OUT:";

	string strDestFilename;

	pos = m_strTargetLayerName.find( strFileTag ); 

	if( pos == string::npos )
	{
//		return false;
		// filename is not specified - copy the original filename and change the extension to ".msh"
		strDestFilename = m_pSrcObject->GetFilename();
		lfs::change_ext( strDestFilename, "msh" );
	}
	else
	{
		strDestFilename = m_strTargetLayerName.c_str() + pos + 5;

		// find the end of the output filename
		pos = strDestFilename.find( ']' );
		if( pos != string::npos )
		{
			strDestFilename[pos] = '\0';
		}
		else
			strDestFilename = m_pSrcObject->GetFilename();
	}


//	pos = m_strTargetLayerName.find( "/ShadowVolume" );
//	if( pos != string::npos )
//		m_MeshFlag |= MESH_SHADOWVOLUME;


//	C3DMeshModelBuilder::BuildMeshModel( strDestFilename.c_str(), m_pSrcObject->GetFilename().c_str() );

	assert( !" C3DMeshModelBuilder_LW::BuildMeshModel() - deprecated function " );

	return true;
}


/// build mesh from the target layer info
bool C3DMeshModelBuilder_LW::BuildMeshModel( SLayerSet& rLayerInfo )
{
	m_TargetLayerInfo = rLayerInfo;

	LoadMeshModel();

//	C3DMeshModelBuilder::BuildMeshModel( rLayerInfo.strOutputFilename.c_str(), m_pSrcObject->GetFilename().c_str() );

	return true;
}


int GetAnyPolygonGroup( const CLWO2_Layer& rLayer, int polygon_index )
{
	const std::vector<CLWO2_PolygonGroup>& groups = rLayer.GetPolygonGroup();
	for( size_t i=0; i<groups.size(); i++ )
	{
		const CLWO2_PolygonGroup& group = groups[i];
		for( size_t j=0; j<group.m_vecPolygonIndex.size(); j++ )
		{
			if( polygon_index == group.m_vecPolygonIndex[j] )
				return group.m_vecTag[j];
		}
	}

	return -1;
}


void C3DMeshModelBuilder_LW::ProcessLayer( CLWO2_Layer& rLayer, const CGeometryFilter& filter )
{
	if( !m_pSrcObject )
	{
		LOG_PRINT_ERROR( " m_pSrcObject == NULL" );
		return;
	}

	// get the current size of the destination vertex buffer
	// used to offest the vertex indices in the polygons
	const size_t vertex_offset = (*m_pMesh->GetVertexBuffer().get()).size();
	const size_t polygon_offset = m_pMesh->GetPolygonBuffer().size();

	// =============== load vertices ===============

	vector<Vector3> rvecVertex = rLayer.GetVertex();
	vector<Vector3> rvecNormal = rLayer.GetVertexNormal();

	int i, j;
	const int iNumVertices = (int)rvecVertex.size();

	// create a temporary buffer to hold vertices
	vector<CGeneral3DVertex> TempVertexBuffer;
	TempVertexBuffer.resize( iNumVertices );

	// set default vertex color
	SFloatRGBAColor default_color;
	default_color.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	for( i=0; i<iNumVertices; i++ )
		TempVertexBuffer[i].m_DiffuseColor = default_color;

	// check if there are any texture uv mappings
	vector<CLWO2_TextureUVMap>& rTexUVMap = rLayer.GetTextureUVMap();
	if( 0 < rTexUVMap.size() )
		RaiseVertexFormatFlags( CMMA_VertexSet::VF_2D_TEXCOORD );

	// every vertex is expected to have one set of uv tex coord
	TEXCOORD2 tex;
	tex.u = tex.v = 0;
	for( i=0; i<iNumVertices; i++ )
		TempVertexBuffer[i].m_TextureCoord.push_back( tex );

	// set position & normal
	for( i=0; i<iNumVertices; i++ )
	{
		TempVertexBuffer[i].m_vPosition = rvecVertex[i];
		TempVertexBuffer[i].m_vNormal = rvecNormal[i];
	}

	// load blend weights for each vertex
	SetVertexWeights( TempVertexBuffer, rLayer );


	// =============== load polygons ===============

	// get surface info
	vector<CLWO2_Surface>& rvecSurface = m_pSrcObject->GetSurface();
	const int iNumSurfaces = (int)rvecSurface.size();

	// surfaces in LightWave are used as materials

	int iMatIndex;

	vector<CLWO2_Face>& rvecPolygon = rLayer.GetFace();

	const int iNumPolygons = (int)rvecPolygon.size();
	int iNumPolVerts;

	const CLWO2_Surface null_surface;

	vector<CIndexedPolygon>& polygon_buffer = m_pMesh->GetPolygonBuffer();
	for( i=0; i<iNumPolygons; i++ )
	{
		CLWO2_Face& rPolygon = rvecPolygon[i];

		CLWO2_Surface *pSurf = m_pSrcObject->FindSurfaceFromTAG( rPolygon.GetSurfaceIndex() );

		const CLWO2_Surface& rSurf = *pSurf;
//		const CLWO2_Surface& rSurf = pSurf ? *pSurf : null_surface;

		if( !filter.IncludeSurface( rSurf.GetName() ) )
			continue;

		// calc the the surface index counted in the surface array in 'pSrcObject'
		// this may be different from the surface index used in TAGS chunk
		iMatIndex = 0;
		for( j=0; j<iNumSurfaces; j++ )
		{
			if( rvecSurface[j].GetName() == rSurf.GetName() )
			{
				iMatIndex = j;
				break;
			}
		}

		// set indexed polygons
		vector<UINT4>& rvecIndex = rPolygon.GetVertexIndex();
		iNumPolVerts = (int)rvecIndex.size();

		polygon_buffer.push_back( CIndexedPolygon( m_pMesh->GetVertexBuffer() ) );
		polygon_buffer.back().m_MaterialIndex = iMatIndex;

		m_PolygonGroupIndices.push_back( GetAnyPolygonGroup( rLayer, i ) );

		for( j=0; j<iNumPolVerts; j++ )
		{
			polygon_buffer.back().m_index.push_back( (int)vertex_offset + (int)rvecIndex[j] );
		}

		// set texture uv
		// right now, only a single texture uvmap is supported
		CLWO2_TextureUVMap* pUVMap = m_pSrcObject->FindTextureUVMapFromSurface( rSurf, ID_COLR, rLayer );
		float u,v;
		int vert_index;

		for( j=0; j<iNumPolVerts; j++ )
		{
			u = v = 0;
			vert_index = rvecIndex[j];

			if( rLayer.GetUV(u, v, vert_index, pUVMap) )
			{	// found a corresponding set of uv coord
				TempVertexBuffer[vert_index].m_TextureCoord[0].u = u;
				TempVertexBuffer[vert_index].m_TextureCoord[0].v = v;
			}
		}

		// set vertex color
		for( j=0; j<iNumPolVerts; j++ )
		{
			SFloatRGBAColor color = SFloatRGBAColor(1,1,1,1);
			vert_index = rvecIndex[j];

			if( rLayer.GetVertexColor( color, vert_index, rSurf ) )
				TempVertexBuffer[vert_index].m_DiffuseColor = color;
		}

	}
/*
	// create polygon groups
	vector<CLWO2_PolygonGroup>& rvecPolygonGroup = rLayer.GetPolygonGroup();
	const size_t num_groups = rvecPolygonGroup.size();

	m_vecPolygonGroupIndex.insert( m_vecPolygonGroupIndex.end(), iNumPolygons );
	for( i=0; i<num_groups; i++ )
	{
		CLWO2_PolygonGroup& group = rvecPolygonGroup[i];
		const size_t num_polygons = group.m_vecPolygonIndex.size();
		const vector<UINT4>& rvecPolygonIndex = group.m_vecPolygonIndex;
		for( j=0; j<num_polygons; j++ )
		{
			if( 0 <= rvecPolygonIndex[j] && rvecPolygonIndex[j] < iNumPolygons )
			{
				m_vecPolygonGroupIndex[ polygon_offset + rvecPolygonIndex[j] ]
				= group.m_vecTag[j];
			}
		}
	}
*/
	std::vector<CGeneral3DVertex>& vertex_buffer = *m_pMesh->GetVertexBuffer().get();

//	m_vecVertexBuffer.reserve( m_vecVertexBuffer.size() + iNumVertices );
//	m_vecVertexBuffer.insert( m_vecVertexBuffer.end(), iNumVertices, CGeneral3DVertex() );
	if( vertex_buffer.size() == 0 )
	{
		vertex_buffer.resize( iNumVertices, CGeneral3DVertex() );
	}
	else
	{
		vertex_buffer.reserve( vertex_buffer.size() + iNumVertices );
		vertex_buffer.insert( vertex_buffer.end(), iNumVertices, CGeneral3DVertex() );
	}

	// copy collected vertices & polygons to the buffers
	for( i=0; i<iNumVertices; i++ )
	{
		vertex_buffer[ vertex_offset + i ] = TempVertexBuffer[i];
	}

}


static int get_material_index( int group_index, vector<int>& already_appeared_group_indices )
{
	for( size_t i=0; i<already_appeared_group_indices.size(); i++ )
	{
		if( group_index == already_appeared_group_indices[i] )
			return (int)i;
	}

	already_appeared_group_indices.push_back( group_index );
	return (int)already_appeared_group_indices.size() - 1;
}


void C3DMeshModelBuilder_LW::BreakPolygonsIntoSubsetsByPolygonGroups()
{
	if( !m_pSrcObject )
		return;

	vector<int> already_appeared_group_indices;
	const vector<CMMA_Material>& materials = GetMaterialBuffer();
	const int num_materials = (int)materials.size();
	std::vector<CIndexedPolygon>& polygons = m_pMesh->GetPolygonBuffer();
	const int num_polygons = (int)polygons.size();
	vector<CMMA_Material> new_materials;

	int mat_index_offset = 0;
	for( int i=0; i<num_materials; i++ )
	{
		mat_index_offset = (int)already_appeared_group_indices.size();
		already_appeared_group_indices.clear();
		for( int j=0; j<num_polygons; j++ )
		{
			if( polygons[j].m_MaterialIndex != i )
				continue;

			int new_index
			= mat_index_offset
			+ get_material_index( m_PolygonGroupIndices[j], already_appeared_group_indices );

			if( new_materials.size() <= new_index )
			{
				// Create a new material by copying the polygons[j]'s
				new_materials.push_back( CMMA_Material() );
				new_materials[new_index] = materials[ polygons[j].m_MaterialIndex ];
			}

			polygons[j].m_MaterialIndex = new_index;

			if( 0 <= m_PolygonGroupIndices[j] )
			{
				const char *group_name = m_pSrcObject->GetTagString( m_PolygonGroupIndices[j] );
				if( group_name )
					new_materials[new_index].Name = group_name;
			}
		}
	}

	GetMaterialBuffer() = new_materials;
}


void C3DMeshModelBuilder_LW::SetMaterials()
{
	if( !m_pSrcObject )
	{
		LOG_PRINT_ERROR( " m_pSrcObject == NULL" );
		return;
	}

	vector<CLWO2_Surface>& rvecSurface = m_pSrcObject->GetSurface();
	size_t i, iNumSurfaces = rvecSurface.size();

//	CMMA_Material material;
	vector<CMMA_Material>& vecMaterial = GetMaterialBuffer();

	size_t j = 0;

	vector<CLWO2_StillClip>& rvecClip = m_pSrcObject->GetStillClip();
	size_t k, iNumClips = rvecClip.size();

	vecMaterial.resize( iNumSurfaces );

	UINT         tex_channel_tags[]    = { ID_COLR, ID_BUMP };
	unsigned int vert_flags_to_raise[] = {       0, CMMA_VertexSet::VF_BUMPMAP };

	for( i=0; i<iNumSurfaces; i++ )
	{
		CMMA_Material &material = vecMaterial[i];

		const CLWO2_Surface& src_furface = rvecSurface[i];

		material.Name = src_furface.GetName();

		material.fSpecular = src_furface.GetBaseShadingValue( CLWO2_Surface::SHADE_SPECULAR );

		material.m_Params.fSpecularity = src_furface.GetBaseShadingValue( CLWO2_Surface::SHADE_SPECULAR);
		material.m_Params.fLuminosity  = src_furface.GetBaseShadingValue( CLWO2_Surface::SHADE_LUMINOSITY );
		material.m_Params.fGlossiness  = src_furface.GetBaseShadingValue( CLWO2_Surface::SHADE_GLOSSINESS );
		material.m_Params.fReflection  = src_furface.GetBaseShadingValue( CLWO2_Surface::SHADE_REFLECTION );

		for( j=0; j<numof(tex_channel_tags); j++ )
		{
			const CLWO2_SurfaceBlock *pBlock = src_furface.GetSurfaceBlockByChannel( tex_channel_tags[j] );

			if( pBlock )
			{
				// found a texture

				RaiseVertexFormatFlags( vert_flags_to_raise[j] );

				for( k=0; k<iNumClips; k++ )
				{
					if( rvecClip[k].uiClipIndex == pBlock->GetImageTag() )
					{
						for( size_t tex = material.vecTexture.size();
							        tex <= j;
									tex++ )
						{
							material.vecTexture.push_back( CMMA_Texture() );
						}

	//					material.SurfaceTexture.strFilename = rvecClip[j].strName;
						material.vecTexture[j].strFilename = rvecClip[k].strName;
					}
				}
			}
		}

#ifdef WIN32

		// fix abs paths on Win32 platform
		// - absolute paths made by LW modeler is not valid on Win32 platform. See below
		// LightWave modeler writes it as "D:dev/models/object.lwo"
		// Win32 wants it to be           "D:/dev/models/object.lwo"
		for( j=0; j<material.vecTexture.size(); j++ )
		{
			string filepath = material.vecTexture[j].strFilename;

			if( filepath[1] == ':' )
			{
				// absolute path
				// - change it to a valid path string for Win32 platform
				material.vecTexture[j].strFilename
					= filepath.substr( 0, 1 ) + ":/" + filepath.substr( 2, 2048 );
			}
		}

#endif /* WIN32 */

/*
		pBlock = rvecSurface[i].GetSurfaceBlockByChannel( ID_BUMP );

		if( pBlock )
		{
			// found a bump map texture
			RaiseVertexFormatFlags( CMMA_VertexSet::VF_BUMPMAP );
			for( j=0; j<iNumClips; j++ )
			{
				if( rvecClip[j].uiClipIndex == pBlock->GetImageTag() )
				{
					for( size_t tex = material.vecTexture.size(); tex < 2; tex++ )
						material.vecTexture.push_back( CMMA_Texture() );

//					material.NormalMapTexture.strFilename = rvecClip[j].strName;
					material.vecTexture[1].strFilename = rvecClip[j].strName;
			}
			}
		}*/	

//		material.NormalMapTexture.strFilename = material.SurfaceTexture.strFilename;
//		CFileNameOperation::AppendStringToBodyFilename( material.NormalMapTexture.strFilename, "_NM" );

		//m_MeshModelArchive.SetMaterial( i, material );
	}

	LOG_PRINT( fmt_string( " - %d material(s) were created.", vecMaterial.size() ) );
}


void C3DMeshModelBuilder_LW::LoadSurfaceCommentOptions()
{
	if( !m_pSrcObject )
	{
		LOG_PRINT_ERROR( " m_pSrcObject == NULL" );
		return;
	}

	vector<CLWO2_Surface>& rvecSurface = m_pSrcObject->GetSurface();
	size_t i, num_surfaces = rvecSurface.size();
	size_t pos;
	const string tex_option = "-t ";
	const string tex_path_option = "-tp ";

	vector<CMMA_Material>& vecMaterial = GetMaterialBuffer(); //m_MeshModelArchive.GetMaterial();

	for( i=0; i<num_surfaces; i++ )
	{
		const string& comment = rvecSurface[i].GetComment();

		// check texture filename overwrite option
		pos = comment.find( tex_option );
		if( pos != string::npos )
		{
			g_Log.Print( "C3DMeshModelBuilder_LW - found '-t' option in the comment of surface[%d]", i );

			// overwrite texture filename
			string tex_filename = comment.substr( pos + tex_option.length(), 1024 );
			pos = tex_filename.find( " " );
			if( pos != string::npos )
				tex_filename = tex_filename.substr(0,pos);

			g_Log.Print( "C3DMeshModelBuilder_LW - texture filename for surface[%d]: %s", i, tex_filename.c_str() );

//			vecMaterial[i].SurfaceTexture.strFilename = tex_filename;

			if( vecMaterial[i].vecTexture.size() == 0 )
				vecMaterial[i].vecTexture.push_back( CMMA_Texture() );

			vecMaterial[i].vecTexture[0].strFilename = tex_filename;

			// now the texture filename has been set, don't do anything
			// in C3DMeshModelBuilder::ProcessTextureFilenames()
			SetTexturePathnameOption( TexturePathnameOption::NO_TEXTURE_FILENAME );
		}

		// check texture filename path option
		pos = comment.find( tex_path_option );
		if( pos != string::npos )
		{
			// overwrite texture filename
			string tex_path = comment.substr( pos + tex_path_option.length(), 1024 );
			pos = tex_path.find( " " );
			if( pos != string::npos )
				tex_path = tex_path.substr(0,pos);

			m_strTexPath = tex_path;
			SetTexturePathnameOption( TexturePathnameOption::FIXEDPATH_AND_FILENAME );
		}
	}
}


void C3DMeshModelBuilder_LW::BuildSkeletonFromSkelegon_r( int iSrcBoneIndex,
														  const vector<CLWO2_Bone>& rvecSrcBone,
														  const CLWO2_Layer& rLayer,
	//													  const Vector3& vParentOffset,
														  CMMA_Bone& rDestBone )
{
	const CLWO2_Bone& rBone = rvecSrcBone[iSrcBoneIndex];
	int next_pnt_index = rBone.GetVertexIndex(1);

	rDestBone.vLocalOffset
		= rLayer.GetVertex()[rBone.GetVertexIndex(1)] - rLayer.GetVertex()[rBone.GetVertexIndex(0)];

	rDestBone.strName = rBone.GetName();

	// bone transforms will be calculated later
//	rDestBone.BoneTransform = ;

	m_vecDestBoneIndex[ iSrcBoneIndex ] = m_iNumDestBones++;

	const size_t num_bones = rvecSrcBone.size();
	for( size_t i=0; i<num_bones; i++ )
	{
		if( rvecSrcBone[i].GetVertexIndex(0) == next_pnt_index )
		{
			// found a child bone
			rDestBone.vecChild.push_back( CMMA_Bone() );
			
			BuildSkeletonFromSkelegon_r( (int)i, rvecSrcBone, rLayer, rDestBone.vecChild.back() );
		}
	}
}


void C3DMeshModelBuilder_LW::BuildBoneTransformsNROT_r(const Vector3& vParentOffset, CMMA_Bone& rDestBone )
{
	// set transform from model space to bone space
	Vector3 vOffset = vParentOffset + rDestBone.vLocalOffset;
///	Vector3 vOffset = vParentOffset;
	if( m_UseBoneStartAsBoneLocalOrigin )
		rDestBone.BoneTransform.vPosition = - vParentOffset;
	else
		rDestBone.BoneTransform.vPosition = - vOffset;

	// all the bone transforms are not supposed to have rotations
	rDestBone.BoneTransform.matOrient.SetIdentity();

	size_t i, num_child_bones = rDestBone.vecChild.size();
	for( i=0; i<num_child_bones; i++ )
	{
		BuildBoneTransformsNROT_r( vOffset, rDestBone.vecChild[i] );
///		BuildBoneTransformsNROT_r( vOffset + rDestBone.vLocalOffset, rDestBone.vecChild[i] );
	}
}


void C3DMeshModelBuilder_LW::BuildSkeletonFromSkelegon( CLWO2_Layer& rLayer )
{
	const vector<CLWO2_Bone>& rvecBone = rLayer.GetBone();

	const string layer_name = rLayer.GetName();
	if( layer_name.find( "--bone-local-origin=bone-start-pos" ) != string::npos )
		m_UseBoneStartAsBoneLocalOrigin = true;
	else
		m_UseBoneStartAsBoneLocalOrigin = false;

	int i,j;
	const int num_bones = (int)rvecBone.size();
	int pnt_index;
	int iRootBoneIndex = -1;

	// find a root bone
	for( i=0; i<num_bones; i++ )
	{
		pnt_index = rvecBone[i].GetVertexIndex(0);
		for( j=0; j<num_bones; j++ )
		{
			if( i == j )
				continue;

			if( pnt_index == rvecBone[j].GetVertexIndex(1) )
				break;	// child of some other bone
		}

		if( j == num_bones )
		{
			iRootBoneIndex = i;
			break;
		}

	}

	if( iRootBoneIndex < 0 )
		return;

	m_iNumDestBones = 0;
	m_vecDestBoneIndex.resize( num_bones ); 

	Matrix34 mat;
	mat.Identity();

	CMMA_Bone& dest_bone_root = m_pMesh->GetSkeletonRootBoneBuffer();

	BuildSkeletonFromSkelegon_r( iRootBoneIndex,
		                         rvecBone,
								 rLayer,
//								 Vector3(0,0,0),
								 dest_bone_root );

	bool RootBoneEndsAtWorldOrigin = false;

	if( RootBoneEndsAtWorldOrigin )
	{
		// used to create character bones ("female01_x1.9j.lwo")

        dest_bone_root.vLocalOffset = Vector3(0,0,0);
	}
	else
	{
		// Use the end position of the root bone as the translation of the root bone transform.
		// - This allows the root bone to start and end at arbitrary positions in the modeler
		if( m_UseBoneStartAsBoneLocalOrigin )
			dest_bone_root.vLocalOffset = rLayer.GetVertex()[rvecBone[iRootBoneIndex].GetVertexIndex(0)];
		else
			dest_bone_root.vLocalOffset = rLayer.GetVertex()[rvecBone[iRootBoneIndex].GetVertexIndex(1)];
	}
/*	else
	{
		// used when the root bone start at the world origin
		dest_bone.vLocalOffset
		= rLayer.GetVertex()[rvecBone[iRootBoneIndex].GetVertexIndex(1)]
		- rLayer.GetVertex()[rvecBone[iRootBoneIndex].GetVertexIndex(0)];
	}*/

	BuildBoneTransformsNROT_r( Vector3(0,0,0), dest_bone_root );

	// XXX find the bone which is an immediate child of the root node
	// XXX skelegon tree does not contains root bone
}


void C3DMeshModelBuilder_LW::SetVertexWeights( vector<CGeneral3DVertex>& rDestVertexBuffer,
											   CLWO2_Layer& rLayer )
{
	vector<CLWO2_WeightMap>& rWeightMap = rLayer.GetVertexWeightMap();

	const int num_maps = (int)rWeightMap.size();

	if( num_maps == 0 )
		return;

	if( m_vecDestBoneIndex.size() == 0 )
	{
		LOG_PRINT_WARNING( " - Cannot find a valid skelegon tree for layer: " + rLayer.GetName() );
		return;
	}

	RaiseVertexFormatFlags( CMMA_VertexSet::VF_WEIGHT );

	int i, j, num_vertices = 0;
	int src_bone_index, dest_bone_index;
	UINT4 pnt_index;
	float fWeight;

	for( i=0; i<num_maps; i++ )
	{
		num_vertices = rWeightMap[i].GetNumMaps();

		src_bone_index = m_pSrcObject->GetBoneIndexForWeightMap( rWeightMap[i], rLayer );

		if( src_bone_index < 0 )
			continue;	// not mapped to any bone

		dest_bone_index = m_vecDestBoneIndex[src_bone_index];

		for( j=0; j<num_vertices; j++ )
		{
			rWeightMap[i].GetWeightMap( j, pnt_index, fWeight );

			CGeneral3DVertex& dest_vertex = rDestVertexBuffer[ pnt_index ];

			if( dest_vertex.m_fMatrixWeight.size() == CGeneral3DVertex::NUM_MAX_BLEND_MATRICES_PER_VERTEX )
				continue;

			dest_vertex.m_fMatrixWeight.push_back( fWeight );
			dest_vertex.m_iMatrixIndex.push_back( dest_bone_index );
		}
	}

	// add default weight and index to vertices that have no mapped weights
	// e.g.) vertices in a mass spring simulation
	num_vertices = (int)rDestVertexBuffer.size();
	int iMatIndex = (int)m_vecDestBoneIndex.size(); // an additional matrix which is always identity druing runtime
	for( i=0; i<num_vertices; i++ )
	{
		if( rDestVertexBuffer[i].m_iMatrixIndex.size() == 0 )
		{
			rDestVertexBuffer[i].m_fMatrixWeight.push_back( 1.0f );
			rDestVertexBuffer[i].m_iMatrixIndex.push_back( iMatIndex );
		}
	}
}


bool C3DMeshModelBuilder_LW::BuildMeshFromLayer( CLWO2_Layer& rLayer )
{
	m_TargetLayerInfo.strOutputFilename = m_pSrcObject->GetFilename();

	SetVertexFormatFlags( m_DefaultVertexFlags );

	SetTexturePathnameOption( TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME );

	// create the basic form of a mesh object
	ProcessLayer( rLayer );

	m_pMesh->UpdatePolygonBuffer();

	SetMaterials();

	return true;
}


void C3DMeshModelBuilder_LW::LoadMeshModel()
{
	SetVertexFormatFlags( m_DefaultVertexFlags );

	if( m_TargetLayerInfo.pSkelegonLayer != NULL )
	{
		BuildSkeletonFromSkelegon( *(m_TargetLayerInfo.pSkelegonLayer) );
	}

	const int num_mesh_layers = (int)m_TargetLayerInfo.vecpMeshLayer.size();
	for( int i=0; i<num_mesh_layers; i++ )
	{
		if( !m_TargetLayerInfo.vecpMeshLayer[i] )
			continue;

		ProcessLayer( *(m_TargetLayerInfo.vecpMeshLayer[i]) );
	}

	m_pMesh->UpdatePolygonBuffer();

	// create mesh materials from the surfaces of the LightWave object
	SetMaterials();

	for( int i=0; i<num_mesh_layers; i++ )
	{
		if( !m_TargetLayerInfo.vecpMeshLayer[i] )
			continue;

		CLWO2_Layer& layer = *(m_TargetLayerInfo.vecpMeshLayer[i]);

		if( !layer.GetPolygonGroup().empty() )
			BreakPolygonsIntoSubsetsByPolygonGroups();
	}

	// load additional options written as the comment on the surface dialog
	LoadSurfaceCommentOptions();
}


/*
bool has_string( const std::vector<std::string>& string_buffer, const std::string& name )
{}
*/


/// loads 3d mesh model from geometry filter
bool C3DMeshModelBuilder_LW::LoadFromLWO2Object( boost::shared_ptr<CLWO2_Object> pObject, const CGeometryFilter& geometry_filter )
{
	LOG_FUNCTION_SCOPE();

	if( !pObject )
		return false;

	m_pSrcObject = pObject;

	SetVertexFormatFlags( m_DefaultVertexFlags );

	list<CLWO2_Layer>& layer_list = m_pSrcObject->GetLayer();

	const vector<string>& layers_to_include = geometry_filter.Include.Layers;
	const vector<string>& layers_to_exclude = geometry_filter.Exclude.Layers;

	/// create polygons and vertices from layers which are
	/// 1. included in the 'include' filter
	/// 2. but not included in the 'exclude' filter
	vector<string>::const_iterator itr;
	list<CLWO2_Layer>::iterator layer;
	for( layer = layer_list.begin();
		 layer != layer_list.end();
		 layer++ )
	{
		if( 0 < layers_to_include.size() )
		{
			itr = find( layers_to_include.begin(), layers_to_include.end(), layer->GetName() );

			if( itr == layers_to_include.end() )
				continue;
		}
//		else
//		{
			// no 'include' filter is specified
			// - include it if it is not listed on the 'exclude' list
//		}

		// layer is included in the 'include' list

		if( 0 < layers_to_exclude.size() )
		{
			itr = find( layers_to_exclude.begin(), layers_to_exclude.end(), layer->GetName() );

			if( itr != layers_to_exclude.end() )
				continue; // in the 'exclude' list. skip this layer
		}

		ProcessLayer( *layer, geometry_filter );
	}

	m_pMesh->UpdatePolygonBuffer();

	SetMaterials();

	// load additional options written as the comment on the surface dialog
	LoadSurfaceCommentOptions();

	return true;
}


bool C3DMeshModelBuilder_LW::LoadFromFile( const std::string& model_filepath, const CGeometryFilter& geometry_filter )
{
	shared_ptr<CLWO2_Object> pObject;

//	if( !m_pSrcObject )
	{
		LOG_SCOPE( " - Loading LightWave object from file " );

		// load the model
		pObject.reset( new CLWO2_Object() );

		bool loaded = pObject->LoadLWO2Object( model_filepath );

		if( !loaded )
		{
			LOG_PRINT_ERROR( " - Failed to load a LightWave object file: " + model_filepath );
			return false;
		}
	}

	return LoadFromLWO2Object( pObject, geometry_filter );
}


std::string C3DMeshModelBuilder_LW::GetBasePath()
{
	return m_pSrcObject->GetFilename();
}
