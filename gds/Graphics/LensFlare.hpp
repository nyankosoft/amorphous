#ifndef  __LensFlare_H__
#define  __LensFlare_H__

#include "../base.hpp"
#include "3DMath/Vector3.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/GraphicsResourceManager.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "Graphics/FVF_TLVertex.h"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/2DPrimitive/2DRectSet.hpp"

#include <vector>
#include <string>


/// holds one lens flare 
/// - corresponds to one 2D rectangle
class CLensFlareComponent
{
public:
	int m_TexIndex;
	float m_fScaleFactor;
	float m_fDistFactor;
	SFloatRGBAColor m_Color;
	float m_fRadius;

	inline CLensFlareComponent();
};


inline CLensFlareComponent::CLensFlareComponent()
:
m_TexIndex(0),
m_fScaleFactor(1.0f),
m_fDistFactor(1.0f),
m_Color(SFloatRGBAColor(1,1,1,1)),
m_fRadius(1.0f)
{}



/// a group of lens flare components that shares the same texture
/// - Ideally, there should be only one lens flare group, since
///   - No texture switching is necessary to draw flares in a group
///   - A single DrawPrimitive() call draws all the flares in a group
class CLensFlareGroup
{
public:
	std::vector<CLensFlareComponent> m_vecComponent;

	C2DRectSet m_RectGroup;
	CTextureHandle m_Texture;

	int m_NumTextureSegmentsX;
	int m_NumTextureSegmentsY;

public:

	CLensFlareGroup();
};



//==============================================================
// 

class CLensFlare
{
	enum Params
	{
		NUM_MAX_TEXTURES = 8,
		NUM_MAX_FLARES = 32
	};

//	TLVERTEX m_avVertex[NUM_MAX_FLARES * 4];
//	WORD m_awIndex[NUM_MAX_FLARES * 6];
//	C2DRectSet m_RectGroup;

	std::vector<CLensFlareGroup> m_vecLensFlareGroup;
//	int m_NumTextures;

//	std::vector<CLensFlareComponent> m_vecFlareComponent;

//	int m_ScreenWidth;
//	int m_ScreenHeight;

	/// camera transform matrix
	D3DXMATRIX m_matView;

	/// projection transform matrix
	D3DXMATRIX m_matProj;

	/// position of the light in world space
	Vector3 m_vLightPosition;

public:

	CLensFlare();
	~CLensFlare();

	void Release();

//	void Render();

	/// renderes lens flare using a shader technique
	/// this method assumes that you have set a valid shader technique that renders
	/// transformed & lit vertices, and uses a texture sampler accessed by 'pTexHandle'
	void Render( CShaderManager& rShaderManager, int texture_stage = 0 );

//	inline void UpdateScreenSize( int screen_width, int screen_height );

	inline void UpdateViewTransform( const D3DXMATRIX& matView ) { m_matView = matView; }

	inline void UpdateProjectionTransform( const D3DXMATRIX& matProj ) { m_matProj = matProj; }

	inline void UpdateTransforms( const D3DXMATRIX& matView, const D3DXMATRIX& matProj );

	/// set lens flare positions, dimensions, etc.
	/// must be called before rendering the flares after all the other updates are done.
	void UpdateLensFlares();

	/// loads a texture and register it for lens flare effect
	/// - returns true on success
	bool AddTexture( const std::string& texture_filename, int texture_index, int num_segments_x = 1, int num_segments_y = 1 );

	/// \param color 32-bit ARGB format
	void AddLensFlareRect( float dim,
		        float scale_factor,
				float dist_factor,
				U32 color,
				int group_index,
				int tex_seg_index_x = 0,
				int tex_seg_index_y = 0 );

//	inline void AddLensFlareRect( const C2DRect& rect, float scale_factor, float dist_factor, int tex_index, int tex_seg_index = 0 ) }

	inline void SetFlareCenterPosition( int x, int y );

	void SetLightPosition( const Vector3& vLightPosition ) { m_vLightPosition = vLightPosition; }

	Vector3 GetLightPosition() const { return m_vLightPosition; }
};


// -------------------------- inline implementations --------------------------


inline void CLensFlare::UpdateTransforms( const D3DXMATRIX& matView, const D3DXMATRIX& matProj )
{
	m_matView = matView;
	m_matProj = matProj;
}

/*
inline void CLensFlare::UpdateScreenSize( int screen_width, int screen_height )
{
	m_ScreenWidth	= screen_width;
	m_ScreenHeight	= screen_height;
}
*/

#endif		/*  __LensFlare_H__  */
