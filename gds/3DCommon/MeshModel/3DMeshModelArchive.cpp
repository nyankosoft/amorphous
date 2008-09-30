#include "3DMeshModelArchive.h"

#include <fstream>

#include "3DCommon/General3DVertex.h"
#include "Support/Vec3_StringAux.h"
#include "Support/ImageArchive.h"

using namespace GameLib1::Serialization;
using namespace MeshModel;

using namespace std;


//=========================================================================================
// CMMA_VertexSet
//=========================================================================================

void CMMA_VertexSet::Resize( int i )
{
	vecPosition.resize(i);
	vecNormal.resize(i);
	vecBinormal.resize(i);
	vecTangent.resize(i);
	vecDiffuseColor.resize(i);

	int j;
	for( j=0; j<vecTex.size(); j++ )
		vecTex[j].resize(i);

	vecfMatrixWeight.resize(i);
	veciMatrixIndex.resize(i);

/*	for( j=0; j<vecfMatrixWeight.size(); j++ )
		vecfMatrixWeight[j].resize(i);

	for( j=0; j<veciMatrixIndex.size(); j++ )
		veciMatrixIndex[j].resize(i);*/
}


void CMMA_VertexSet::Clear()
{
	vecPosition.clear();
	vecNormal.clear();
	vecBinormal.clear();
	vecTangent.clear();
	vecDiffuseColor.clear();

	int j;
	for( j=0; j<vecTex.size(); j++ )
		vecTex[j].clear();

	vecfMatrixWeight.clear();
	veciMatrixIndex.clear();

/*	for( j=0; j<vecfMatrixWeight.size(); j++ )
		vecfMatrixWeight[j].clear();

	for( j=0; j<veciMatrixIndex.size(); j++ )
		veciMatrixIndex[j].clear();*/
}


void CMMA_VertexSet::GetBlendMatrixIndices_4Bytes( int iVertexIndex, unsigned char *pIndices )
{
	int i;
	TCFixedVector<int,NUM_MAX_BLEND_MATRICES_PER_VERTEX>& rMatIndex = veciMatrixIndex[iVertexIndex];
//	vector<int>& rMatIndex = veciMatrixIndex[iVertexIndex];
	int iNumMatrices = rMatIndex.size();

	for( i=0; i<iNumMatrices; i++ )
	{
		if( rMatIndex[i] < 0 ) pIndices[i] = 0;
		if( 255 < rMatIndex[i] ) pIndices[i] = 255;
		else pIndices[i] = (unsigned char)rMatIndex[i];
	}

	for( ; i<4; i++ )
	{
		pIndices[i] = 0;
	}
}


void CMMA_VertexSet::GetBlendMatrixWeights_4Floats( int iVertexIndex, float *pWeight )
{
	int i;
	TCFixedVector<float,NUM_MAX_BLEND_MATRICES_PER_VERTEX>& rMatWeight = vecfMatrixWeight[iVertexIndex];
//	vector<float>& rMatWeight = vecfMatrixWeight[iVertexIndex];
	int iNumMatrices = rMatWeight.size();

	for( i=0; i<iNumMatrices; i++ )
	{
		pWeight[i] = rMatWeight[i];
	}

	for( ; i<4; i++ )
	{
		pWeight[i] = 0;
	}
}


