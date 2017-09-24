#ifndef __SharedMeshContainer_HPP__
#define __SharedMeshContainer_HPP__


#include "fwd.hpp"
#include "EntityGroupHandle.hpp"
#include "amorphous/3DMath/AABB3.hpp"
#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/MeshObjectContainer.hpp"
#include "amorphous/Graphics/MeshContainerRenderMethod.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "amorphous/Support/TextFileScanner.hpp"
#include "amorphous/Support/Serialization/Serialization.hpp"


namespace amorphous
{

using namespace serialization;

class MeshBoneControllerBase;
class CEntityShaderLightParamsLoader;
class CRenderContext;
class CoreBaseEntitiesLoader;


/**
* holds mesh object for base entity
* serialized with base entity
*/
class SharedMeshContainer : public MeshObjectContainer
{
public:

	/// names of the materials that should be z-sorted
	std::vector<std::string> m_vecTransparentMaterialName;

	/// used by skeletal mesh (not serialized)
	std::vector<MeshBoneControllerBase*> m_vecpMeshBoneController;

	std::shared_ptr<MeshContainerRenderMethod> m_pMeshRenderMethod;

	std::vector< std::shared_ptr<ShaderParamsLoader> > m_vecpShaderParamsLoader;

	//	std::shared_ptr<CBlendMatricesLoader> m_pBlendMatricesLoader;
	std::shared_ptr<BlendTransformsLoader> m_pBlendTransformsLoader;

	std::shared_ptr<CEntityShaderLightParamsLoader> m_pShaderLightParamsLoader;

	/// subsets of the mesh that should be rendered by the entity
	/// - Holds non-transparant materials(subsets) of the mesh
	/// - Used to separate material(s) that have transparant polygons
	/// - Default: 0 (render all the materials)
	std::vector<int> m_vecTargetMaterialIndex;

	enum ePropertyFlags
	{
		PF_USE_SINGLE_TECHNIQUE_FOR_ALL_MATERIALS = (1 << 0),
	};

	int m_PropertyFlags;

	//	int m_NumProgressiveMeshes;

private:

	void ValidateShaderTechniqueTable();

public:

	SharedMeshContainer();

	SharedMeshContainer( const std::string& filename );

	~SharedMeshContainer();

	bool LoadMeshObject();

	void Release();

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		MeshObjectContainer::Serialize( ar, version );

		ar & m_vecTargetMaterialIndex;

		//		if( ar.GetMode() == IArchive::MODE_INPUT )
		//		{
		// delete the current mesh controllers
		//		}
	}
};


} // namespace my_namespace


#endif /* __SharedMeshContainer_HPP__ */
