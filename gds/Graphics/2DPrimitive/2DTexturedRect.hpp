#ifndef __2DTexturedRect_HPP__
#define __2DTexturedRect_HPP__




inline void CopyColor( SFloatRGBAColor& dest, const SFloatRGBAColor& src )
{
	dest = src;
}


inline void CopyColor( SFloatRGBAColor& dest, U32 src_argb32 )
{
	dest.SetARGB32( src_argb32 );
}


inline void CopyColor( U32 dest_argb32, const SFloatRGBAColor& src )
{
	dest_argb32 = src.GetARGB32();
}


inline void CopyColor( U32 dest_argb32, U32 src_argb32 )
{
	dest_argb32 = src_argb32;
}


template<int NumTextures, typename ColorType=U32>
class CTex2DVertex
{
public:

	Vector3 m_vPosition;

	float m_fRHW;

	ColorType m_DiffuseColor; /// U32 or SFloatRGBAColor;

	TEXCOORD2 m_TextureCoords[NumTextures];

public:

	CTex2DVertex()
		:
	m_vPosition( Vector3(0,0,0) ),
	m_fRHW( 1.0f )//,
//	m_DiffuseColor( GetDefaultDiffuseColor<ColorType>SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f) )
	{
		for( int i=0; i<NumTextures; i++ )
		{
			m_TextureCoords[i] = TEXCOOORD2(0,0);
		}
	}

	void SetDiffuseColor( U32 src )
	{
		CopyColor( m_DiffuseColor, src );
	}
};



template<int NumTextures, typename ColorType=U32>
class C2DTexturedRect
{
public:
	CTex2DVertex<NumTextures,ColorType> m_Vertices[4];

	void Draw( CTextureHandle& tex0 );
	void Draw( CTextureHandle& tex0, CTextureHandle& tex1 );
	void Draw( CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2 );
	void Draw( CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2, CTextureHandle& tex3 );

	inline void SetPosition( const Vector2& vMin, const Vector2& vMax );

	inline void SetPosition( int vert_index, const Vector2& rvPosition );

	inline void SetTextureCoords( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax );

	inline void SetColorARGB32( U32 color )
	{
		for( int i=0; i<4; i++ )
		{
			m_Vertices.SetDiffuseColor( color );
		}

	}

	inline void SetColor( const SFloatRGBAColor& color );

	const CTex2DVertex<NumTextures,ColorType> *GetVertices() const { return m_Vertices; }
};


template<int NumTextures, typename ColorType>
inline void C2DTexturedRect<NumTextures,ColorType>::SetPosition( const Vector2& vMin, const Vector2& vMax )
{
	CTex2DVertex<NumTextures,ColorType>* pVerts = m_Vertices;

	pVerts[0].m_vPosition.x = vMin.x;		// top-left corner of the rectangle
	pVerts[0].m_vPosition.y = vMin.y;

	pVerts[1].m_vPosition.x = vMax.x;
	pVerts[1].m_vPosition.y = vMin.y;

	pVerts[2].m_vPosition.x = vMax.x;		// bottom-right corner of the rectangle
	pVerts[2].m_vPosition.y = vMax.y;

	pVerts[3].m_vPosition.x = vMin.x;
	pVerts[3].m_vPosition.y = vMax.y;
}


template<int NumTextures, typename ColorType>
inline void C2DTexturedRect<NumTextures,ColorType>::SetPosition( int vert_index, const Vector2& rvPosition )
{
	m_Vertices[vert_index].x = rvPosition.x;
	m_Vertices[vert_index].y = rvPosition.y;
}


template<int NumTextures, typename ColorType>
inline void C2DTexturedRect<NumTextures,ColorType>::SetTextureCoords( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax )
{
}


class C2DTexturedRectRenderer
{

public:

	C2DTexturedRectRenderer();

	virtual ~C2DTexturedRectRenderer() {}