void CMMA_VertexSet::GetVertices( vector<CGeneral3DVertex>& dest_buffer ) const
{
	dest_buffer.resize( GetNumVertices() );

	size_t i, num_verts = GetNumVertices();
	if( m_VertexFormatFlag & VF_POSITION )
	{
		for( i=0; i<num_verts; i++ )
			dest_buffer[i].m_vPosition = vecPosition[i];
	}

	if( m_VertexFormatFlag & VF_NORMAL )
	{
		for( i=0; i<num_verts; i++ )
			dest_buffer[i].m_vNormal = vecNormal[i];
	}

	if( m_VertexFormatFlag & VF_DIFFUSE_COLOR )
	{
		for( i=0; i<num_verts; i++ )
			dest_buffer[i].m_DiffuseColor = vecDiffuseColor[i];
	}

	if( m_VertexFormatFlag & VF_BUMPMAP )
	{
		for( i=0; i<num_verts; i++ )
		{
			dest_buffer[i].m_vBinormal = vecBinormal[i];
			dest_buffer[i].m_vTangent  = vecTangent[i];
		}
	}

	if( m_VertexFormatFlag & VF_2D_TEXCOORD )
	{
		for( i=0; i<num_verts; i++ )
		{
			dest_buffer[i].m_TextureCoord.resize( 1 );
			dest_buffer[i].m_TextureCoord[0] = vecTex[0][i];
		}
	}

	if( m_VertexFormatFlag & VF_WEIGHT )
	{
		for( i=0; i<num_verts; i++ )
		{
//			dest_buffer[i].m_fMatrixWeight = ;
//			dest_buffer[i].m_fMatrixWeight = ;
		}
	}

/*	if( m_VertexFormatFlag & VF_POSITION )
	{
		for( i=0; i<num_verts; i++ )
			dest_buffer[i]. = ;
	}*/
}


void CMMA_VertexSet::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_VertexFormatFlag;

	ar & vecPosition;

	ar & vecNormal;

	ar & vecBinormal;
	ar & vecTangent;

	ar & vecDiffuseColor;

	ar & vecTex;

	ar & vecfMatrixWeight;
	ar & veciMatrixIndex;
}


//=========================================================================================
// CMMA_TriangleSet
//=========================================================================================

void CMMA_TriangleSet::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_iStartIndex;
	
	ar & m_iMinIndex;

	ar & m_iNumVertexBlocksToCover;

	ar & m_iNumTriangles;

	if( 1 <= version )
        ar & m_AABB;
}


//=========================================================================================
// CMMA_Texture
//=========================================================================================

void CMMA_Texture::Serialize( IArchive& ar, const unsigned int version )
{
	ar & type;
	ar & strFilename;
	ar & vecTexelData;
	ar & vecfTexelData;
}


//=========================================================================================
// CMMA_Material
//=========================================================================================

void CMMA_Material::Serialize( IArchive& ar, const unsigned int version )
{
	if( 2 <= version )
		ar & Name;

	ar & fSpecular;
//	ar & SurfaceTexture;
//	ar & NormalMapTexture;

	if( version < 1 )
		assert( !string( string(__FUNCTION__) + " : archive version outdated - no longer supported" ).c_str() );

	ar & vecTexture;

	if( 3 <= version )
		ar & fMinVertexDiffuseAlpha;
}


void CMMA_Material::SetDefault()
{
	fSpecular = 0;
	vecTexture.resize( 0 );

	fMinVertexDiffuseAlpha = 1.0f;

//	strSurfaceTexture = "";
//	strNormalMapTexture = "";
}


//=========================================================================================
// CMMA_Bone
//=========================================================================================

int CMMA_Bone::GetNumBones_r() const
{
	int num = 0;
	for( size_t i=0; i<vecChild.size(); i++ )
	{
		num += vecChild[i].GetNumBones_r();
	}

	return num + 1;
}


void CMMA_Bone::Serialize( IArchive& ar, const unsigned int version )
{
	ar & strName;
	ar & vLocalOffset;
	ar & BoneTransform;
	ar & vecChild;
}



//void AddTexturesToBinaryDatabase( C3DMeshModelArchive& mesh_archive, ... ) /// error: must specify the namespace. See below

