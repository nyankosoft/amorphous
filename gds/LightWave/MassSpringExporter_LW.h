
#ifndef  __MASSSPRINGEXPORTER_LW_H__
#define  __MASSSPRINGEXPORTER_LW_H__


#include "PhysicsSim/MassSpringExporter.h"

#include "LightWave/LightWaveObject.h"


//==========================================================================
// CMassSpringExporter_LW
//==========================================================================

class CMassSpringExporter_LW : public CMassSpringExporter
{
	struct STempPointInfo
	{
		int iNewIndex;
		bool bIsFixed;
		int iFixedPointGroup;
		float fMass;

		/// indices to the connected points (the original point index of the LightWave object)
		vector<int> veciAdjacentPointIndex;

		STempPointInfo() { iNewIndex = -1; bIsFixed = false; iFixedPointGroup = -1; fMass = 0.1f; }

	};

	struct STempSpringInfo
	{
		string strName;
		float fSpring;
		int iTagIndex;

		STempSpringInfo() { fSpring = 100.0f; iTagIndex = -1; }
	};

	// used to create collision objects
	struct STempPolygonGroup
	{
		vector<int> vecIndex;
		int iOrigBoneIndex;
		int iDestBoneIndex;	// used as the matrix index during runtime
		
		STempPolygonGroup() { iDestBoneIndex = 0; }
	};


	// stores the mass spring model created by this class
	CMS_MassSpringArchive m_MassSpringArchive;

private:

	void SetSpringInfo( vector<STempSpringInfo>& rvecSpringInfo, CLWO2_Object& rOrigObject );

	bool GetPointInfo( vector<STempPointInfo>& rvecPointInfo, CLWO2_Layer& rLayer );

	/// find mass for each control point
	void SetMass( vector<STempPointInfo>& rvecPointInfo, CLWO2_Layer& rLayer );

	int FindSpringInfo( int iPartIndex, vector<STempSpringInfo>& rvecSpringInfo );

	void SetPointsForNormalCalculation( vector<CMS_PointProperty>& rvecControlPoint,
		                                vector<STempPointInfo>& rvecPointInfo );

	void SetIndicesToFixedPoints();

	/// for collision shape creation

	void GroupConnectedPolygons( CLWO2_Layer& rLayer, vector<STempPolygonGroup>& vecPolygonGroup );

	void GroupConnectedPolygons_r( STempPolygonGroup& rGroup,
								   vector<CLWO2_Face>& rvecPolygon,
								   vector<bool>& Selected );

	/// set fixed group indices for control points
	void SetFixedGroupIndex( CLWO2_Layer& rLayer,
							 CLWO2_Object& rOrigObject,
							 vector<STempPointInfo>& rvecPointInfo,
							 vector<int>& rvecDestBoneIndex );

	/// set fixed group indices for collision shapes
	void SetFixedGroupIndex( vector<STempPolygonGroup>& rvecPolygonGroup,
							 CLWO2_Layer& rLayer,
							 CLWO2_Object& rOrigObject,
							 vector<int>& rvecDestBoneIndex );

    void BuildCollisionShapes( CLWO2_Layer& rLayer, vector<STempPolygonGroup>& vecPolygonGroup );

public:

	CMassSpringExporter_LW();

	~CMassSpringExporter_LW();

	void BuildMassSpringModel( const char *pcFilename );

	bool CreateMassSpringModel( CLWO2_Layer& rLayer,
		                        CLWO2_Object& rOrigObject,
								vector<int>& rvecDestBoneIndex );

	void CreateCollisionShapes( CLWO2_Layer& rLayer,
		                        CLWO2_Object& rOrigObject,
								vector<int>& rvecDestBoneIndex );

//	void SetBoneIndexMap( vector<int>& rvecDestBoneIndex )


	CMS_MassSpringArchive& GetArchive() { return m_MassSpringArchive; }

};



#endif		/*  __MASSSPRINGEXPORTER_LW_H__  */