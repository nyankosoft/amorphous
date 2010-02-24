#include "StaticGeometryArchiveFG.hpp"

#include "Support/StringAux.hpp"

using namespace std;


const string CStaticGeometryArchiveFG::ms_CollisionMeshTreeKey = "CollisionMeshTree";


CStaticGeometryArchiveFG::CStaticGeometryArchiveFG()
{
	m_AmbientColor	= SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	m_FogColor		= SFloatRGBAColor( 0.5f, 0.5f, 0.5f, 0.5f );

	m_FogStartDist	= 15000.0f;
	m_FarClipDist	= 50000.0f;

	m_strTileTextureFilename = "";
	m_fTileTextureBlendWeight = 0.0f;

	m_SpecularColor = SFloatRGBColor( 1.0f, 1.0f, 1.0f );
	m_fSpecularIntensity = 0.0f;
}

/*
AABB3 CStaticGeometryArchiveFG::GetAABB() const
{
	AABB3 aabb;
	aabb.Nullify();

	for( size_t i=0; i<m_vecMeshArchive.size(); i++ )
		aabb.MergeAABB( m_vecMeshArchive[i].GetAABB() );

//	aabb.MergeAABB( m_CollisionMeshArchive.GetAABB() );

	return aabb;
}
*/

/*
void CStaticGeometryArchiveFG::Scale( float factor )
{

	for( size_t i=0; i<m_vecMeshArchive.size(); i++ )
	{
		C3DMeshModelArchive& mesh_archive = m_vecMeshArchive[i];

		mesh_archive.Scale( factor );
	}

	m_SkyboxMeshArchive.Scale( factor );

	m_CollisionMeshArchive.Scale( factor );

}
*/


#include "3DMath/stream.hpp"


void CStaticGeometryArchiveFG::WriteToTextFile( const string& filename )
{
//	for( size_t i=0; i<m_vecMeshArchive.size(); i++ )
//		m_vecMeshArchive[i].WriteToTextFile( fmt_string( "%s_terrain%02d.txt", filename.c_str(), i ) );

	m_SkyboxMeshArchive.WriteToTextFile( fmt_string( "%s_skybox.txt", filename.c_str() ) );

	ofstream ofs;
	ofs.open( filename.c_str() );

	for( size_t i=0; i<m_vecMeshArchiveKey.size(); i++ )
		ofs << m_vecMeshArchiveKey[i];

	ofs << fmt_string( "fog color: %f %f %f", m_FogColor.fRed, m_FogColor.fGreen, m_FogColor.fBlue );

	ofs << m_AABB;

	ofs.close();

//	m_CollisionMeshArchive
}


void CStaticGeometryArchiveFG::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecMeshArchiveKey;

	ar & m_vecMeshGroup;

	ar & m_SkyboxMeshArchive;

	ar & m_CollisionMeshArchive;

	ar & m_CollisionMeshArchiveKey;

	ar & m_AABB;

	ar & m_AmbientColor;
	ar & m_FogColor;

	ar & m_FogStartDist;
	ar & m_FarClipDist;

	ar & m_strTileTextureFilename;
	ar & m_fTileTextureBlendWeight;

	ar & m_SpecularColor;
	ar & m_fSpecularIntensity;
}
