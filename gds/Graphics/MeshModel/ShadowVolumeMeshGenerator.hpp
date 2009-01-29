
#ifndef __SHADOWVOLUMEMESHGENERATOR_H__
#define __SHADOWVOLUMEMESHGENERATOR_H__


//#include "FVF_SHADOWVERTEXex.h"

#include <d3dx9.h>


namespace MeshModel
{


class CEdgeMapping;
class C3DMeshModelArchive;


//=========================================================================================
// CShadowVolumeMeshGenerator
//=========================================================================================

class CShadowVolumeMeshGenerator
{

	int FindEdgeInMappingTable( int nV1, int nV2, CEdgeMapping *pMapping, int nCount );

	void SetShadowVolumeMeshProperties( C3DMeshModelArchive& mesh, C3DMeshModelArchive& rSrcMesh );

	void Cleanup( C3DMeshModelArchive& rSVMesh, C3DMeshModelArchive& rPatchMesh );

public:

	bool GenerateShadowMesh( C3DMeshModelArchive& rShadowVolumeMesh, C3DMeshModelArchive& rOriginalMesh );
};



//=========================================================================================
// CEdgeMapping
//=========================================================================================

class CEdgeMapping
{
public:
	/// vertex index of the original edge
	int m_anOldEdge[2];

	/// vertex indexes of the new edge
	/// First subscript = index of the new edge
	/// Second subscript = index of the vertex for the edge
	int m_aanNewEdge[2][2];

public:

	CEdgeMapping()
	{
		FillMemory( m_anOldEdge, sizeof(m_anOldEdge), -1 );
		FillMemory( m_aanNewEdge, sizeof(m_aanNewEdge), -1 );
	}
};


}  /*  MeshModel  */


#endif  /*  __SHADOWVOLUMEMESHGENERATOR_H__  */
