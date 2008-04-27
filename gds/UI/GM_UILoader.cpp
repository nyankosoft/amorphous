
#include "GM_Loader.h"

#include "Support/memory_helpers.h"
#include "Support/TextFileScanner.h"

#include "UI/ui.h"


CGM_Loader::CGM_Loader()
{
}


CGM_Loader::~CGM_Loader()
{
}


void CGM_Loader::LoadFromFile( const std::string& filename )
{
}


void CGM_Loader::MemberFunction03()
{
}


void CGM_Loader::LoadDesc( CTextFileScanner& scanner, CGM_ControlDesc& desc )
{
	int left, top, width, height;

	if( scanner.TryScanLine( "title", desc.strText ) )	return;

	if( scanner.TryScanLine( "ltwh", left, top, width, height ) )
	{
		desc.Rect.SetPositionLTWH( left, top, width, height );
		return;
	}
}


void CGM_Loader::LoadDesc( CTextFileScanner& scanner, CGM_StaticDesc& desc )
{
	string str, tag, strFilename;
	Vector3 vSide;

	if( scanner.TryScanLine( "title", desc.strText ) )	continue;

	for( ; !scanner.End(); scanner.NextLine() )
	{
		tag = scanner.GetTagString();

		if( scanner.TryScanLine( "NAME",          m_strName ) ) continue;
		else if( scanner.TryScanLine( "AABB_MAX", m_aabb.vMax ) ) continue;
		else if( scanner.TryScanLine( "AABB_MIN", m_aabb.vMin ) ) continue;

		else if( scanner.TryScanLine( "AABB", vSide ) )
		{
			// aabb represented by full lengths
			m_aabb.vMin = -vSide * 0.5f;
			m_aabb.vMax =  vSide * 0.5f;
			continue;
		}

		if( scanner.TryScanLine( "BV_TYPE", str ) )
		{
			if( str == "BVTYPE_DOT" )			bv_type = BVTYPE_DOT;
			else if( str == "BVTYPE_AABB" )		bv_type = BVTYPE_AABB;
			else if( str == "BVTYPE_OBB" )		bv_type = BVTYPE_OBB;
			else if( str == "BVTYPE_CONVEX" )	bv_type = BVTYPE_CONVEX;
			else if( str == "BVTYPE_COMPLEX" )	bv_type = BVTYPE_COMPLEX;
			continue;
		}

		if( scanner.TryScanLine( "NOCLIP", str ) )
		{
			if( str == "TRUE" )
				m_bNoClip = true;
		}

		if( scanner.TryScanLine( "BSPTREE", strFilename ) )
		{
			m_pBSPTree = new CBSPTree;
			m_pBSPTree->LoadFromFile( strFilename.c_str() );

			// output the bsp-tree to a text file
			string tree_output_file = "debug/" + strFilename + ".txt";
			m_pBSPTree->WriteToFile( tree_output_file.c_str() );
			continue;
		}


		if( scanner.GetTagString() == "TRANSLUCENT" )
			m_EntityFlag |=  BETYPE_TRANSLUCENT;

		if( scanner.GetTagString() == "INDESTRUCTIBLE" )
			m_EntityFlag |=  BETYPE_INDESTRUCTIBLE;

		if( scanner.GetTagString() == "RIGIDBODY" )
			m_EntityFlag |=  BETYPE_RIGIDBODY;

		if( scanner.TryScanLine( "3DMODEL", m_MeshProperty.m_str3DModelFileName ) )	continue;
		if( scanner.TryScanLine( "SPEC_TEX", m_MeshProperty.m_SpecTex.filename ) )	continue;

		if( scanner.TryScanLine( "LIGHTING", str ) )
		{
			if( str == "TRUE" )
				m_bLighting = true;
			else
				m_bLighting = false;
			continue;
		}

		if( tag == "END" || tag == "END\n" )
			break;

		LoadSpecificPropertiesFromFile( scanner );
	}

	Sphere sphere = m_aabb.CreateBoundingSphere();
	m_fRadius = sphere.radius;

}