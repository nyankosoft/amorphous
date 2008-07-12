#include "GraphicsResourceLoaders.h"
#include "D3DXMeshObjectBase.h"
#include "3DCommon/MeshModel/3DMeshModelArchive.h"

using namespace boost;


//===================================================================================
// CGraphicsResourceLoader
//===================================================================================

bool CGraphicsResourceLoader::LoadFromDisk()
{
	bool loaded = false;
	string target_filepath;
	const string src_filepath = GetSourceFilepath();

	if( is_db_filepath_and_keyname( src_filepath ) )
	{
		// decompose the string
		string db_filename, keyname;
		decompose_into_db_filepath_and_keyname( src_filepath, db_filename, keyname );

		string cwd = fnop::get_cwd();

		//if( !boost::filesystem::exists(  ) )
		//	return InvalidPath; db path is invalid - abort

		CBinaryDatabase<string> db;
		bool db_open = db.Open( db_filename );
		if( !db_open )
			return false; // the database is being used by someone else - retry later

		loaded = LoadFromDB( db, keyname );

		target_filepath = db_filename;
	}
	else
	{
		loaded = LoadFromFile( src_filepath );

		target_filepath = src_filepath;
	}

	return loaded;
}



//===================================================================================
// CDiskTextureLoader
//===================================================================================

bool CDiskTextureLoader::LoadFromFile( const std::string& filepath )
{
	return m_Image.LoadFromFile( GetSourceFilepath() );
}


bool CDiskTextureLoader::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	CImageArchive img_archive;
	bool retrieved = db.GetData( keyname, img_archive );
	if( retrieved )
	{
		bool image_loaded = m_Image.CreateFromImageArchive( img_archive );
		return image_loaded;
	}
	else
		return false;
}


bool CDiskTextureLoader::CopyTo( CGraphicsResourceEntry *pDestEntry )
{
	CTextureEntry *pTextureEntry = dynamic_cast<CTextureEntry *>(pDestEntry);
	if( !pTextureEntry )
		return false;

	CLockedTexture locked_texture;
	bool retrieved = pTextureEntry->GetLockedTexture( locked_texture );
	if( retrieved )
	{
		FillTexture( locked_texture );
		return true;
	}
	else
		return false;
}


void CDiskTextureLoader::FillTexture( CLockedTexture& texture )
{
//	if( !m_Image.IsLoaded() )
//		return;

//	texture.Clear( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ) );

	const int w = m_Image.GetWidth();
	const int h = m_Image.GetHeight();
	for( int y=0; y<h; y++ )
	{
		for( int x=0; x<w; x++ )
		{
			texture.SetPixelARGB32( x, y, m_Image.GetPixelARGB32(x,y) );
		}
	}
}


//===================================================================================
// CMeshLoader
//===================================================================================

CMeshLoader::CMeshLoader( boost::weak_ptr<CMeshObjectEntry> pMeshEntry )
:
m_pMeshEntry(pMeshEntry)
{
	m_pArchive = boost::shared_ptr<C3DMeshModelArchive>( new C3DMeshModelArchive() );
}


bool CMeshLoader::LoadFromFile( const std::string& filepath )
{
	if( m_pArchive )
		return m_pArchive->LoadFromFile( GetSourceFilepath() );
	else
		return false;
}


bool CMeshLoader::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	return db.GetData( keyname, *m_pArchive );
}


bool CMeshLoader::CopyTo( CGraphicsResourceEntry *pDestEntry )
{
	CMeshObjectEntry *pMeshEntry = dynamic_cast<CMeshObjectEntry *>(pDestEntry);
	if( !pMeshEntry )
		return false;

	CD3DXMeshObjectBase *pMesh = pMeshEntry->GetMeshObject();
	if( !pMesh )
		return false;

	// copy vertices to VB / IB?

//	pMesh->


	return true;
}


bool CD3DXMeshVerticesLoader::LoadFromArchive()
{
	GetMesh()->LoadVertices( m_pVertexBufferContent, *(m_pArchive.get()) );

	return true;
}


bool CD3DXMeshVerticesLoader::Lock()
{
	bool locked = GetMesh()->LockVertexBuffer( m_pLockedVertexBuffer );
	return locked;
}


bool CD3DXMeshVerticesLoader::Unlock()
{
	bool unlocked = GetMesh()->UnlockVertexBuffer();
	m_pLockedVertexBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshVerticesLoader::CopyTo( CGraphicsResourceEntry *pDestEntry )
{
	if( m_pLockedVertexBuffer )
		memcpy( m_pLockedVertexBuffer, m_pVertexBufferContent, GetMesh()->GetVertexSize() * m_pArchive->GetVertexSet().GetNumVertices() );

	return true;
}


bool CD3DXMeshIndicesLoader::LoadFromArchive()
{
	unsigned short *pIBData;
	GetMesh()->LoadIndices( pIBData, *(m_pArchive.get()) );
	m_pIndexBufferContent = (void *)pIBData;
	return true;
}


bool CD3DXMeshIndicesLoader::Lock()
{
	bool locked = GetMesh()->LockIndexBuffer( m_pLockedIndexBuffer );
	return locked;
}


bool CD3DXMeshIndicesLoader::Unlock()
{
	bool unlocked = GetMesh()->UnlockIndexBuffer();
	m_pLockedIndexBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshIndicesLoader::CopyTo( CGraphicsResourceEntry *pDestEntry )
{
	if( m_pLockedIndexBuffer )
		memcpy( m_pLockedIndexBuffer, m_pIndexBufferContent, m_IndexBufferSize );

	return true;
}


bool CD3DXMeshAttributeTableLoader::Lock()
{
//	HRESULT hr = GetMesh()->SetAttributeTable( paAttribTable, GetMesh()->GetNumMaterials() );

//	SafeDeleteArray( paAttribTable );

	bool locked = GetMesh()->LockAttributeBuffer( m_pLockedAttributeBuffer );
	return locked;
}


bool CD3DXMeshAttributeTableLoader::Unlock()
{
	bool unlocked = GetMesh()->UnlockAttributeBuffer();
	m_pLockedAttributeBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshAttributeTableLoader::CopyTo( CGraphicsResourceEntry *pDestEntry )
{
	if( !m_pLockedAttributeBuffer )
		return false;

	const vector<CMMA_TriangleSet>& vecTriangleSet = m_pArchive->GetTriangleSet();

	DWORD *pdwBuffer = m_pLockedAttributeBuffer;
	DWORD face = 0;
	for( int i=0; i<GetMesh()->GetNumMaterials(); i++ )
	{
		const CMMA_TriangleSet& triangle_set = vecTriangleSet[i];

		DWORD face_start = triangle_set.m_iStartIndex / 3;
		DWORD num_faces = triangle_set.m_iNumTriangles;
		for( face=face_start; face<face_start + num_faces; face++ )
		{
			pdwBuffer[face] = i;
		}
	}

	return true;
}
