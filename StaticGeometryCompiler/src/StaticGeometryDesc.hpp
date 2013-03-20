#ifndef  __StaticGeometryDesc_H__
#define  __StaticGeometryDesc_H__


#include "Graphics/MeshModel/General3DMesh.hpp"
#include "Graphics/LightStructs.hpp"
#include "Graphics/Shader/ShaderParameter.hpp"
#include "BSPMapCompiler/LightmapBuilder.hpp"
#include "XML/XMLDocumentLoader.hpp"
#include "XML/XMLNodeReader.hpp"
using namespace xercesc;

using namespace amorphous;


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
	m_Enabled(true),
	m_OutputImageFormat("jpg"),
	m_SplitSize(1024)
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

	GeometryFilter m_CollisionGeometryFilter;
	GeometryFilter m_GraphcisGeometryFilter;

	std::vector<CGeometrySurfaceDesc> m_vecSurfaceDesc;

	std::map<std::string,std::string> m_SurfaceToDesc;

	std::vector< boost::shared_ptr<Light> > m_vecpLight;

	std::map<std::string,ShaderParameterGroup> m_ShaderFileToParamGroup;

	CLightmapDesc m_Lightmap;

	CTextureSubdivisionOptions m_TextureSubdivisionOptions;

	CMeshTreeOptions m_MeshTreeOptions;

	static std::string ms_LightPolygonsSurfaceName;

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
