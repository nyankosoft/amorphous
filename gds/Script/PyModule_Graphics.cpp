#include "PyModule_Graphics.hpp"
#include "../Graphics/MeshObjectHandle.hpp"
#include "../Graphics/ShaderHandle.hpp"
#include "../Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "../Graphics/Shader/ShaderVariableLoader.hpp"
#include "../Graphics/LensFlare.hpp"
#include "../Support/Log/DefaultLog.hpp"
#include <boost/python.hpp>


namespace amorphous
{


bool (TextureHandle::*LoadTextureFromResourcePath)( const std::string& ) = &TextureHandle::Load;
bool (MeshHandle::*LoadMeshFromResourcePath)( const std::string& ) = &MeshHandle::Load;
bool (ShaderHandle::*LoadShaderFromResourcePath)( const std::string& ) = &ShaderHandle::Load;

boost::shared_ptr<TextElement> (GraphicsElementManager::*CreateTextWithTLPos)( int, const std::string&, float, float, const SFloatRGBAColor&, int, int, int ) = &GraphicsElementManager::CreateText;
boost::shared_ptr<TextElement> (GraphicsElementManager::*CreateTextInBox)( int, const std::string&, const SRect&, int, int, const SFloatRGBAColor&, int, int, int ) = &GraphicsElementManager::CreateText;
bool (GraphicsElementManager::*LoadTextureWithID)( int, const std::string& ) = &GraphicsElementManager::LoadTexture;
int (GraphicsElementManager::*LoadTextureWithoutID)( const std::string& ) = &GraphicsElementManager::LoadTexture;
bool (GraphicsElementManager::*LoadFontWithID)( int, const std::string&, int, int, float, float, float ) = &GraphicsElementManager::LoadFont;
int (GraphicsElementManager::*LoadFontWithoutID)( const std::string&, int, int, float, float, float ) = &GraphicsElementManager::LoadFont;

GraphicsElementAnimationHandle (GraphicsElementAnimationManager::*ChangeColor_RGBA)( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color, int trans_mode ) = &GraphicsElementAnimationManager::ChangeColor;
GraphicsElementAnimationHandle (GraphicsElementAnimationManager::*ChangeColor_RGB)( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time,  double end_time, int color_index, const SFloatRGBColor& start_color, const SFloatRGBColor& end_color, int trans_mode )    = &GraphicsElementAnimationManager::ChangeColor;
GraphicsElementAnimationHandle (GraphicsElementAnimationManager::*ChangeColorTo_RGBA)( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBAColor& end_color, int trans_mode ) = &GraphicsElementAnimationManager::ChangeColorTo;
GraphicsElementAnimationHandle (GraphicsElementAnimationManager::*ChangeColorTo_RGB)( boost::shared_ptr<GraphicsElement> pTargetElement, double start_time, double end_time, int color_index, const SFloatRGBColor& end_color, int trans_mode )   = &GraphicsElementAnimationManager::ChangeColorTo;

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(gem_member_overloads, CreateText, 1, 3)


class GraphicsElementWrap : GraphicsElement, boost::python::wrapper<GraphicsElement>
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
		.def_readwrite("red",   &SFloatRGBAColor::red)
		.def_readwrite("green", &SFloatRGBAColor::green)
		.def_readwrite("blue",  &SFloatRGBAColor::blue)
		.def_readwrite("alpha", &SFloatRGBAColor::alpha)
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

	class_<SRect>("Rect")
		.def(init<int,int,int,int>())
		.def_readwrite("left",    &SRect::left)
		.def_readwrite("top",     &SRect::top)
		.def_readwrite("right",   &SRect::right)
		.def_readwrite("bottom",  &SRect::bottom)
//		.def("SetLTRB",           &SRect::SetPositionLTRB )
//		.def("SetLTWH",           &SRect::SetPositionLTWH )
		.def("GetWidth",          &SRect::GetWidth )
		.def("GetHeight",         &SRect::GetHeight )
		.def("GetCenter",         &SRect::GetCenter )
		.def("GetCenterX",        &SRect::GetCenterX )
		.def("GetCenterY",        &SRect::GetCenterY )
		.def("ContainsPoint",     &SRect::ContainsPoint )
		.def("GetTopLeftCorner",  &SRect::GetTopLeftCorner )
//		.def("Offset",            &SRect::Offset )
		.def("Inflate",           &SRect::Inflate )
		.def("Scale",             &SRect::Scale )
	;

