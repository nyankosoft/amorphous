#ifndef  __SkyboxMisc_HPP__
#define  __SkyboxMisc_HPP__


#include <boost/shared_ptr.hpp>
#include "../3DMath/Matrix34.hpp"
#include "MeshObjectHandle.hpp"


//namespace graphics
//{


boost::shared_ptr<CBoxMeshGenerator> CreateSkyboxMeshGenerator( const std::string& texture_filepath );
CMeshObjectHandle CreateSkyboxMesh( const std::string& texture_filepath );
void RenderAsSkybox( CMeshObjectHandle& mesh, const Matrix34& vCamPose );
void RenderSkybox( CTextureHandle& sky_texture, const Matrix34& vCamPose );
void RenderSkyCylinder( CTextureHandle& sky_texture, const Matrix34& vCamPose );

CTextureHandle CreateClearDaySkyTexture();
CTextureHandle CreateCloudyDaySkyTexture();
CTextureHandle CreateClearNightSkyTexture();
CTextureHandle CreateCloudyNightSkyTexture();


//}


#endif		/*  __SkyboxMisc_HPP__  */
