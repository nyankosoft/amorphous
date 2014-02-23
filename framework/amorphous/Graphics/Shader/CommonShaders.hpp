#ifndef __CommonShaders_HPP__
#define __CommonShaders_HPP__


#include "GenericShaderGenerator.hpp"
#include "../GraphicsResourceDescs.hpp"
#include "../ShaderHandle.hpp"


namespace amorphous
{


/// Shader that does not apply lighting and does not consider the vertex blending.
inline ShaderHandle GetNoLightingShader()
{
	static ShaderHandle s_NoLightingShader;
	static bool s_initialized = false;

	if( !s_initialized )
	{
		GenericShaderDesc gs_desc;
		gs_desc.Lighting = false;

		ShaderResourceDesc desc;
		desc.pShaderGenerator.reset( new GenericShaderGenerator(gs_desc) );

		bool res = s_NoLightingShader.Load(desc);

		s_initialized = true;
	}

	return s_NoLightingShader;
}


// Necessary?
//inline ShaderHandle GetVertexBlendedNoLightingShader()
//{
//}


} // namespace amorphous


#endif /* __CommonShaders_HPP__ */
