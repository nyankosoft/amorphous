#include "gtest/gtest.h"
#include "../amorphous/3DMath/Matrix34.hpp"
#include "../amorphous/Graphics/Camera.hpp"
#include "../amorphous/Graphics/TextureHandle.hpp"
#include "../amorphous/Graphics/GraphicsResourceManager.hpp"
#include "../amorphous/Graphics/GraphicsDevice.hpp"
#include "../amorphous/Graphics/TextureUtilities.hpp"
#include "../amorphous/Graphics/Font/ASCIIFont.hpp"
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

TEST(TextureFont_test, TextureFont_Tests) {
	TextureFont font;
	bool res = font.InitFont("",0,0);
	ASSERT_EQ(res,false);
}


TEST(ASCIIFont_test, ASCIIFont_Tests) {

	//std::unique_ptr<ASCIIFont> font( new ASCIIFont );
	ASCIIFont font;

	bool res = font.InitFont("",0,0,0);

	ASSERT_EQ(res,false);

	ASCIIFont ttffont;
	res = ttffont.InitFont(
		"../../demo/GraphicsDemos/app/TextureFontDemo/fonts/rationalinteger.ttf",
		64,
		0,
		32);
	
	ASSERT_EQ(res,true);
}

TEST(TextureHandle_test, TextureHandle_Tests) {

	TextureHandle tx;
	bool res = tx.Load("nonexistentfile");
	ASSERT_EQ(res,false);

	TextureHandle t;
	res = t.Load("../../demo/GraphicsDemos/app/MeshSplitterDemo/models/brick012-s512.jpg");
	//res = t.Load("../../demo/GraphicsDemos/app/AsyncLoadingDemo/models/textures/FlakySlate.jpg");
	ASSERT_EQ(res,true);

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

	GraphicsDevice().Init( 800, 600, ScreenMode::WINDOWED );

	SetCurrentThreadAsRenderThread();
}