void MeshModel::AddTexturesToBinaryDatabase( C3DMeshModelArchive& mesh_archive,
								             const string& db_filepath,
								             CBinaryDatabase<string> &db,
								             bool bUseTextureBasenameForKey )
{
	const size_t num_materials = mesh_archive.GetMaterial().size();
	for( size_t i=0; i<num_materials; i++ )
	{
		const size_t num_textures = mesh_archive.GetMaterial()[i].vecTexture.size();
		for( size_t j=0; j<num_textures; j++ )
		{
			CMMA_Texture& rTexture = mesh_archive.GetMaterial()[i].vecTexture[j];

			const string tex_filename = rTexture.strFilename;

			if( tex_filename.length() == 0 )
				continue;

			if( tex_filename.find("::") != string::npos )
			{
				LOG_PRINT( " - The texture filepath already includes db & keyname separator '::'. Skipping the process for" + tex_filename );
				continue;
			}

			string tex_key;
			
			if( bUseTextureBasenameForKey )
				tex_key = fnop::get_nopath(tex_filename);
			else
				tex_key = tex_filename;

	//		string tex_resource_name = db_filename + "::" + tex_key;
			string tex_resource_name = db_filepath + "::" + tex_key;

			// overwrite original texture filename with the resource name
			// - (db filename) + "::" + (archive key name)
			rTexture.strFilename = tex_resource_name;

			if( db.KeyExists(tex_key) )
			{
				// the texture file has been already saved to database
				// - skip this texture
				continue;
			}

			CImageArchive img_archive = CImageArchive( tex_filename );

			if( img_archive.IsValid() )
			{
				// add image archive to db
				db.AddData( tex_key, img_archive );
			}
			else
				LOG_PRINT_ERROR( " - invalid texture filepath: " + tex_filename );
		}
	}
}


//=========================================================================================
// C3DMeshModelArchive
//=========================================================================================

C3DMeshModelArchive::C3DMeshModelArchive()
{
	m_AABB.Nullify();
}


C3DMeshModelArchive::~C3DMeshModelArchive()
{
}


void C3DMeshModelArchive::Serialize( IArchive& ar, const unsigned int version )
{
	if( version < 2 )
		assert( !string( string(__FUNCTION__) + " : archive version outdated - no longer supported" ).c_str() );

	ar & m_VertexSet;

	ar & m_vecVertexIndex;

	ar & m_vecMaterial;
	ar & m_vecTriangleSet;

	ar & m_SkeletonRootBone;

	ar & m_AABB;
}


void C3DMeshModelArchive::SetMaterial( unsigned int index, CMMA_Material& rMaterial )
{
	if( m_vecMaterial.size() <= index )
		m_vecMaterial.resize( index + 1 );

	m_vecMaterial[index] = rMaterial;
}


void C3DMeshModelArchive::UpdateAABBs()
{
	m_AABB.Nullify();

	// create aabb for each triangle set
	size_t i, num_triangle_sets = m_vecTriangleSet.size();
	for( i=0; i<num_triangle_sets; i++ )
	{
		CMMA_TriangleSet& rTriSet = m_vecTriangleSet[i];
		const vector<Vector3>& vecVertPosition = m_VertexSet.vecPosition;
		const vector<unsigned int>& index_buffer = m_vecVertexIndex;

		AABB3 aabb;
		aabb.Nullify();
		size_t offset = rTriSet.m_iStartIndex;
		size_t j, num_triangles = rTriSet.m_iNumTriangles;
		for( j=0; j<num_triangles*3; j++ )
		{
			aabb.AddPoint( vecVertPosition[index_buffer[offset+j]] );
		}

		rTriSet.m_AABB = aabb;

		m_AABB.MergeAABB( aabb );
	}
}


void C3DMeshModelArchive::UpdateMinimumVertexDiffuseAlpha()
{
	if( m_VertexSet.vecDiffuseColor.size() == 0 )
		return; // the mesh vertices do not have diffuse colors

	const vector<SFloatRGBAColor>& vecDiffuseColor = m_VertexSet.vecDiffuseColor;

	float min_alpha;
	const size_t num_materials = m_vecMaterial.size();
	for( size_t i=0; i<num_materials; i++ )
	{
		CMMA_TriangleSet& rTriSet = m_vecTriangleSet[i];

		min_alpha = 1.0f;
		float alpha = min_alpha;
		const size_t start_index = rTriSet.m_iStartIndex;
		const size_t end_index   = rTriSet.m_iStartIndex + rTriSet.m_iNumTriangles * 3;
		for( size_t j=start_index; j<end_index; j++ )
		{
			alpha = vecDiffuseColor[ m_vecVertexIndex[j] ].fAlpha;
			if( alpha < min_alpha )
				min_alpha = alpha;
		}

		m_vecMaterial[i].fMinVertexDiffuseAlpha = min_alpha;
	}
}


