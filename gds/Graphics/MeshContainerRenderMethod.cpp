#include "MeshContainerRenderMethod.hpp"
#include "Support/Serialization/Serialization_BoostSmartPtr.hpp"
#include "XML.hpp"
#include "XML/LoadFromXMLNode_3DMath.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "Graphics/D3DXMeshObjectBase.hpp"


using namespace std;
using namespace boost;


class CCubeTextureParamsWriter : public CShaderParamsWriter
{
	int m_CubeTexIndex;
	LPDIRECT3DCUBETEXTURE9 m_pCubeTexture;

public:

//	CCubeTextureParamsWriter( int stage, CTextureHandle& cube_texture );
	CCubeTextureParamsWriter( int cube_tex_index = 0, LPDIRECT3DCUBETEXTURE9 pCubeTexture = NULL )
		:
	m_CubeTexIndex(cube_tex_index),
	m_pCubeTexture(pCubeTexture)
	{}

	void SetCubeTexture( int cube_tex_index, LPDIRECT3DCUBETEXTURE9 pCubeTexture )
	{
		m_CubeTexIndex = cube_tex_index;
		m_pCubeTexture = pCubeTexture;
	}

	void UpdateShaderParams( CShaderManager& rShaderMgr )
	{
		rShaderMgr.SetCubeTexture( m_CubeTexIndex, m_pCubeTexture );
	}
};


/*
Defines a method to render the meshes in a mesh container node
XML sample for MeshContainerNode CGameItem::m_MeshContainerRootNode


- A simple render method to render a single mesh with a single shader technique

<MeshRenderMethod>
	<MeshContainerNodeRenderMethod>
		<MeshContainerRenderMethod>
			<MeshRenderMethod lod="0">
				<ShaderFilepath>simple.fx</ShaderFilepath>
				<Technique>Simple</Technique>
			</MeshRenderMethod>
		</MeshContainerRenderMethod>
	</MeshContainerNodeRenderMethod>
</MeshRenderMethod>

- render method to render a mesh with different shaders and shader techniques for each subset
  - Note that <SubsetRenderMethod> is used instead of <MeshRenderMethod>,
    and <Name> nodes are added at every <SubsetRenderMethod> node
<MeshRenderMethod>
	<MeshContainerNodeRenderMethod>
		<MeshContainerRenderMethod>
			<SubsetRenderMethod lo>
				<Name>RubberParts</Name> << name of the subset to render with this technique
				<ShaderFilepath>Rubber.fx</ShaderFilepath>
				<Technique>Rubber</Technique>
			</SubsetRenderMethod>
			<SubsetRenderMethod>
				<Name>MetallicParts</Name>
				<ShaderFilepath>Metal.fx</ShaderFilepath>
				<Technique>Metal</Technique>
			</SubsetRenderMethod>
		</MeshContainerRenderMethod>
	</MeshContainerNodeRenderMethod>
</MeshRenderMethod>

<MeshRenderMethod>
	<MeshNode>
		<!-- Mesh node contains one or more 'mesh units'-->
		<MeshUnit>
			<MeshContainer>
				<SubsetsToRender>
					<Name>Eyes</Name>
					<Name>Cloths</Name>
				<SubsetsToRender>
				<Shader>
			</MeshContainer>
		</MeshUnit>
		<MeshUnit>
			<MeshContainer>
				
			</MeshContainer>
			<LocalPose>
				<Heading>60</Heading>
				<Pitch>60</Pitch>
				<Position>0 0 0</Position>
			</LocalPose>
		</MeshUnit>

		<!-- Child mesh noes -->
		<MeshNode>

		</MeshNode>
	</MeshNode>
</MeshRenderMethod>


*/



/*
--------------------------------------------------
 XML format for shader technique
--------------------------------------------------

no LOD, single technique

<SingleShaderTechnique>HSLighting</SingleShaderTechnique>


no LOD, multiple materials (Not implemented yet)

<ShaderTechniques>
	<Material index="0">EnvMap</Material>
	<Material index="1">HSLighting</Material>
	<Material index="2">HSLighting</Material>
</ShaderTechniques>


LOD, multiple materials (Not implemented yet)

<ShaderTechniques>
	<Material index="0">
		<High>EnvMap</High>
		<Mid>EnvMap</Mid>
		<Low>Default</Low>
	</Material>
	<Material index="1">
		<High>HSLighting</High>
		<Mid>HSLighting</Mid>
		<Low>Default</Low>
	</Material>
	<Material index="2">
		<High>HSLighting</High>
		<Mid>HSLighting</Mid>
		<Low>Default</Low>
	</Material>
</ShaderTechniques>

*/




//  old text desc file (deprecated. Use the XML format instead)
/*

How to load from text file?

text file
MeshContainer.MeshObjectFilename	plane.msh

begin: MeshContainer
	filename	models/plane.msh
	extra_tex	0 textures/specular.msh
	shader_tech 0 VertexBlend_1HSDL_Specular_CTS  SingleHSDL_Specular_CTS     VertLights
	shader_tech 1 SpecularMembrane_1HSDL_CTS      SpecularMembrane_1HSDL_CTS  VertLights
end: MeshContainer
*/