	def( "RectLTWH", RectLTWH );
	def( "RectLTRB", RectLTRB );

	class_<TextureHandle>("TextureHandle")
		.def( "Load", LoadTextureFromResourcePath,      ( python::arg("resource_path") ) )
	;

	class_<MeshHandle>("MeshHandle")
		.def( "Load", LoadMeshFromResourcePath,   ( python::arg("resource_path") ) )
	;

	class_<ShaderHandle>("ShaderHandle")
		.def( "Load", LoadShaderFromResourcePath,       ( python::arg("resource_path") ) )
	;

	class_<ShaderTechniqueHandle>("ShaderTechniqueHandle")
		.def( "SetTechniqueName", &ShaderTechniqueHandle::SetTechniqueName, ( python::arg("name") ) )
	;

//	class_< GraphicsElementWrap, boost::shared_ptr<GraphicsElement> >( "GraphicsElement" )
//	class_< GraphicsElementWrap, boost::noncopyable >( "GraphicsElement" )
//		.def( "GetElementType",  pure_virtual(&GraphicsElement::GetElementType) )
//		.def( "GetColor",       &GraphicsElement::GetColor )
//		.def( "SetColor",       &GraphicsElement::SetColor )
//		.def( "GetAlpha",       &GraphicsElement::GetAlpha )
//		.def( "SetAlpha",       &GraphicsElement::SetAlpha )
	;

	class_< CombinedRectElement, boost::shared_ptr<CombinedRectElement> >("CombinedRectElement")
		.def( "FillRectElement",  &CombinedRectElement::FillRectElement )
		.def( "FrameRectElement", &CombinedRectElement::FrameRectElement )
	;

	class_< CombinedRoundRectElement, boost::shared_ptr<CombinedRoundRectElement> >("CombinedRoundRectElement")
		.def( "RoundFillRectElement",  &CombinedRoundRectElement::RoundFillRectElement )
		.def( "RoundFrameRectElement", &CombinedRoundRectElement::RoundFrameRectElement )
	;

	class_< CombinedTriangleElement, boost::shared_ptr<CombinedTriangleElement> >("CombinedTriangleElement")
		.def( "FillTriangleElement",  &CombinedTriangleElement::FillTriangleElement )
		.def( "FrameTriangleElement", &CombinedTriangleElement::FrameTriangleElement )
	;

	class_< TextElement, boost::shared_ptr<TextElement> >("TextElement")
		.def( "GetText",   &TextElement::GetTextCopy )
		.def( "SetText",   &TextElement::SetText )
		.def( "SetFontID", &TextElement::SetFontID )
	;

	class_< GraphicsElementManager, boost::shared_ptr<GraphicsElementManager> >("GraphicsElementManager")
//	class_< GraphicsElementManager >("GraphicsElementManager")
		.def( "CreateRect",             &GraphicsElementManager::CreateRect,      ( python::arg("rect"), python::arg("fill_color_0"), python::arg("frame_color_0"), python::arg("frame_width")=4, python::arg("layer") = 0 ) )
		.def( "CreateFillRect",         &GraphicsElementManager::CreateFillRect,  ( python::arg("rect"), python::arg("fill_color_0"), python::arg("layer") = 0 ) )
		.def( "CreateFrameRect",        &GraphicsElementManager::CreateFrameRect, ( python::arg("rect"), python::arg("frame_color_0"), python::arg("frame_width"), python::arg("layer") = 0 ) )

		.def( "CreateRoundRect",        &GraphicsElementManager::CreateRoundRect, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("frame_color_0"), python::arg("corner_radius"), python::arg("frame_width"), python::arg("layer") = 0 ) )
		.def( "CreateRoundFillRect",    &GraphicsElementManager::CreateRoundFillRect, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("corner_radius"), python::arg("layer") = 0 ) )
		.def( "CreateRoundFrameRect",   &GraphicsElementManager::CreateRoundFrameRect, ( python::arg("rect"), python::arg("frame_color_0"), python::arg("corner_radius"), python::arg("frame_width"), python::arg("layer") = 0 ) )

