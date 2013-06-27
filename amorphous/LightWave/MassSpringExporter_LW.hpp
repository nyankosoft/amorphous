#ifndef  __MASSSPRINGEXPORTER_LW_H__
#define  __MASSSPRINGEXPORTER_LW_H__


#include "amorphous/PhysicsSim/MassSpringExporter.hpp"
#include "amorphous/LightWave/LightWaveObject.hpp"


namespace amorphous
{


//==========================================================================
// CMassSpringExporter_LW
//==========================================================================
/**

  Fixed (i.e. pinned) points
  -------------------------------
  The points with any of the following properties are saved as "fixed points"
  1. Points which belong to a point set named "FixedPoints"
    - These are all saved as the group 0 of fixed points.
  2. Points with the weightmaps which are mapped to skelegon bones.
    - Bone indices are used as groups of the points
	- Points are not saved as fixed points if their weightmaps are not mapped to skelegon bones.

  Point masses
  -------------------------------
  - All points have the mass of 0.1[kg] by default.
  - If a point has a verex weightmap or a vertex color map with the name "CPMass",
    the vertex map value is saved as the mass of the point.
    - In the case of a color map, value of alpha channel is used.

  Connections between control points
  -------------------------------
  - 2-piont polygons are detected as springs between control points,
    i.e. all the control points must be connected by 2 point polys
	in order for the input model to be compiled as a valid mass spring model.

  Springs
  -------------------------------
  2-point polygon with tags that contain the string "Spring" are assigned
  spring prpperties encoded in the tag name.
  The tag named "Spring 100.0 abc 10.0" is interpreted as the spring with
  the strength of 100.0.

  Collision shapes
  -------------------------------
  - Each conneced set of polygons is detected as a collision shape.
  - Shpere is the only support shape.

*/
class CMassSpringExporter_LW : public CMassSpringExporter
{
	struct STempPointInfo
	{
		int iNewIndex;
		bool bIsFixed;
		int iFixedPointGroup;
		float fMass;

		/// indices to the connected points (the original point index of the LightWave object)
		std::vector<int> veciAdjacentPointIndex;

		STempPointInfo() { iNewIndex = -1; bIsFixed = false; iFixedPointGroup = -1; fMass = 0.1f; }

	};

	struct STempSpringInfo
	{
		std::string strName;
		float fSpring;
		int iTagIndex;

		STempSpringInfo() { fSpring = 100.0f; iTagIndex = -1; }
	};

	// used to create collision objects
	struct STempPolygonGroup
	{
		std::vector<int> vecIndex;
		int iOrigBoneIndex;
		int iDestBoneIndex;	// used as the matrix index during runtime
		
		STempPolygonGroup() { iDestBoneIndex = 0; }
	};


	// stores the mass spring model created by this class
	CMS_MassSpringArchive m_MassSpringArchive;

private:

	void SetSpringInfo( std::vector<STempSpringInfo>& rvecSpringInfo, LWO2_Object& rOrigObject );

	bool GetPointInfo( std::vector<STempPointInfo>& rvecPointInfo, LWO2_Layer& rLayer );

	/// find mass for each control point
	void SetMass( std::vector<STempPointInfo>& rvecPointInfo, LWO2_Layer& rLayer );

	int FindSpringInfo( int iPartIndex, std::vector<STempSpringInfo>& rvecSpringInfo );

	void SetPointsForNormalCalculation( std::vector<CMS_PointProperty>& rvecControlPoint,
		                                std::vector<STempPointInfo>& rvecPointInfo );

	void SetIndicesToFixedPoints();

	/// for collision shape creation

	void GroupConnectedPolygons( LWO2_Layer& rLayer, std::vector<STempPolygonGroup>& vecPolygonGroup );

	void GroupConnectedPolygons_r( STempPolygonGroup& rGroup,
								   std::vector<LWO2_Face>& rvecPolygon,
								   std::vector<bool>& Selected );

	/// set fixed group indices for control points
	void SetFixedGroupIndex( LWO2_Layer& rLayer,
							 LWO2_Object& rOrigObject,
							 std::vector<STempPointInfo>& rvecPointInfo,
							 std::vector<int>& rvecDestBoneIndex );

	/// set fixed group indices for collision shapes
	void SetFixedGroupIndex( std::vector<STempPolygonGroup>& rvecPolygonGroup,
							 LWO2_Layer& rLayer,
							 LWO2_Object& rOrigObject,
							 std::vector<int>& rvecDestBoneIndex );

    void BuildCollisionShapes( LWO2_Layer& rLayer, std::vector<STempPolygonGroup>& vecPolygonGroup );

public:

	CMassSpringExporter_LW();

	~CMassSpringExporter_LW();

	void BuildMassSpringModel( const char *pcFilename );

	bool CreateMassSpringModel( LWO2_Layer& rLayer,
		                        LWO2_Object& rOrigObject,
								std::vector<int>& rvecDestBoneIndex );

	void CreateCollisionShapes( LWO2_Layer& rLayer,
		                        LWO2_Object& rOrigObject,
								std::vector<int>& rvecDestBoneIndex );

//	void SetBoneIndexMap( vector<int>& rvecDestBoneIndex )


	CMS_MassSpringArchive& GetArchive() { return m_MassSpringArchive; }

};


} // amorphous



#endif		/*  __MASSSPRINGEXPORTER_LW_H__  */
