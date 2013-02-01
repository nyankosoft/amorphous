#ifndef  __ShaderVariableLoader_HPP__
#define  __ShaderVariableLoader_HPP__


//#include "ShaderParamsLoader.hpp"
#include "ShaderParameter.hpp"
#include "ShaderManager.hpp"
#include "../MeshContainerRenderMethod.hpp"


namespace amorphous
{


template<typename T>
class CShaderVariableLoader : public ShaderParamsLoader
{
	CShaderParameter<T> m_Parameter;

public:

	CShaderVariableLoader() {}

	CShaderVariableLoader( const char *param_name, const T& param_value )
		:
	m_Parameter( CShaderParameter<T>(param_name) )
	{
		m_Parameter.Parameter() = param_value;
	}
	
	const std::string& GetParamName() const { return m_Parameter.GetParameterName(); }

	void SetParamValue( const T& value ) { m_Parameter.Parameter() = value; }

//	CShaderVariableLoader( CShaderParameter<T>& shader_parameter ) // This causes a compiler error in PyModule_Graphics.cpp
	CShaderVariableLoader( const CShaderParameter<T>& shader_parameter )
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
