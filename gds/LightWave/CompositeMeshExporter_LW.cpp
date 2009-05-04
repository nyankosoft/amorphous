#include "CompositeMeshExporter_LW.hpp"
#include "MassSpringExporter_LW.hpp"
#include "Support/StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "LightWave/3DMeshModelBuilder_LW.hpp"

#include "Graphics/MeshModel/ShadowVolumeMeshGenerator.hpp"

using namespace std;
using namespace boost;


//==========================================================================
// CCompositeMeshExporter_LW
//==========================================================================

void CCompositeMeshExporter_LW::SetIndicesForMeshAndMassSpringModel()
{
	CMS_MassSpringArchive& rMassSpringModel = m_CompositeMeshArchive.m_MassSpringModel;

	vector<CMS_PointProperty>& rvecControlPoint = rMassSpringModel.m_vecControlPoint;

	CMMA_VertexSet& rVertex = m_CompositeMeshArchive.m_Mesh.GetVertexSet();

	vector<CMSPointToMeshVertexMap>& rMeshVertexMap = m_CompositeMeshArchive.m_vecMeshVertexIndex;

	// error within which a point in the mass spring sim
	// and a vertex in the mesh is regarded as the same points
	float fTolerance = 0.0005f; // 0.002f;
	float fToleranceSq = fTolerance * fTolerance;

	// set indices for pinned points
	// mass spring points <-> mesh vertices
	size_t i, iNumPoints = rvecControlPoint.size();
	int j, iNumMeshVertices = rVertex.GetNumVertices();
	int iNumSamePosPoints;
	int vert_index[2];
	Vector3 vPos, vNormal;

	rMeshVertexMap.reserve( iNumPoints );

	for( i=0; i<iNumPoints; i++ )
	{
		rMeshVertexMap.push_back( CMSPointToMeshVertexMap() );

		if( 0 <= rvecControlPoint[i].iFixedPointGroup )
		{
			// pinned points - sould be excluded from the physics simulation
			rMeshVertexMap.back().Index[0] = -1;
			rMeshVertexMap.back().Index[1] = -1;
			continue;
		}

		// find a vertex in the mesh which is the closest to the control point
		vPos = rvecControlPoint[i].vBasePosition;

		iNumSamePosPoints = 0;
		for( j=0; j<iNumMeshVertices; j++ )
		{
//			if( rVertex.vecPosition[j] == vPos )
			if( Vec3LengthSq(rVertex.vecPosition[j] - vPos) < fToleranceSq )
			{
				vert_index[iNumSamePosPoints++] = j;

				if( iNumSamePosPoints == 2 )
					break;
			}
		}

		if( iNumSamePosPoints == 0 )
		{
			// no corresponding mesh vertices are found
			rMeshVertexMap.back().Index[0] = -1;
			rMeshVertexMap.back().Index[1] = -1;
			continue;
		}

		// get the default normal for the control points
		int *pNormalIndex = rvecControlPoint[i].aiNormalPointIndex;
		vNormal = Vec3Cross( rvecControlPoint[pNormalIndex[0]].vBasePosition - vPos,
			                 rvecControlPoint[pNormalIndex[1]].vBasePosition - vPos );
			Vec3Normalize( vNormal, vNormal );

		if( iNumSamePosPoints == 1 )
		{
			// only one corresponding mesh vertex
			// i.e. one side polygon is attached to the mass spring
			rMeshVertexMap.back().Index[0] = vert_index[0];
			rMeshVertexMap.back().Index[1] = -1;

            // correct normal direction if necessary
			if( Vec3Dot(vNormal,rVertex.vecNormal[vert_index[0]]) < 0 )
				swap( pNormalIndex[0], pNormalIndex[1] );
		}
		else if( iNumSamePosPoints == 2 )
		{
			// polygons for both sides are attached to the mass spring
			if( 0 < Vec3Dot(vNormal,rVertex.vecNormal[vert_index[0]]) )
			{
				rMeshVertexMap.back().Index[0] = vert_index[0];
				rMeshVertexMap.back().Index[1] = vert_index[1];
			}
			else
			{
				rMeshVertexMap.back().Index[0] = vert_index[1];
				rMeshVertexMap.back().Index[1] = vert_index[0];
			}

		}

	}
}


