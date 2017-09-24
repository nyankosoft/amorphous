#ifndef __BlendTransformsLoader_HPP__
#define __BlendTransformsLoader_HPP__


#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/MeshContainerRenderMethod.hpp"
#include "amorphous/3DMath/Transform.hpp"


namespace amorphous
{


class BlendTransformsLoader : public ShaderParamsLoader
{
	std::vector<Transform> m_BlendTransforms;

public:

	BlendTransformsLoader() {}

	void UpdateShaderParams( ShaderManager& rShaderMgr )
	{
		// Set the vertex blend transforms to the shader
		rShaderMgr.SetVertexBlendTransforms( m_BlendTransforms );
	}

	std::vector<Transform>& BlendTransforms() { return m_BlendTransforms; }
};


class CBlendMatricesLoader : public ShaderParamsLoader
{
	std::vector<Matrix44> m_BlendMatrices;

public:

	CBlendMatricesLoader() {}

	void UpdateShaderParams( ShaderManager& rShaderMgr )
	{
		char acParam[32];
		const int num_matrices = (int)m_BlendMatrices.size();
		for( int i=0; i<num_matrices; i++ )
		{
			sprintf( acParam, "g_aBlendMatrix[%d]", i );
			rShaderMgr.SetParam( acParam, m_BlendMatrices[i] );
		}
	}

	std::vector<Matrix44>& BlendMatrices() { return m_BlendMatrices; }
};

} // namespace amorphous



#endif /* __BlendTransformsLoader_HPP__ */
