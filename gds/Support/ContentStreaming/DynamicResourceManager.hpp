#ifndef  __DynamicResourceManager_H__
#define  __DynamicResourceManager_H__


#include <vector>
#include <string>

#include <d3dx9.h>

#include "Support/Singleton.h"
using namespace NS_KGL;

//#include "PackedFile.h"

class CAsyncLoader;
class CResourceReuseCache;
class CPackedFile;


#define DynamicResourceManager (*(CDynamicResourceManager::Get()))

class CDynamicResourceManager
{
	int m_NumResourceToLoadPerFrame;

	CAsyncLoader *m_pAsyncLoader;
	CResourceReuseCache *m_pResourceReuseCache;
//	CPackedFile m_PackFile;

	static CSingleton<CDynamicResourceManager> m_obj;

public:

	CDynamicResourceManager();

	~CDynamicResourceManager();

	static CDynamicResourceManager *Get() { return m_obj.get(); }

	void OnFrameRender();

	/// \param filename resource profile filename
	void Init( const std::string& ResourceProfileFilename );

	void LoadResourceProfile( const std::string& ResourceProfileFilename );

	void CreateTextureFromFile_Async( const std::string& filename,
		                              CPackedFile& packfile,
                                      IDirect3DTexture9** ppTexture,
                                      void* pContext );

};



#endif		/*  __DynamicResourceManager_H__  */
