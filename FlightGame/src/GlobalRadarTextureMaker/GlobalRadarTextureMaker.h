#ifndef  __GlobalRadarTextureMaker_H__
#define  __GlobalRadarTextureMaker_H__


#include <vector>
#include <string>

#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"

class CD3DXMeshObject;

class CGlobalRadarTextureMaker
{

	CShaderManager m_ShaderManager;

	std::vector<CD3DXMeshObject *> m_vecpMeshObject;

	float m_fViewVolumeWidth;

	bool m_IsReady;

public:

	enum eType
	{
		SHADER_TECH_HEIGHTMAP = 0,
//		TAG1,
//		TAG2,
		NUM_SHADER_TECHS
	};


	CGlobalRadarTextureMaker();

	~CGlobalRadarTextureMaker();

	bool InitShader();

	bool LoadGeometry( const std::string& filename );

	void SetViewWidth( float fViewVolumeWidth );
	void RenderMesh( int mesh_index );
	void Render();
};



#endif		/*  __GlobalRadarTextureMaker_H__  */
