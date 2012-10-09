#ifndef  __SkyboxMisc_HPP__
#define  __SkyboxMisc_HPP__


#include <boost/shared_ptr.hpp>
#include "../3DMath/Vector3.hpp"
#include "MeshObjectHandle.hpp"


//namespace graphics
//{


boost::shared_ptr<CBoxMeshGenerator> CreateSkyboxMeshGenerator( const std::string& texture_filepath );
CMeshObjectHandle CreateSkyboxMesh( const std::string& texture_filepath );
void RenderAsSkybox( CMeshObjectHandle& mesh, const Vector3& vCamPos );
void RenderSkybox( CTextureHandle& sky_texture, const Vector3& vCamPos );


//}


#endif		/*  __SkyboxMisc_HPP__  */
