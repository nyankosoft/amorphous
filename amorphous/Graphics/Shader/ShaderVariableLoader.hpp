#ifndef  __ShaderVariableLoader_HPP__
#define  __ShaderVariableLoader_HPP__


//#include "ShaderParamsLoader.hpp"
#include "ShaderParameter.hpp"
#include "ShaderManager.hpp"
#include "../MeshContainerRenderMethod.hpp"


namespace amorphous
{


template<typename T>
class ShaderVariableLoader : public ShaderParamsLoader
{
	ShaderParameter<T> m_Parameter;

public:

	ShaderVariableLoader() {}

	ShaderVariableLoader( const char *param_name, const T& param_value )
		:
	m_Parameter( ShaderParameter<T>(param_name) )
	{
		m_Parameter.Parameter() = param_value;
	}
	
	const std::string& GetParamName() const { return m_Parameter.GetParameterName(); }

	void SetParamValue( const T& value ) { m_Parameter.Parameter() = value; }

//	ShaderVariableLoader( ShaderParameter<T>& shader_parameter ) // This causes a compiler error in PyModule_Graphics.cpp
	ShaderVariableLoader( const ShaderParameter<T>& shader_parameter )
		:
	m_Parameter(shader_parameter)
	{}

	void UpdateShaderParams( ShaderManager& rShaderMgr )
	{
		rShaderMgr.SetParam( m_Parameter );
	}

	bool LoadResource()
	{
		return LoadShaderParam( m_Parameter );
	}
};


} // namespace amorphous



#endif		/*  __ShaderVariableLoader_HPP__  */
