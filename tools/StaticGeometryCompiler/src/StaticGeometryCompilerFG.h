#ifndef  __STATICGEOMETRYCOMPILERFG_H__
#define  __STATICGEOMETRYCOMPILERFG_H__


#include <memory>

#include "3DMath/BSPTreeForTriangleMesh.hpp"
#include "Graphics/MeshModel/TerrainMeshGenerator.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
#include "LightWave/fwd.hpp"

#include "Stage/StaticGeometryArchiveFG.hpp"

using namespace amorphous;


/*
class FRESULT
{
public:
	enum type
	{
		OK		= 1,
		FAIL	= 0,
		ERR_OUTOFMEMORY	= -1,
	};
};


inline bool Success( FRESULT r )
{
	if( 0 < r )
		return true;
	else
		return false;
}

inline bool Failed( FRESULT r )
{
	if( r <= 0 )
		return true;
	else
		return false;
}*/


class CStaticGeometryCompilerFG
{
	// added to output db
	// - key: "Main"
	CStaticGeometryArchiveFG m_Archive;

	/// saved to binary database separately from 'm_Archive'
	/// - key: "mesh0", "mesh1", ...
	std::vector<C3DMeshModelArchive> m_vecMeshArchive;

	/// added to output db
	/// - key: "CollisionMesh"
    BSPTreeForTriangleMesh m_TriangleMesh;

	CTerrainMeshGenerator m_TerrainMeshGenerator;

	std::shared_ptr<LWO2_Object> m_pObject;

	std::string m_SkyboxTextureFilename;

	std::string m_TextureOutputDirectory;

	std::string m_OutputTextureImageFormat;

	C3DMeshModelArchive m_DefaultSkybox;

	/// temporarily hold shader technique name for terrain rendering
	std::string m_TerrainMeshShaderTechnique;

	/// 01:07 2007/09/29 - created to hold output filename
	/// for separate collision mesh archive file
	std::string m_OutputFilename;

private:

	/// creates triangle mesh for collision test and stores it to m_Archive.m_CollisionMeshArchive
	void CreateCollisionMesh();

	/// creates a mesh archive from the layers with a given name
	bool CreateMeshArchive( const std::string& target_layer_name,
		                    C3DMeshModelArchive& dest_mesh );

	/// special static geometry for terrain
	bool CreateTerrainMesh( int split_tex_width, double tex_coord_shift_u, double tex_coord_shift_v );

	bool CreateBuildingsMesh();

	/// general static geometry
//	bool CreateStaticGeometryMesh();

	/// mesh for skybox.
	/// default model is used when no layer is specified for skybox
	bool CreateSkyboxMesh();

	bool NeedToCompileModelFile( const std::string& input_model_filename,
		                         const std::string& output_filename );

	bool CompileModelFile( const std::string& model_filename,
						   int split_tex_width,
						   double tex_coord_shift_u,
						   double tex_coord_shift_v );

public:

	CStaticGeometryCompilerFG();

	~CStaticGeometryCompilerFG();

	void Release();

	bool SetDefaultSkyboxMesh( const std::string& mesh_archive_filename );

	bool Build( const std::string& desc_filename );

	bool SaveToFile( const std::string& output_filename );
};



#endif		/*  __STATICGEOMETRYCOMPILERFG_H__  */
