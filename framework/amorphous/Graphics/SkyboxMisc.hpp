#ifndef  __SkyboxMisc_HPP__
#define  __SkyboxMisc_HPP__


#include <memory>
#include "../3DMath/Matrix34.hpp"
#include "MeshObjectHandle.hpp"


namespace amorphous
{


//namespace graphics
//{


std::shared_ptr<BoxMeshGenerator> CreateSkyboxMeshGenerator( const std::string& texture_filepath );
MeshHandle CreateSkyboxMesh( const std::string& texture_filepath );
void RenderAsSkybox( MeshHandle& mesh, const Matrix34& vCamPose );
void RenderSkybox( TextureHandle& sky_texture, const Matrix34& vCamPose );
void RenderSkyCylinder( TextureHandle& sky_texture, const Matrix34& vCamPose );

TextureHandle CreateClearDaySkyTexture();
TextureHandle CreateCloudyDaySkyTexture();
TextureHandle CreateClearNightSkyTexture();
TextureHandle CreateCloudyNightSkyTexture();


//}

} // namespace amorphous



#endif		/*  __SkyboxMisc_HPP__  */
