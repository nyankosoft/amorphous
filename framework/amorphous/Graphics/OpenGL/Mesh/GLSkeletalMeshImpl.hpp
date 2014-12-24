#ifndef __GLSkeletalMeshImpl_HPP__
#define __GLSkeletalMeshImpl_HPP__


#include "GLProgressiveMeshImpl.hpp"


namespace amorphous
{


/// NOT IMPLEMENTED YET
class GLSkeletalMeshImpl : public GLProgressiveMeshImpl
{
private:

//	void BuildVBOs( C3DMeshModelArchive& archive );

public:

	GLSkeletalMeshImpl();

	GLSkeletalMeshImpl( const std::string& filename );

//	virtual ~GLSkeletalMeshImpl();

	void Release() {}

//	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

//	bool LoadFromFile( const std::string& filename, U32 option_flags = 0 );

//	virtual bool CreateMesh( int num_vertices, int num_indices, U32 option_flags, std::vector<D3DVERTEXELEMENT9>& vecVertexElement );

	/// render object by using the fixed function pipeline
//	void Render();

//	virtual void Render( ShaderManager& rShaderMgr );

//	virtual void RenderSubsets( ShaderManager& rShaderMgr,
//		                        const std::vector<int>& vecMaterialIndex,
//								std::vector<ShaderTechniqueHandle>& vecShaderTechnique );

//	virtual void RenderSubsets( ShaderManager& rShaderMgr,
//		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */);

//	virtual MeshType::Name GetMeshType() const { return MeshType::SKELETAL; }
};


} // namespace amorphous



#endif	/*  __GLSkeletalMeshImpl_HPP__  */