bool CSubsetRenderMethod::Load()
{
	return m_Shader.Load( m_ShaderFilepath );
}


void CSubsetRenderMethod::LoadFromXMLNode( CXMLNodeReader& reader )
{
	reader.GetChildElementTextContent( "ShaderFilepath", m_ShaderFilepath );

	string tech_name;
	reader.GetChildElementTextContent( "Technique", tech_name );
	m_Technique.SetTechniqueName( tech_name.c_str() );
}


void CSubsetRenderMethod::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_ShaderFilepath;
	ar & m_Technique;
}


static int sg_iPrevShaderManagerID = -1;


void CMeshContainerRenderMethod::RenderMeshContainer( CMeshObjectContainer& mesh_container,
													 const Matrix34& world_transform,
													 std::vector< boost::shared_ptr<CShaderParamsWriter> >& vecpShaderParamsWriter )
{
	shared_ptr<CD3DXMeshObjectBase> pMesh
		= mesh_container.m_MeshObjectHandle.GetMesh();

	if( !pMesh )
		return;

	uint lod_index = m_LODIndex;
	if( 0 < m_vecMeshRenderMethod.size() )
	{
		// Render with a single shader & a single technique 
		CShaderManager *pShaderMgr = m_vecMeshRenderMethod[lod_index].m_Shader.GetShaderManager();
		if( pShaderMgr )
		{
			pShaderMgr->SetWorldTransform( world_transform );

			if( true /*sg_iPrevShaderManagerID != pShaderMgr->GetShaderManagerID()*/ )
			{
				for( size_t i=0; i<vecpShaderParamsWriter.size(); i++ )
					vecpShaderParamsWriter[i]->UpdateShaderParams( *pShaderMgr );
			}

			pShaderMgr->SetTechnique( m_vecMeshRenderMethod[lod_index].m_Technique );
			if( m_vecIndicesOfSubsetsToRender.size() == 0 )
			{
				// Render all the mesh subsets with a single shader & a single technique 
				pMesh->Render( *pShaderMgr );
			}
			else
			{
				// render only the specified subsets with a single shader & a single technique
				for( int i=0; i<(int)m_vecIndicesOfSubsetsToRender.size(); i++ )
				{
					pMesh->RenderSubset( *pShaderMgr, m_vecIndicesOfSubsetsToRender[i] );
				}
			}
		}
	}
	else
	{
		// set different shaders / techniques for each subset
		uint shader_lod = m_LODIndex;
		const int num_subsets = pMesh->GetNumMaterials();
		std::vector<int> *pvecIndicesOfSubsetsToRender = NULL;
		if( m_vecIndicesOfSubsetsToRender.size() == 0 )
		{
			// render all the subsets
			// - create the full indices list
			for( int j=(int)m_vecFullIndicesOfSubsets.size(); j<num_subsets; j++ )
				m_vecFullIndicesOfSubsets.push_back( j );
			pvecIndicesOfSubsetsToRender = &m_vecFullIndicesOfSubsets;
		}
		else
		{
			pvecIndicesOfSubsetsToRender = &m_vecIndicesOfSubsetsToRender;
		}

//		for( i=0; i<num_subsets; i++ )
		for( size_t i=0; i<pvecIndicesOfSubsetsToRender->size(); i++ )
		{
			int index = (*pvecIndicesOfSubsetsToRender)[i];

			map< string, CSubsetRenderMethod >::iterator itr
				= m_vecSubsetNameToRenderMethod[shader_lod].find( pMesh->GetMaterial(index).Name );

			if( itr == m_vecSubsetNameToRenderMethod[shader_lod].end() )
				continue;

			CShaderManager *pShaderMgr = (*itr).second.m_Shader.GetShaderManager();
			if( !pShaderMgr )
				continue;

			pShaderMgr->SetTechnique( (*itr).second.m_Technique );

			pMesh->RenderSubset( *pShaderMgr, index );
		}
	}
}


bool CMeshContainerRenderMethod::LoadRenderMethodResources()
{
	for( size_t i=0; i<m_vecMeshRenderMethod.size(); i++ )
		m_vecMeshRenderMethod[i].Load();

	for( size_t i=0; i<m_vecSubsetNameToRenderMethod.size(); i++ )
	{
		map< string, CSubsetRenderMethod >::iterator itr;
		for( itr = m_vecSubsetNameToRenderMethod[i].begin();
			 itr != m_vecSubsetNameToRenderMethod[i].end();
			 itr++ )
		{
			(*itr).second.Load();
		}
	}

	return true;
}


void CMeshContainerRenderMethod::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecIndicesOfSubsetsToRender;
	ar & m_LODIndex;
	ar & m_vecSubsetNameToRenderMethod;
	ar & m_vecMeshRenderMethod;
}


