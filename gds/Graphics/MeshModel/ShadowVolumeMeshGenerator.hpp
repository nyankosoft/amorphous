#ifndef __ShadowVolumeMeshGenerator_HPP__
#define __ShadowVolumeMeshGenerator_HPP__


#include "../fwd.hpp"


namespace MeshModel
{


class CEdgeMapping;


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
		for( int i=0; i<2; i++ )
			m_anOldEdge[i] = -1;

		for( int i=0; i<2; i++ )
		{
			for( int j=0; j<2; j++ )
				m_aanNewEdge[i][j] = -1;
		}
	}
};


}  /*  MeshModel  */


#endif  /*  __ShadowVolumeMeshGenerator_HPP__  */
