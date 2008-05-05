#ifndef  __StaticGeometryDesc_H__
#define  __StaticGeometryDesc_H__


#include <vector>
#include <string>

#include "3DCommon/MeshModel/General3DMesh.h"
#include "XML/XMLDocumentLoader.h"
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

	CGeometryFilter m_CollisionGeometryFilter;
	CGeometryFilter m_GraphcisGeometryFilter;

	vector<CGeometrySurfaceDesc> m_vecSurfaceDesc;

	std::map<std::string,std::string> m_SurfaceToDesc;

//	CLightmapDesc lightmap;

private:

	bool LoadGraphicsDesc( DOMNode *pNode );

	bool LoadCollisionDesc( DOMNode *pNode );

	bool LoadSurfaceDescs( DOMNode *pSurfaceNode );

	bool LoadSurfaceToDescMaps( DOMNode *pSurfaceToDescMapsNode );

public:

	/// returns true on success
	bool LoadFromXML( const std::string& xml_filepath );
};


#endif /* __StaticGeometryDesc_H__ */
