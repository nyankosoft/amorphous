#ifndef __PrimitiveRenderer_HPP__
#define __PrimitiveRenderer_HPP__


#include <vector>
#include "../3DMath/Vector3.hpp"
#include "FloatRGBAColor.hpp"
#include "GraphicsDevice.hpp" // TODO: Separate graphics device-related enums


/*
class CPrimitiveRendererImpl
{
public:
	CPrimitiveRenderer();
	virtual ~CPrimitiveRenderer();
};
*/

class CPrimitiveRenderer
{
//	CPrimitiveRendererImpl *m_pImpl;

public:

	CPrimitiveRenderer() {}
	virtual ~CPrimitiveRenderer() {}

	/**
	NOTE: The caller is responsible for setting the appropriate transforms
	via FixedFunctionPipelineManager().Set*Transform() (* = World, View, or Projection).
	Especially, don't forget to set the world transform.
	*/
	virtual Result::Name DrawPoint( const Vector3& pos, const SFloatRGBAColor& color = SFloatRGBAColor::White() ) = 0;

	virtual Result::Name DrawPoints( const std::vector<Vector3>& points, const SFloatRGBAColor& color = SFloatRGBAColor::White() ) = 0;

//	virtual Result::Name DrawPoints( const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors ) = 0;

	virtual Result::Name DrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& color = SFloatRGBAColor::White() ) = 0;

	virtual Result::Name DrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color ) = 0;

	virtual Result::Name DrawConnectedLines( const std::vector<Vector3>& points, const SFloatRGBAColor& color = SFloatRGBAColor::White() ) = 0;

	virtual Result::Name DrawConnectedLines( const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors ) = 0;

//	virtual Result::Name DrawLines( const std::vector<Vector3>& vertices, const SFloatRGBAColor& color = SFloatRGBAColor::White() ) = 0;

//	virtual Result::Name DrawLines( const std::vector<Vector3>& vertices, const std::vector<SFloatRGBAColor>& colors ) = 0;
/*
	// \param mode: PrimitiveType::LINE_LIST or PrimitiveType::LINE_STRIP
	virtual Result::Name DrawLines( PrimitiveType::Name mode, const std::vector<Vector3>& points, const SFloatRGBAColor& color = SFloatRGBAColor::White() ) = 0;

	// Should support programmable shader for drawing lines?
//	virtual Result::Name DrawLines( PrimitiveType::Name mode, const std::vector<Vector3>& points, const SFloatRGBAColor& color, CShaderManager& shader_mgr ) = 0;

	virtual Result::Name DrawLines( PrimitiveType::Name mode, const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors ) = 0;

//	virtual Result::Name DrawLines( PrimitiveType::Name mode, const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors, CShaderManager& shader_mgr ) = 0;*/
};

/*
/// Should each graphics library implement the above interfaces,
/// or should graphics device classes have a method to render primitives, like
class CD3DGraphicsDevice
{
public:

	// render points, lines, or triangles
	void RenderPrimitives( PrimitiveType::Name mode, const std::vector<CGeneral3DVertex>& vertices, U32 vertex_format_flags );
};*/


extern CPrimitiveRenderer& GetPrimitiveRenderer();


inline CPrimitiveRenderer*& RefPrimitiveRendererPtr()
{
	static CPrimitiveRenderer *s_pRenderer = NULL;
	return s_pRenderer;
}


/// primitive renderer must be initialized before calling this.
inline CPrimitiveRenderer& GetPrimitiveRenderer() { return *RefPrimitiveRendererPtr(); }



#endif /* __PrimitiveRenderer_HPP__ */
