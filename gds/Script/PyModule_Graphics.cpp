#include "PyModule_Graphics.hpp"
#include <gds/Graphics/MeshObjectHandle.hpp>
#include <gds/Graphics/ShaderHandle.hpp>
#include <gds/Graphics/Shader/ShaderTechniqueHandle.hpp>
#include <gds/Graphics/Shader/ShaderVariableLoader.hpp>
#include <gds/Graphics/LensFlare.hpp>
#include <gds/Support/Log/DefaultLog.hpp>
#include <boost/python.hpp>


bool (CTextureHandle::*LoadTextureFromResourcePath)( const std::string& ) = &CTextureHandle::Load;
bool (CMeshObjectHandle::*LoadMeshFromResourcePath)( const std::string& ) = &CMeshObjectHandle::Load;
bool (CShaderHandle::*LoadShaderFromResourcePath)( const std::string& ) = &CShaderHandle::Load;

boost::shared_ptr<CTextElement> (CGraphicsElementManager::*CreateTextWithTLPos)( int, const std::string&, float, float, const SFloatRGBAColor&, int, int, int ) = &CGraphicsElementManager::CreateText;
boost::shared_ptr<CTextElement> (CGraphicsElementManager::*CreateTextInBox)( int, const std::string&, const SRect&, int, int, const SFloatRGBAColor&, int, int, int ) = &CGraphicsElementManager::CreateText;
bool (CGraphicsElementManager::*LoadTextureWithID)( int, const std::string& ) = &CGraphicsElementManager::LoadTexture;
int (CGraphicsElementManager::*LoadTextureWithoutID)( const std::string& ) = &CGraphicsElementManager::LoadTexture;
bool (CGraphicsElementManager::*LoadFontWithID)( int, const std::string&, int, int, float, float, float ) = &CGraphicsElementManager::LoadFont;
int (CGraphicsElementManager::*LoadFontWithoutID)( const std::string&, int, int, float, float, float ) = &CGraphicsElementManager::LoadFont;

CGraphicsEffectHandle (CAnimatedGraphicsManager::*ChangeColor_RGBA)( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color, int trans_mode );
CGraphicsEffectHandle (CAnimatedGraphicsManager::*ChangeColor_RGB)( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBColor& start_color, const SFloatRGBColor& end_color, int trans_mode );
CGraphicsEffectHandle (CAnimatedGraphicsManager::*ChangeColorTo_RGBA)( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBAColor& end_color, int trans_mode );
CGraphicsEffectHandle (CAnimatedGraphicsManager::*ChangeColorTo_RGB)( boost::shared_ptr<CGraphicsElement> pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBColor& end_color, int trans_mode );

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(gem_member_overloads, CreateText, 1, 3)


class CGraphicsElementWrap : CGraphicsElement, boost::python::wrapper<CGraphicsElement>
{
public:
	int GetElementType() const
	{
		return this->get_override("GetElementType")();
	}
};



