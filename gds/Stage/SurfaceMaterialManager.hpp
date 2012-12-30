#ifndef  __SurfaceMaterialManager_H__
#define  __SurfaceMaterialManager_H__


#include "SurfaceMaterial.hpp"
#include "SurfaceMaterialHandle.hpp"


namespace amorphous
{


class CSurfaceMaterialManager : public IArchiveObjectBase
{
	std::vector<CSurfaceMaterial> m_vecMaterial;

public:

	CSurfaceMaterialManager();
	~CSurfaceMaterialManager();

	void Release();

	/// get i-th material
	inline CSurfaceMaterial& GetSurfaceMaterial( int index );

	inline CSurfaceMaterial& GetSurfaceMaterial( CSurfaceMaterialHandle& handle );

	/// load binary format material file (*.mat)
//	bool LoadFromFile( const std::string& filename );

	bool LoadFromTextFile( const std::string& filename );

	int GetNumMaterials() { return (int)m_vecMaterial.size(); }

	void Serialize( IArchive& ar, const unsigned int version );
};


inline CSurfaceMaterial& CSurfaceMaterialManager::GetSurfaceMaterial( int index )
{
	if( 0 <= index && index < (int)m_vecMaterial.size() )
		return m_vecMaterial[index];
	else
		return CSurfaceMaterial::Null();
}


inline CSurfaceMaterial &CSurfaceMaterialManager::GetSurfaceMaterial( CSurfaceMaterialHandle& handle )
{
	if( 0 <= handle.m_iIndex )
		return m_vecMaterial[handle.m_iIndex];

	else if( handle.m_iIndex == CSurfaceMaterialHandle::INVALID_INDEX )
		return CSurfaceMaterial::Null();
	else if( handle.m_iIndex == CSurfaceMaterialHandle::UNINITIALIZED )
	{
		size_t i, num_mats = m_vecMaterial.size();
		for( i=0; i<num_mats; i++ )
		{
			if( handle.GetName() == m_vecMaterial[i].GetName() )
			{
				handle.m_iIndex = (int)i;
				return m_vecMaterial[i];
			}
		}

		/// the requested material was not found
		handle.m_iIndex = CSurfaceMaterialHandle::INVALID_INDEX;
		return CSurfaceMaterial::Null();
	}
	else
		return CSurfaceMaterial::Null();
}


} // namespace amorphous



#endif		/*  __SurfaceMaterialManager_H__  */
