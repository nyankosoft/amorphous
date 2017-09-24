#ifndef  __GraphicsResources_HPP__
#define  __GraphicsResources_HPP__


#include <boost/thread.hpp>
#include "fwd.hpp"
#include "GraphicsResourceDescs.hpp"
#include "TextureResourceVisitor.hpp"
#include "SamplingParameter.hpp"
#include "amorphous/Support/Serialization/BinaryDatabase.hpp"
#include "amorphous/Support/fwd.hpp"
#include "amorphous/3DMath/Rectangular.hpp"
#include "amorphous/Graphics/OpenGL/GLHeaders.h"


namespace amorphous
{

typedef SRectangular SDim2;


class GraphicsResourceState
{
public:
	enum Name
	{
		CREATED,                  ///< Memory for the resource has been allocated. The content has not been loaded on memory. i.e.) empty state
		LOADING_SYNCHRONOUSLY,
		LOADING_ASYNCHRONOUSLY,
		LOADED,                   ///< The resource is ready to use
		RELEASING_SYNCHRONOUSLY,
		RELEASING_ASYNCHRONOUSLY,
		RELEASED,
		NUMSTATES
	};
};


class GraphicsResourceFlag
{
public:
	enum Flags
	{
		DontShare = ( 1 << 0 ),
		Flag1     = ( 1 << 1 ),
		Flag2     = ( 1 << 2 ),
	};
};


class GraphicsResource
{
protected:

	unsigned int m_OptionFlags;

	/// stores the time when the file was updated last
	time_t m_LastModifiedTimeOfFile;

	GraphicsResourceState::Name m_State;

	/// If true, kept in the array of cahced resources
	bool m_IsCachedResource;

	int m_Index;

	boost::mutex m_StateChangeLock;

protected:

	/// reference count is not changed in this function?
	virtual void Release() = 0;

	virtual bool IsDiskResource() const { return true; }

	/// load resource from a file or a binary database
	bool LoadFromDisk();

	/// Added to create empty texture as a graphics resource
	virtual bool CreateFromDesc() { return false; }

	void SetIndex( int index ) { m_Index = index; }

	virtual void UpdateDescForCachedResource( const GraphicsResourceDesc& desc ) {}

public:

	GraphicsResource();

	virtual ~GraphicsResource();

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	int GetIndex() const { return m_Index; }

	bool Load();

//	bool LoadAsync();

	void ReleaseNonChachedResource();

	void ReleaseCachedResource();

	bool IsCachedResource() const { return m_IsCachedResource; }

	/// \param filepath [in] pathname of the file to load
	/// - This is usually m_Filename itself
	virtual bool LoadFromFile( const std::string& filepath ) = 0;

	/// \param db [in] The database which contains the target graphics resource
	/// \param keyname [in] Required to read the graphics resource from the db
	/// - These are the strings obtained by decomposing m_Filename to "(db_filepath)::(keyname)"
	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname ) = 0;

	virtual bool CanBeSharedAsSameResource( const GraphicsResourceDesc& desc );

	virtual int CanBeUsedAsCache( const GraphicsResourceDesc& desc ) { return 0; }

	void Refresh();

	virtual const GraphicsResourceDesc& GetDesc() const = 0;

	virtual bool Lock() { return false; }

	virtual bool Unlock() { return false; }

	virtual bool Create() { return false; }

	inline void SetState( GraphicsResourceState::Name state );

	inline GraphicsResourceState::Name GetState();

	/// Appends a string which represents the status of the graphics resource to dest_buffer
	virtual void GetStatus( std::string& dest_buffer );

	friend class GraphicsResourceManager;
	friend class GraphicsResourceCacheManager;
};


class TextureResource : public GraphicsResource
{
protected:

	TextureResourceDesc m_TextureDesc;

	std::shared_ptr<LockedTexture> m_pLockedTexture;

protected:

	/// Release texture without changing the reference count
	/// called only from GraphicsResourceManager
	/// and from TextureResource if Refresh() is used
	virtual void Release();

	/// returns false if m_TextureDesc has valid width, height, and format 
	virtual bool IsDiskResource() const;

	/// create an empty texture
	/// - texture settings are read from m_TextureDesc
	virtual bool CreateFromDesc() = 0;

	void UpdateDescForCachedResource( const GraphicsResourceDesc& desc );

public:

	TextureResource( const TextureResourceDesc *pDesc );

	virtual ~TextureResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	virtual bool LoadFromFile( const std::string& filepath ) = 0;

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname ) = 0;

	virtual bool CanBeSharedAsSameResource( const GraphicsResourceDesc& desc );

	int CanBeUsedAsCache( const GraphicsResourceDesc& desc );

	/// For Direct3D
	inline virtual LPDIRECT3DTEXTURE9 GetTexture() { return NULL; }

	/// For OpenGL
	inline virtual GLuint GetGLTextureID() const { return 0; }

	const GraphicsResourceDesc& GetDesc() const { return m_TextureDesc; }

	/// Save the texture to disk as an image file
	/// - For debugging
	/// - Use this if the texture is loaded from file and filename is stored in GetDesc().ResourcePath
	///   - Wait! It will overwrite the original image file!
