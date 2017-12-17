#ifndef __GenericShaderHelpers_HPP__
#define __GenericShaderHelpers_HPP__


#include "GenericShaderGenerator.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"

// Helper function here are similar to those defined in CommonShaders.hpp,
// so perhaps they should be merged into one.
// The differences are that functions below create and returns a new shader handle
// whoere ones in CommonShaders.hpp returns a static reference to shader handles.


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


inline ShaderHandle CreateHSDirectionalLightingShader()
{
	ShaderHandle shader;
	ShaderResourceDesc desc;
	GenericShaderDesc gs_desc;
	gs_desc.NumDirectionalLights = 1;
	std::shared_ptr<GenericShaderGenerator> pGenerator( new GenericShaderGenerator(gs_desc) );
	desc.pShaderGenerator = pGenerator;
	bool shader_loaded = shader.Load( desc );
	if( !shader_loaded )
		LOG_PRINT_ERROR( " Failed to load a shader (hs dl)." );

	return shader;
	
}


} // namespace amorphous


#endif /* __GenericShaderHelpers_HPP__ */
