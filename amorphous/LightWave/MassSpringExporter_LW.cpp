#include "MassSpringExporter_LW.hpp"
#include "amorphous/LightWave/3DMeshModelBuilder_LW.hpp"
#include "amorphous/PhysicsSim/MassSpringSim.hpp"
//#include "assert.h"


namespace amorphous
{

using namespace std;


/*
struct STempSpringInfo
{
	float fStrength;
};
*/

CMassSpringExporter_LW::CMassSpringExporter_LW()
{
}

CMassSpringExporter_LW::~CMassSpringExporter_LW()
{
}


void CMassSpringExporter_LW::BuildMassSpringModel( const char *pcFilename )
{
/*	m_OrigModel.LoadLWO2Object( pcFilename );


	string strTargetLayerName = "LYR_MassSpringMesh";

	list<LWO2_Layer>& rlstLayer = m_OrigModel.GetLayer();
	list<LWO2_Layer>::iterator itrLayer;

	// search the layers and process ones that have the same name as 'strTargetLayerName'
	for(itrLayer = rlstLayer.begin();
		itrLayer != rlstLayer.end();
		itrLayer++)
	{
		if( itrLayer->GetName() == strTargetLayerName )
			CreateMassSpringModel(*itrLayer);
	}
*/
}


bool CMassSpringExporter_LW::GetPointInfo( vector<STempPointInfo>& rvecPointInfo, LWO2_Layer& rLayer )
{
	vector<LWO2_PointSelectionSet>& rvecPointSet = rLayer.GetPointSelectionSet();

	size_t i, iNumPointSelectionSets = rvecPointSet.size();
	for( i=0; i<iNumPointSelectionSets; i++ )
	{
		if( rvecPointSet[i].GetName() == "FixedPoints" )
		{
			int j, index, iNumPoints = rvecPointSet[i].GetNumPoints();
			for( j=0; j<iNumPoints; j++ )
			{
				// mark vertices as pinned control points in the mass spring grid
				index = rvecPointSet[i].GetPointIndex()[j];
				rvecPointInfo[index].bIsFixed = true;
				rvecPointInfo[index].iFixedPointGroup = 0;
			}
			return true;
		}
	}

	// no points are marked to be pinned
	return false;
}


void CMassSpringExporter_LW::SetMass( vector<STempPointInfo>& rvecPointInfo, LWO2_Layer& rLayer )
{
	size_t j, num_points = rvecPointInfo.size();

	vector<LWO2_WeightMap>& rvecWeightMap = rLayer.GetVertexWeightMap();

	// find a vertex map named "CPMass", which is supposed to include mass info for control points
	size_t i, num_weightmaps = rvecWeightMap.size();
	for( i=0; i<num_weightmaps; i++ )
	{
		if( rvecWeightMap[i].GetName() == "CPMass" )
			break;
	}

	if( i < num_weightmaps )
	{
		LWO2_WeightMap& rWeightMap = rvecWeightMap[i];

		float fWeight;
		for( j=0; j<num_points; j++ )
		{
			if( rWeightMap.FindWeight( (UINT4)j, fWeight ) )
			{
				rvecPointInfo[j].fMass = fWeight;
			}
		}
	}

	// a vertex colormap named "CPMass" also represents mass info
	vector<LWO2_VertexColorMap>& rvecVertColorMap = rLayer.GetVertexColorMap();
	size_t num_colormaps = rvecVertColorMap.size();
	for( i=0; i<num_colormaps; i++ )
	{
		if( rvecVertColorMap[i].GetName() == "CPMass" )
			break;
	}

	if( i < num_colormaps )
	{
        LWO2_VertexColorMap& rVertColorMap = rvecVertColorMap[i];
		SFloatRGBAColor color;
		for( j=0; j<num_points; j++ )
		{
			if( rVertColorMap.FindVertexColor( (UINT4)j, color ) )
			{
				rvecPointInfo[j].fMass = color.alpha;
			}
		}
	}
}


void CMassSpringExporter_LW::SetSpringInfo( vector<STempSpringInfo>& rvecSpringInfo, LWO2_Object& rOrigObject )
{
	string strTag;
	size_t pos;
	int i, iNumTags = rOrigObject.GetNumTagStrings();
	float fSpring, fDamper;
	char acStr1[256], acStr2[256];

	for( i=0; i<iNumTags; i++ )
	{
		strTag = rOrigObject.GetTagString( i );

		pos = strTag.find( "Spring" );

		if( pos == string::npos )
			continue;

		// found a spring info
		STempSpringInfo spring_info;
		spring_info.iTagIndex = i;
		sscanf( strTag.c_str() + pos, "%s %f %s %f", acStr1, &fSpring, acStr2, &fDamper );

		spring_info.fSpring = fSpring;

//		if( strcmp( acStr2, "Damper" ) == 0 )
//			spring_info.fDamper = fDamper;

		// save the first part of the string as a spring name (mainly for debugging)
		sscanf( strTag.c_str(), "%s", acStr1 );
		spring_info.strName = acStr1;

		rvecSpringInfo.push_back( spring_info );
	}
}


void CMassSpringExporter_LW::SetPointsForNormalCalculation( vector<CMS_PointProperty>& rvecControlPoint,
		                                                    vector<STempPointInfo>& rvecPointInfo)
{
	int i, iNumOrigPoints = rvecPointInfo.size();

	int j, k, iNumAdjacentPoints;

	Vector3 v0, v1;
	float theta, best_theta;
	int best_pair[2];

	for( i=0; i<iNumOrigPoints; i++ )
	{
		if( rvecPointInfo[i].iNewIndex < 0 )
			continue;	// not used as a control point in the mass spring simulation

		best_theta = 0;
		best_pair[0] = -1;

		CMS_PointProperty& rCPoint = rvecControlPoint[ rvecPointInfo[i].iNewIndex ];

		vector<int>& rvecIndex = rvecPointInfo[i].veciAdjacentPointIndex;
		iNumAdjacentPoints = rvecPointInfo[i].veciAdjacentPointIndex.size();

		if( iNumAdjacentPoints < 2 )
			continue;

		for( j=0; j<iNumAdjacentPoints; j++ )
		{
			for( k=j+1; k<iNumAdjacentPoints; k++ )
			{
//				v0 = rvecControlPoint[rvecIndex[j]].vBasePosition - rCPoint.vBasePosition;
//				v1 = rvecControlPoint[rvecIndex[k]].vBasePosition - rCPoint.vBasePosition;
				v0 = rvecControlPoint[rvecPointInfo[rvecIndex[j]].iNewIndex].vBasePosition - rCPoint.vBasePosition;
				v1 = rvecControlPoint[rvecPointInfo[rvecIndex[k]].iNewIndex].vBasePosition - rCPoint.vBasePosition;
				Vec3Normalize( v0, v0 );
				Vec3Normalize( v1, v1 );
				theta = (float)acos( Vec3Dot( v0, v1 ) );
				if( fabsf(theta - 3.141592f * 0.5f) < fabsf(best_theta - 3.141592f * 0.5f) )
				{
					best_theta = theta;
					best_pair[0] = j;
					best_pair[1] = k;
				}
			}
		}

		if( best_pair[0] == -1 )
			best_pair[0] = best_pair[1] = 0;

//		rCPoint.aiNormalPointIndex[0] = rvecIndex[best_pair[0]];
//		rCPoint.aiNormalPointIndex[1] = rvecIndex[best_pair[1]];
		rCPoint.aiNormalPointIndex[0] = rvecPointInfo[ rvecIndex[best_pair[0]] ].iNewIndex;
		rCPoint.aiNormalPointIndex[1] = rvecPointInfo[ rvecIndex[best_pair[1]] ].iNewIndex;
	}
}


void CMassSpringExporter_LW::SetIndicesToFixedPoints()
{
	vector<CMS_PointProperty>& rvecPointProperty = m_MassSpringArchive.m_vecControlPoint;
	int i, iNumPoints = m_MassSpringArchive.m_vecControlPoint.size();

	m_MassSpringArchive.m_veciFixedPointIndex.clear();
	m_MassSpringArchive.m_veciFixedPointIndex.reserve( iNumPoints / 2.0f );

	for( i=0; i<iNumPoints; i++ )
	{
		if( 0 <= rvecPointProperty[i].iFixedPointGroup )
			m_MassSpringArchive.m_veciFixedPointIndex.push_back( i );
	}
}


void CMassSpringExporter_LW::SetFixedGroupIndex( LWO2_Layer& rLayer,
						                         LWO2_Object& rOrigObject,
												 vector<STempPointInfo>& rvecPointInfo,
						                         vector<int>& rvecDestBoneIndex )
{
	int j;
	int pnt_index, bone_index;
	float fWeight;

	vector<LWO2_WeightMap>& rvecWeightMap = rLayer.GetVertexWeightMap();
	int num_weightmaps = rvecWeightMap.size();

	int num_points = rvecPointInfo.size();

	for( pnt_index=0; pnt_index<num_points; pnt_index++ )
	{
		for( j=0; j<num_weightmaps; j++ )
		{
			if( rvecWeightMap[j].FindWeight( pnt_index, fWeight ) )
			{
				bone_index = rOrigObject.GetBoneIndexForWeightMap( rvecWeightMap[j], rLayer );

				if( bone_index < 0 )
					continue;	// the weightmap is not attached to any bone

				rvecPointInfo[pnt_index].bIsFixed = true;
				rvecPointInfo[pnt_index].iFixedPointGroup = rvecDestBoneIndex[ bone_index ];
			}
		}
	}
}


/// \pre rTwoPointPolygon is a 2-point polygon
inline float GetEdgeLength( LWO2_Face& rTwoPointPolygon, vector<Vector3>& rvecVertex )
{
	return Vec3Length( rvecVertex[ rTwoPointPolygon.GetVertexIndex()[1] ] - rvecVertex[ rTwoPointPolygon.GetVertexIndex()[0] ] );
}


inline int CMassSpringExporter_LW::FindSpringInfo( int iPartIndex, vector<STempSpringInfo>& rvecSpringInfo )
{
	if( iPartIndex < 0 )
		return -1;

	for( size_t i=0; i<rvecSpringInfo.size(); i++ )
	{
		if( iPartIndex == rvecSpringInfo[i].iTagIndex )
			return i;
	}

	return -1;
}


bool CMassSpringExporter_LW::CreateMassSpringModel( LWO2_Layer& rLayer,
												    LWO2_Object& rOrigObject,
													vector<int>& rvecDestBoneIndex )
{
	vector<STempSpringInfo> vecSpringInfo;
	SetSpringInfo( vecSpringInfo, rOrigObject );

	int iNumOrigVertices = rLayer.GetVertex().size();
	vector<Vector3>& rvecvOrigPoint = rLayer.GetVertex();

	int iNumOrigPolygons = rLayer.GetFace().size();
	vector<LWO2_Face>& rvecOrigPolygon = rLayer.GetFace();

	// used to check if a vertex has been already registered as a control point in the mass spring grid
	// each i-th entry in 'vecPointInfo' corresponds to an i-th vertex in 'rLayer'
	vector<STempPointInfo> vecPointInfo;
	vecPointInfo.resize( iNumOrigVertices );

	GetPointInfo( vecPointInfo, rLayer );

	SetFixedGroupIndex( rLayer, rOrigObject, vecPointInfo, rvecDestBoneIndex );

	SetMass( vecPointInfo, rLayer );

//	GetPolygonInfo( vecPolygontInfo, rLayer );

	vector<CMS_PointProperty> vecControlPoint;

	int i, j, k, index;
	int pnt_index[2];
	int spring_index;
	for( i=0; i<iNumOrigPolygons; i++ )
	{
		LWO2_Face& rOrigPolygon = rvecOrigPolygon[i];

		if( rOrigPolygon.GetNumPoints() != 2 )
			continue;

		// found a 2-point polygon
		// create control points and a spring

		for( j=0; j<2; j++ )
		{
			pnt_index[j] = rOrigPolygon.GetVertexIndex()[j];

			if( 0 <= vecPointInfo[pnt_index[j]].iNewIndex )
			{
				// this vertex has been already registered
			}
			else
			{
				// record that this vertex has been registered
				vecPointInfo[pnt_index[j]].iNewIndex = vecControlPoint.size(); //i;

				vecControlPoint.push_back( CMS_PointProperty() );

				// set base position
				vecControlPoint.back().vBasePosition = rvecvOrigPoint[pnt_index[j]];

				// set mass
				vecControlPoint.back().fMass = vecPointInfo[pnt_index[j]].fMass;

				if( vecPointInfo[pnt_index[j]].bIsFixed )
					vecControlPoint.back().iFixedPointGroup = vecPointInfo[pnt_index[j]].iFixedPointGroup;
//					vecControlPoint.back().iFixedPointGroup = 0;
			}
		}

//		vecPointInfo[pnt_index[0]].veciAdjacentPointIndex.push_back( vecPointInfo[pnt_index[1]].iNewIndex );
//		vecPointInfo[pnt_index[1]].veciAdjacentPointIndex.push_back( vecPointInfo[pnt_index[0]].iNewIndex );
		vecPointInfo[pnt_index[0]].veciAdjacentPointIndex.push_back( pnt_index[1] );
		vecPointInfo[pnt_index[1]].veciAdjacentPointIndex.push_back( pnt_index[0] );

		CMS_PointProperty* pCPoint[2];
//		pCPoint[0] = &vecControlPoint[ vecControlPoint.size() - 2 ];
//		pCPoint[1] = &vecControlPoint.back();
		pCPoint[0] = &vecControlPoint[ vecPointInfo[pnt_index[0]].iNewIndex ];
		pCPoint[1] = &vecControlPoint[ vecPointInfo[pnt_index[1]].iNewIndex ];

		if( pCPoint[0]->iNumSprings < CMS_PointProperty::NUM_MAX_SPRINGS_PER_VERTEX
			&& pCPoint[0]->iFixedPointGroup < 0 )
			k = 0;
		else if( pCPoint[1]->iNumSprings < CMS_PointProperty::NUM_MAX_SPRINGS_PER_VERTEX
			&& pCPoint[1]->iFixedPointGroup < 0 )
			k = 1;
		else
			k = -1;

		if( k < 0 )
			continue;

		index = pCPoint[k]->iNumSprings;
		pCPoint[k]->aiPointIndex[index]   = vecPointInfo[pnt_index[(k+1)%2]].iNewIndex;
  		pCPoint[k]->afSpringLength[index] = GetEdgeLength(rOrigPolygon,rvecvOrigPoint);

		// find the corresponding spring and set spring strength
		spring_index = FindSpringInfo( rOrigPolygon.GetPartIndex(), vecSpringInfo );
		if( 0 <= spring_index )
            pCPoint[k]->afSpring[index] = vecSpringInfo[spring_index].fSpring;
		else
			pCPoint[k]->afSpring[index] = 300.0f;

		pCPoint[k]->iNumSprings++;
	}

	if( vecControlPoint.size() == 0 )
		return false;	// no 2 point polygons - cannot create mass spring model

	SetPointsForNormalCalculation( vecControlPoint, vecPointInfo );

	int iNumControlPoints = vecControlPoint.size();
	m_MassSpringArchive.m_vecControlPoint.resize(iNumControlPoints);
	for( i=0; i<iNumControlPoints; i++ )
		m_MassSpringArchive.m_vecControlPoint[i] = vecControlPoint[i];

	SetIndicesToFixedPoints();

	return true;

}



void CMassSpringExporter_LW::SetFixedGroupIndex( vector<STempPolygonGroup>& rvecPolygonGroup,
						                         LWO2_Layer& rLayer,
						                         LWO2_Object& rOrigObject,
						                         vector<int>& rvecDestBoneIndex )
{
	int i, j, k, num_groups;
	int pnt_index, bone_index;
	float fWeight;

	vector<LWO2_WeightMap>& rvecWeightMap = rLayer.GetVertexWeightMap();
	int num_weightmaps = rvecWeightMap.size();
	num_groups = rvecPolygonGroup.size();

	for( i=0; i<num_groups; i++ )
	{
		pnt_index = rvecPolygonGroup[i].vecIndex[0];

		for( j=0; j<num_weightmaps; j++ )
		{
			if( rvecWeightMap[j].FindWeight( pnt_index, fWeight ) )
			{
				bone_index = rOrigObject.GetBoneIndexForWeightMap( rvecWeightMap[j], rLayer );

				rvecPolygonGroup[i].iDestBoneIndex = rvecDestBoneIndex[ bone_index ];
			}
/*
			num_maps = rvecWeightMap[j].GetNumMaps();
			for( k=0; k<num_maps; k++ )
			{
				rvecWeightMap[j].GetWeightMap( k, weight_pnt_index, fWeight );
				if( weight_pnt_index == pnt_index )
				{
					bone_index = rOrigObject.GetBoneIndexForWeightMap( rvecWeightMap[j], rLayer );

					rvecPolygonGroup[i].iDestBoneIndex = rvecDestBoneIndex[ bone_index ];
				}
			}*/
		}
	}
}

void CMassSpringExporter_LW::GroupConnectedPolygons_r( STempPolygonGroup& rGroup,
													   vector<LWO2_Face>& rvecPolygon,
													   vector<bool>& Selected )
{
	int i, num_pols = (int)rvecPolygon.size();
	int j, k, num_verts0, num_verts1;

	LWO2_Face& rPolygon0 = rvecPolygon[rGroup.vecIndex.back()];
	num_verts0 = rPolygon0.GetNumPoints();

	for( i=0; i<num_pols; i++ )
	{
		if( Selected[i] )
			continue;

		LWO2_Face& rPolygon1 = rvecPolygon[i];
		num_verts1 = rPolygon1.GetNumPoints();
		for( j=0; j<num_verts0; j++ )
		{
			for( k=0; k<num_verts1; k++ )
			{
				if( rPolygon0.GetVertexIndex(j) == rPolygon1.GetVertexIndex(k) )
				{
					Selected[i] = true;
					rGroup.vecIndex.push_back(i);
					GroupConnectedPolygons_r( rGroup, rvecPolygon, Selected );
					break;
				}
			}
		}
	}
}


void CMassSpringExporter_LW::GroupConnectedPolygons( LWO2_Layer& rLayer,
													 vector<STempPolygonGroup>& rvecPolygonGroup )
{
	vector<LWO2_Face>& rvecPolygon = rLayer.GetFace();
	int i, num_pols = rvecPolygon.size();

	vector<bool> Selected;
	Selected.resize( num_pols, false );

	for( i=0; i<num_pols; i++ )
	{
		if( Selected[i] )
			continue;

		Selected[i] = true;

		// collect connected polygons
		rvecPolygonGroup.push_back( STempPolygonGroup() );

		rvecPolygonGroup.back().vecIndex.push_back( i );

		GroupConnectedPolygons_r( rvecPolygonGroup.back(), rvecPolygon, Selected );
	}


/*	vector<LWO2_Face>& rvecPolygon = rLayer.GetFace();
	int i, num_pols = rvecPolygon.size();
	int j, k, m, num_verts0, num_verts1;

	vector<bool> IsSelected;
	IsSelected.resize( num_pols, false );

	for( i=0; i<num_pols; i++ )
	{
		if( IsSelected[i] )
			continue;

		IsSelected[i] = true;

		rvecPolygonGroup.push_back( STempPolygonGroup() );
		rvecPolygonGroup.back().vecIndex.push_back(i);

		LWO2_Face& rPolygon0 = rvecPolygon[i];
		num_verts0 = rPolygon0.GetNumPoints();

		for( j=i+1; j<num_pols; j++ )
		{
			if( IsSelected[j] )
				continue;

			LWO2_Face& rPolygon1 = rvecPolygon[j];
			num_verts1 = rPolygon1.GetNumPoints();
			for( k=0; k<num_verts0; k++ )
			{
				for( m=0; m<num_verts1; m++ )
				{
					if( rPolygon0.GetVertexIndex(k) == rPolygon1.GetVertexIndex(m) )
					{
						IsSelected[j] = true;
						rvecPolygonGroup.back().vecIndex.push_back(j);
						break;
					}
				}

				if( IsSelected[j] )
					break;
			}	
		}
	}*/
}


void CMassSpringExporter_LW::BuildCollisionShapes( LWO2_Layer& rLayer,
												   vector<STempPolygonGroup>& rvecPolygonGroup )
{
	vector<LWO2_Face>& rvecPolygon = rLayer.GetFace();
	int i, num_pols = (int)rvecPolygon.size();
	int j, k, num_verts;
	int num_grouped_pols;

	vector<Vector3>& rvecVertex = rLayer.GetVertex();
	int num_groups = (int)rvecPolygonGroup.size();

	for( i=0; i<num_groups; i++ )
	{
		if( 1 < rvecPolygonGroup[i].vecIndex.size() )
		{
			// create a sphere
			AABB3 aabb;
			aabb.Nullify();
			num_grouped_pols = rvecPolygonGroup[i].vecIndex.size();
			for( j=0; j<num_grouped_pols; j++ )
			{
				LWO2_Face& rPolygon = rvecPolygon[ rvecPolygonGroup[i].vecIndex[j] ];
				num_verts = rPolygon.GetNumPoints();
				for( k=0; k<num_verts; k++ )
				{
					aabb.AddPoint( rvecVertex[rPolygon.GetVertexIndex(k)] );
				}
			}

			CMS_Sphere sphere;
			sphere.vLocalPosition = aabb.GetCenterPosition();
			sphere.fRadius = (aabb.vMax.x - aabb.vMin.x) * 0.5f;
			sphere.fRadiusSq = sphere.fRadius * sphere.fRadius;

			sphere.iFixedPointGroup = rvecPolygonGroup[i].iDestBoneIndex;

			m_MassSpringArchive.m_vecSphere.push_back( sphere );
		}
	}
}


void CMassSpringExporter_LW::CreateCollisionShapes( LWO2_Layer& rLayer,
		                                            LWO2_Object& rOrigObject,
								                    vector<int>& rvecDestBoneIndex )
{
	vector<STempPolygonGroup> vecPolygonGroup;

	GroupConnectedPolygons( rLayer, vecPolygonGroup );

	SetFixedGroupIndex( vecPolygonGroup, rLayer, rOrigObject, rvecDestBoneIndex );

	BuildCollisionShapes( rLayer, vecPolygonGroup );
}


} // amorphous
