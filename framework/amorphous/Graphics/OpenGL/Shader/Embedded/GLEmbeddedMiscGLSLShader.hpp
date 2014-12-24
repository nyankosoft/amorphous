#ifndef __amorphous_GLEmbeddedMiscGLSLShader_HPP__
#define __amorphous_GLEmbeddedMiscGLSLShader_HPP__


#include "amorphous/Graphics/Shader/MiscShader.hpp"
#include "amorphous/Graphics/Shader/Embedded/EmbeddedMiscShader.hpp"


namespace amorphous
{


class GLEmbeddedMiscGLSLShader : public EmbeddedMiscShader
{
public:

	GLEmbeddedMiscGLSLShader(){}
	//GLEmbeddedMiscGLSLShader( MiscShader::ID shader_id ) : m_ShaderID(shader_id) {}
	~GLEmbeddedMiscGLSLShader(){}

	static void GetSingleDiffuseColorVertexShader( std::string& shader );

	static void GetVertexWeightMapDisplayVertexShader( std::string& shader );

	static void GetSingleDiffuseColorFragmentShader( std::string& shader );

	static void GetVertexWeightMapDisplayFragmentShader( std::string& shader );

	/// vertex shader
	static Result::Name GetVertexShader( MiscShader::ID shader_id, std::string& shader );

	/// pixel(fragment) shader
	static Result::Name GetFragmentShader( MiscShader::ID shader_id, std::string& shader );
};


} // namespace amorphous


#endif /* __amorphous_GLEmbeddedMiscGLSLShader_HPP__ */
