#ifndef __D3DTextureResourceVisitor_HPP__
#define __D3DTextureResourceVisitor_HPP__


#include "../TextureResourceVisitor.hpp"
#include "D3DGraphicsResources.hpp"


namespace amorphous
{


class D3D_FFP_TextureResourceVisitor : public TextureResourceVisitor
{
	uint m_Stage;

public:

	D3D_FFP_TextureResourceVisitor(uint stage) : m_Stage(stage) {}
	~D3D_FFP_TextureResourceVisitor() {}

	Result::Name Visit( CD3DTextureResource& texture_resource )
	{
		HRESULT hr = DIRECT3D9.GetDevice()->SetTexture( m_Stage, texture_resource.GetTexture() );
		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}

	Result::Name Visit( CD3DCubeTextureResource& texture_resource )
	{
		HRESULT hr = DIRECT3D9.GetDevice()->SetTexture( m_Stage, texture_resource.GetCubeTexture() );
		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}
};


class D3DShaderTextureResourceVisitor : public TextureResourceVisitor
{
	LPD3DXEFFECT m_pEffect;
	D3DXHANDLE m_hParameter;
	uint m_Stage;

public:

	D3DShaderTextureResourceVisitor( LPD3DXEFFECT pEffect, D3DXHANDLE hParameter, uint stage ) : m_pEffect(pEffect), m_hParameter(hParameter), m_Stage(stage) {}
	~D3DShaderTextureResourceVisitor() {}

	Result::Name Visit( CD3DTextureResource& texture_resource )
	{
		HRESULT hr = E_FAIL;

		if( m_pEffect )
			hr = m_pEffect->SetTexture( m_hParameter, texture_resource.GetTexture() );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}

	Result::Name Visit( CD3DCubeTextureResource& texture_resource )
	{
		HRESULT hr = E_FAIL;

		if( m_pEffect )
			hr = m_pEffect->SetTexture( m_hParameter, texture_resource.GetTexture() );

		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}
};


} // namespace amorphous


#endif /* __D3DTextureResourceVisitor_HPP__ */
