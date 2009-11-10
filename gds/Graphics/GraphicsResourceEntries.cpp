#include "GraphicsResourceEntries.hpp"

#include "Graphics/Direct3D9.hpp"
#include "Graphics/Direct3D/Mesh/D3DXMeshObjectBase.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/ImageArchive.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;

using namespace std;
using namespace boost;


static inline D3DXIMAGE_FILEFORMAT ArchiveImgFmt2D3DImgFmt( CImageArchive::ImageFormat img_archive_format )
{
	switch(img_archive_format)
	{
	case CImageArchive::IMGFMT_BMP24: return D3DXIFF_BMP;
	case CImageArchive::IMGFMT_BMP32: return D3DXIFF_BMP;
	case CImageArchive::IMGFMT_JPEG:  return D3DXIFF_JPG;
	case CImageArchive::IMGFMT_TGA:   return D3DXIFF_TGA;
	case CImageArchive::IMGFMT_PNG:   return D3DXIFF_PNG;
//	case CImageArchive::IMGFMT_ : return D3DXIFF_DDS,
//	case CImageArchive::IMGFMT_ : return D3DXIFF_PPM,
//	case CImageArchive::IMGFMT_ : return D3DXIFF_DIB,
//	case CImageArchive::IMGFMT_ : return D3DXIFF_HDR,       //high dynamic range formats
//	case CImageArchive::IMGFMT_ : return D3DXIFF_PFM,       //
	default: return D3DXIFF_BMP;
	}

	return D3DXIFF_BMP;
}


/**
 Graphics Resource
 - Graphics resources are not sharable if the loading modes are different
   even if all the other properties are the same.
 Rationale:
 - Sharing resources with different loading methods complicates loading process.
   - What if the user tries to load a resource(A) synchronously and the same resource(B) is being loaded asynchronously.
     1. The user wants to load (A) synchronously. i.e., no rendering before loading the resource
     2. (B) is being loaded asynchronously. If the system simply cancel the synchronous loading of (A),
	    some rendering may be done before loading (B).
		-> User's request mentioned in 1. is not satisfied.

*/


inline bool str_includes( const std::string& src, const std::string& target )
{
	if( src.find(target.c_str()) != std::string::npos )
		return true;
	else
		return false;
}



//==================================================================================================
// CGraphicsResourceEntry
//==================================================================================================

void CGraphicsResourceEntry::GetStatus( char *pDestBuffer )
{
	sprintf( pDestBuffer, "ref: %02d", m_iRefCount );

	if( GetResource() )
	{
		char buffer[512];
		GetResource()->GetStatus( buffer );
		strcat( pDestBuffer, buffer );
	}
}