//	bool SaveTextureToImageFile(); commented out

	/// Save the texture to disk as an image file
	/// - For debugging
	/// - NOTE: image_filepath should be different from GetDesc().ResourcePath,
	///         or it will overwrite the original image file
	virtual bool SaveTextureToImageFile( const std::string& image_filepath ) { return false; }

	virtual SDim2 GetSize2D( unsigned int level = 0 ) { return SDim2(0,0); }

	virtual Result::Name SetSamplingParameter( SamplingParameter::Name param, uint value ) { return Result::UNKNOWN_ERROR; }

	virtual bool Lock() { return Lock( 0 ); }

	/// Used by texture resources
	/// \param mip_level the mip level of the texture resource to lock
	virtual bool Lock( uint mip_level ) { return false; }

	virtual bool Unlock() { return false; }

	/// Creates an empty texture from the current desc.
	/// Called by the render thread.
	virtual bool Create() = 0;

	virtual Result::Name Accept( TextureResourceVisitor& visitor ) = 0;

	/// Returns true on success
	/// - Succeeds only between a pair of Lock() and Unlock() calls
	/// - Returns an object that provides access to the locked texture surface
	bool GetLockedTexture( std::shared_ptr<LockedTexture>& pLockedTexture );

	void GetStatus( std::string& dest_buffer );

	friend class GraphicsResourceManager;
};


class MeshSubResource
{
public:
	enum Name
	{
		VERTEX,
		INDEX,
		ATTRIBUTE_TABLE,
		NUM_SUBRESOURCES,
	};
};


class MeshResource : public GraphicsResource
{
	std::shared_ptr<BasicMesh> m_pMeshObject;

	MeshResourceDesc m_MeshDesc;

	GraphicsResourceState::Name m_aSubResourceState[MeshSubResource::NUM_SUBRESOURCES];

protected:

	/// release mesh object without changing the reference count
	/// called only from this class and CMeshObjectManager
	virtual void Release();

	/// returns false if there is a mesh generator
	bool IsDiskResource() const;

	/// create a mesh from a mesh generator
	bool CreateFromDesc();

public:

	MeshResource( const MeshResourceDesc *pDesc );

	virtual ~MeshResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const GraphicsResourceDesc& desc );

	int CanBeUsedAsCache( const GraphicsResourceDesc& desc );

	inline std::shared_ptr<BasicMesh> GetMesh();

	inline std::shared_ptr<BasicMesh> GetMeshInLoading();

	MeshType::Name GetMeshType() const { return m_MeshDesc.MeshType; }

	const GraphicsResourceDesc& GetDesc() const { return m_MeshDesc; }

	void GetStatus( std::string& dest_buffer );

//	bool Lock();

//	bool Unlock();

	bool Create();

	GraphicsResourceState::Name GetSubResourceState( MeshSubResource::Name subresource ) const { return m_aSubResourceState[subresource]; }

	void SetSubResourceState( MeshSubResource::Name subresource, GraphicsResourceState::Name state );

	void CreateMeshAndLoadNonAsyncResources( C3DMeshModelArchive& archive );

	bool LoadMeshFromArchive( C3DMeshModelArchive& mesh_archive );

	friend class GraphicsResourceManager;
};


class ShaderResource : public GraphicsResource
{
	ShaderManager *m_pShaderManager;

protected:

	ShaderResourceDesc m_ShaderDesc;

protected:

	/// Release the shader manager without changing the reference count
	/// - Called only from this class and CMeshObjectManager
	virtual void Release();

	bool IsDiskResource() const;

	virtual bool CreateFromDesc();

	virtual ShaderManager *CreateShaderManager() { return NULL; }

	virtual ShaderManager *CreateFixedFunctionPipelineManager() = 0;

public:

	ShaderResource( const ShaderResourceDesc *pDesc );

	virtual ~ShaderResource();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Shader; }

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	virtual bool CanBeSharedAsSameResource( const GraphicsResourceDesc& desc );

	inline ShaderManager *GetShaderManager() { return m_pShaderManager; }

	const GraphicsResourceDesc& GetDesc() const { return m_ShaderDesc; }

	bool Create();

	bool CreateShaderFromTextBuffer( stream_buffer& buffer );

	bool CreateProgramFromSource( const std::string& vertex_shader, const std::string& fragment_shader );

	friend class GraphicsResourceManager;
};


//---------------------------- inline implementations ----------------------------

//================================================================================
// GraphicsResource
//================================================================================

inline void GraphicsResource::SetState( GraphicsResourceState::Name state )
{
	boost::mutex::scoped_lock scoped_lock(m_StateChangeLock);

	m_State = state;
}


inline GraphicsResourceState::Name GraphicsResource::GetState()
{
	boost::mutex::scoped_lock scoped_lock(m_StateChangeLock);

	GraphicsResourceState::Name current_state = m_State;

	return current_state;
}


//================================================================================
// MeshResource
//================================================================================

inline std::shared_ptr<BasicMesh> MeshResource::GetMesh()
{
	if( GetState() == GraphicsResourceState::LOADED )
		return m_pMeshObject;
	else
		return std::shared_ptr<BasicMesh>();
}


inline std::shared_ptr<BasicMesh> MeshResource::GetMeshInLoading()
{
	return m_pMeshObject;
}


//---------------------------- forward declarations ----------------------------

std::shared_ptr<CustomMesh> GetCustomMesh( BasicMesh& src_mesh );
std::shared_ptr<CustomMesh> GetCustomMesh( std::shared_ptr<BasicMesh> pSrcMesh );
//std::shared_ptr<CustomMesh> GetCustomMesh( MeshHandle& src_mesh );

} // namespace amorphous



#endif /* __GraphicsResources_HPP__ */
