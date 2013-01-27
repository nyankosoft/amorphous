
#ifndef __SHADER_SINGLETON_H__
#define __SHADER_SINGLETON_H__


#include "Support/Singleton.hpp"
#include <stdlib.h>


namespace amorphous
{

class ShaderManager;


class CShader
{
	ShaderManager *m_pCurrentShaderManager;


	static CSingleton<CShader> m_obj;	///< singleton instance

public:

//	static CShader* Get() { return m_obj->get(); }
	static CShader* Get() { return m_obj.get(); }

	CShader() { m_pCurrentShaderManager = NULL; }

	inline void SetShaderManager( ShaderManager *pShaderManager ) { m_pCurrentShaderManager = pShaderManager; }

	inline ShaderManager *GetCurrentShaderManager() { return m_pCurrentShaderManager; }

//	void PushShaderManager( ShaderManager *pShaderManager ) { m_vecpShaderManager.push(  pShaderManager ); }
//	ShaderManager *PopShaderManager() { if( 0 < m_vecpShaderManager.size() ) return m_pShaderManager.pop(); else return NULLL}
};

} // namespace amorphous



#endif  /*  __SHADER_SINGLETON_H__  */
