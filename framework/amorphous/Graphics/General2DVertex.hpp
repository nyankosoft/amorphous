#ifndef  __GENERAL2DVERTEX_H__
#define  __GENERAL2DVERTEX_H__

#include "amorphous/3DMath/Vector3.hpp"
#include "TextureCoord.hpp"
#include "FloatRGBAColor.hpp"


namespace amorphous
{


//=========================================================================================
// General2DVertex
//=========================================================================================


class CColored2DVertex
{
public:

	Vector3 m_vPosition;

	float m_fRHW;

	SFloatRGBAColor m_DiffuseColor;

public:

	CColored2DVertex()
		:
	m_vPosition( Vector3(0,0,0) ),
	m_fRHW( 1.0f ),
	m_DiffuseColor( SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f) )
	{}
};


class General2DVertex
{
public:

	enum eProperty
	{
		NUM_MAX_TEXTURECOORD_SETS = 4,
	};

public:

	Vector3 m_vPosition;

	float m_fRHW;

	SFloatRGBAColor m_DiffuseColor;

//	TCFixedVector<TEXCOORD2, NUM_MAX_TEXTURECOORD_SETS> m_TextureCoord;
	TEXCOORD2 m_TextureCoord[NUM_MAX_TEXTURECOORD_SETS];

public:

	inline General2DVertex();

	/// TODO: support vertices with blend weights
	inline General2DVertex operator+( const General2DVertex& v ) const;
	inline General2DVertex operator-( const General2DVertex& v ) const;

	inline General2DVertex operator*( float f ) const;
	inline General2DVertex operator/( float f ) const;
};


inline General2DVertex::General2DVertex()
:
m_vPosition(Vector3(0,0,0)),
m_fRHW(1.0f),
m_DiffuseColor( SFloatRGBAColor(1.0f, 1.0f, 1.0f, 1.0f) )
{
	for( int i=0; i<NUM_MAX_TEXTURECOORD_SETS; i++ )
		m_TextureCoord[i] = TEXCOORD2(0,0);
}


inline General2DVertex General2DVertex::operator+( const General2DVertex& v ) const
{
	General2DVertex out;

	out.m_vPosition	= m_vPosition	+ v.m_vPosition;

//	out.m_fRHW = ???

	out.m_DiffuseColor = m_DiffuseColor + v.m_DiffuseColor;

//	int i, num = m_TextureCoord.size();
//	out.m_TextureCoord.resize( num );
//	for( i=0; i<num; i++ )
	for( int i=0; i<NUM_MAX_TEXTURECOORD_SETS; i++ )
		out.m_TextureCoord[i]	= m_TextureCoord[i]  + v.m_TextureCoord[i];

	return out;
}


inline General2DVertex General2DVertex::operator-( const General2DVertex& v ) const
{
	General2DVertex out;

	out.m_vPosition	= m_vPosition	- v.m_vPosition;

//	out.m_fRHW = ???

	out.m_DiffuseColor = m_DiffuseColor - v.m_DiffuseColor;

//	int i, num = m_TextureCoord.size();
//	out.m_TextureCoord.resize( num );
//	for( i=0; i<num; i++ )
	for( int i=0; i<NUM_MAX_TEXTURECOORD_SETS; i++ )
		out.m_TextureCoord[i]	= m_TextureCoord[i]  - v.m_TextureCoord[i];

	return out;
}


inline General2DVertex General2DVertex::operator*( float f ) const
{
	General2DVertex out;

	out.m_vPosition	= m_vPosition	* f;

//	out.m_fRHW = ???

	out.m_DiffuseColor = m_DiffuseColor * f;

//	int i, num = m_TextureCoord.size();
//	out.m_TextureCoord.resize( num );
//	for( i=0; i<num; i++ )
	for( int i=0; i<NUM_MAX_TEXTURECOORD_SETS; i++ )
		out.m_TextureCoord[i]	= m_TextureCoord[i] * f;

	return out;
}


inline General2DVertex General2DVertex::operator/( float f ) const
{
	General2DVertex out;

	out.m_vPosition	= m_vPosition	/ f;

//	out.m_fRHW = ???

	out.m_DiffuseColor = m_DiffuseColor / f;

//	int i, num = m_TextureCoord.size();
//	out.m_TextureCoord.resize( num );
//	for( i=0; i<num; i++ )
	for( int i=0; i<NUM_MAX_TEXTURECOORD_SETS; i++ )
		out.m_TextureCoord[i]	= m_TextureCoord[i] / f;

	return out;
}

} // namespace amorphous



#endif  /*  __GENERAL2DVERTEX_H__  */
