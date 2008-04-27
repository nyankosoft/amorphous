#include "LightWave/LWO2_Object.h"
#include "LightWave/LWO2_Layer.h"

#include "Support/fnop.h"
#include "Support/StringAux.h"
#include "Support/Log/DefaultLog.h"

#include "3DMeshModelBuilder_LW.h"
using namespace MeshModel;


C3DMeshModelBuilder_LW::C3DMeshModelBuilder_LW( boost::shared_ptr<CLWO2_Object> pSrcObject )
:
m_pSrcObject(pSrcObject)
{
}


C3DMeshModelBuilder_LW::~C3DMeshModelBuilder_LW()
{
}

const std::string C3DMeshModelBuilder_LW::GetInputDirectoryPath() const
{
	return fnop::get_path( m_pSrcObject->GetFilename() );
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
		fnop::change_ext( strDestFilename, "msh" );
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


void C3DMeshModelBuilder_LW::LoadMeshModel()
{
	m_VertexFormatFlag = CMMA_VertexSet::VF_POSITION|CMMA_VertexSet::VF_NORMAL;

	m_VertexFormatFlag |= CMMA_VertexSet::VF_DIFFUSE_COLOR;


	if( m_TargetLayerInfo.pSkelegonLayer != NULL )
	{
		BuildSkeletonFromSkelegon( *(m_TargetLayerInfo.pSkelegonLayer) );
	}

	int i, num_mesh_layers = m_TargetLayerInfo.vecpMeshLayer.size();
	for( i=0; i<num_mesh_layers; i++ )
	{
		ProcessLayer( *(m_TargetLayerInfo.vecpMeshLayer[i]) );
	}

	// create mesh materials from the surfaces of the LightWave object
	SetMaterials();

	// load additional options written as the comment on the surface dialog
	LoadSurfaceCommentOptions();
}


void C3DMeshModelBuilder_LW::ProcessLayer( CLWO2_Layer& rLayer )
{

	// get the current size of the destination vertex buffer
	// used to offest the vertex indices in the polygons
	const size_t vertex_offset = m_vecVertexBuffer.size();
	const size_t polygon_offset = m_vecIndexedPolygon.size();

	// =============== load vertices ===============

	vector<Vector3> rvecVertex = rLayer.GetVertex();
	vector<Vector3> rvecNormal = rLayer.GetVertexNormal();

	int i, j, iNumVertices = rvecVertex.size();

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
		m_VertexFormatFlag |= CMMA_VertexSet::VF_2D_TEXCOORD;
	
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
	size_t iNumSurfaces = rvecSurface.size();

	// surfaces in LightWave are used as materials
//	m_NumMaterials = iNumSurfaces;

	int iMatIndex;

	vector<CLWO2_Face>& rvecPolygon = rLayer.GetFace();

	int iNumPolygons = rvecPolygon.size();
	int iNumPolVerts;

	for( i=0; i<iNumPolygons; i++ )
	{
		CLWO2_Face& rPolygon = rvecPolygon[i];

		CLWO2_Surface& rSurf = m_pSrcObject->FindSurfaceFromTAG( rPolygon.GetSurfaceIndex() );

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
		iNumPolVerts = rvecIndex.size();
		m_vecIndexedPolygon.push_back( CIndexedPolygon() );
		m_vecIndexedPolygon.back().m_MaterialIndex = iMatIndex;
		for( j=0; j<iNumPolVerts; j++ )
		{
			m_vecIndexedPolygon.back().m_index.push_back( vertex_offset + rvecIndex[j] );
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
//	m_vecVertexBuffer.reserve( m_vecVertexBuffer.size() + iNumVertices );
//	m_vecVertexBuffer.insert( m_vecVertexBuffer.end(), iNumVertices, CGeneral3DVertex() );
	if( m_vecVertexBuffer.size() == 0 )
	{
		m_vecVertexBuffer.resize( iNumVertices, CGeneral3DVertex() );
	}
	else
	{
		m_vecVertexBuffer.reserve( m_vecVertexBuffer.size() + iNumVertices );
		m_vecVertexBuffer.insert( m_vecVertexBuffer.end(), iNumVertices, CGeneral3DVertex() );
	}

	// copy collected vertices & polygons to the buffers
	for( i=0; i<iNumVertices; i++ )
	{
		m_vecVertexBuffer[ vertex_offset + i ] = TempVertexBuffer[i];
	}

}


void C3DMeshModelBuilder_LW::SetMaterials()
{
	vector<CLWO2_Surface>& rvecSurface = m_pSrcObject->GetSurface();
	size_t i, iNumSurfaces = rvecSurface.size();

//	CMMA_Material material;
	vector<CMMA_Material>& vecMaterial = GetMaterialBuffer();

	vector<CLWO2_StillClip>& rvecClip = m_pSrcObject->GetStillClip();
	size_t j, iNumClips = rvecClip.size();

	CLWO2_SurfaceBlock *pBlock;

	vecMaterial.resize( iNumSurfaces );

	for( i=0; i<iNumSurfaces; i++ )
	{
		CMMA_Material &material = vecMaterial[i];

		material.fSpecular = rvecSurface[i].GetBaseShadingValue( CLWO2_Surface::SHADE_SPECULAR );

		pBlock = rvecSurface[i].GetSurfaceBlockByChannel( ID_COLR );

		if( pBlock )
		{
			// found a texture for color channel
			for( j=0; j<iNumClips; j++ )
			{
				if( rvecClip[j].uiClipIndex == pBlock->GetImageTag() )
				{
					if( material.vecTexture.size() == 0 )
						material.vecTexture.push_back( CMMA_Texture() );
//					material.SurfaceTexture.strFilename = rvecClip[j].strName;
					material.vecTexture[0].strFilename = rvecClip[j].strName;
				}
			}
		}

		pBlock = rvecSurface[i].GetSurfaceBlockByChannel( ID_BUMP );

		if( pBlock )
		{
			// found a bump map texture
			m_VertexFormatFlag |= CMMA_VertexSet::VF_BUMPMAP;
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
		}

//		material.NormalMapTexture.strFilename = material.SurfaceTexture.strFilename;
//		CFileNameOperation::AppendStringToBodyFilename( material.NormalMapTexture.strFilename, "_NM" );

		//m_MeshModelArchive.SetMaterial( i, material );
	}

	LOG_PRINT( fmt_string( " - %d material(s) were created.", vecMaterial.size() ) );
}


void C3DMeshModelBuilder_LW::LoadSurfaceCommentOptions()
{
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
														  vector<CLWO2_Bone>& rvecSrcBone,
														  CLWO2_Layer& rLayer,
	//													  const Vector3& vParentOffset,
														  CMMA_Bone& rDestBone )
{
	CLWO2_Bone& rBone = rvecSrcBone[iSrcBoneIndex];
	int next_pnt_index = rBone.GetVertexIndex(1);

	rDestBone.vLocalOffset
		= rLayer.GetVertex()[rBone.GetVertexIndex(1)] - rLayer.GetVertex()[rBone.GetVertexIndex(0)];

	rDestBone.strName = rBone.GetName();

	// bone transforms will be calculated later
//	rDestBone.BoneTransform = ;

	m_vecDestBoneIndex[ iSrcBoneIndex ] = m_iNumDestBones++;

	int i, num_bones = rvecSrcBone.size();
	for( i=0; i<num_bones; i++ )
	{
		if( rvecSrcBone[i].GetVertexIndex(0) == next_pnt_index )
		{
			// found a child bone
			rDestBone.vecChild.push_back( CMMA_Bone() );
			
			BuildSkeletonFromSkelegon_r( i, rvecSrcBone, rLayer, rDestBone.vecChild.back() );
		}
	}
}


void C3DMeshModelBuilder_LW::BuildBoneTransformsNROT_r(const Vector3& vParentOffset, CMMA_Bone& rDestBone )
{

	// set transform from model space to bone space
	Vector3 vOffset = vParentOffset + rDestBone.vLocalOffset;
///	Vector3 vOffset = vParentOffset;
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
	vector<CLWO2_Bone>& rvecBone = rLayer.GetBone();

	int i, j, num_bones = rvecBone.size();
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

	CMMA_Bone& dest_bone = m_SkeletonRootBone; //m_MeshModelArchive.GetSkeletonRootBone();

	BuildSkeletonFromSkelegon_r( iRootBoneIndex,
		                         rvecBone,
								 rLayer,
//								 Vector3(0,0,0),
								 dest_bone );

	bool RootBoneEndsAtWorldOrigin = false;

	if( RootBoneEndsAtWorldOrigin )
	{
		// used to create character bones ("female01_x1.9j.lwo")

        dest_bone.vLocalOffset = Vector3(0,0,0);
	}
	else
	{
		// used when the root bone start at the world origin
		dest_bone.vLocalOffset
		= rLayer.GetVertex()[rvecBone[iRootBoneIndex].GetVertexIndex(1)]
		- rLayer.GetVertex()[rvecBone[iRootBoneIndex].GetVertexIndex(0)];
	}

	BuildBoneTransformsNROT_r( Vector3(0,0,0), dest_bone );

	// XXX find the bone which is an immediate child of the root node
	// XXX skelegon tree does not contains root bone
}


void C3DMeshModelBuilder_LW::SetVertexWeights( vector<CGeneral3DVertex>& rDestVertexBuffer,
											   CLWO2_Layer& rLayer )
{
	vector<CLWO2_WeightMap>& rWeightMap = rLayer.GetVertexWeightMap();

	int num_maps = rWeightMap.size();

	if( num_maps == 0 )
		return;

	if( m_vecDestBoneIndex.size() == 0 )
	{
		g_Log.Print( WL_WARNING, "cannot find a valid skelegon tree for layer: %s", rLayer.GetName().c_str() );
		return;
	}

	m_VertexFormatFlag |= CMMA_VertexSet::VF_WEIGHT;

	int i, j, num_vertices;
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

			rDestVertexBuffer[ pnt_index ].m_fMatrixWeight.push_back( fWeight );

			rDestVertexBuffer[ pnt_index ].m_iMatrixIndex.push_back( dest_bone_index );
		}
	}

	// add default weight and index to vertices that have no mapped weights
	// e.g.) vertices in a mass spring simulation
	num_vertices = rDestVertexBuffer.size();
	int iMatIndex = m_vecDestBoneIndex.size(); // an additional matrix which is always identity druing runtime
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
//	m_strSrcFilename = m_pSrcObject->GetFilename();
	m_TargetLayerInfo.strOutputFilename = m_pSrcObject->GetFilename();

	m_VertexFormatFlag = CMMA_VertexSet::VF_POSITION | CMMA_VertexSet::VF_NORMAL;

	m_VertexFormatFlag |= CMMA_VertexSet::VF_DIFFUSE_COLOR;

	SetTexturePathnameOption( TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME );

	// create the basic form of a mesh object
	ProcessLayer( rLayer );

	SetMaterials();

//	LoadMeshModel_PostProcess();

	// check if the mesh should be created as a shadow volume mesh
//	CheckShadowVolume();

	return true;
}

