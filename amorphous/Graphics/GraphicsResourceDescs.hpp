#ifndef  __GraphicsResourceDescs_H__
#define  __GraphicsResourceDescs_H__


#include <string>
#include <boost/weak_ptr.hpp>
#include "D3DHeaders.hpp"

#include "../base.hpp"
#include "fwd.hpp"
#include "SurfaceFormat.hpp"
#include "../XML/fwd.hpp"
#include "../Support/Serialization/Serialization.hpp"


namespace amorphous
{
using namespace serialization;


class GraphicsResourceType
{
public:
	enum Name
	{
		Texture,
		Mesh,
		Shader,
		Font,
		NumTypes
	};
};


class CResourceLoadingMode
{
public:
	enum Name
	{
		SYNCHRONOUS,
		ASYNCHRONOUS,
		NUM_LOADING_MODES
	};
};


class UsageFlag
{
public:
	enum Name
	{
		RENDER_TARGET          = (1 << 0),
//		ANOTHER_USAGE_FLAG     = (1 << 1),
//		YET_ANOTHER_USAGE_FLAG = (1 << 2),
	};
};


class TextureTypeFlag
{
public:
	enum Name
	{
		CUBE_MAP              = (1 << 0),
//		ANOTHER_TYPE_FLAG     = (1 << 1),
//		YET_ANOTHER_TYPE_FLAG = (1 << 2),
	};
};


class GraphicsResourceDesc : public IArchiveObjectBase
{
	bool m_IsCachedResource;

public:

	/// filled out by the system
	/// - User chooses a mode by calling GraphicsResourceHandle::Load() or GraphicsResourceHandle::LoadAsync();
	CResourceLoadingMode::Name LoadingMode;

	int LoadingPriority;

	/// Used when the resource is loaded from disk
	std::string ResourcePath;

	/// Register to the resource loading state holder if it exists for the caller thread
	bool RegisterToLoadingStateHolder;

	bool Sharable;

public:

	inline GraphicsResourceDesc();

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	virtual bool IsValid() const { return ( 0 < ResourcePath.length() ); }

	virtual bool IsDiskResource() const { return true; }

	bool IsCachedResource() const { return m_IsCachedResource; }

	virtual boost::shared_ptr<GraphicsResourceDesc> GetCopy() const = 0;

	virtual bool CanBeSharedAsSameTextureResource( const TextureResourceDesc& desc ) const { return false; }
	virtual bool CanBeSharedAsSameMeshResource( const MeshResourceDesc& desc ) const { return false; }
	virtual bool CanBeSharedAsSameShaderResource( const ShaderResourceDesc& desc ) const { return false; }

	/// Returns the score that shows how much the cache is preferable to be used as the requested resource.
	/// 0 means the cache cannot be used for a requested resource.
	/// These CanBeUsedAsXXXCache() should actually be CanUseAsXXXCache()?
	virtual int CanBeUsedAsTextureCache( const TextureResourceDesc& desc ) const { return 0; }
	virtual int CanBeUsedAsMeshCache( const MeshResourceDesc& desc ) const { return 0; } // CanUseAsMeshCache
	virtual int CanBeUsedAsShaderCache( const ShaderResourceDesc& desc ) const { return 0; }

	/// Copy attributes to the desc of the cached resource
	/// e.g., resource path
	/// - Cached texture resources maintain preloaded empty textures.
	///   A resource path (usu. a filepath) needs to be copied to it every time a new texture is loaded
	///   so that the same texture can be shared by multiple texture handles.
	virtual void UpdateCachedTextureResourceDesc( TextureResourceDesc& desc ) const {}
	virtual void UpdateCachedMeshResourceDesc( MeshResourceDesc& desc ) const {}
	virtual void UpdateCachedShaderResourceDesc( TextureResourceDesc& desc ) const {}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & (int&)LoadingMode;
		ar & LoadingPriority;
		ar & ResourcePath;
	}

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	friend class GraphicsResourceCacheManager;
};


class TextureResourceDesc : public GraphicsResourceDesc
{
public:

	/// Used together with the texture loader (TextureResourceDesc::pLoader).
	/// If you load texture from disk (i.e. file or database(CBinaryDatabase)),
	/// there is nothing you have to do with these variables (Width and Height).
	int Width;
	int Height;

	/// 0 is set to create complete mipmap chain (default: 0).
	int MipLevels;

	TextureFormat::Format Format;

	uint UsageFlags;

	uint TypeFlags;

	boost::shared_ptr<TextureFillingAlgorithm> pLoader;

public:

