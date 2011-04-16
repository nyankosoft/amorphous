#ifndef __PrimitiveRenderer_D3D_HPP__
#define __PrimitiveRenderer_D3D_HPP__


#include "../PrimitiveRenderer.hpp"
#include "../Direct3D/FVF_ColorVertex.h"


class CPrimitiveRenderer_D3D : public CPrimitiveRenderer
{
	std::vector<COLORVERTEX> m_ColorVertices;

private:

	HRESULT D3DDrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color )
	{
		COLORVERTEX verts[2];
		verts[0].color = start_color.GetARGB32();
		verts[1].color =   end_color.GetARGB32();

		verts[0].vPosition = ToD3DXVECTOR3( start );
		verts[1].vPosition = ToD3DXVECTOR3( end );

		HRESULT hr = S_OK;
		hr = DIRECT3D9.GetDevice()->SetFVF( D3DFVF_COLORVERTEX );

		// render the line
		hr = DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, 1, verts, sizeof(COLORVERTEX) );

		return hr;
	}

	HRESULT D3DDrawConnectedLines( int num_primitives_to_draw )
	{
		HRESULT hr = S_OK;
		hr = DIRECT3D9.GetDevice()->SetFVF( D3DFVF_COLORVERTEX );
		hr = DIRECT3D9.GetDevice()->SetVertexShader( NULL );
		hr = DIRECT3D9.GetDevice()->SetPixelShader( NULL );
		hr = DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_LINESTRIP, num_primitives_to_draw, &(m_ColorVertices[0]), sizeof(COLORVERTEX) );

		return hr;
	}

public:

	CPrimitiveRenderer_D3D(){}
	~CPrimitiveRenderer_D3D(){}

	Result::Name DrawPoint( const Vector3& pos, const SFloatRGBAColor& color )
	{
		COLORVERTEX v;
		v.vPosition = ToD3DXVECTOR3( pos );
		v.color =   color.GetARGB32();

		HRESULT hr = S_OK;
		hr = DIRECT3D9.GetDevice()->SetFVF( D3DFVF_COLORVERTEX );
		hr = DIRECT3D9.GetDevice()->SetVertexShader( NULL );
		hr = DIRECT3D9.GetDevice()->SetPixelShader( NULL );
		hr = DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_POINTLIST, 1, &v, sizeof(COLORVERTEX) );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}

//	Result::Name DrawPoints();

	Result::Name DrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& color )
	{
		DIRECT3D9.GetDevice()->SetVertexShader( NULL );
		DIRECT3D9.GetDevice()->SetPixelShader( NULL );
		HRESULT hr = D3DDrawLine( start, end, color, color );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}

	Result::Name DrawLine( const Vector3& start, const Vector3& end, const SFloatRGBAColor& start_color, const SFloatRGBAColor& end_color )
	{
		DIRECT3D9.GetDevice()->SetVertexShader( NULL );
		DIRECT3D9.GetDevice()->SetPixelShader( NULL );
		HRESULT hr = D3DDrawLine( start, end, start_color, end_color );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}

	Result::Name DrawConnectedLines( const std::vector<Vector3>& points, const SFloatRGBAColor& color )
	{
		if( points.size() < 2 )
			return Result::INVALID_ARGS;

		const size_t num_points = points.size();

		const D3DCOLOR d3d_color = color.GetARGB32();
		m_ColorVertices.resize( 0 );
		m_ColorVertices.resize( num_points );
		for( size_t i=0; i<num_points; i++ )
		{
			m_ColorVertices[i].vPosition = ToD3DXVECTOR3( points[i] );
			m_ColorVertices[i].color     = d3d_color;
		}

		HRESULT hr = D3DDrawConnectedLines( num_points - 1 );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}

	Result::Name DrawConnectedLines( const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors )
	{
		if( points.size() < 2 || points.size() != colors.size() )
			return Result::INVALID_ARGS;

		const size_t num_points = points.size();

		m_ColorVertices.resize( 0 );
		m_ColorVertices.resize( num_points );
		for( size_t i=0; i<num_points; i++ )
		{
			m_ColorVertices[i].vPosition = ToD3DXVECTOR3( points[i] );
			m_ColorVertices[i].color     = colors[i].GetARGB32();
		}

		HRESULT hr = D3DDrawConnectedLines( num_points - 1 );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}

/*
	Result::Name DrawLines( PrimitiveType::Name mode, const std::vector<Vector3>& points, const SFloatRGBAColor& color )
	{
		if( points.empty() )
			return Result::INVALID_ARGS;

		const size_t num_points = points.size();

		m_ColorVertices.resize( num_points );
		for( size_t i=0; i<num_points; i++ )
		{
			m_ColorVertices[i].vPosition = points[i];
			m_ColorVertices[i].color     = color.GetARGB32();
		}

		D3DPRIMITIVETYPE d3d_pt = D3DPT_LINELIST;//ToD3DPT( mode );

		HRESULT hr = S_OK;
		hr = DIRECT3D9.GetDevice()->SetFVF( D3DFVF_COLORVERTEX );
		hr = DIRECT3D9.GetDevice()->SetVertexShader( NULL );
		hr = DIRECT3D9.GetDevice()->SetPixelShader( NULL );
		hr = DIRECT3D9.GetDevice()->DrawPrimitiveUP( d3d_pt, 1, &(m_ColorVertices[0]), sizeof(COLORVERTEX) );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}

	Result::Name DrawLines( PrimitiveType::Name mode, const std::vector<Vector3>& points, const std::vector<SFloatRGBAColor>& colors )
	{
		if( points.empty() )
			return Result::INVALID_ARGS;

		if( points.size() != colors.size() )
			return Result::INVALID_ARGS;

		const size_t num_points = points.size();

		m_ColorVertices.resize( num_points );
		for( size_t i=0; i<num_points; i++ )
		{
			m_ColorVertices[i].vPosition = points[i];
			m_ColorVertices[i].color     = colors[i].GetARGB32();
		}

		HRESULT hr = S_OK;
		hr = DIRECT3D9.GetDevice()->SetFVF( D3DFVF_COLORVERTEX );
		hr = DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, 1, &(m_ColorVertices[0]), sizeof(COLORVERTEX) );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}*/
};


inline CPrimitiveRenderer_D3D& GetPrimitiveRenderer_D3D()
{
	static CPrimitiveRenderer_D3D s_Renderer;
	return s_Renderer;
}



#endif /* __PrimitiveRenderer_D3D_HPP__ */
