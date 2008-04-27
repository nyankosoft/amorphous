
#include "SurfaceMaterialManager.h"

#include "Support/memory_helpers.h"
#include "Support/TextFileScanner.h"
#include "Support/Log/DefaultLog.h"


CSurfaceMaterialManager::CSurfaceMaterialManager()
{}


CSurfaceMaterialManager::~CSurfaceMaterialManager()
{}


void CSurfaceMaterialManager::Release()
{
	m_vecMaterial.clear();
}


bool CSurfaceMaterialManager::LoadFromTextFile( const std::string& filename )
{
	CTextFileScanner scanner( filename );

	if( !scanner.IsReady() )
	{
		LOG_PRINT_ERROR( "cannot open file: " + filename );
		return false;
	}

	m_vecMaterial.reserve( 16 );

	for( ; !scanner.End(); scanner.NextLine() )
	{
		if( scanner.GetTagString() == "[Material:Start]" )
		{
			m_vecMaterial.push_back( CSurfaceMaterial() );
			m_vecMaterial.back().LoadFromTextFile( scanner );
		}
	}

	return true;
}


void CSurfaceMaterialManager::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecMaterial;
}

/*
bool CSurfaceMaterialManager::LoadFromFile( const string& filename )
{
	CBinaryArchive_Input archive( filename.c_str() );

	CSurfaceMaterialArchive material_archive;

	if( !(archive >> material_archive) )
		return false;

	size_t i, iNumMaterials = material_archive.m_vecSurfaceMaterial.size();

	m_vecMaterial.resize( iNumMaterials );

	for( i=0; i<iNumMaterials; i++ )
	{
		m_vecMaterial[i].Load( material_archive.m_vecSurfaceMaterial[i] );
	}

	return true;
}
*/