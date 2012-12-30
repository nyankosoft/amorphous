#ifndef  __GENERAL3DVERTEX_H__
#define  __GENERAL3DVERTEX_H__

#include "../Support/FixedVector.hpp"
#include "../3DMath/Vector3.hpp"
#include "TextureCoord.hpp"
#include "FloatRGBAColor.hpp"


namespace amorphous
{


//=========================================================================================
// CGeneral3DVertex
//=========================================================================================

class CGeneral3DVertex
{
public:

	enum eProperty
	{
		NUM_MAX_TEXTURECOORD_SETS = 4,
		NUM_MAX_BLEND_MATRICES_PER_VERTEX = 4,
	};

public:

	Vector3 m_vPosition;

	Vector3 m_vNormal;

	/// represents a local space of a vertex
	Vector3 m_vBinormal;
	Vector3 m_vTangent;

	TCFixedVector<TEXCOORD2, NUM_MAX_TEXTURECOORD_SETS> m_TextureCoord;

	SFloatRGBAColor m_DiffuseColor;

	TCFixedVector<float, NUM_MAX_BLEND_MATRICES_PER_VERTEX> m_fMatrixWeight;

	TCFixedVector<int, NUM_MAX_BLEND_MATRICES_PER_VERTEX> m_iMatrixIndex;

public:

	inline CGeneral3DVertex();

	/// TODO: support vertices with blend weights
	inline CGeneral3DVertex operator+( const CGeneral3DVertex& v ) const;
	inline CGeneral3DVertex operator-( const CGeneral3DVertex& v ) const;

	inline CGeneral3DVertex operator*( float f ) const;
	inline CGeneral3DVertex operator/( float f ) const;
};


inline CGeneral3DVertex::CGeneral3DVertex()
:
m_vPosition(Vector3(0,0,0)),
m_vNormal(Vector3(0,0,0)),
m_vBinormal(Vector3(0,0,0)),
m_vTangent(Vector3(0,0,0)),
m_DiffuseColor( SFloatRGBAColor(1.0f, 1.0f, 1.0f, 1.0f) )
{
}


inline CGeneral3DVertex CGeneral3DVertex::operator+( const CGeneral3DVertex& v ) const
{
	CGeneral3DVertex out;

	out.m_vPosition	= m_vPosition	+ v.m_vPosition;
	out.m_vNormal	= m_vNormal		+ v.m_vNormal;
	out.m_vBinormal	= m_vBinormal	+ v.m_vBinormal;
	out.m_vTangent	= m_vTangent	+ v.m_vTangent;

	out.m_DiffuseColor = m_DiffuseColor + v.m_DiffuseColor;

	int i, num = m_TextureCoord.size();
	out.m_TextureCoord.resize( num );
	for( i=0; i<num; i++ )
		out.m_TextureCoord[i]	= m_TextureCoord[i]  + v.m_TextureCoord[i];
/*
	num = m_fMatrixWeight.size();
	out.m_fMatrixWeight.resize( num );
	for( i=0; i<num; i++ )
		out.m_fMatrixWeight[i]	= m_fMatrixWeight[i] + v.m_fMatrixWeight[i];

	num = m_iMatrixIndex.size();
	out.m_iMatrixIndex.resize( num );
	for( i=0; i<num; i++ )
		out.m_iMatrixIndex[i]	= m_iMatrixIndex[i]  + v.m_iMatrixIndex[i];
*/
	return out;
}


inline CGeneral3DVertex CGeneral3DVertex::operator-( const CGeneral3DVertex& v ) const
{
	CGeneral3DVertex out;

	out.m_vPosition	= m_vPosition	- v.m_vPosition;
	out.m_vNormal	= m_vNormal		- v.m_vNormal;
	out.m_vBinormal	= m_vBinormal	- v.m_vBinormal;
	out.m_vTangent	= m_vTangent	- v.m_vTangent;

	out.m_DiffuseColor = m_DiffuseColor - v.m_DiffuseColor;

	int i, num = m_TextureCoord.size();
	out.m_TextureCoord.resize( num );
	for( i=0; i<num; i++ )
		out.m_TextureCoord[i]	= m_TextureCoord[i]  - v.m_TextureCoord[i];

/*	num = m_fMatrixWeight.size();
	out.m_fMatrixWeight.resize( num );
	for( i=0; i<num; i++ )
		out.m_fMatrixWeight[i]	= m_fMatrixWeight[i] - v.m_fMatrixWeight[i];

	num = m_iMatrixIndex.size();
	out.m_iMatrixIndex.resize( num );
	for( i=0; i<num; i++ )
		out.m_iMatrixIndex[i]	= m_iMatrixIndex[i]  - v.m_iMatrixIndex[i];
*/
	return out;
}


inline CGeneral3DVertex CGeneral3DVertex::operator*( float f ) const
{
	CGeneral3DVertex out;

	out.m_vPosition	= m_vPosition	* f;
	out.m_vNormal	= m_vNormal		* f;
	out.m_vBinormal	= m_vBinormal	* f;
	out.m_vTangent	= m_vTangent	* f;

	out.m_DiffuseColor = m_DiffuseColor * f;

	int i, num = m_TextureCoord.size();
	out.m_TextureCoord.resize( num );
	for( i=0; i<num; i++ )
		out.m_TextureCoord[i]	= m_TextureCoord[i] * f;

	num = m_fMatrixWeight.size();
	out.m_fMatrixWeight.resize( num );
	for( i=0; i<num; i++ )
		out.m_fMatrixWeight[i]	= m_fMatrixWeight[i] * f;

	// no changes to vertex weight indices
//	num = m_iMatrixIndex.size();
//	out.m_iMatrixIndex.resize( num );
//	for( i=0; i<num; i++ )
//		out.m_iMatrixIndex[i]	= m_iMatrixIndex[i] * f;

	return out;
}


inline CGeneral3DVertex CGeneral3DVertex::operator/( float f ) const
{
	CGeneral3DVertex out;

	out.m_vPosition	= m_vPosition	/ f;
	out.m_vNormal	= m_vNormal		/ f;
	out.m_vBinormal	= m_vBinormal	/ f;
	out.m_vTangent	= m_vTangent	/ f;

	out.m_DiffuseColor = m_DiffuseColor / f;

	int i, num = m_TextureCoord.size();
	out.m_TextureCoord.resize( num );
	for( i=0; i<num; i++ )
		out.m_TextureCoord[i]	= m_TextureCoord[i] / f;

	num = m_fMatrixWeight.size();
	out.m_fMatrixWeight.resize( num );
	for( i=0; i<num; i++ )
		out.m_fMatrixWeight[i]	= m_fMatrixWeight[i] / f;

	// no changes to vertex weight indices
//	m_iMatrixIndex;

//	out.m_iMatrixIndex.resize( num );
//	for( i=0; i<num; i++ )
//		out.m_iMatrixIndex[i]	= m_iMatrixIndex[i] / f;

	return out;
}

} // namespace amorphous



#endif  /*  __GENERAL3DVERTEX_H__  */