void C3DMeshModelArchive::Scale( float factor )
{
	CMMA_VertexSet& vertex_set = GetVertexSet();
	vector<Vector3>& vecPosition = vertex_set.vecPosition;

//	int num_vertices = vertex_set.GetNumVertices();
	size_t i, num_vertices = vecPosition.size();

	for( i=0; i<num_vertices; i++ )
	{
		vecPosition[i] *= factor;
	}
}


#define INVALID_POINT_REPRESENTATIVE	65535

void C3DMeshModelArchive::GeneratePointRepresentatives( vector<unsigned short>& rvecusPtRep )
{
	int i, j;
	const int iNumVeritices = m_VertexSet.GetNumVertices();

	vector<Vector3>& vecvPosition = m_VertexSet.vecPosition;

	rvecusPtRep.resize( iNumVeritices, INVALID_POINT_REPRESENTATIVE );

	for( i=0; i<iNumVeritices; i++ )
	{
		if( rvecusPtRep[i] != INVALID_POINT_REPRESENTATIVE )
			continue;

		rvecusPtRep[i] = i;

		for( j=i+1; j<iNumVeritices; j++ )
		{
			if( vecvPosition[i] == vecvPosition[j] )
				rvecusPtRep[j] = i;
		}

	}
}


void C3DMeshModelArchive::CopySkeletonFrom( C3DMeshModelArchive& rMesh )
{
	m_SkeletonRootBone = rMesh.m_SkeletonRootBone;
}


