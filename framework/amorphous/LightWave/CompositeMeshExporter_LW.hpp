#ifndef	__COMPOSITEMESHEXPORTER_LW_H__
#define	__COMPOSITEMESHEXPORTER_LW_H__


#include <memory>
#include "amorphous/LightWave/fwd.hpp"
#include "amorphous/Graphics/MeshModel/CompositeMesh.hpp"


namespace amorphous
{


//==========================================================================
// CCompositeMeshExporter_LW
//==========================================================================

class CCompositeMeshExporter_LW
{
	std::shared_ptr<LWO2_Object> m_pOrigModel;

	CMA_CompositeMeshArchive m_CompositeMeshArchive;

private:

    /// create indices that are necessary to update positions
	/// between mesh & mass spring physics at runtime
    void SetIndicesForMeshAndMassSpringModel();

public:

	bool Compile( const char *pcFilename );

	bool SaveToFile( const char *pcFilename );
};

} // amorphous



#endif		/*  __COMPOSITEMESHEXPORTER_LW_H__  */
