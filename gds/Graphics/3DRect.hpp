#ifndef __3DRect_HPP__
#define __3DRect_HPP__


#include "TextureCoord.hpp"


template<typename DiffuseColorType = SFloatRGBAColor, int NumTextureCoords = 1>
class C3DRect
{
	enum Params
	{
		NUM_VERTICES = 4,
	};

	Vector3 m_Positions[4];
	Vector3 m_Normals[4];
	DiffuseColorType m_DiffuseColor[4];
	TEXCOORD2 m_TexCoords[NumTextureCoords][4];

public:

	C3DRect( const Vector3& center, unsigned int axis, float edge0_length, float edge1_length );

	C3DRect( const Vector3& center, const Vector3& normal, float edge0_length, float edge1_length );

	C3DRect()
	{
		for( int i=0; i<NUM_VERTICES; i++ )
		{
			m_Positions[i] = Vector3(0,0,0);
			m_Normals[i]   = Vector3(0,1,0);
//			for( int j=0; j<NumTextureCoords; j++ )
//				m_TexCoords[j][i] = TEXCOORD2(0,0);
		}

		for( int i=0; i<NumTextureCoords; i++ )
			SetTextureCoordsLTRB( i, TEXCOORD2(0,0), TEXCOORD2(1,1) );
	}

	~C3DRect(){}

	void SetPositions( const Vector3* positions );

	void SetNormals( const Vector3& normal );

	void SetDiffuseColors( const DiffuseColorType& color );

	void SetTextureCoordsLTRB( unsigned int tex_coord_index, const TEXCOORD2& top_left, const TEXCOORD2& bottom_right );

	void Draw();

	void Draw( CShaderManager& shader_mgr );
};


/*
template<typename DiffuseColorType = SFloatRGBAColor, int NumTextureCoords = 1>
class TL3DRect
{
	enum Params
	{
		NUM_VERTICES = 4,
	};

	Vector3 m_Positions[4];
	DiffuseColorType m_DiffuseColor[4];
	TEXCOORD2 m_TexCoords[NumTextureCoords];

public:
	TL3DRect(){}
	~TL3DRect(){}
};
*/



#endif /* __3DRect_HPP__ */