//		.def( "CreateTriangle",         &GraphicsElementManager::CreateTriangle, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("frame_color_0"), python::arg("frame_width"), python::arg("layer") = 0 ) )
//		.def( "CreateFillTriangle",     &GraphicsElementManager::CreateFillTriangle, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("layer") = 0 ) )
//		.def( "CreateFrameTriangle",    &GraphicsElementManager::CreateFrameTriangle, ( python::arg("rect"), python::arg("frame_color_0"), python::arg("frame_width"), python::arg("layer") = 0 ) )

//		.def( "CreateText",             &GraphicsElementManager::CreateTLAlignedText, ( python::arg("font_id"), python::arg("text"), python::arg("x") = 0, python::arg("y") = 0, python::arg("color") = SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f), python::arg("font_width") = 16, python::arg("font_height") = 32, python::arg("layer") = 0 ) )
//		.def( "CreateText",             &GraphicsElementManager::CreateTextInRect,    ( python::arg("font_id"), python::arg("text"), python::arg("rect"), python::arg("align_h"), python::arg("align_v"), python::arg("color"), python::arg("font_width"), python::arg("font_height"), python::arg("layer") = 0 ) )
		.def( "CreateText",             CreateTextWithTLPos, ( python::arg("font_id"), python::arg("text"), python::arg("x") = 0, python::arg("y") = 0, python::arg("color") = SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f), python::arg("font_width") = 0, python::arg("font_height") = 0, python::arg("layer") = 0 ) )
		.def( "CreateText",             CreateTextInBox,     ( python::arg("font_id"), python::arg("text"), python::arg("rect"), python::arg("align_h"), python::arg("align_v"), python::arg("color"), python::arg("font_width") = 0, python::arg("font_height") = 0, python::arg("layer") = 0 ) )

//		.def("CreateGroup",             &GraphicsElementManager::CreateRect, ( python::arg("rect"), python::arg("fill_color_0"), python::arg("frame_color_0"), python::arg("frame_width"), python::arg("layer") = 0 ) )

		.def("LoadTexture",             LoadTextureWithID,    ( python::arg("texture_id"), python::arg("texture_path") ) )
		.def("LoadTexture",             LoadTextureWithoutID, (                            python::arg("texture_path") ) )

		.def("LoadFont",                LoadFontWithID,    ( python::arg("font_id"), python::arg("font_name"), python::arg("width") = 16, python::arg("height") = 32, python::arg("bold") = 0.0f, python::arg("italic") = 0.0f, python::arg("shadow") = 0.0f ) )
		.def("LoadFont",                LoadFontWithoutID, (                         python::arg("font_name"), python::arg("width") = 16, python::arg("height") = 32, python::arg("bold") = 0.0f, python::arg("italic") = 0.0f, python::arg("shadow") = 0.0f ) )
	;

	enum_<TextElement::eTextAlignment>("TextAlignment")
		.value( "Left",   TextElement::TAL_LEFT )
		.value( "Top",    TextElement::TAL_TOP )
		.value( "Center", TextElement::TAL_CENTER )
		.value( "Right",  TextElement::TAL_RIGHT )
		.value( "Bottom", TextElement::TAL_BOTTOM )
	;

	def( "GetGraphicsElementManager", GetGraphicsElementManager );


//	class_<GraphicsElementAnimationHandle>("GraphicsEffectHandle")
//		.def( ""