	virtual void Render( const C2DTexturedRect<1,U32>& rect, CTextureHandle& tex0 ) {}
	virtual void Render( const C2DTexturedRect<2,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1 ) {}
	virtual void Render( const C2DTexturedRect<3,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2 ) {}
	virtual void Render( const C2DTexturedRect<4,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2, CTextureHandle& tex3 ) {}
	virtual void Render( const C2DTexturedRect<1,SFloatRGBAColor>& rect, CTextureHandle& tex0 ) {}
	virtual void Render( const C2DTexturedRect<2,SFloatRGBAColor>& rect, CTextureHandle& tex0, CTextureHandle& tex1 ) {}
//	virtual void Render( const C2DTexturedRect<3,SFloatRGBAColor>& rect ) {}
//	virtual void Render( const C2DTexturedRect<4,SFloatRGBAColor>& rect ) {}

	virtual void Render( const C2DTexturedRect<1,U32>& rect, CShaderManager& shader_mgr ) {}
	virtual void Render( const C2DTexturedRect<2,U32>& rect, CShaderManager& shader_mgr ) {}
	virtual void Render( const C2DTexturedRect<3,U32>& rect, CShaderManager& shader_mgr ) {}
	virtual void Render( const C2DTexturedRect<4,U32>& rect, CShaderManager& shader_mgr ) {}
	virtual void Render( const C2DTexturedRect<1,SFloatRGBAColor>& rect, CShaderManager& shader_mgr ) {}
	virtual void Render( const C2DTexturedRect<2,SFloatRGBAColor>& rect, CShaderManager& shader_mgr ) {}
	virtual void Render( const C2DTexturedRect<3,SFloatRGBAColor>& rect, CShaderManager& shader_mgr ) {}
	virtual void Render( const C2DTexturedRect<4,SFloatRGBAColor>& rect, CShaderManager& shader_mgr ) {}
};


class C2DTexturedRectRenderer_D3D : public C2DTexturedRectRenderer
{
	DWORD m_FVFs[4];
//	uint m_VertexSizes[4];

	inline void Render( const void *pVertices, DWORD fvf, uint vert_size );

public:

	C2DTexturedRectRenderer_D3D();

	void Render( const C2DTexturedRect<1,U32>& rect, CTextureHandle& tex0 );
	void Render( const C2DTexturedRect<2,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1 );
	void Render( const C2DTexturedRect<3,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2 );
	void Render( const C2DTexturedRect<4,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2, CTextureHandle& tex3 );
	void Render( const C2DTexturedRect<1,SFloatRGBAColor>& rect, CTextureHandle& tex0 );
	void Render( const C2DTexturedRect<2,SFloatRGBAColor>& rect, CTextureHandle& tex0, CTextureHandle& tex1 );
//	void Render( const C2DTexturedRect<3,SFloatRGBAColor>& rect );
//	void Render( const C2DTexturedRect<4,SFloatRGBAColor>& rect );

	void Render( const C2DTexturedRect<1,U32>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<2,U32>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<3,U32>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<4,U32>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<1,SFloatRGBAColor>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<2,SFloatRGBAColor>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<3,SFloatRGBAColor>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<4,SFloatRGBAColor>& rect, CShaderManager& shader_mgr );
};


class C2DTexturedRectRenderer_GL : public C2DTexturedRectRenderer
{
public:

	C2DTexturedRectRenderer_GL();

	void Render( const C2DTexturedRect<1,U32>& rect, CTextureHandle& tex0 );
	void Render( const C2DTexturedRect<2,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1 );
	void Render( const C2DTexturedRect<3,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2 );
	void Render( const C2DTexturedRect<4,U32>& rect, CTextureHandle& tex0, CTextureHandle& tex1, CTextureHandle& tex2, CTextureHandle& tex3 );
	void Render( const C2DTexturedRect<1,SFloatRGBAColor>& rect, CTextureHandle& tex0 );
	void Render( const C2DTexturedRect<2,SFloatRGBAColor>& rect, CTextureHandle& tex0, CTextureHandle& tex1 );
//	void Render( const C2DTexturedRect<3,SFloatRGBAColor>& rect );
//	void Render( const C2DTexturedRect<4,SFloatRGBAColor>& rect );

	void Render( const C2DTexturedRect<1,U32>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<2,U32>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<3,U32>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<4,U32>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<1,SFloatRGBAColor>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<2,SFloatRGBAColor>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<3,SFloatRGBAColor>& rect, CShaderManager& shader_mgr );
	void Render( const C2DTexturedRect<4,SFloatRGBAColor>& rect, CShaderManager& shader_mgr );
};



#endif /* __2DTexturedRect_HPP__ */
