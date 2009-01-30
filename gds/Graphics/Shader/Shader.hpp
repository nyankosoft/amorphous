
#ifndef __SHADER_SINGLETON_H__
#define __SHADER_SINGLETON_H__


#include "Support/Singleton.hpp"
using namespace NS_KGL;

#include <stdlib.h>

class CShaderManager;


class CShader
{
	CShaderManager *m_pCurrentShaderManager;


	static CSingleton<CShader> m_obj;	///< singleton instance

public:

//	static CShader* Get() { return m_obj->get(); }
	static CShader* Get() { return m_obj.get(); }

	CShader() { m_pCurrentShaderManager = NULL; }

	inline void SetShaderManager( CShaderManager *pShaderManager ) { m_pCurrentShaderManager = pShaderManager; }

	inline CShaderManager *GetCurrentShaderManager() { return m_pCurrentShaderManager; }

//	void PushShaderManager( CShaderManager *pShaderManager ) { m_vecpShaderManager.push(  pShaderManager ); }
//	CShaderManager *PopShaderManager() { if( 0 < m_vecpShaderManager.size() ) return m_pShaderManager.pop(); else return NULLL}
};


#endif  /*  __SHADER_SINGLETON_H__  */
