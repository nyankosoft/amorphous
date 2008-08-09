#ifndef  __ShaderHandle_H__
#define  __ShaderHandle_H__


#include "GraphicsResourceHandle.h"
#include "GraphicsResourceManager.h"


class CShaderHandle : public CGraphicsResourceHandle
{
protected:

	static const CShaderHandle ms_NullHandle;

	virtual void IncResourceRefCount();
	virtual void DecResourceRefCount();

public:

	inline CShaderHandle() {}

	~CShaderHandle() { Release(); }

	inline void Release();

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Shader; }

	bool Load( const std::string& resource_path );

	bool Load( const CShaderResourceDesc& desc );

	static const CShaderHandle& Null() { return ms_NullHandle; }

	CShaderManager *GetShaderManager();

//	inline const CShaderHandle &operator=( const CShaderHandle& handle );
};

/*
inline const CShaderHandle &CShaderHandle::operator=( const CShaderHandle& handle ){}
*/


inline void CShaderHandle::Release()
{
	if( 0 <= m_EntryID )
	{
		DecResourceRefCount();
		m_EntryID = -1;
	}
}



#endif  /* __ShaderHandle_H__ */
