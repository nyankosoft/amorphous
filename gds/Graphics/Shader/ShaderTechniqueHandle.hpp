#ifndef  __ShaderTechniqueHandle_H__
#define  __ShaderTechniqueHandle_H__


#include <string.h>


namespace amorphous
{


/**
  - Don't use a shader technique handle for different shader managers.
    even if the shaders have techniques with the same name.
    - One shader technique handle for one shader

  - Use ShaderTechniqueHandle as member variables, not as a local variable,
    otherwise caching mechanism does not work.
*/
class ShaderTechniqueHandle
{
	/// avoid dynamic allocation when the name is short
	enum eParams { CACHE_SIZE = 24 };

	enum eShaderTechIndex { INVALID_INDEX = -1, UNINITIALIZED = -2 };

	/// stores a short name
	char m_acName[CACHE_SIZE];

	/// name of shader technique
	char *m_pcName;
	
	/// id of the resource
	int m_Index;

private:

	void SetTechniqueIndex( int index ) { m_Index = index; }
	int GetTechniqueIndex() const { return m_Index; }

public:

	ShaderTechniqueHandle() : m_pcName(NULL), m_Index(UNINITIALIZED)
	{
		memset( m_acName, '\0', sizeof(m_acName) );
	}

	inline ShaderTechniqueHandle( const ShaderTechniqueHandle& handle );

	~ShaderTechniqueHandle() { if( m_pcName ) delete [] m_pcName; }

	inline const char *GetTechniqueName() const;

	inline void SetTechniqueName( const char *pcName );

	inline const ShaderTechniqueHandle& operator=( const ShaderTechniqueHandle& handle );

	friend class ShaderManager;
};

//============================= inline implementations =============================


inline ShaderTechniqueHandle::ShaderTechniqueHandle( const ShaderTechniqueHandle& handle )
{
	m_pcName = NULL;
	m_Index = UNINITIALIZED;
    memset( m_acName, '\0', sizeof(m_acName) );

	SetTechniqueName( handle.GetTechniqueName() );
}


inline const char *ShaderTechniqueHandle::GetTechniqueName() const
{
	if( m_pcName )
		return m_pcName;
	else
		return m_acName;
}


inline void ShaderTechniqueHandle::SetTechniqueName( const char *pcName )
{
	// clear the current technique name
	if( m_pcName )
	{
		delete [] m_pcName;
		m_pcName = NULL;
	}

	memset( m_acName, 0, sizeof(m_acName) );

	// copy the technique name
	if( !pcName )
	{
		return;
	}
	else if( strlen(pcName) < CACHE_SIZE - 1 )
	{
		// short enough to store in the fixed sized buffer
		strcpy( m_acName, pcName );
	}
	else
	{
		m_pcName = new char [strlen(pcName) + 1];
		strcpy( m_pcName, pcName );
		m_acName[0] = '\0';
	}

	// the previous technique index is invalid
	// - mark it as uninitialized
	m_Index = UNINITIALIZED;
}


inline const ShaderTechniqueHandle& ShaderTechniqueHandle::operator=( const ShaderTechniqueHandle& handle )
{
	m_Index = handle.m_Index;

	SetTechniqueName( handle.GetTechniqueName() );

	return *this;
}

} // namespace amorphous



#endif		/*  __ShaderTechniqueHandle_H__  */
