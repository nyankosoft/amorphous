#ifndef  __GraphicsResourceDescs_H__
#define  __GraphicsResourceDescs_H__


#include <string>
#include <boost/weak_ptr.hpp>

#include "fwd.h"
#include "GraphicsResource.h"
#include "Support/Serialization/Serialization.h"
using namespace GameLib1::Serialization;


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


class CGraphicsResourceDesc : public IArchiveObjectBase
{
public:

	/// filled out by the system
	/// - User chooses a mode by calling CGraphicsResourceHandle::Load() or CGraphicsResourceHandle::LoadAsync();
	CResourceLoadingMode::Name LoadingMode;

	int LoadingPriority;

	/// Used when the resource is loaded from disk
	std::string ResourcePath;

public:

	inline CGraphicsResourceDesc();

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	virtual bool IsDiskResource() const { return true; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & (int&)LoadingMode;
		ar & LoadingPriority;
		ar & ResourcePath;
	}
};


class CMeshResourceDesc : public CGraphicsResourceDesc
{
public:

	CMeshType::Name MeshType;	///< used by mesh object

	U32 LoadOptionFlags;

public:

	inline CMeshResourceDesc();

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Mesh; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		CGraphicsResourceDesc::Serialize( ar, version );

		ar & (int&)MeshType;
		ar & LoadOptionFlags;
	}
};


class CTextureResourceDesc : public CGraphicsResourceDesc
{
public:

	int Width;
	int Height;
	int MipLevels; ///< 0 is set to create complete mipmap chain. (default: 0)

	TextureFormat::Format Format;

	boost::weak_ptr<CTextureLoader> pLoader;

public:

	CTextureResourceDesc()
		:
	Width(0),
	Height(0),
	MipLevels(0),
	Format(TextureFormat::Invalid)
	{}

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		CGraphicsResourceDesc::Serialize( ar, version );

		ar & Width & Height & MipLevels;
		ar & (int&)Format;
	}
};


class CShaderResourceDesc : public CGraphicsResourceDesc
{
public:

	virtual GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Shader; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		CGraphicsResourceDesc::Serialize( ar, version );
	}
};

//-------------------------------------- inline implementations --------------------------------------



//==================================================================================================
// CGraphicsResourceDesc and its derived classes
//==================================================================================================

inline CGraphicsResourceDesc::CGraphicsResourceDesc()
:
LoadingMode(CResourceLoadingMode::SYNCHRONOUS),
LoadingPriority(0)
{}


inline CMeshResourceDesc::CMeshResourceDesc()
	:
MeshType(CMeshType::BASIC)
{}


#endif  __GraphicsResourceDescs_H__
