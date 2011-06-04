#ifndef	__COMPOSITEMESHEXPORTER_LW_H__
#define	__COMPOSITEMESHEXPORTER_LW_H__


#include <boost/shared_ptr.hpp>
#include "gds/LightWave/fwd.hpp"
#include "gds/Graphics/MeshModel/CompositeMesh.hpp"


//==========================================================================
// CCompositeMeshExporter_LW
//==========================================================================

class CCompositeMeshExporter_LW
{
	boost::shared_ptr<CLWO2_Object> m_pOrigModel;

	CMA_CompositeMeshArchive m_CompositeMeshArchive;

private:

    /// create indices that are necessary to update positions
	/// between mesh & mass spring physics at runtime
    void SetIndicesForMeshAndMassSpringModel();

public:

	bool Compile( const char *pcFilename );

	bool SaveToFile( const char *pcFilename );
};


#endif		/*  __COMPOSITEMESHEXPORTER_LW_H__  */
