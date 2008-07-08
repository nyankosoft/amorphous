#include "GraphicsResource.h"
#include "GraphicsResourceEntries.h"
#include "3DCommon/MeshModel/3DMeshModelArchive.h"
using namespace MeshModel;
#include "Support/fnop.h"
#include "Support/BitmapImage.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;


inline bool is_db_filepath_and_keyname( const std::string& filepath )
{
	return ( filepath.find("::") != std::string::npos );
}

inline bool decompose_into_db_filepath_and_keyname( const std::string& src_filepath,
												    std::string& db_filepath,
												    std::string& keyname
												    )
{
	const size_t pos = src_filepath.find( "::" );
	if( pos == std::string::npos )
		return false;

	// found "::" in filename
	// - "(binary database filename)::(key)"
	db_filepath = src_filepath.substr( 0, pos );
	keyname     = src_filepath.substr( pos + 2, src_filepath.length() );

	return true;
}


class CGraphicsResourceLoader
{
protected:

	virtual const std::string& GetSourceFilepath() = 0;

public:

	inline bool LoadFromDisk();

	virtual bool LoadFromFile( const std::string& filepath ) = 0;

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname ) = 0;

	/// Called by the resource IO thread
	/// - copy the loaded resource to locked buffer
	virtual bool CopyTo( CGraphicsResourceEntry *pDestEntry ) { return false; }
};


inline bool CGraphicsResourceLoader::LoadFromDisk()
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


/// loads a texture from disk
class CDiskTextureLoader : public CGraphicsResourceLoader
{
	CTextureResourceDesc m_Desc;

	/// stores texture data loaded from disk
	CBitmapImage m_Image;

protected:

	const std::string& GetSourceFilepath() { return m_Desc.Filename; }

public:

	bool LoadFromFile( const std::string& filepath );

	/// load image from the db as an image archive
	bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	/// copy the bitmap image to the locked texture surface
	bool CopyTo( CGraphicsResourceEntry *pDestEntry );

	/// called by the system
	/// - called inside CopyTo()
	void FillTexture( CLockedTexture& texture );
};


class CMeshLoader : public CGraphicsResourceLoader
{
	CMeshResourceDesc m_Desc;

	C3DMeshModelArchive m_Archive;

protected:

	const std::string& GetSourceFilepath() { return m_Desc.Filename; }

public:

	bool LoadFromFile( const std::string& filepath )
	{
		return m_Archive.LoadFromFile( m_Desc.Filename );
	}

	bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
	{
		return db.GetData( keyname, m_Archive );
	}

	bool CopyTo( CGraphicsResourceEntry *pDestEntry );

	void FillLockedMeshVB();

	void FillLockedMeshIB();
};