void CMeshContainerRenderMethod::LoadFromXMLNode( CXMLNodeReader& reader )
{
//	m_vecSubsetNameToRenderMethod.resize( 1 );

//	m_MeshRenderMethod.LoadFromXMLNode( reader );

	vector<CXMLNodeReader> vecMeshRenderMethod = reader.GetImmediateChildren( "MeshRenderMethod" );
	m_vecMeshRenderMethod.resize( vecMeshRenderMethod.size() );
	for( size_t i=0; i<vecMeshRenderMethod.size(); i++ )
	{
		m_vecMeshRenderMethod[i].LoadFromXMLNode( vecMeshRenderMethod[i] );
	}

	vector<CXMLNodeReader> vecSubsetRenderMethod = reader.GetImmediateChildren( "SubsetRenderMethod" );
	for( size_t i=0; i<vecSubsetRenderMethod.size(); i++ )
	{
		int lod = 0;
		string lod_str = vecSubsetRenderMethod[i].GetAttributeText( "lod" );
		if( lod_str == "" )
			lod = 0;
		else
			lod = to_int(lod_str);

		clamp( lod, 0, 7 );

		while( (int)m_vecSubsetNameToRenderMethod.size() <= lod )
			m_vecSubsetNameToRenderMethod.push_back( map< string, CSubsetRenderMethod >() );

		string subset_name = "";
		vecSubsetRenderMethod[i].GetChildElementTextContent( "Name", subset_name );

//		shared_ptr<CSubsetRenderMethod> pSubsetRenderMethod = ;
		CSubsetRenderMethod subset_render_method;
		subset_render_method.LoadFromXMLNode( vecSubsetRenderMethod[i] );
		m_vecSubsetNameToRenderMethod[lod][subset_name] = subset_render_method;
	}
}



CMeshContainerNodeRenderMethod::CMeshContainerNodeRenderMethod()
{
}


void CMeshContainerNodeRenderMethod::RenderMeshContainerNode( CMeshContainerNode& node,
															  std::vector< boost::shared_ptr<CShaderParamsWriter> >& vecpShaderParamsWriter )
{
	const int num_mesh_containers = node.GetNumMeshContainers();
	for( int i=0; i<num_mesh_containers; i++ )
	{
		Matrix34 transform = node.GetMeshContainerWorldTransform( i );

		if( (int)m_vecpContainerRenderMethod.size() <= i )
			break;

		m_vecpContainerRenderMethod[i]->RenderMeshContainer(
			*(node.MeshContainer(i).get()),
			transform,
			vecpShaderParamsWriter
			);
	}

	for( int i=0; i<(int)m_vecpChild.size(); i++ )
	{
		if( node.GetNumChildren() <= (int)i )
			break;

		m_vecpChild[i]->RenderMeshContainerNode( *(node.Child( i ).get()), vecpShaderParamsWriter );
	}
}


/// Load shaders, etc.
bool CMeshContainerNodeRenderMethod::LoadRenderMethodResources()
{
	if( m_vecpContainerRenderMethod.size() == 0
	 && m_vecpChild.size() == 0 )
	{
		// nothing to load
		return false;
	}

	bool loaded = false;
	bool loaded_so_far = true;
	for( size_t i=0; i<m_vecpContainerRenderMethod.size(); i++ )
	{
		loaded = m_vecpContainerRenderMethod[i]->LoadRenderMethodResources();
		if( !loaded )
			loaded_so_far = false;
	}

	// recursively load the meshses of the child nodes
	for( size_t i=0; i<m_vecpChild.size(); i++ )
	{
		loaded = m_vecpChild[i]->LoadRenderMethodResources();
		if( !loaded )
			loaded_so_far = false;
	}

	return loaded_so_far;
}


void CMeshContainerNodeRenderMethod::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecpContainerRenderMethod;
	ar & m_vecpChild;
}



/// Use "MeshNode" as the element name of the argument 'reader'
void CMeshContainerNodeRenderMethod::LoadFromXMLNode( CXMLNodeReader& reader )
{
	vector<CXMLNodeReader> vecReader = reader.GetImmediateChildren( "MeshContainerRenderMethod" );
	m_vecpContainerRenderMethod.resize( vecReader.size() );
	for( size_t i=0; i<vecReader.size(); i++ )
	{
		m_vecpContainerRenderMethod[i] = shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );
		m_vecpContainerRenderMethod[i]->LoadFromXMLNode( vecReader[i] );
	}

	vector<CXMLNodeReader> vecChild = reader.GetImmediateChildren( "MeshContainerNodeRenderMethod" );
	m_vecpChild.resize( vecChild.size() );
	for( size_t i=0; i<vecChild.size(); i++ )
	{
		m_vecpChild[i] = shared_ptr<CMeshContainerNodeRenderMethod>( new CMeshContainerNodeRenderMethod() ); 
		m_vecpChild[i]->LoadFromXMLNode( vecChild[i] );
	}
}
