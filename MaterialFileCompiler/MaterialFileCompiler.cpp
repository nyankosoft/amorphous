
#include "MaterialFileCompiler.h"

#include "Support/FilenameOperation.h"

#define MAX_LINE_LENGTH 1024


CMaterialFileCompiler::CMaterialFileCompiler()
{
//	m_strOutputDirectory = "D:\\3D_Development\\Project1\\GameMain\\Stage\\";
}


bool CMaterialFileCompiler::Compile( const char *pcFilename )
{
	FILE* fp = fopen( pcFilename, "r");

	if(!fp)
		return false;

	char acLine[MAX_LINE_LENGTH+1];
	while( fgets(acLine, MAX_LINE_LENGTH, fp) )
	{
		if( strcmp(acLine, "[BEGIN]\n") == 0 )
		{
//			CSurfaceMaterialData material;
//			m_vecMaterial.push_back( material );
//			m_vecMaterial.back().LoadFromTextFile( fp );
			m_MaterialArchive.m_vecSurfaceMaterial.push_back( CSurfaceMaterialData() );
			m_MaterialArchive.m_vecSurfaceMaterial.back().LoadFromTextFile( fp );
		}
	}

	fclose(fp);

	return SaveToBinaryFile( pcFilename );

//	return true;
}


bool CMaterialFileCompiler::SaveToBinaryFile( const char* pcFilename )
{
	// make the output filename

	char acBodyFilename[512];
	CFileNameOperation::GetBodyFilename( acBodyFilename, pcFilename );

	string strOutputFile = m_strOutputDirectory + acBodyFilename;

	// change extension to ".mat"
	CFileNameOperation::ChangeExtension( strOutputFile, "mat" );

	CBinaryArchive_Output archive( strOutputFile.c_str() );

	archive << m_MaterialArchive;

	return true;

/*	FILE* fp = fopen( strOutputFile.c_str(), "wb");

	if(!fp)
		return false;

	int i, iNumMaterials = m_vecMaterial.size();

	// write header
	SMaterialFileHeader header;

	header.iNumMaterials = iNumMaterials;

	fwrite( &header, sizeof(SMaterialFileHeader), 1, fp );

	// write each material
	for( i=0; i<m_vecMaterial.size(); i++ )
	{
		m_vecMaterial[i].SaveToBinaryFile( fp );
	}

	fclose(fp);
	return true;*/
}