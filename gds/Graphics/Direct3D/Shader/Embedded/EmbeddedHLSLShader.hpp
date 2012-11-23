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

	// About member variable naming:
	//
	// ms_?S_P?L_*L{s}{_QVertexBlend}{_Specular}
	//   [1] [2] [3]  [4]            [5]
	//
	// [1]: VS (vertex shader) or PS (pixel shader)
	// [2]: PVL (per-vertex lighting) or PPL (per-pixel lighting)
	// [3]: HSL(s) = hemishperical lights.
	// [4]: _QVertexBlend = vertex blending via quaternion
	// [5]: _Specular = supports specular highlight

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
