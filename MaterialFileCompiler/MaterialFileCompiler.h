
#ifndef  __MATERIALFILECOMPILER_H__
#define  __MATERIALFILECOMPILER_H__

#include <stdio.h>

#include "Support/FixedVector.h"

//#include "BSPStageCommon/SurfaceMaterial.h"
#include "BSPStageCommon/SurfaceMaterialData.h"


#include <string>
using namespace std;


class CMaterialFileCompiler
{

//	TCFixedVector<CSurfaceMaterial, 256> m_vecMaterial;
//	TCFixedVector<CSurfaceMaterialData, 256> m_vecMaterial;

	/// holds an array of surface materials
	CSurfaceMaterialArchive m_MaterialArchive;

	string m_strOutputDirectory;

	bool SaveToBinaryFile( const char* pcFilename );

public:

	CMaterialFileCompiler();

	// load the text file to compile the binary material data file
	bool Compile( const char *pcFilename );

	void SetOutputDirectory( const char* pcOutputPath ) { m_strOutputDirectory = pcOutputPath; }

};


#endif  /*  __MATERIALFILECOMPILER_H__  */


