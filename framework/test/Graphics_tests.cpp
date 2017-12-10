#include "gtest/gtest.h"
#include "../amorphous/3DMath/Matrix34.hpp"
#include "../amorphous/Graphics/Camera.hpp"
#include "../amorphous/Graphics/TextureHandle.hpp"
#include "../amorphous/Graphics/GraphicsResourceManager.hpp"
#include "../amorphous/Graphics/TextureUtilities.hpp"
#include "../amorphous/Graphics/OpenGL/GLInitialization.hpp"
#include "../amorphous/Support/Vec3_StringAux.hpp"

using namespace amorphous;

TEST(Camera_test, Camera_Tests) {

	Camera cam;

	// Set position to the origin
	cam.SetPose( Matrix34Identity() );
	ASSERT_EQ(cam.GetPose(),Matrix34Identity());

	const Matrix34 pose( Vector3(1,2,3), Matrix33RotationY(1.0f) );
	cam.SetPose(pose);
	ASSERT_EQ(cam.GetPosition(),pose.vPosition);
	Matrix33 cam_orient;
	cam.GetOrientation(cam_orient);
	ASSERT_EQ(cam_orient,pose.matOrient);

	Camera c;
	c.SetNearClip(0.01f);
	c.SetFarClip(300.0f);
	Matrix44 mat = c.GetProjectionMatrix();

	// TODO: test if mat is the intended projection matrix.

	//std::cout << to_string(mat);
}
/*
TEST(TextureHandle_test, TextureHandle_Tests) {

	SetCurrentThreadAsRenderThread();

//	TextureHandle green_tex = CreateSingleColorTexture(SFloatRGBAColor::Green(),16,16);
//	auto entry = green_tex.GetEntry();
//	ASSERT_EQ(entry != nullptr,true);

//	bool res = texture.Load("scenery-0256.jpg");
//	ASSERT_EQ(res,true);

//	auto tex = texture.GetEntry()->GetTextureResource();
//	ASSERT_EQ(tex != nullptr,true);
}

void init_opengl() {
	InitializeOpenGLClasses();
}*/