void C3DMeshModelArchive::WriteToTextFile( const string& filename )
{
//	fstream f;	f.open( filename, ios::out );

	FILE *fp = fopen( filename.c_str(), "w" );

	if( !fp )
		return;

	int i, iNumVeritices = m_VertexSet.GetNumVertices();

	fprintf( fp, "%d vertices\n", iNumVeritices );

	fprintf( fp, "============== POSITION ==============\n" );

	for( i=0; i<iNumVeritices; i++ )
	{
		Vector3& vPos = m_VertexSet.vecPosition[i];
		fprintf( fp, "%.3f %.3f %.3f\n", vPos.x, vPos.y, vPos.z );
//		f << vPos.x << vPos.y << vPos.z;
	}


	if( 0 < m_VertexSet.vecNormal.size() )
	{
		fprintf( fp, "============== NORMAL ==============\n" );

		for( i=0; i<iNumVeritices; i++ )
		{
			Vector3& vNormal = m_VertexSet.vecNormal[i];
			fprintf( fp, "%.3f %.3f %.3f\n", vNormal.x, vNormal.y, vNormal.z );
		}
	}

	if( m_VertexSet.GetVertexFormat() & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		fprintf( fp, "============== VERTEX COLOR (RGBA) ==============\n" );

		for( i=0; i<iNumVeritices; i++ )
		{
			const SFloatRGBAColor& color = m_VertexSet.vecDiffuseColor[i];
			fprintf( fp, "%.2f %.2f %.2f %.2f\n", color.fRed, color.fGreen, color.fBlue, color.fAlpha );
		}
	}

	if( 0 < m_VertexSet.vecBinormal.size() )
	{
		fprintf( fp, "============== BINORMAL ==============\n" );

		for( i=0; i<iNumVeritices; i++ )
		{
			const Vector3& vBinormal = m_VertexSet.vecBinormal[i];
			fprintf( fp, "%.3f %.3f %.3f\n", vBinormal.x, vBinormal.y, vBinormal.z );
		}
	}

	if( 0 < m_VertexSet.vecTangent.size() )
	{
		fprintf( fp, "============== TANGENT ==============\n" );

		for( i=0; i<iNumVeritices; i++ )
		{
			const Vector3& vTangent = m_VertexSet.vecTangent[i];
			fprintf( fp, "%.3f %.3f %.3f\n", vTangent.x, vTangent.y, vTangent.z );
		}
	}

	if( m_VertexSet.GetVertexFormat() & CMMA_VertexSet::VF_WEIGHT )
	{
		fprintf( fp, "============== VERTEX BLEND WEIGHT (w1, w2, w3, ... ) ==============\n" );

		int j;
		for( i=0; i<iNumVeritices; i++ )
		{
			TCFixedVector<int,CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX> weight_index = m_VertexSet.veciMatrixIndex[i];
			TCFixedVector<float,CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX> weight = m_VertexSet.vecfMatrixWeight[i];

			for( j=0; j<weight_index.size(); j++ )	{ fprintf( fp, "%02d", weight_index[j] );	if( j<weight_index.size()-1 ) fprintf( fp, ", " ); }
			fprintf( fp, "\n" );
			for( j=0; j<weight.size(); j++ )		{ fprintf( fp, "%.3f", weight[j] );			if( j<weight.size()-1 ) fprintf( fp, ",    " ); }
			fprintf( fp, "\n" );
		}
	}
/*
	for( i=0; i<iNumVeritices; i++ )
	{
		Vector3& vBinormal = m_VertexSet.vecBinormal[i];
		fprintf( fp, "%.3f %.3f %.3f\n", vBinormal.x, vBinormal.y, vBinormal.z );
	}

	for( i=0; i<iNumVeritices; i++ )
	{
		Vector3& vPos = m_VertexSet.vecPosition[i];
		fprintf( fp, "%.3f %.3f %.3f\n", vPos.x, vPos.y, vPos.z );
	}*/

	int iNumTris;
	const int num_materials = (int)m_vecTriangleSet.size();
	int offset, tri;

	fprintf( fp, "\n\n" );
	fprintf( fp, "%d materials\n", num_materials );
	fprintf( fp, "total %d triangles\n\n", m_vecVertexIndex.size() / 3 );

	vector<unsigned int>& rvecIndex = m_vecVertexIndex;
	for( i=0; i<num_materials; i++ )
	{
		fprintf( fp, "material[%d]\n", i );

		const size_t num_textures = m_vecMaterial[i].vecTexture.size();
		for( size_t tex=0; tex<num_textures; tex++ )
			fprintf( fp, "texture[%d]: \"%s\"\n", tex, m_vecMaterial[i].vecTexture[tex].strFilename.c_str() );

		fprintf( fp, "min. vertex alpha: %f\n", m_vecMaterial[i].fMinVertexDiffuseAlpha );

//		fprintf( fp, "surface texture:    \"%s\"\n", m_vecMaterial[i].SurfaceTexture.strFilename.c_str() );
//		fprintf( fp, "normal map texture: \"%s\"\n", m_vecMaterial[i].NormalMapTexture.strFilename.c_str() );

		iNumTris = m_vecTriangleSet[i].m_iNumTriangles;
		offset = m_vecTriangleSet[i].m_iStartIndex;

		fprintf( fp, "%d triangles\n", iNumTris );

		for( tri=0; tri<iNumTris; tri++ )
		{
			fprintf( fp, "%d ",  m_vecVertexIndex[offset + tri * 3] );
			fprintf( fp, "%d ",  m_vecVertexIndex[offset + tri * 3 + 1] );
			fprintf( fp, "%d\n", m_vecVertexIndex[offset + tri * 3 + 2] );
		}
        fprintf( fp, "\n" );
	}

	fprintf( fp, "\n============== TEXTURE COORDINATES ==============\n" );

	if( 0 < m_VertexSet.vecTex.size() )
	{
		for( i=0; i<iNumVeritices; i++ )
		{
			TEXCOORD2& t = m_VertexSet.vecTex[0][i];
			fprintf( fp, "%.3f %.3f\n", t.u, t.v );
		}
	}

	fprintf( fp, "\n### triangle sets ###\n" );

	const int num_triangle_sets = (int)m_vecTriangleSet.size();
	for( i=0; i<num_triangle_sets; i++ )
	{
		CMMA_TriangleSet& tri_set = m_vecTriangleSet[i];
		fprintf( fp, "[%02d]-------------------------\n", i );
		fprintf( fp, "start index:  %d\n", tri_set.m_iStartIndex );
		fprintf( fp, "triangles:    %d\n", tri_set.m_iNumTriangles );
		fprintf( fp, "min index:    %d\n", tri_set.m_iMinIndex );
		fprintf( fp, "vert. blocks: %d\n", tri_set.m_iNumVertexBlocksToCover	);
		fprintf( fp, "aabb:         %s\n", to_string(tri_set.m_AABB).c_str() );
	}

	fclose(fp);
}
