#ifndef  __StaticGeometryDesc_H__
#define  __StaticGeometryDesc_H__


#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "3DCommon/MeshModel/General3DMesh.h"
#include "3DCommon/LightStructs.h"
#include "3DCommon/Shader/ShaderParameter.h"
#include "BSPMapCompiler/LightmapBuilder.h"
#include "XML/XMLDocumentLoader.h"
#include "XML/XMLNodeReader.h"
using namespace xercesc_2_8;


class CGeometryGroupParams
{
public:
	std::string Name;
	std::string ShaderTechnique;
	bool UseLightmap;

public:

	CGeometryGroupParams()
		:
	UseLightmap(false)
	{}
};


class CGeometryGroup
{
public:
	std::vector<CGeometryGroupParams> Layers;
	std::vector<CGeometryGroupParams> Surfaces;
	std::vector<CGeometryGroupParams> PolygonGroups;
};


class CTextureSubdivisionOptions
{
public:

	bool m_Enabled;

	std::string m_OutputImageFormat;

	int m_SplitSize;

	/// name of the surface that contains texture to subdivide
	std::string m_TargetSurfaceName;

public:

	CTextureSubdivisionOptions()
		:
	m_Enabled(false),
	m_SplitSize(0)
	{}

	bool Load( CXMLNodeReader& node_reader );
};


class CMeshTreeOptions
{
public:

	float MinimumCellVolume;
	int NumMaxGeometriesPerCell;
	int MaxDepth;
	int RecursionStopCondition;

public:

	CMeshTreeOptions()
		:
	MinimumCellVolume(0.0f),
	NumMaxGeometriesPerCell(0),
	MaxDepth(0),
	RecursionStopCondition(0)
	{}

	bool Load( CXMLNodeReader& node_reader );
};


/*
class GeometryTarget
{
public:
	std::vector<std::string> Layers;
	std::vector<std::string> Surfaces;
	std::vector<std::string> PolygonGroups;
};

class GeometryFilter
{
public:

	/// exclusive filter
	/// - compile only the geometry specified as Include targets
	GeometryTarget Include;

	GeometryTarget Exclude;
};
*/

class CGeometrySurfaceDesc
{
public:

	std::string m_Name;

	bool m_UseLightmap;

	std::string m_ShaderFilepath;

	std::string m_ShaderTechnique;

	float m_fZShift;

public:

	CGeometrySurfaceDesc()
		:
	m_UseLightmap(false),
	m_fZShift(0.0f)
	{}

	bool Load( DOMNode *pNode );
};


class CStaticGeometryDesc
{

public:

	/// usually a 3D model file
	/// - e.g. aaa.lwo
	std::string m_InputFilepath;

	std::string m_OutputFilepath;

	std::string m_ProgramRootDirectoryPath;

	CGeometryFilter m_CollisionGeometryFilter;
	CGeometryFilter m_GraphcisGeometryFilter;

	vector<CGeometrySurfaceDesc> m_vecSurfaceDesc;

	std::map<std::string,std::string> m_SurfaceToDesc;

	std::vector< boost::shared_ptr<CLight> > m_vecpLight;

	std::map<std::string,CShaderParameterGroup> m_ShaderFileToParamGroup;

	CLightmapDesc m_Lightmap;

	CTextureSubdivisionOptions m_TextureSubdivisionOptions;

	CMeshTreeOptions m_MeshTreeOptions;

private:

	bool LoadGraphicsDesc( DOMNode *pNode );

	bool LoadCollisionDesc( DOMNode *pNode );

	bool LoadSurfaceDescs( DOMNode *pSurfaceNode );

	bool LoadShaderOptions( DOMNode *pShaderNode );

	// bool LoadShaderParams( DOMNode *pSurfaceNode );

	bool LoadShaderParamsFromFile( const std::string& shaderparam_filepath );

	bool LoadSurfaceToDescMaps( DOMNode *pSurfaceToDescMapsNode );

	bool LoadLightingDesc( DOMNode *pLightingNode );

	void LoadLights( DOMNode *pLightsNode );

	bool LoadTextureSubdivisionOptions( DOMNode *pNode );

	bool LoadMeshTreeOptions( DOMNode *pNode );

	void LoadLightsFromColladaFile( const std::string& dae_filepath );

public:

	/// returns true on success
	bool LoadFromXML( const std::string& xml_filepath );
};


#endif /* __StaticGeometryDesc_H__ */