	TextureResourceDesc()
		:
	Width(0),
	Height(0),
	MipLevels(0),
	Format(TextureFormat::Invalid),
	UsageFlags(0),
	TypeFlags(0)
	{}

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	virtual bool IsValid() const;

	virtual boost::shared_ptr<GraphicsResourceDesc> GetCopy() const { return boost::shared_ptr<TextureResourceDesc>( new TextureResourceDesc(*this) ); }

	bool CanBeSharedAsSameTextureResource( const TextureResourceDesc& desc ) const;

	int CanBeUsedAsTextureCache( const TextureResourceDesc& desc ) const
	{
		if( Width     == desc.Width
		 && Height    == desc.Height
		 && MipLevels == desc.MipLevels
		 && Format    == desc.Format )
			return 1;
		else
			return 0;
	}

	void UpdateCachedTextureResourceDesc( TextureResourceDesc& desc ) const
	{
		desc.ResourcePath = ResourcePath;
		desc.pLoader      = pLoader;
	}

	void Serialize( IArchive& ar, const unsigned int version );

	void LoadFromXMLNode( CXMLNodeReader& reader );
};


class MeshResourceDesc : public GraphicsResourceDesc
{
public:

	MeshType::Name MeshType;	///< used by mesh object

	U32 LoadOptionFlags;

	/// Set after the mesh archive is loaded from the desc
	/// or used to create an empty mesh as cached resource
	/// in asynchronous loading.
	int NumVertices;
	int NumIndices;

	U32 VertexFormatFlags;

	boost::shared_ptr<MeshGenerator> pMeshGenerator;

	//
	// Used by Direct3D
	//

	int VertexSize;

public:

	inline MeshResourceDesc();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	bool IsValid() const;

	virtual boost::shared_ptr<GraphicsResourceDesc> GetCopy() const { return boost::shared_ptr<MeshResourceDesc>( new MeshResourceDesc(*this) ); }

	bool CanBeSharedAsSameMeshResource( const MeshResourceDesc& desc ) const
	{
		if( MeshType        == desc.MeshType
		 && LoadOptionFlags == desc.LoadOptionFlags )
		{
			if( 0 < ResourcePath.length()
			 && ResourcePath == desc.ResourcePath )
				return true;
			else
				return false;
		}
		else
			return false;
	}

	int CanBeUsedAsMeshCache( const MeshResourceDesc& desc ) const;

	void UpdateCachedTextureResourceDesc( MeshResourceDesc& desc ) const
	{
		desc.ResourcePath = ResourcePath;
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		GraphicsResourceDesc::Serialize( ar, version );

		ar & (int&)MeshType;
		ar & LoadOptionFlags;
		ar & NumVertices & NumIndices & VertexFormatFlags;
	}

	void LoadFromXMLNode( CXMLNodeReader& reader );
};


class ShaderType
{
public:
	enum Name
	{
		PROGRAMMABLE,
		VERTEX_SHADER,
		PIXEL_SHADER,
		NON_PROGRAMMABLE,
		NUM_TYPES
	};
};


class ShaderResourceDesc : public GraphicsResourceDesc
{
public:

	ShaderType::Name ShaderType;

	boost::shared_ptr<ShaderGenerator> pShaderGenerator;

	ShaderResourceDesc()
		:
	ShaderType(ShaderType::PROGRAMMABLE)
	{}

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Shader; }

	virtual bool IsValid() const;

	virtual boost::shared_ptr<GraphicsResourceDesc> GetCopy() const { return boost::shared_ptr<ShaderResourceDesc>( new ShaderResourceDesc(*this) ); }

	virtual bool CanBeSharedAsSameShaderResource( const ShaderResourceDesc& desc ) const;

	int CanBeUsedAsShaderCache( const ShaderResourceDesc& desc ) const { return 0; }

	void UpdateCachedTextureResourceDesc( MeshResourceDesc& desc ) const
	{
		desc.ResourcePath = ResourcePath;
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		GraphicsResourceDesc::Serialize( ar, version );
	}
};


//-------------------------------------- inline implementations --------------------------------------

//==================================================================================================
// GraphicsResourceDesc and its derived classes
//==================================================================================================

inline GraphicsResourceDesc::GraphicsResourceDesc()
:
m_IsCachedResource(false),
LoadingMode(CResourceLoadingMode::SYNCHRONOUS),
LoadingPriority(0),
RegisterToLoadingStateHolder(true),
Sharable(true)
{}


inline MeshResourceDesc::MeshResourceDesc()
:
MeshType(MeshType::BASIC),
LoadOptionFlags(0),
NumVertices(0),
NumIndices(0),
VertexFormatFlags(0),
VertexSize(0)
{}

} // namespace amorphous



#endif /* __GraphicsResourceDescs_H__ */
