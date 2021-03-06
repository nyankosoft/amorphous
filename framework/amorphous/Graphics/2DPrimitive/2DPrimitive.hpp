#ifndef  __2DPrimitive_H__
#define  __2DPrimitive_H__


#include "../../base.hpp"
#include "../../Graphics/TextureCoord.hpp"
#include "../../Graphics/AlphaBlend.hpp"
#include "../../Graphics/TextureHandle.hpp"
#include "../../Graphics/FloatRGBAColor.hpp"
#include "../../Graphics/Rect.hpp"
#include "../../Graphics/General2DVertex.hpp"
#include "../../Graphics/GraphicsDevice.hpp"
#include "../../Graphics/TextureStage.hpp"
#include "../../3DMath/Vector2.hpp"
#include "../../3DMath/Matrix22.hpp"
#include "../../3DMath/Vector3.hpp"


namespace amorphous
{


/**
 hold a 2D primitive
 - About corner indexing of rectangular primitives:
   - indexing for vertices is done in clockwise order: [0,3] > top-left, top-right, bottom-right, bottom-left
 */
class C2DPrimitive
{
protected:

	AlphaBlend::Mode m_DestAlphaBlend;

//	TextureAddress::Mode m_TextureAddressingMode;

protected:

	inline void SetBasicRenderStates();

	/// set render states to draw without texture
	inline void SetStdRenderStates();

	/// set render states to draw with texture
	inline void SetStdRenderStatesForTexture( const TextureHandle& texture );

public:

	inline C2DPrimitive() : m_DestAlphaBlend(AlphaBlend::InvSrcAlpha) { /*SetDefault();*/ }

	virtual ~C2DPrimitive() {}

	enum ePrimitiveType
	{
		TYPE_TRIANGLE,
		TYPE_RECT,
		TYPE_FRAMERECT,
		TYPE_ROUNDRECT,
		TYPE_ROUNDFRAMERECT,
		TYPE_POLYGON,
		TYPE_RECTSET,
		NUM_PRIMITIVE_TYPES
	};

	virtual int unsigned GetPrimitiveType() const = 0;

//	virtual void SetDefault() {}

	/// draw rect without any render state changes
	inline virtual void draw() {}

	inline virtual void Draw();

	inline virtual void Draw( const TextureHandle& texture );

	virtual Vector2 GetPosition2D( int vert_index ) const { return Vector2(0,0); }

	virtual void SetVertexPosition( int vert_index, const Vector2& rvPosition ) {}

	/// set the position of the rect through top-left and bottom-right corner positions
	virtual void SetPosition( const Vector2& vMin, const Vector2& vMax) {}

	virtual void SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax ) {}

	/// \param stretch_x horizontal stretch in pixels
	/// \param stretch_y vertical stretch in pixels
	virtual void SetTextureCoords( int stretch_x, int stretch_y,
		                           const TEXCOORD2& left_top_offset = TEXCOORD2(0,0),
								   TextureAddress::Mode mode = TextureAddress::Wrap );

	virtual void SetColor( const SFloatRGBAColor& color ) {}

	virtual void SetCornerColor( int corner, const SFloatRGBAColor& color ) {}

	/// \param color 32-bit ARGB color with 8bits for each component (NOTE: bit order. alpha component fist)
	inline void SetColor( U32 color );

	inline void SetCornerColor( int corner, U32 color );

	virtual void SetZDepth(float fZValue) {}

//	void ScalePosition( float fScale );

	virtual void Translate( float dx, float dy ) {}
	virtual void Translate( int dx, int dy ) {}

	virtual void Rotate( float angle ) {}

	virtual void Rotate( const Matrix22& matOrient ) {}

	inline void SetDestAlphaBlendMode( AlphaBlend::Mode dest_blend_mode ) { m_DestAlphaBlend = dest_blend_mode; }
};

// =================================== inline implementations =================================== 



inline void C2DPrimitive::SetBasicRenderStates()
{
	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, true );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::SrcAlpha );
	GraphicsDevice().SetDestBlendMode( m_DestAlphaBlend );

	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST,   false );
//	GraphicsDevice().SetRenderState( RenderStateType::FACE_CULLING, true );
}



inline void C2DPrimitive::SetStdRenderStates()
{
	GraphicsDevice().SetTexture( 0, TextureHandle() );

	TextureStage tex_stage_0, tex_stage_1;

	tex_stage_0.ColorOp   = TexStageOp::SELECT_ARG0;
	tex_stage_0.ColorArg0 = TexStageArg::DIFFUSE;
	tex_stage_0.AlphaOp   = TexStageOp::SELECT_ARG0;
	tex_stage_0.AlphaArg0 = TexStageArg::DIFFUSE;
	GraphicsDevice().SetTextureStageParams( 0, tex_stage_0 );

	tex_stage_1.ColorOp = TexStageOp::DISABLE;
	tex_stage_1.AlphaOp = TexStageOp::DISABLE;
	GraphicsDevice().SetTextureStageParams( 1, tex_stage_1 );
}


inline void C2DPrimitive::SetStdRenderStatesForTexture( const TextureHandle& texture )
{
	GraphicsDevice().SetTexture( 0, texture );

	TextureStage tex_stage_0, tex_stage_1;

	tex_stage_0.ColorOp   = TexStageOp::MODULATE;
	tex_stage_0.ColorArg0 = TexStageArg::DIFFUSE;
	tex_stage_0.ColorArg1 = TexStageArg::TEXTURE;
	tex_stage_0.AlphaOp   = TexStageOp::MODULATE;
	tex_stage_0.AlphaArg0 = TexStageArg::DIFFUSE;
	tex_stage_0.AlphaArg1 = TexStageArg::TEXTURE;
	GraphicsDevice().SetTextureStageParams( 0, tex_stage_0 );

	tex_stage_1.ColorOp = TexStageOp::DISABLE;
	tex_stage_1.AlphaOp = TexStageOp::DISABLE;
	GraphicsDevice().SetTextureStageParams( 1, tex_stage_1 );
}


inline void C2DPrimitive::Draw()
{
	SetBasicRenderStates();

	SetStdRenderStates();

	draw();
}


inline void C2DPrimitive::Draw( const TextureHandle& texture )
{
	SetBasicRenderStates();

	SetStdRenderStatesForTexture( texture );

	draw();
}


inline void C2DPrimitive::SetColor( U32 color )
{
	SFloatRGBAColor c;
	c.SetARGB32(color);
	SetColor( c );
}


inline void C2DPrimitive::SetCornerColor( int corner, U32 color )
{
	SFloatRGBAColor c;
	c.SetARGB32(color);
	SetCornerColor( corner, c );
}

} // namespace amorphous



#endif		/*  __2DPrimitive_H__  */