BOOST_PYTHON_MODULE(gfx)
{
	using namespace boost;
	using namespace boost::python;
	namespace py = boost::python;

	class_<SFloatRGBAColor>("Color")
		.def(init<float,float,float,float>())
		.def_readwrite("red",   &SFloatRGBAColor::fRed)
		.def_readwrite("green", &SFloatRGBAColor::fGreen)
		.def_readwrite("blue",  &SFloatRGBAColor::fBlue)
		.def_readwrite("alpha", &SFloatRGBAColor::fAlpha)
		.def(self + self)
		.def(self - self)
		.def(self += self)
		.def(self -= self)
		.def(self * float())
		.def("SetToWhite",      &SFloatRGBAColor::SetToWhite)
		.def("SetToBlack",      &SFloatRGBAColor::SetToBlack)
		.def("SetToRed",        &SFloatRGBAColor::SetToRed)
		.def("SetToGreen",      &SFloatRGBAColor::SetToGreen)
		.def("SetToBlue",       &SFloatRGBAColor::SetToBlue)
		.def("SetToYellow",     &SFloatRGBAColor::SetToYellow)
		.def("SetToMagenta",    &SFloatRGBAColor::SetToMagenta)
		.def("SetToAqua",       &SFloatRGBAColor::SetToAqua)
//		.def("SetRGBA",         &SFloatRGBAColor::SetRGBA)
		.def("SetRGB",          &SFloatRGBAColor::SetRGB)
	;

	class_<Graphics::SRect>("Rect")
		.def(init<int,int,int,int>())
		.def_readwrite("left",    &Graphics::SRect::left)
		.def_readwrite("top",     &Graphics::SRect::top)
		.def_readwrite("right",   &Graphics::SRect::right)
		.def_readwrite("bottom",  &Graphics::SRect::bottom)
//		.def("SetLTRB",           &Graphics::SRect::SetPositionLTRB )
//		.def("SetLTWH",           &Graphics::SRect::SetPositionLTWH )
		.def("GetWidth",          &Graphics::SRect::GetWidth )
		.def("GetHeight",         &Graphics::SRect::GetHeight )
		.def("GetCenter",         &Graphics::SRect::GetCenter )
		.def("GetCenterX",        &Graphics::SRect::GetCenterX )
		.def("GetCenterY",        &Graphics::SRect::GetCenterY )
		.def("ContainsPoint",     &Graphics::SRect::ContainsPoint )
		.def("GetTopLeftCorner",  &Graphics::SRect::GetTopLeftCorner )
//		.def("Offset",            &Graphics::SRect::Offset )
		.def("Inflate",           &Graphics::SRect::Inflate )
		.def("Scale",             &Graphics::SRect::Scale )
	;

	def( "RectLTWH", RectLTWH );
	def( "RectLTRB", RectLTRB );

	class_<CTextureHandle>("TextureHandle")
		.def( "Load", LoadTextureFromResourcePath,      ( python::arg("resource_path") ) )
	;

	class_<CMeshObjectHandle>("MeshHandle")
		.def( "Load", LoadMeshFromResourcePath,   ( python::arg("resource_path") ) )
	;

	class_<CShaderHandle>("ShaderHandle")
		.def( "Load", LoadShaderFromResourcePath,       ( python::arg("resource_path") ) )
	;

	class_<CShaderTechniqueHandle>("ShaderTechniqueHandle")
		.def( "SetTechniqueName", &CShaderTechniqueHandle::SetTechniqueName, ( python::arg("name") ) )
	;

//	class_< CGraphicsElementWrap, boost::shared_ptr<CGraphicsElement> >( "GraphicsElement" )
//	class_< CGraphicsElementWrap, boost::noncopyable >( "GraphicsElement" )
//		.def( "GetElementType",  pure_virtual(&CGraphicsElement::GetElementType) )
//		.def( "GetColor",       &CGraphicsElement::GetColor )
//		.def( "SetColor",       &CGraphicsElement::SetColor )
//		.def( "GetAlpha",       &CGraphicsElement::GetAlpha )
//		.def( "SetAlpha",       &CGraphicsElement::SetAlpha )
	;

	class_< CCombinedRectElement, boost::shared_ptr<CCombinedRectElement> >("CombinedRectElement")
		.def( "FillRectElement",  &CCombinedRectElement::FillRectElement )
		.def( "FrameRectElement", &CCombinedRectElement::FrameRectElement )
	;

	class_< CCombinedRoundRectElement, boost::shared_ptr<CCombinedRoundRectElement> >("CombinedRoundRectElement")
		.def( "RoundFillRectElement",  &CCombinedRoundRectElement::RoundFillRectElement )
		.def( "RoundFrameRectElement", &CCombinedRoundRectElement::RoundFrameRectElement )
	;

	class_< CCombinedTriangleElement, boost::shared_ptr<CCombinedTriangleElement> >("CombinedTriangleElement")
		.def( "FillTriangleElement",  &CCombinedTriangleElement::FillTriangleElement )
		.def( "FrameTriangleElement", &CCombinedTriangleElement::FrameTriangleElement )
	;

	class_< CTextElement, boost::shared_ptr<CTextElement> >("TextElement")
		.def( "GetText",   &CTextElement::GetTextCopy )
		.def( "SetText",   &CTextElement::SetText )
		.def( "SetFontID", &CTextElement::SetFontID )
	;

	class_< CGraphicsElementManager, boost::shared_ptr<CGraphicsElementManager> >("GraphicsElementManager")
//	class_< CGraphicsElementManager >("GraphicsElementManager")
		.def( "CreateRect",             &CGraphicsElementManager::CreateRect,      ( python::arg("rect"), python::arg("fill_color_0"), python::arg("frame_color_0"), python::arg("frame_width")=4, python::arg("layer") = 0 ) )
		.def( "CreateFillRect",         &CGraphicsElementManager::CreateFillRect,  ( python::arg("rect"), python::arg("fill_color_0"), python::arg("layer") = 0 ) )
		.def( "CreateFrameRect",        &CGraphicsElementManager::CreateFrameRect, ( python::arg("rect"), python::arg("frame_color_0"), python::arg("frame_width"), python::arg("layer") = 0 ) )

		.def( "CreateRoundRect",        &CGraphicsElementManager::CreateRoundRect, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("frame_color_0"), python::arg("corner_radius"), python::arg("frame_width"), python::arg("layer") = 0 ) )
		.def( "CreateRoundFillRect",    &CGraphicsElementManager::CreateRoundFillRect, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("corner_radius"), python::arg("layer") = 0 ) )
		.def( "CreateRoundFrameRect",   &CGraphicsElementManager::CreateRoundFrameRect, ( python::arg("rect"), python::arg("frame_color_0"), python::arg("corner_radius"), python::arg("frame_width"), python::arg("layer") = 0 ) )