//	def( "GetGraphicsEffectManager", GetGraphicsEffectManager );

	class_< GraphicsElementAnimationManager, boost::shared_ptr<GraphicsElementAnimationManager> >("GraphicsEffectManager")
		.def( "ChangeAlpha",           &GraphicsElementAnimationManager::ChangeAlpha,           (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("start_alpha"), py::arg("end_alpha"), py::arg("trans_mode") ) )
		.def( "ChangeAlphaTo",         &GraphicsElementAnimationManager::ChangeAlphaTo,         (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("end_alpha"), py::arg("trans_mode") ) )
		.def( "ChangeColor",           ChangeColor_RGBA,                                 (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("start_color"), py::arg("end_color"), py::arg("trans_mode") ) )
		.def( "ChangeColorTo",         ChangeColorTo_RGBA,                               (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("end_color"), py::arg("trans_mode") ) )
		.def( "ChangeColor",           ChangeColor_RGB,                                  (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("start_color"), py::arg("end_color"), py::arg("trans_mode") ) )
		.def( "ChangeColorTo",         ChangeColorTo_RGB,                                (  py::arg("target"), py::arg("start_time"), py::arg("end_time"), py::arg("color_index"), py::arg("end_color"), py::arg("trans_mode") ) )
		.def( "TranslateNonLinear",    &GraphicsElementAnimationManager::TranslateNonLinear,    (  py::arg("target"), py::arg("start_time"), py::arg("dest_pos"), py::arg("init_vel"), py::arg("smooth_time"), py::arg("coord_type"), py::arg("flags") ) )
		.def( "BlinkAlpha",            &GraphicsElementAnimationManager::BlinkAlpha,            (  py::arg("target"), py::arg("interval"), py::arg("color_index") ) )
		.def( "ChangeAlphaInSineWave", &GraphicsElementAnimationManager::ChangeAlphaInSineWave, (  py::arg("target"), py::arg("start_time"), py::arg("period"), py::arg("color_index"), py::arg("alpha0"), py::arg("alpha1"), py::arg("num_periods") ) )
		.def( "ChangeColorInSineWave", &GraphicsElementAnimationManager::ChangeColorInSineWave, (  py::arg("target"), py::arg("start_time"), py::arg("period"), py::arg("color_index"), py::arg("color0"), py::arg("color1"), py::arg("num_periods") ) )
/*		.def( "DrawText",              &GraphicsElementAnimationManager::DrawText,              (  boost::shared_ptr<TextElement> pTargetTextElement, double start_time, int num_chars_per_sec ) )
		.def( "CancelEffect",          &GraphicsElementAnimationManager::CancelEffect,          (  py::arg("effect_handle") ) )
*/
	;


/*
	class_<Light>("Light")
		.def_readwrite("DiffuseColor", &Light::DiffuseColor)
		.def_readwrite("fIntensity",   &Light::fIntensity)
		.def_readwrite("fRange",       &Light::fRange)
	;
*/

	class_< LensFlare, boost::shared_ptr<LensFlare> >("LensFlare")
		.def( "AddTexture",            &LensFlare::AddTexture,                          (  py::arg("texture_path"), py::arg("group_index") = 0, py::arg("num_segs_x") = 1, py::arg("num_segs_y") = 1 ) )
		.def( "AddLensFlareRect",      &LensFlare::AddLensFlareRect,                    (  py::arg("dim"), py::arg("scale_factor") = 1, py::arg("dist_factor") = 1, py::arg("color") = SFloatRGBAColor::White(), py::arg("group_index") = 0, py::arg("tex_seg_index_x") = 0, py::arg("tex_seg_index_y") = 0 ) )
	;

	class_< ShaderVariableLoader<float>, boost::shared_ptr< ShaderVariableLoader<float> > >("ShaderFloatLoader")
		.def( init< const char *, float >() )
		.def( init< ShaderParameter<float> >() )
		.def( "UpdateShaderParams",   &ShaderVariableLoader<float>::UpdateShaderParams ) // compile test
	;

	class_< ShaderVariableLoader<Vector3>, boost::shared_ptr< ShaderVariableLoader<Vector3> > >("ShaderVec3Loader")
//		.def( init< const char *, Vector3 >() ) // (A) Either (A) or (B) compiles. (B) conforms to the original argument types.
		.def( init< const char *, const Vector3& >() ) // (B)
		.def( init< ShaderParameter<Vector3> >() )
	;
/*
	class_< ShaderVariableLoader<SFloatRGBAColor>, boost::shared_ptr< ShaderVariableLoader<SFloatRGBAColor> > >("ShaderColorLoader")
		.def( init< ShaderParameter<SFloatRGBAColor> >() )
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


} // namespace amorphous
