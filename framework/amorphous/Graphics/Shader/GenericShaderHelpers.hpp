#ifndef __GenericShaderHelpers_HPP__
#define __GenericShaderHelpers_HPP__


#include "GenericShaderGenerator.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{


inline ShaderHandle CreateNoLightingShader()
{
	ShaderHandle shader;
	ShaderResourceDesc desc;
	GenericShaderDesc gs_desc;
	gs_desc.Lighting = false;
	std::shared_ptr<GenericShaderGenerator> pGenerator( new GenericShaderGenerator(gs_desc) );
	desc.pShaderGenerator = pGenerator;
	bool shader_loaded = shader.Load( desc );
	if( !shader_loaded )
		LOG_PRINT_ERROR( " Failed to load the shader." );

	return shader;
}


} // namespace amorphous


#endif /* __GenericShaderHelpers_HPP__ */