//		.def( "CreateTriangle",         &CGraphicsElementManager::CreateTriangle, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("frame_color_0"), python::arg("frame_width"), python::arg("layer") = 0 ) )
//		.def( "CreateFillTriangle",     &CGraphicsElementManager::CreateFillTriangle, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("layer") = 0 ) )
//		.def( "CreateFrameTriangle",    &CGraphicsElementManager::CreateFrameTriangle, ( python::arg("rect"), python::arg("frame_color_0"), python::arg("frame_width"), python::arg("layer") = 0 ) )

//		.def( "CreateText",             &CGraphicsElementManager::CreateTLAlignedText, ( python::arg("font_id"), python::arg("text"), python::arg("x") = 0, python::arg("y") = 0, python::arg("color") = SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f), python::arg("font_width") = 16, python::arg("font_height") = 32, python::arg("layer") = 0 ) )
//		.def( "CreateText",             &CGraphicsElementManager::CreateTextInRect,    ( python::arg("font_id"), python::arg("text"), python::arg("rect"), python::arg("align_h"), python::arg("align_v"), python::arg("color"), python::arg("font_width"), python::arg("font_height"), python::arg("layer") = 0 ) )
		.def( "CreateText",             CreateTextWithTLPos, ( python::arg("font_id"), python::arg("text"), python::arg("x") = 0, python::arg("y") = 0, python::arg("color") = SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f), python::arg("font_width") = 0, python::arg("font_height") = 0, python::arg("layer") = 0 ) )
		.def( "CreateText",             CreateTextInBox,     ( python::arg("font_id"), python::arg("text"), python::arg("rect"), python::arg("align_h"), python::arg("align_v"), python::arg("color"), python::arg("font_width") = 0, python::arg("font_height") = 0, python::arg("layer") = 0 ) )

//		.def("CreateGroup",             &CGraphicsElementManager::CreateRect, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("frame_color_0"), python::arg("frame_width"), python::arg("layer") = 0 ) )

		.def("LoadTexture",             LoadTextureWithID,    ( python::arg("texture_id"), python::arg("texture_path") ) )
		.def("LoadTexture",             LoadTextureWithoutID, (                            python::arg("texture_path") ) )

		.def("LoadFont",                LoadFontWithID,    ( python::arg("font_id"), python::arg("font_name"), python::arg("width") = 16, python::arg("height") = 32, python::arg("bold") = 0.0f, python::arg("italic") = 0.0f, python::arg("shadow") = 0.0f ) )
		.def("LoadFont",                LoadFontWithoutID, (                         python::arg("font_name"), python::arg("width") = 16, python::arg("height") = 32, python::arg("bold") = 0.0f, python::arg("italic") = 0.0f, python::arg("shadow") = 0.0f ) )
	;

	enum_<CTextElement::eTextAlignment>("TextAlignment")
		.value( "Left",   CTextElement::TAL_LEFT )
		.value( "Top",    CTextElement::TAL_TOP )
		.value( "Center", CTextElement::TAL_CENTER )
		.value( "Right",  CTextElement::TAL_RIGHT )
		.value( "Bottom", CTextElement::TAL_BOTTOM )
	;

	def( "GetGraphicsElementManager", GetGraphicsElementManager );


//	class_<CGraphicsEffectHandle>("GraphicsEffectHandle")
//		.def( ""