bool CCompositeMeshExporter_LW::Compile( const char *pcFilename )
{
//	m_OrigModel.LoadLWO2Object( pcFilename );
	m_pOrigModel = shared_ptr<CLWO2_Object>( new CLWO2_Object() );
	m_pOrigModel->LoadLWO2Object( pcFilename );

	string strToken = "CreateMesh";

	list<CLWO2_Layer>& rlstLayer = m_pOrigModel->GetLayer();
	list<CLWO2_Layer>::iterator itrLayer;

	size_t pos;

	list<CLWO2_Layer>::iterator itrMeshLayer = rlstLayer.end(), itrShadowMeshLayer = rlstLayer.end(), itrSkelegonLayer = rlstLayer.end();
	list<CLWO2_Layer>::iterator itrMSpringCollision = rlstLayer.end();

	// search the layers and process ones that have the same name as 'strTargetLayerName'
	for(itrLayer = rlstLayer.begin();
		itrLayer != rlstLayer.end();
		itrLayer++)
	{
		pos = itrLayer->GetName().find( "CreateMesh" );

		if( pos != string::npos )
			itrMeshLayer = itrLayer;

		pos = itrLayer->GetName().find( "MassSpringCollisionShape" );

		if( pos != string::npos )
			itrMSpringCollision = itrLayer;

		pos = itrLayer->GetName().find( "ShadowVolume" );

		if( pos != string::npos )
			itrShadowMeshLayer = itrLayer;

		pos = itrLayer->GetName().find( "Skeleton" );

		if( pos != string::npos )
			itrSkelegonLayer = itrLayer;
/*
		if( 
			// create mesh
			C3DMeshModelBuilder_LW mesh_builder( &m_OrigModel );
			if( mesh_builder.BuildMeshFromLayer( *itrLayer ) )
				m_CompositeMeshArchive.m_Mesh = mesh_builder.GetArchive();
			else
				assert(0);

			// create mass spring model
			CMassSpringExporter_LW mspring_exporter;
			if( mspring_exporter.CreateMassSpringModel( *itrLayer, m_OrigModel ) )
				m_CompositeMeshArchive.m_MassSpringModel = mspring_exporter.GetArchive();
			else
				assert(0);

			// create indices that are necessary to update positions
			// between mesh & mass spring physics at runtime
			SetIndicesForMeshAndMassSpringModel();
		}*/

	}

	if( itrMeshLayer == rlstLayer.end() )
		return false;

//	C3DMeshModelBuilder_LW mesh_builder( m_pOrigModel );

	shared_ptr<C3DMeshModelBuilder_LW> pLWO2Loder
		= shared_ptr<C3DMeshModelBuilder_LW>( new C3DMeshModelBuilder_LW(m_pOrigModel) );//lwo2_model_loader( m_pOrigModel );

	C3DMeshModelBuilder mesh_builder;
	mesh_builder.BuildMeshModel( pLWO2Loder );


	// load skeleton if there is any skelegon layer
//	if( itrSkelegonLayer != rlstLayer.end() )
//		mesh_builder.BuildSkeletonFromSkelegon( *itrSkelegonLayer );


	// create mesh
	if( pLWO2Loder->BuildMeshFromLayer( *itrMeshLayer ) )
		m_CompositeMeshArchive.m_Mesh = mesh_builder.GetArchive();
	else
		assert(0);

	vector<int>& rvecDestBoneIndex = pLWO2Loder->GetDestBoneIndexArray();

	// create mass spring model
	CMassSpringExporter_LW mspring_exporter;
	if( mspring_exporter.CreateMassSpringModel( *itrMeshLayer, *(m_pOrigModel.get()), rvecDestBoneIndex ) )
	{
		// a valid mass spring object has been created
		m_CompositeMeshArchive.m_OptionFlag |= CMA_CompositeMeshArchive::CMA_MASSSPRINGSIM;

		// check for collision objects for mass spring simulation
		// For now, sphere is the only supported geometry
		if( itrMSpringCollision != rlstLayer.end() )
		{
			vector<int> vecIndex;
			mspring_exporter.CreateCollisionShapes( *itrMSpringCollision, *(m_pOrigModel.get()), rvecDestBoneIndex );
		}

		// copy the archive data
		m_CompositeMeshArchive.m_MassSpringModel = mspring_exporter.GetArchive();

		// create indices that are necessary to update positions
		// between mesh & mass spring physics at runtime
		SetIndicesForMeshAndMassSpringModel();
	}
	else
	{
		LOG_PRINT( fmt_string("No mass spring model was found in '%s'", pcFilename) );
	}


/*	if( itrShadowMeshLayer != rlstLayer.end() )
	{
		C3DMeshModelBuilder shadow_mesh_builder( m_pOrigModel );

		// check if a skelegon is defined in another layer
        if( itrSkelegonLayer != rlstLayer.end() )
			shadow_mesh_builder.BuildSkeletonFromSkelegon( *itrSkelegonLayer );

		if( shadow_mesh_builder.BuildMeshFromLayer( *itrShadowMeshLayer ) )
		{
			CShadowVolumeMeshGenerator svmesh_generator;
			C3DMeshModelArchive sv_mesh_archive;

			if( svmesh_generator.GenerateShadowMesh( sv_mesh_archive, shadow_mesh_builder.GetArchive() ) )
			{
				m_CompositeMeshArchive.m_ShadowVolumeMesh = sv_mesh_archive;
				m_CompositeMeshArchive.m_OptionFlag |= CMA_CompositeMeshArchive::CMA_SHADOWVOLUMEMESH;
			}
		}
		else
			assert(0);
	}
*/

	return true;
};


bool CCompositeMeshExporter_LW::SaveToFile( const char *pcFilename )
{
	CBinaryArchive_Output output_archive( pcFilename );

	if( !(output_archive << m_CompositeMeshArchive ) )
		return false;

	return true;
}
