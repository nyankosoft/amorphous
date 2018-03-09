cmake_minimum_required(VERSION 3.5)
project(Amorphous)

# See if OpenGL is available
find_package(OpenGL REQUIRED)

include_directories(
"${CMAKE_CURRENT_LIST_DIR}/../../externals/glew-2.1.0/include"
"${CMAKE_CURRENT_LIST_DIR}/../../externals/googletest-release-1.8.0/googletest/include"
"${CMAKE_CURRENT_LIST_DIR}/../../externals/freetype-2.8.1/include"
)

# Enable C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(stdc++fs UNKNOWN IMPORTED)
set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/5/libstdc++fs.a")

# TODO: do this after checking the window system and confirming
# that the current OS is based on X11 (X Window System)
add_definitions(-DBUILD_WITH_X11_LIBS)

include_directories(../)
add_library(amorphous-core STATIC
../amorphous/3DMath/AABB3.cpp
../amorphous/3DMath/AABTree.cpp
../amorphous/3DMath/BSPTreeForBox.cpp
../amorphous/3DMath/BSPTreeForTriangleMesh.cpp
../amorphous/3DMath/ConvexPolygon.cpp
../amorphous/3DMath/OBBTree.cpp
../amorphous/3DMath/Perlin.cpp
#../amorphous/3DMath/PolygonMesh.cpp
../amorphous/3DMath/PrimitivePolygonModelMaker.cpp
../amorphous/3DMath/HelixPolygonModelMaker.cpp
../amorphous/Support/BitmapImage_stb.cpp
../amorphous/Support/MTRand.cpp
../amorphous/Support/Profile.cpp
../amorphous/Support/CameraControllerBase.cpp
../amorphous/Support/CameraController.cpp
../amorphous/Support/Log/LogInputBase.cpp
../amorphous/Support/Log/LogOutput.cpp
../amorphous/Support/Log/StateLog.cpp
../amorphous/Support/ZLib/adler32.c
../amorphous/Support/ZLib/compress.c
../amorphous/Support/ZLib/crc32.c
../amorphous/Support/ZLib/deflate.c
../amorphous/Support/ZLib/gzclose.c
../amorphous/Support/ZLib/gzlib.c
../amorphous/Support/ZLib/gzread.c
../amorphous/Support/ZLib/gzwrite.c
../amorphous/Support/ZLib/infback.c
../amorphous/Support/ZLib/inffast.c
../amorphous/Support/ZLib/inflate.c
../amorphous/Support/ZLib/inftrees.c
../amorphous/Support/ZLib/trees.c
../amorphous/Support/ZLib/uncompr.c
../amorphous/Support/ZLib/zutil.c
../amorphous/Support/ZLib/zpipe_stream.cpp
../amorphous/Support/Serialization/ArchiveObjectBase.cpp
../amorphous/Support/Serialization/BinaryArchive.cpp
../amorphous/Support/Serialization/BinaryArchive_Input.cpp
../amorphous/Support/Serialization/BinaryArchive_Output.cpp
../amorphous/Graphics/AsyncResourceLoader.cpp
../amorphous/Graphics/BoundingVolumeTreeRenderers.cpp
../amorphous/Graphics/Camera.cpp
../amorphous/Graphics/CubeMapManager.cpp
../amorphous/Graphics/CubeTextureRenderTarget.cpp
../amorphous/Graphics/DoubleConeScrollEffect.cpp
../amorphous/Graphics/GraphicsComponentCollector.cpp
../amorphous/Graphics/GraphicsDevice.cpp
../amorphous/Graphics/GraphicsEffectManager.cpp
../amorphous/Graphics/GraphicsElementManager.cpp
../amorphous/Graphics/GraphicsElements.cpp
../amorphous/Graphics/GraphicsResourceCacheManager.cpp
../amorphous/Graphics/GraphicsResourceDescs.cpp
../amorphous/Graphics/GraphicsResourceEntries.cpp
../amorphous/Graphics/GraphicsResourceHandle.cpp
../amorphous/Graphics/GraphicsResourceLoaders.cpp
../amorphous/Graphics/GraphicsResourceManager.cpp
../amorphous/Graphics/GraphicsResources.cpp
../amorphous/Graphics/IndexedPolygon.cpp
../amorphous/Graphics/LensFlare.cpp
../amorphous/Graphics/MeshContainerRenderMethod.cpp
../amorphous/Graphics/MeshObjectContainer.cpp
../amorphous/Graphics/MeshObjectHandle.cpp
../amorphous/Graphics/OnScreenDebugInfoRenderer.cpp
../amorphous/Graphics/PostProcessEffect.cpp
../amorphous/Graphics/PostProcessEffectFilter.cpp
../amorphous/Graphics/PostProcessEffectManager.cpp
../amorphous/Graphics/PrimitiveShapeRenderer.cpp
../amorphous/Graphics/PseudoNoiseEffect.cpp
../amorphous/Graphics/RectTree.cpp
../amorphous/Graphics/RectTriListIndex.cpp
../amorphous/Graphics/ResourceLoadingStateHolder.cpp
../amorphous/Graphics/ShaderHandle.cpp
../amorphous/Graphics/ShadowMapManager.cpp
../amorphous/Graphics/ShadowMaps.cpp
../amorphous/Graphics/ShapeDetector.cpp
../amorphous/Graphics/ShapesExtractor.cpp
../amorphous/Graphics/SimpleMotionBlur.cpp
../amorphous/Graphics/SkyboxMisc.cpp
../amorphous/Graphics/TextureHandle.cpp
../amorphous/Graphics/TextureRenderTarget.cpp
../amorphous/Graphics/2DPrimitive/2DPrimitive.cpp
../amorphous/Graphics/2DPrimitive/2DRect.cpp
../amorphous/Graphics/2DPrimitive/2DFrameRect.cpp
../amorphous/Graphics/2DPrimitive/2DRoundRect.cpp
../amorphous/Graphics/2DPrimitive/2DPolygon.cpp
../amorphous/Graphics/2DPrimitive/2DTriangle.cpp
../amorphous/Graphics/2DPrimitive/2DRectSet.cpp
../amorphous/Graphics/2DPrimitive/2DPrimitiveRenderer.cpp
../amorphous/Graphics/Font/TextureFont.cpp
../amorphous/Graphics/Font/ASCIIFont.cpp
../amorphous/Graphics/Font/UTFFont.cpp
../amorphous/Graphics/Font/FontFactory.cpp
../amorphous/Graphics/Font/BitstreamVeraSansMono_Bold_256.cpp
../amorphous/Graphics/Mesh/BasicMesh.cpp
../amorphous/Graphics/Mesh/CustomMesh.cpp
../amorphous/Graphics/Mesh/ProgressiveMesh.cpp
../amorphous/Graphics/Mesh/SkeletalMesh.cpp
../amorphous/Graphics/Mesh/MeshFactory.cpp
../amorphous/Graphics/Mesh/CustomMeshRenderer.cpp
../amorphous/Graphics/Mesh/MeshInfoRenderer.cpp
../amorphous/Graphics/Mesh/RectSetMesh.cpp
../amorphous/Graphics/Mesh/ConvexMeshSplitter.cpp
../amorphous/Graphics/MeshGenerators/MeshGenerators.cpp
../amorphous/Graphics/MeshGenerators/PipeMeshGenerator.cpp
../amorphous/Graphics/MeshModel/3DMeshModelArchive.cpp
../amorphous/Graphics/MeshModel/3DMeshModelBuilder.cpp
../amorphous/Graphics/MeshModel/General3DMeshFunctions.cpp
../amorphous/Graphics/MeshModel/MeshArchiveToGeneral3DMeshConverer.cpp
../amorphous/Graphics/MeshModel/MeshBone.cpp
../amorphous/Graphics/MeshModel/NVMeshMender.cpp
../amorphous/Graphics/MeshModel/PrimitiveShapeMeshes.cpp
../amorphous/Graphics/Shader/ShaderManager.cpp
../amorphous/Graphics/Shader/ShaderLightManager.cpp
../amorphous/Graphics/Shader/ShaderManagerHub.cpp
../amorphous/Graphics/Shader/FixedFunctionPipelineManager.cpp
../amorphous/Graphics/Shader/Embedded/EmbeddedGenericShader.cpp
../amorphous/Graphics/TextureGenerators/ParticleTextureGenerator.cpp
../amorphous/Graphics/TextureGenerators/PerlinNoiseTextureGenerator.cpp
../amorphous/Graphics/TextureGenerators/NoiseTextureGenerators.cpp
../amorphous/Graphics/TextureGenerators/TextureFilter.cpp
../amorphous/Graphics/TextureGenerators/TextureGeneratorFactory.cpp
../amorphous/Graphics/OpenGL/GLGraphicsDevice.cpp
../amorphous/Graphics/OpenGL/GLGraphicsResources.cpp
../amorphous/Graphics/OpenGL/GLInitialization.cpp
../amorphous/Graphics/OpenGL/GLTextureRenderTarget.cpp
../amorphous/Graphics/OpenGL/GLCubeTextureRenderTarget.cpp
../amorphous/Graphics/OpenGL/GLTextureUtilities.cpp
../amorphous/Graphics/OpenGL/2DPrimitive/2DPrimitiveRenderer_GL.cpp
../amorphous/Graphics/OpenGL/Mesh/GLBasicMeshImpl.cpp
../amorphous/Graphics/OpenGL/Mesh/GLProgressiveMeshImpl.cpp
../amorphous/Graphics/OpenGL/Mesh/GLSkeletalMeshImpl.cpp
../amorphous/Graphics/OpenGL/Mesh/GLCustomMeshRenderer.cpp
../amorphous/Graphics/OpenGL/Shader/GLShader.cpp
../amorphous/Graphics/OpenGL/Shader/GLFixedFunctionPipelineManager.cpp
../amorphous/Graphics/OpenGL/Shader/GLFixedPipelineLightManager.cpp
../amorphous/Graphics/OpenGL/Shader/GLSLShaderLightManager.cpp
../amorphous/Graphics/OpenGL/Shader/Embedded/EmbeddedGenericGLSLShader.cpp
../amorphous/Graphics/OpenGL/Shader/Embedded/EmbeddedPostProcessEffectGLSLShader.cpp
../amorphous/Graphics/OpenGL/Shader/Embedded/GLEmbeddedMiscGLSLShader.cpp
../amorphous/Input/InputHub.cpp
../amorphous/Input/InputDevice.cpp
../amorphous/App/GameWindowManager.cpp
../amorphous/App/GameWindowManager_Generic.cpp
../amorphous/App/X11_GLWindow.cpp
../amorphous/App/ApplicationBase.cpp
../amorphous/App/GraphicsApplicationBase.cpp
../amorphous/Utilities/ScreenshotRenderer.cpp
)

target_link_libraries(amorphous-core
${OPENGL_LIBRARIES}
stdc++fs
${CMAKE_CURRENT_LIST_DIR}/../../externals/glew-2.1.0/lib/libGLEW.a
${CMAKE_CURRENT_LIST_DIR}/../../externals/freetype-2.8.1/objs/.libs/libfreetype.a
${CMAKE_CURRENT_LIST_DIR}/../../externals/libpng-1.6.34/libpng16.a
${CMAKE_CURRENT_LIST_DIR}/../../externals/zlib-1.2.11/libz.a
)

# Enable -pthread option
find_package(Threads)
target_link_libraries(amorphous-core ${CMAKE_THREAD_LIBS_INIT})


# unittesting
add_executable(amorphous-core-test
../test/3DMath_tests.cpp
../test/Support_tests.cpp
../test/Graphics_tests.cpp
)

target_link_libraries(amorphous-core-test amorphous-core)

target_link_libraries(amorphous-core-test
${CMAKE_CURRENT_LIST_DIR}/../../externals/googletest-release-1.8.0/googlemock/gtest/libgtest.a
)