//	def( "GetGraphicsEffectManager", GetGraphicsEffectManager );

	class_< CAnimatedGraphicsManager, boost::shared_ptr<CAnimatedGraphicsManager> >("GraphicsEffectManager")
		.def( "ChangeAlpha",           &CAnimatedGraphicsManager::ChangeAlpha,           (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("start_alpha"), py::arg("end_alpha"), py::arg("trans_mode") ) )
		.def( "ChangeAlphaTo",         &CAnimatedGraphicsManager::ChangeAlphaTo,         (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("end_alpha"), py::arg("trans_mode") ) )
		.def( "ChangeColor",           ChangeColor_RGBA,                                 (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("start_color"), py::arg("end_color"), py::arg("trans_mode") ) )
		.def( "ChangeColorTo",         ChangeColorTo_RGBA,                               (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("end_color"), py::arg("trans_mode") ) )
		.def( "ChangeColor",           ChangeColor_RGB,                                  (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("start_color"), py::arg("end_color"), py::arg("trans_mode") ) )
		.def( "ChangeColorTo",         ChangeColorTo_RGB,                                (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("end_color"), py::arg("trans_mode") ) )
		.def( "TranslateNonLinear",    &CAnimatedGraphicsManager::TranslateNonLinear,    (  py::arg("target"), py::arg("start_time"), py::arg("dest_pos"), py::arg("init_vel"), py::arg("smooth_time"), py::arg("coord_type"), py::arg("flags") ) )
		.def( "BlinkAlpha",            &CAnimatedGraphicsManager::BlinkAlpha,            (  py::arg("target"), py::arg("interval"), py::arg("color_index") ) )
		.def( "ChangeAlphaInSineWave", &CAnimatedGraphicsManager::ChangeAlphaInSineWave, (  py::arg("target"), py::arg("start_time"), py::arg("period"), py::arg("color_index"), py::arg("alpha0"), py::arg("alpha1"), py::arg("num_periods") ) )
		.def( "ChangeColorInSineWave", &CAnimatedGraphicsManager::ChangeColorInSineWave, (  py::arg("target"), py::arg("start_time"), py::arg("period"), py::arg("color_index"), py::arg("color0"), py::arg("color1"), py::arg("num_periods") ) )
/*		.def( "DrawText",              &CAnimatedGraphicsManager::DrawText,              (  boost::shared_ptr<CTextElement> pTargetTextElement, double start_time, int num_chars_per_sec ) )
		.def( "CancelEffect",          &CAnimatedGraphicsManager::CancelEffect,          (  py::arg("effect_handle") ) )
*/
	;


/*
	class_<CLight>("Light")
		.def_readwrite("DiffuseColor", &CLight::DiffuseColor)
		.def_readwrite("fIntensity",   &CLight::fIntensity)
		.def_readwrite("fRange",       &CLight::fRange)
	;
*/

	class_< CLensFlare, boost::shared_ptr<CLensFlare> >("LensFlare")
		.def( "AddTexture",            &CLensFlare::AddTexture,                          (  py::arg("tex_path"), py::arg("group_index") = 0, py::arg("num_segs_x") = 1, py::arg("num_segs_y") = 1 ) )
		.def( "AddLensFlareRect",      &CLensFlare::AddLensFlareRect,                    (  py::arg("dim"), py::arg("scale_factor") = 1, py::arg("dist_factor") = 1, py::arg("color") = SFloatRGBAColor::White(), py::arg("group_index") = 0, py::arg("tex_seg_index_x") = 0, py::arg("tex_seg_index_y") = 0 ) )
	;

	class_< CShaderVariableLoader<float>, boost::shared_ptr< CShaderVariableLoader<float> > >("ShaderFloatLoader")
		.def( init< const char *, float >() )
		.def( init< CShaderParameter<float> >() )
		.def( "UpdateShaderParams",   &CShaderVariableLoader<float>::UpdateShaderParams ) // compile test
	;

	class_< CShaderVariableLoader<Vector3>, boost::shared_ptr< CShaderVariableLoader<Vector3> > >("ShaderVec3Loader")
//		.def( init< const char *, Vector3 >() ) // (A) Either (A) or (B) compiles. (B) conforms to the original argument types.
		.def( init< const char *, const Vector3& >() ) // (B)
		.def( init< CShaderParameter<Vector3> >() )
	;
/*
	class_< CShaderVariableLoader<SFloatRGBAColor>, boost::shared_ptr< CShaderVariableLoader<SFloatRGBAColor> > >("ShaderColorLoader")
		.def( init< CShaderParameter<SFloatRGBAColor> >() )
	;*/
}


void RegisterPythonModule_gfx()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("gfx", initgfx) == -1)
	{
		const char *msg = "Failed to add gfx to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}
