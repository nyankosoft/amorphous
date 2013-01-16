#ifndef  __ShaderHandle_H__
#define  __ShaderHandle_H__


#include "GraphicsResourceHandle.hpp"


namespace amorphous
{


class ShaderHandle : public GraphicsResourceHandle
{
protected:

	static const ShaderHandle ms_NullHandle;

public:

	inline ShaderHandle() {}

	~ShaderHandle() { Release(); }

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Shader; }

	bool Load( const std::string& resource_path );

	bool Load( const CShaderResourceDesc& desc );

	static const ShaderHandle& Null() { return ms_NullHandle; }

	CShaderManager *GetShaderManager();

//	inline const ShaderHandle &operator=( const ShaderHandle& handle );
};

/*
inline const ShaderHandle &ShaderHandle::operator=( const ShaderHandle& handle ){}
*/

} // namespace amorphous



#endif  /* __ShaderHandle_H__ */
