#ifndef __EmbeddedHLSLShader_HPP__
#define __EmbeddedHLSLShader_HPP__


//#include <string.h>
#include <vector>


class CEmbeddedHLSLShader
{
public:
	std::vector<const char *> pDependencies;
//	std::vector<const char *> pArgs;
	const char *pName;
	const char *pContent;

	CEmbeddedHLSLShader( const char *_pName = "", const char *_pContent = "" )
		:
	pName(_pName),
	pContent(_pContent)
	{}

	bool IsValid() const
	{
		if( pName    && 0 < strlen(pName)
		 && pContent && 0 < strlen(pContent) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};



class CEmbeddedHLSLShaders
{
public:

	static CEmbeddedHLSLShader ms_VS_PVL_HSLs_QVertexBlend;
	static CEmbeddedHLSLShader ms_PS_PVL_HSLs_QVertexBlend;

//	static CEmbeddedHLSLShader ms_VS_PVL_HSLs_QVertexBlend_Specular;
//	static CEmbeddedHLSLShader ms_VS_PVL_HSLs_QVertexBlend_Specular;

	static CEmbeddedHLSLShader ms_VS_PVL_HSLs;
	static CEmbeddedHLSLShader ms_PS_PVL_HSLs;

	static CEmbeddedHLSLShader ms_VS_PVL_HSLs_Specular;
	static CEmbeddedHLSLShader ms_PS_PVL_HSLs_Specular;

	static CEmbeddedHLSLShader ms_VS_PPL_HSLs;
	static CEmbeddedHLSLShader ms_PS_PPL_HSLs;

	static CEmbeddedHLSLShader ms_VS_PPL_HSLs_Specular;
	static CEmbeddedHLSLShader ms_PS_PPL_HSLs_Specular;

	static CEmbeddedHLSLShader ms_VS_PPL_HSLs_QVertexBlend_Specular;
};



#endif  /* __EmbeddedHLSLShader_HPP__ */
