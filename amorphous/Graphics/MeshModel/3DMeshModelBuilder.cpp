#include "3DMeshModelBuilder.hpp"
#include "NVMeshMender.hpp"
#include "ShadowVolumeMeshGenerator.hpp"
#include "Support/lfs.hpp"
#include "Support/StringAux.hpp"
#include "Support/ImageArchive.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <assert.h>
#include <algorithm>
#include <boost/filesystem.hpp>


namespace amorphous
{

using namespace std;
using namespace boost;
using namespace boost::filesystem;


C3DModelLoader::C3DModelLoader()
:
m_TextureFilePathOption(TexturePathnameOption::ORIGINAL_FILENAME)
{
}


//=========================================================================================
// C3DMeshModelBuilder
//=========================================================================================

C3DMeshModelBuilder::C3DMeshModelBuilder()
{
}


C3DMeshModelBuilder::~C3DMeshModelBuilder()
{
}


static int s_MeshNum = 0;


void C3DMeshModelBuilder::BuildMeshModel( boost::shared_ptr<C3DModelLoader> pModelLoader, U32 build_option_flags )
{
	LOG_FUNCTION_SCOPE();

	m_pModelLoader = pModelLoader;

	BuildMeshModelArchive( m_pModelLoader->GetGeneral3DMeshSharedPtr(), build_option_flags );
}


/// \param [in] borrowed reference
void C3DMeshModelBuilder::BuildMeshModelArchive( boost::shared_ptr<General3DMesh> pGeneralMesh,
												 U32 build_option_flags )
{
	LOG_FUNCTION_SCOPE();

	m_pMesh = pGeneralMesh;

	// save non-triangulated polygons
	m_vecNonTriangulatedPolygon = m_pMesh->GetPolygonBuffer();

	string src_dirpath;
	string dest_filepath;

	if( m_pModelLoader )
	{
		src_dirpath   = m_pModelLoader->GetInputDirectoryPath();
		dest_filepath = m_pModelLoader->GetOutputFilePath();
	}

	CreateMeshArchive();

	// check if the mesh should be created as a shadow volume mesh
	CheckShadowVolume();

	if( build_option_flags & C3DMeshModelBuilder::BOF_CHANGE_TEXTURE_FILE_EXTENSIONS_TO_IA )
	{
		const int num_materials = (int)m_MeshModelArchive.GetMaterial().size();
		for( int i=0; i<num_materials; i++ )
		{
			const int num_textures = (int)m_MeshModelArchive.GetMaterial()[i].vecTexture.size();
			for( int j=0; j<num_textures; j++ )
			{
				TextureResourceDesc& tex = m_MeshModelArchive.GetMaterial()[i].vecTexture[j];
				if( tex.pLoader || tex.ResourcePath.length() == 0 )
					continue;

				lfs::change_ext( tex.ResourcePath, "ia" );
			}
		}

	}

	// save mesh model to file
	// Moved to C3DMeshModelExportManager_LW::BuildMeshModels()
//	if( 0 < dest_filepath.length() )
//		m_MeshModelArchive.SaveToFile( dest_filepath );

	// Commented out: decided to do this with AutoResourceArchiver.
/*	if( build_option_flags && C3DMeshModelBuilder::BOF_SAVE_TEXTURES_AS_IMAGE_ARCHIVES )
	{
//		const int num_materials = (int)m_MeshModelArchive.GetMaterial().size();
		const int num_materials = (int)m_OrigTextureFilepaths.size();
		for( int i=0; i<num_materials; i++ )
		{
//			const int num_textures = (int)m_MeshModelArchive.GetMaterial()[i].vecTexture.size();
			const int num_textures = (int)m_OrigTextureFilepaths[i].size();
			for( int j=0; j<num_textures; j++ )
			{
				const CMMA_Texture& tex = m_MeshModelArchive.GetMaterial()[i].vecTexture[j];
				if( tex.type != CMMA_Texture::FILENAME )
					continue;

//				path src_tex_path = path(src_dirpath) / tex.strFilename;
				path src_tex_path = path(src_dirpath) / m_OrigTextureFilepaths[i][j];
				ImageArchive ia( src_tex_path.string() );
				if( !ia.IsValid() )
					continue;

				path dest_tex_path = path(dest_filepath).parent_path() / tex.strFilename;
				create_directories( dest_tex_path.parent_path() );
				ia.SaveToFile( dest_tex_path.string() );
			}
		}
	}*/

	// debug - output text file
	if( build_option_flags & BOF_OUTPUT_AS_TEXTFILE )
	{
		string strTextFile;
		if( 0 < dest_filepath.length() )
		{
			strTextFile = dest_filepath;
			lfs::change_ext( strTextFile, "txt" );

			if( src_dirpath.length() )
			{
				// place the file to the directory of the source model
				// to keep the dest directory clean
//				strTextFile = fnop::get_path(src_dirpath) + lfs::get_leaf(strTextFile);
				strTextFile = src_dirpath + "/" + lfs::get_leaf(strTextFile);
			}
		}
		else
		{
			strTextFile = fmt_string( "mesh%02d.txt", s_MeshNum );
			s_MeshNum++;
		}

		m_MeshModelArchive.WriteToTextFile( strTextFile.c_str() );
	}
}


Result::Name C3DMeshModelBuilder::GetArchive( C3DMeshModelArchive& dest )
{
	if( m_MeshModelArchive.GetVertexSet().GetNumVertices() == 0 )
		return Result::UNKNOWN_ERROR;

	dest = m_MeshModelArchive;

	return Result::SUCCESS;
}


void C3DMeshModelBuilder::CreateMeshArchive()
{
	General3DMesh& general_mesh = *m_pMesh;

	// copy materials
	vector<CMMA_Material>& dest_material_buffer = m_MeshModelArchive.GetMaterial();
	const int num_materials = m_pMesh->GetNumMaterials();
	dest_material_buffer.resize( num_materials );
	LOG_PRINT( fmt_string( " - Copying %d material(s)", num_materials ) );
	for( int i=0; i<num_materials; i++ )
	{
		dest_material_buffer[i] = general_mesh.GetMaterialBuffer()[i];
	}

	// copy skeleton
	m_MeshModelArchive.GetSkeletonRootBone() = general_mesh.GetSkeletonRootBoneBuffer();
		
	// convert all polygons to triangles
	Triangulate();

	// if the bumpmap is enabled, local space has to be calculated for each vertex
	if( general_mesh.GetVertexFormatFlags() & CMMA_VertexSet::VF_BUMPMAP )
		CalculateTangentSpace();

	// triangulate polygons and save their indices and triangle sets in mesh model archive
	CreateTriangleSets();

	if( general_mesh.GetVertexFormatFlags() & CMMA_VertexSet::VF_WEIGHT )
	{
		// make sure that every vertex has the same number of blend matrices
		ForceWeightMatrixCount();

		NormalizeVertexBlendWeights();
	}

	// copy general vertices to vertex set in the mesh archive
	CreateVertices();

	// eliminate invalid triangle sets and the corresponding materials
	TrimTriangleSetsAndMaterials();

	// update bounding boxes for triangle sets
	m_MeshModelArchive.UpdateAABBs();

	// calculate the minimum vertex alpha for each material
	m_MeshModelArchive.UpdateMinimumVertexDiffuseAlpha();

	// save the filepaths of the textures
	SaveOrigTextureFilepaths();

	// modify the output texture filenames according to 'm_TextureFilenameOption'
	ProcessTextureFilenames();
}


void C3DMeshModelBuilder::CalculateTangentSpace()
{
	std::vector< MeshMender::Vertex > theVerts;
	std::vector< unsigned int > theIndices;
	std::vector< unsigned int > mappingNewToOld;

	// put indexed triangle polygons into a single index array
	std::vector<IndexedPolygon>& polygon_buffer = m_vecTriangulatedPolygon;

	size_t i, iNumTriangles = polygon_buffer.size();
	theIndices.reserve( iNumTriangles * 3 );
	for( i=0; i<iNumTriangles; i++ )
	{
		IndexedPolygon& triangle = polygon_buffer[i];
		assert( triangle.m_index.size() == 3 );

		theIndices.push_back( triangle.m_index[0] );
		theIndices.push_back( triangle.m_index[1] );
		theIndices.push_back( triangle.m_index[2] );
	}


	// fill up the vectors with your mesh's data
	std::vector<General3DVertex>& vert_buffer = *m_pMesh->GetVertexBuffer().get();
    size_t iNumVertices = vert_buffer.size();
	for( i = 0; i < iNumVertices; ++i )
	{
		MeshMender::Vertex v;
		v.pos = vert_buffer[i].m_vPosition;
		v.s   = vert_buffer[i].m_TextureCoord[0].u;
		v.t   = vert_buffer[i].m_TextureCoord[0].v;

		v.normal = vert_buffer[i].m_vNormal;
		// meshmender will computer normals, tangents, and binormals, no need to fill those in.
		// however, if you do not have meshmender compute the normals, you _must_ pass in valid
		// normals to meshmender
		theVerts.push_back(v);
	}

	MeshMender mender;

	size_t iNumIndices = theIndices.size();

    // pass it in to the mender to do it's stuff
	mender.Mend( theVerts,
		         theIndices,
				 mappingNewToOld,
				 -1.0f,						// minNormalCreaseCos
				 -1.0f,						// minTangentCreaseCos
				 -1.0f,						// minBinormalCreaseCos
				 0.0f,						// weightNormalsByArea
		         MeshMender::DONT_CALCULATE_NORMALS,
				 MeshMender::RESPECT_SPLITS );


	// check indices
	assert( iNumIndices == theIndices.size() );

	// update vertices
	// new vertices may have been created and added to 'theVerts' by mesh mender
	size_t iNumUpdatedVertices = theVerts.size();

	vector<General3DVertex> vecNewVertexBuffer;	// temporary vertex buffer to hold new vertices
	vecNewVertexBuffer.resize( iNumUpdatedVertices );

	for( i=0; i<iNumUpdatedVertices; ++i )
	{
		vecNewVertexBuffer[i] = vert_buffer[ mappingNewToOld[i] ];

		vecNewVertexBuffer[i].m_vNormal   = theVerts[i].normal;
		vecNewVertexBuffer[i].m_vBinormal = theVerts[i].binormal;
		vecNewVertexBuffer[i].m_vTangent  = theVerts[i].tangent;
	}

	// copy vertices to the original vertex buffer
	vert_buffer.clear();
	vert_buffer.reserve( iNumUpdatedVertices );
	vert_buffer = vecNewVertexBuffer;


	// update indices
	int vertex_index = 0;
	for( i=0; i<iNumTriangles; ++i )
	{
		IndexedPolygon& triangle = polygon_buffer[i];

		triangle.m_index[0] = theIndices[vertex_index++];
		triangle.m_index[1] = theIndices[vertex_index++];
		triangle.m_index[2] = theIndices[vertex_index++];
	}
}


void C3DMeshModelBuilder::Triangulate()
{
	amorphous::Triangulate( m_vecTriangulatedPolygon, m_vecNonTriangulatedPolygon );
}


void C3DMeshModelBuilder::CreateVertices()
{
	General3DMesh& general_mesh = *m_pMesh;

	const std::vector<General3DVertex>& vert_buffer = *general_mesh.GetVertexBuffer().get();
	const size_t iNumVertices = vert_buffer.size();
	size_t i;

	if( vert_buffer.size() == 0 )
	{
		LOG_PRINT_WARNING( " - No vertex in the source general 3D mesh." );
		return;
	}

	int j, iNumBlendMatrices;

	CMMA_VertexSet& rVertexSet = m_MeshModelArchive.GetVertexSet();

	rVertexSet.SetVertexFormat( general_mesh.GetVertexFormatFlags() );

	rVertexSet.vecPosition.resize(iNumVertices);
	rVertexSet.vecNormal.resize(iNumVertices);
	rVertexSet.vecDiffuseColor.resize(iNumVertices);


	for( i=0; i<iNumVertices; i++ )
	{
		rVertexSet.vecPosition[i]     = vert_buffer[i].m_vPosition;

		rVertexSet.vecNormal[i]       = vert_buffer[i].m_vNormal;

		rVertexSet.vecDiffuseColor[i] = vert_buffer[i].m_DiffuseColor;
	}

	// set texture coordinates

	// Determine how many sets of texture coodinates are required
	// - Just look at the texture coordinates of the first vertex
	//   since all the vertices should have the same number of tex coords
	rVertexSet.vecTex.resize( vert_buffer[0].m_TextureCoord.size() );

	for( j=0; j<rVertexSet.vecTex.size(); j++ )
	{
		rVertexSet.vecTex[j].resize(iNumVertices);
		for( i=0; i<iNumVertices; i++ )
		{
			rVertexSet.vecTex[j][i] = vert_buffer[i].m_TextureCoord[j];
		}
	}

	// set vertex weights

	if( general_mesh.GetVertexFormatFlags() & CMMA_VertexSet::VF_WEIGHT )
	{
		rVertexSet.vecfMatrixWeight.resize(iNumVertices);
		rVertexSet.veciMatrixIndex.resize(iNumVertices);

		for( i=0; i<iNumVertices; i++ )
		{
			iNumBlendMatrices = vert_buffer[i].m_fMatrixWeight.size();
			rVertexSet.vecfMatrixWeight[i].resize(iNumBlendMatrices);
			rVertexSet.veciMatrixIndex[i].resize(iNumBlendMatrices);

			for( j=0; j<iNumBlendMatrices; j++ )
			{
				rVertexSet.vecfMatrixWeight[i][j] = vert_buffer[i].m_fMatrixWeight[j];
				rVertexSet.veciMatrixIndex[i][j]  = vert_buffer[i].m_iMatrixIndex[j];
			}
		}
	}

	// set binormal & tengents for bumpmap
	if( general_mesh.GetVertexFormatFlags() & CMMA_VertexSet::VF_BUMPMAP )
	{
		rVertexSet.vecBinormal.resize(iNumVertices);
		rVertexSet.vecTangent.resize(iNumVertices);

		for( i=0; i<iNumVertices; i++ )
		{
			rVertexSet.vecBinormal[i]	= vert_buffer[i].m_vBinormal;
			rVertexSet.vecTangent[i]	= vert_buffer[i].m_vTangent;
		}
	}
}


void C3DMeshModelBuilder::ForceWeightMatrixCount()
{
	std::vector<General3DVertex>& vert_buffer = *m_pMesh->GetVertexBuffer().get();
	const size_t num_vertices = vert_buffer.size();

	int num_blend_mats;

	const int num_max_blend_mats = General3DVertex::NUM_MAX_BLEND_MATRICES_PER_VERTEX;

	for( size_t i=0; i<num_vertices; i++ )
	{
		General3DVertex& rVertex = vert_buffer[i];

		num_blend_mats = rVertex.m_iMatrixIndex.size();

		assert( 0 < num_blend_mats );

//		rVertex.m_iMatrixIndex.resize( num_max_blend_mats );

		for( int j=num_blend_mats; j<num_max_blend_mats; j++ )
		{
			rVertex.m_fMatrixWeight.push_back( 0 );
			rVertex.m_iMatrixIndex.push_back( 0 );
		}
	}
}


void C3DMeshModelBuilder::NormalizeVertexBlendWeights()
{
	std::vector<General3DVertex>& vert_buffer = *m_pMesh->GetVertexBuffer().get();

	size_t i, iNumVertices = vert_buffer.size();
	int j, num_blend_mats;

	// normalize blend weights
	float fWeightSum;
	for( i=0; i<iNumVertices; i++ )
	{
		num_blend_mats = vert_buffer[i].m_fMatrixWeight.size();
		fWeightSum = 0;
		for( j=0; j<num_blend_mats; j++ )
			fWeightSum += vert_buffer[i].m_fMatrixWeight[j];

		for( j=0; j<num_blend_mats; j++ )
			vert_buffer[i].m_fMatrixWeight[j] /= fWeightSum;
	}
}


void C3DMeshModelBuilder::CreateTriangleSets()
{
	// m_vecIndexedPolygon must be triangulated before calling this function

	std::vector<IndexedPolygon>& polygon_buffer = m_vecTriangulatedPolygon;

	int iMat, i;
	size_t iNumTriangles = polygon_buffer.size();
	int iNumMaterials = m_pMesh->GetNumMaterials();
	int iNumTris, iMinIndex, iMaxIndex;
	int index[3];

	m_MeshModelArchive.m_vecTriangleSet.resize( iNumMaterials );

	vector<unsigned int>& rvecDestIndexBuffer = m_MeshModelArchive.m_vecVertexIndex;

	rvecDestIndexBuffer.clear();
	rvecDestIndexBuffer.reserve( iNumTriangles * 3 );

	for( iMat=0; iMat<iNumMaterials; iMat++ )
	{
		m_MeshModelArchive.m_vecTriangleSet[iMat].m_iStartIndex = (int)rvecDestIndexBuffer.size();

		// create triangle set for each material
		iNumTris = 0;
		iMinIndex =  999999999;
		iMaxIndex = -999999999;

		for( size_t j=0; j<iNumTriangles; j++ )
		{
            IndexedPolygon& rTriangle = polygon_buffer[j];

			if( rTriangle.m_MaterialIndex != iMat )
				continue;

			assert( rTriangle.m_index.size() == 3 );

			// set triangle indices
			index[0] = rTriangle.m_index[0];
			index[1] = rTriangle.m_index[1];
			index[2] = rTriangle.m_index[2];
			rvecDestIndexBuffer.push_back( index[0] );
			rvecDestIndexBuffer.push_back( index[1] );
			rvecDestIndexBuffer.push_back( index[2] );
			iNumTris++;

			// update min & max index
			for( i=0; i<3; i++)
			{
				iMinIndex = min( iMinIndex, index[i] );
				iMaxIndex = max( iMaxIndex, index[i] );
			}
		}

//		assert( 0 < iNumTris );
//		assert( iMinIndex < iMaxIndex );

		if( iNumTris == 0 )
		{
			m_MeshModelArchive.m_vecTriangleSet[iMat].m_iNumTriangles = 0;
			m_MeshModelArchive.m_vecTriangleSet[iMat].m_iMinIndex = 0;
			m_MeshModelArchive.m_vecTriangleSet[iMat].m_iNumVertexBlocksToCover = 0;
			continue;
		}

		m_MeshModelArchive.m_vecTriangleSet[iMat].m_iNumTriangles = iNumTris;

		m_MeshModelArchive.m_vecTriangleSet[iMat].m_iMinIndex = iMinIndex;
		m_MeshModelArchive.m_vecTriangleSet[iMat].m_iNumVertexBlocksToCover = iMaxIndex - iMinIndex + 1;
	}
}


const char* GetTexFilenameOptionTitle( TexturePathnameOption::Option option )
{
	switch(option)
	{
	case TexturePathnameOption::ORIGINAL_FILENAME:					return "TexturePathnameOption::ORIGINAL_FILENAME";
	case TexturePathnameOption::BODY_FILENAME_ONLY:					return "TexturePathnameOption::BODY_FILENAME_ONLY";
	case TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME:	return "TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME";
	case TexturePathnameOption::FIXEDPATH_AND_FILENAME:				return "TexturePathnameOption::FIXEDPATH_AND_FILENAME";
	case TexturePathnameOption::NO_TEXTURE_FILENAME:				return "TexturePathnameOption::NO_TEXTURE_FILENAME";
	default: return "";
	}
	return "";
}


void C3DMeshModelBuilder::SaveOrigTextureFilepaths()
{
	if( !m_pModelLoader )
		return;

	m_OrigTextureFilepaths.clear();

	const vector<CMMA_Material>& rvecMaterial = m_MeshModelArchive.GetMaterial();

	const size_t num_materials = rvecMaterial.size();
	m_OrigTextureFilepaths.resize( num_materials );
	for( size_t i=0; i<num_materials; i++ )
	{
		const size_t num_textures = rvecMaterial[i].vecTexture.size();
		m_OrigTextureFilepaths[i].resize( num_textures );
		for( size_t tex=0; tex<num_textures; tex++ )
		{
			m_OrigTextureFilepaths[i][tex] = rvecMaterial[i].vecTexture[tex].ResourcePath;
		}
	}
}


void C3DMeshModelBuilder::ProcessTextureFilenames()
{
	if( !m_pModelLoader )
		return;

	vector<CMMA_Material>& rvecMaterial = m_MeshModelArchive.GetMaterial();

	size_t pathlen = 0;
	string strTemp, strDest;

	string strDestFilename, strSrcFilename, strModelPath;
	strSrcFilename = m_strSrcFilename;
	strDestFilename = m_strDestFilename;

	string basepath = m_pModelLoader->GetBasePath();

	LOG_PRINT( string(" - tex filename option: ") + GetTexFilenameOptionTitle(m_pModelLoader->GetTexturePathnameOption()) );

	const size_t num_materials = rvecMaterial.size();
	for( size_t i=0; i<num_materials; i++ )
	{
		const size_t num_textures = rvecMaterial[i].vecTexture.size();
		for( size_t tex=0; tex<num_textures; tex++ )
		{
			string& strTextureFilename = rvecMaterial[i].vecTexture[tex].ResourcePath;

			switch( m_pModelLoader->GetTexturePathnameOption() )
			{
			case TexturePathnameOption::BODY_FILENAME_ONLY:
				break;

			case TexturePathnameOption::FIXEDPATH_AND_FILENAME:
				if( 0 < strTextureFilename.length() )
				{
//					strTextureFilename = m_strTexPath + "\\" + strTemp;
					strTextureFilename = m_pModelLoader->GetFixedPathForTextureFilename() + "\\" + lfs::get_leaf(strTextureFilename);
				}
				break;

			case TexturePathnameOption::ORIGINAL_FILENAME:
				break;

			case TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME:
				strModelPath = lfs::get_parent_path(basepath);
				pathlen = strModelPath.length();

				if( pathlen == 0 )
					break;

				if( strModelPath.rfind("\\") == pathlen-1
				 || strModelPath.rfind("/")  == pathlen-1 )
					strModelPath = strModelPath.substr( 0, pathlen-1 );

				// TODO: find / replace paths correctly

				if( strModelPath.length() < strTextureFilename.length() )
//				if( strTextureFilename.find( strModelPath ) == 0 )
				{
					strDest.assign( strTextureFilename, strModelPath.length(), 1024 );
					strTextureFilename = "." + strDest;
				}
				break;

			case TexturePathnameOption::NO_TEXTURE_FILENAME:
			default:
				break;
			}
		}
	}
}


void C3DMeshModelBuilder::CheckShadowVolume()
{
	if( true /*!(m_MeshFlag & MESH_SHADOWVOLUME)*/ )
		return;


	// convert to shadow volume mesh

	C3DMeshModelArchive sv_mesh;

	CShadowVolumeMeshGenerator sv_mesh_generator;

	sv_mesh_generator.GenerateShadowMesh( sv_mesh, m_MeshModelArchive );

	m_MeshModelArchive = sv_mesh;
}


void C3DMeshModelBuilder::TrimTriangleSetsAndMaterials()
{
	int num_orig_materials = m_pMesh->GetNumMaterials();

	vector<CMMA_TriangleSet>& rvecTriangleSet	= m_MeshModelArchive.GetTriangleSet();
	vector<CMMA_Material>& rvecMaterial			= m_MeshModelArchive.GetMaterial();

	vector<CMMA_TriangleSet>::iterator itrTriSet = rvecTriangleSet.begin();
	vector<CMMA_Material>::iterator itrMaterial = rvecMaterial.begin();

	while( itrTriSet != rvecTriangleSet.end() )
	{
		if( itrTriSet->m_iNumTriangles == 0 )
		{
			itrTriSet = rvecTriangleSet.erase( itrTriSet );
			itrMaterial = rvecMaterial.erase( itrMaterial );
		}
		else
		{
			itrTriSet++;
			itrMaterial++;
		}
	}

//	m_iNumMaterials = (int)rvecMaterial.size();
}


/*
void C3DMeshModelBuilder::SetFilename( int iModelIndex, const char *pcFilename )
{
}*/

/*
void C3DMeshModelBuilder::ProcessTextureFilenames()
{
	vector<CMMA_Material>& rvecMaterial = m_MeshModelArchive.GetMaterial();

	size_t pathlen = 0;
	string strTemp, strDest;

	string strDestFilename, strSrcFilename, strModelPath;
	strSrcFilename = m_strSrcFilename;
	strDestFilename = m_strDestFilename;

	LOG_PRINTF(( "texture filename option: %s", GetTexFilenameOptionTitle(m_TextureFilenameOption) ));

	int i;
	for( i=0; i<m_iNumMaterials; i++ )
	{
		for( size_t tex=0; tex<num_textures; tex++ )
		{
		}
		string& strSurfaceTexture = rvecMaterial[i].SurfaceTexture.strFilename;
		string& strNormalMapTexture = rvecMaterial[i].NormalMapTexture.strFilename;

		switch( m_TextureFilenameOption )
		{
		case TexturePathnameOption::BODY_FILENAME_ONLY:
			break;

		case TexturePathnameOption::FIXEDPATH_AND_FILENAME:
			if( 0 < strSurfaceTexture.length() )
			{
//				strSurfaceTexture = m_strTexPath + "\\" + strTemp;
				strSurfaceTexture = m_strTexPath + "\\" + lfs::get_leaf(strSurfaceTexture);
			}

			if( 0 < strNormalMapTexture.length() )
			{
//				strNormalMapTexture = m_strTexPath + "\\" + strTemp;
				strNormalMapTexture = m_strTexPath + "\\" + lfs::get_leaf(strNormalMapTexture);
			}
			break;

		case TexturePathnameOption::ORIGINAL_FILENAME:
			break;

		case TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME:
			strModelPath = fnop::get_path(strSrcFilename);
			pathlen = strModelPath.length();
			if( pathlen == 0 )
				break;
			if( strModelPath.rfind("\\") == pathlen-1
			 || strModelPath.rfind("/")  == pathlen-1 )
				strModelPath = strModelPath.substr( 0, pathlen-1 );

			// TODO: find / replace paths correctly

			if( strModelPath.length() < strSurfaceTexture.length() )
//			if( strSurfaceTexture.find( strModelPath ) == 0 )
			{
				strDest.assign( strSurfaceTexture, strModelPath.length(), 1024 );
				strSurfaceTexture = strDest;
			}

			if( strModelPath.length() < strNormalMapTexture.length() )
//			if( strNormalMapTexture.find( strModelPath ) == 0 )
			{
				strDest.assign( strNormalMapTexture, strModelPath.length(), 1024 );
				strNormalMapTexture = strDest;
			}
			break;

		case TexturePathnameOption::NO_TEXTURE_FILENAME:
		default:
			break;
		}
	}
}
*/



} // namespace amorphous
