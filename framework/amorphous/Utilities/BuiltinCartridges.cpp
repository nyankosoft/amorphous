#include "BuiltinCartridges.hpp"
#include "../Support/Macro.h"


namespace amorphous
{

using std::pair;


namespace firearm
{

// Client code is responsible for explicitly specify the number of control points via num_control_points,
// i.e. the system does not detect the end of the array by checking (d,h) == (0,0).
// Rationale: (d,h) == (0,0) could be a valid position in case of a hollow point bullet.
static std::pair<int,BulletDesc> bd(
	int cal,
	double diameter,
	double exposed_part_length,
	int num_control_points,
	int exposed_part_cp_start_index,
	float d0,     float h0,     float t0,     float c0,     float b0,
	float d1,     float h1,     float t1,	  float c1,     float b1,
	float d2 = 0, float h2 = 0, float t2 = 0, float c2 = 0, float b2 = 0,
	float d3 = 0, float h3 = 0, float t3 = 0, float c3 = 0, float b3 = 0,
	float d4 = 0, float h4 = 0, float t4 = 0, float c4 = 0, float b4 = 0,
	float d5 = 0, float h5 = 0, float t5 = 0, float c5 = 0, float b5 = 0,
	float d6 = 0, float h6 = 0, float t6 = 0, float c6 = 0, float b6 = 0,
	float d7 = 0, float h7 = 0, float t7 = 0, float c7 = 0, float b7 = 0,
	float d8 = 0, float h8 = 0, float t8 = 0, float c8 = 0, float b8 = 0,
	float d9 = 0, float h9 = 0, float t9 = 0, float c9 = 0, float b9 = 0
	)
{
	BulletDesc desc;
	desc.diameter           = (float)diameter              * 0.001f;
//	desc.length             = (float)???                   * 0.001f;
	desc.exposed_length     = (float)exposed_part_length   * 0.001f;
	desc.num_control_points = num_control_points;
	desc.exposed_part_cp_start_index = exposed_part_cp_start_index;

	typedef BulletSliceControlPoint bscp;
	bscp& cp0 = desc.bullet_slice_control_points[0]; cp0.position.x = d0 * 0.5f * 0.001f; cp0.position.y = h0 * 0.001f; cp0.tension = t0; cp0.continuity = c0; cp0.bias = b0;
	bscp& cp1 = desc.bullet_slice_control_points[1]; cp1.position.x = d1 * 0.5f * 0.001f; cp1.position.y = h1 * 0.001f; cp1.tension = t1; cp1.continuity = c1; cp1.bias = b1;
	bscp& cp2 = desc.bullet_slice_control_points[2]; cp2.position.x = d2 * 0.5f * 0.001f; cp2.position.y = h2 * 0.001f; cp2.tension = t2; cp2.continuity = c2; cp2.bias = b2;
	bscp& cp3 = desc.bullet_slice_control_points[3]; cp3.position.x = d3 * 0.5f * 0.001f; cp3.position.y = h3 * 0.001f; cp3.tension = t3; cp3.continuity = c3; cp3.bias = b3;
	bscp& cp4 = desc.bullet_slice_control_points[4]; cp4.position.x = d4 * 0.5f * 0.001f; cp4.position.y = h4 * 0.001f; cp4.tension = t4; cp4.continuity = c4; cp4.bias = b4;
	bscp& cp5 = desc.bullet_slice_control_points[5]; cp5.position.x = d5 * 0.5f * 0.001f; cp5.position.y = h5 * 0.001f; cp5.tension = t5; cp5.continuity = c5; cp5.bias = b5;
	bscp& cp6 = desc.bullet_slice_control_points[6]; cp6.position.x = d6 * 0.5f * 0.001f; cp6.position.y = h6 * 0.001f; cp6.tension = t6; cp6.continuity = c6; cp6.bias = b6;
	bscp& cp7 = desc.bullet_slice_control_points[7]; cp7.position.x = d7 * 0.5f * 0.001f; cp7.position.y = h7 * 0.001f; cp7.tension = t7; cp7.continuity = c7; cp7.bias = b7;
	bscp& cp8 = desc.bullet_slice_control_points[8]; cp8.position.x = d8 * 0.5f * 0.001f; cp8.position.y = h8 * 0.001f; cp8.tension = t8; cp8.continuity = c8; cp8.bias = b8;
	bscp& cp9 = desc.bullet_slice_control_points[9]; cp9.position.x = d9 * 0.5f * 0.001f; cp9.position.y = h9 * 0.001f; cp9.tension = t9; cp9.continuity = c9; cp9.bias = b9;

	return std::pair<int,BulletDesc>( cal, desc );
}


static std::pair<int,CaseDesc> cd(
	int cal,
	int top_outer_slice_index,
	float d0,      float l0,      float r0, 
	float d1,      float l1,      float r1, 
	float d2,      float l2,      float r2, 
	float d3,      float l3,      float r3,
	float d4,      float l4,      float r4,
	float d5,      float l5,      float r5,
	float d6 = 0,  float l6 = 0,  float r6 = 0,
	float d7 = 0,  float l7 = 0,  float r7 = 0,
	float d8 = 0,  float l8 = 0,  float r8 = 0,
	float d9 = 0,  float l9 = 0,  float r9 = 0,
	float d10 = 0, float l10 = 0, float r10 = 0,
	float d11 = 0, float l11 = 0, float r11 = 0,
	float d12 = 0, float l12 = 0, float r12 = 0,
	float d13 = 0, float l13 = 0, float r13 = 0,
	float d14 = 0, float l14 = 0, float r14 = 0,
	float d15 = 0, float l15 = 0, float r15 = 0
	)
{
	CaseDesc desc;
	desc.caliber = (Caliber::Name)cal;
	desc.top_outer_slice_index = top_outer_slice_index;
	desc.case_slices[ 0].diameter = d0  * 0.001f; desc.case_slices[ 0].height = l0  * 0.001f; desc.case_slices[ 0].curvature_radius = r0  * 0.001f;
	desc.case_slices[ 1].diameter = d1  * 0.001f; desc.case_slices[ 1].height = l1  * 0.001f; desc.case_slices[ 1].curvature_radius = r1  * 0.001f;
	desc.case_slices[ 2].diameter = d2  * 0.001f; desc.case_slices[ 2].height = l2  * 0.001f; desc.case_slices[ 2].curvature_radius = r2  * 0.001f;
	desc.case_slices[ 3].diameter = d3  * 0.001f; desc.case_slices[ 3].height = l3  * 0.001f; desc.case_slices[ 3].curvature_radius = r3  * 0.001f;
	desc.case_slices[ 4].diameter = d4  * 0.001f; desc.case_slices[ 4].height = l4  * 0.001f; desc.case_slices[ 4].curvature_radius = r4  * 0.001f;
	desc.case_slices[ 5].diameter = d5  * 0.001f; desc.case_slices[ 5].height = l5  * 0.001f; desc.case_slices[ 5].curvature_radius = r5  * 0.001f;
	desc.case_slices[ 6].diameter = d6  * 0.001f; desc.case_slices[ 6].height = l6  * 0.001f; desc.case_slices[ 6].curvature_radius = r6  * 0.001f;
	desc.case_slices[ 7].diameter = d7  * 0.001f; desc.case_slices[ 7].height = l7  * 0.001f; desc.case_slices[ 7].curvature_radius = r7  * 0.001f;
	desc.case_slices[ 8].diameter = d8  * 0.001f; desc.case_slices[ 8].height = l8  * 0.001f; desc.case_slices[ 8].curvature_radius = r8  * 0.001f;
	desc.case_slices[ 9].diameter = d9  * 0.001f; desc.case_slices[ 9].height = l9  * 0.001f; desc.case_slices[ 9].curvature_radius = r9  * 0.001f;
	desc.case_slices[10].diameter = d10 * 0.001f; desc.case_slices[10].height = l10 * 0.001f; desc.case_slices[10].curvature_radius = r10 * 0.001f;
	desc.case_slices[11].diameter = d11 * 0.001f; desc.case_slices[11].height = l11 * 0.001f; desc.case_slices[11].curvature_radius = r11 * 0.001f;
	desc.case_slices[12].diameter = d12 * 0.001f; desc.case_slices[12].height = l12 * 0.001f; desc.case_slices[12].curvature_radius = r12 * 0.001f;
	desc.case_slices[13].diameter = d13 * 0.001f; desc.case_slices[13].height = l13 * 0.001f; desc.case_slices[13].curvature_radius = r13 * 0.001f;
	desc.case_slices[14].diameter = d14 * 0.001f; desc.case_slices[14].height = l14 * 0.001f; desc.case_slices[14].curvature_radius = r14 * 0.001f;
	desc.case_slices[15].diameter = d15 * 0.001f; desc.case_slices[15].height = l15 * 0.001f; desc.case_slices[15].curvature_radius = r15 * 0.001f;


	for( int i=0; i<CaseDesc::MAX_NUM_CASE_SLICES; i++ )
	{
		if( fabs(desc.case_slices[i].diameter) < 0.000001
		 && fabs(desc.case_slices[i].height)   < 0.000001 )
		{
			desc.num_case_slices = i;
			break;
		}
	}

	return std::pair<int,CaseDesc>( cal, desc );
}


// unit: millimeters (mm)
// each 2 digits represent the diameter and the height
// EPO: Exposed Portion Offset: the index of the control point above which bullet is exposed outside the case.
static std::pair<int,BulletDesc> sg_FMJBulletDimensions[] =
{
	// [template]
//	bd( Caliber::OTHER,                 0.000,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),

	//  caliber                                                     [0]                           [1]                          [2]                          [3]                          [4]
	//  caliber                        diameter  E.P.L.   #CPs  EPO D       H      T    C    B    D      H      T    C    B    D      H      T    C    B    D      H      T    C    B    D      H      T    C    B
//	bd( Caliber::_9MM,                  0.000,   10.54,   3,    0,   9.03,  00.00, 0.0, 0.0, 1.0,  6.00, 10.00, 0.0, 0.0, 0.0,  0.00, 12.54, 0.0, 0.0, 0.0 ),
//	bd( Caliber::_9MM,                  9.017,   10.54,   4,    1,   9.03,  00.00, 0.0, 0.0, 1.0,  9.03,  3.00,-1.0, 1.0, 1.0,  7.20,  9.00, 0.0, 1.0, 1.0,  0.00, 13.54, 0.0, 0.0, 0.0 ),
	bd( Caliber::_9X18MM_MAKAROV,       9.270,    6.90,   5,    1,   9.27,  00.00, 0.0, 0.0, 0.0,  9.27,  3.00, 0.0, 0.0, 0.0,  8.25,  6.00, 0.0, 0.0, 0.0,  5.10,  8.60, 0.0, 0.0, 0.0,  0.00,  9.90, 0.0, 0.0, 0.0 ),
	bd( Caliber::_9MM,                  9.017,   10.54,   5,    1,   9.03,  00.00, 0.0, 0.0, 0.0,  9.03,  3.00, 0.0, 0.0, 0.0,  8.10,  7.60, 0.0, 0.0, 0.0,  5.30, 11.50, 0.0, 0.0, 0.0,  0.00, 13.54, 0.0, 0.0, 0.0 ),
	bd( Caliber::_40_SW,               10.170,    6.54,   6,    1,  10.17,  00.00, 1.0, 0.0, 0.0, 10.17,  3.00, 0.0, 0.0, 0.0,  9.47,  5.00, 0.0, 0.0, 0.0,  6.00,  8.50, 0.0, 0.0, 0.0,  4.00,  9.54, 0.0, 0.0, 0.0,  0.00,  9.54, 0.0, 0.0, 0.0 ),
//	bd( Caliber::_45_ACP,              11.480,    9.58,   4,    0,  11.48,  00.00, 1.0, 0.0, 0.0, 11.48,  3.00, 0.0, 0.0, 0.0,  9.50, 11.00, 0.0, 0.0, 0.0,  0.00, 14.58, 0.0, 0.0, 0.0 ),
	bd( Caliber::_45_ACP,              11.480,    9.58,   5,    1,  11.48,  00.00, 0.0, 0.0, 0.0, 11.48,  3.00, 0.0, 0.0, 0.0, 10.10,  7.60, 0.0, 0.0, 0.0,  6.50, 11.00, 0.0, 0.0, 0.0,  0.00, 12.58, 0.0, 0.0, 0.0 ),
	bd( Caliber::_10MM_AUTO,           10.170,    6.54,   6,    1,  10.17,  00.00, 1.0, 0.0, 0.0, 10.17,  3.00, 0.0, 0.0, 0.0,  9.47,  5.00, 0.0, 0.0, 0.0,  6.00,  8.50, 0.0, 0.0, 0.0,  4.00,  9.54, 0.0, 0.0, 0.0,  0.00,  9.54, 0.0, 0.0, 0.0 ),
	bd( Caliber::_500_SW_MAGNUM,       12.700,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),

	bd( Caliber::HK_4_6X30,             4.650,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_7_62X25MM_TOKAREV,    7.870,    0.00,   0,    1,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_357_SIG,              9.020,    4.50,   6,    1,   9.02,  00.00, 1.0, 0.0, 0.0,  9.02,  3.00, 0.0, 0.0, 0.0,  7.37,  5.00, 0.0, 0.0, 0.0,  6.00,  6.00, 0.0, 0.0, 0.0,  3.50,  7.50, 0.0, 0.0, 0.0,  0.00,  7.50, 0.0, 0.0, 0.0 ),
	bd( Caliber::_5_7X28,               5.700,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_5_56X45,              5.700,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_5_45X39,              5.600,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_6_8MM_REMINGTON_SPC,  7.000,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_7_62X39,              7.920,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
//	bd( Caliber::_308_WINCHESTER,       0.000,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_30_06_SPRINGFIELD,    7.850,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_338_LAPUA_MAGNUM,     8.580,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
	bd( Caliber::_50BMG,               12.950,    0.00,   0,    0,   0.00,  00.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00,  0.00, 0.0, 0.0, 0.0,  0.00, 0.0, 0.0, 0.0  ),
};


// unit: millimeters (mm)
// each 3 digits represent the following dimensions : diameter, height, and radius
static std::pair<int,CaseDesc> sg_CaseDimensions[] =
{
	// non-bottle necked cases          bottom            rim (bottom/top)                  groove (bottom/top)                base (bottom/top)
	cd( Caliber::_380_ACP,           6,  0.00, 0.00,  0,   9.50, 0.00, 0,   9.50, 1.14, 0,   0.00, 0.00, 0,   0.00,  0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0 ), // Wikipedia, http://www.handloads.com/images/cartridge/380acp.htm
	cd( Caliber::_9X18MM_MAKAROV,    6,  9.35, 0.00,  0,   9.95, 0.60, 0,   9.95, 1.25, 0,   8.55, 1.25, 0,   8.55,  2.25, 0,   9.95, 3.50, 0,   9.91, 19.10, 0 ),
	cd( Caliber::_9MM,               6,  9.66, 0.00,  0,   9.96, 0.30, 0,   9.96, 1.27, 0,   8.79, 1.27, 0,   8.79,  2.17, 0,   9.93, 2.98, 0,   9.65, 19.15, 0 ),
	cd( Caliber::_45_ACP,            6, 11.65, 0.00,  0,  12.19, 0.30, 0,  12.19, 1.24, 0,  10.16, 1.24, 0,  10.16,  2.13, 0,  12.09, 4.11, 0,  12.01, 22.81, 0 ),
	cd( Caliber::_40_SW,             6, 10.04, 0.00,  0,  10.77, 0.51, 0,  10.77, 1.40, 0,   8.81, 1.40, 0,   8.81,  2.54, 0,  10.77, 3.52, 0,  10.74, 21.59, 0 ),
	cd( Caliber::_10MM_AUTO,         6, 10.35, 0.00,  0,  10.85, 0.50, 0,  10.85, 1.40, 0,   8.85, 1.40, 0,   8.85,  2.65, 0,  10.80, 3.63, 0,  10.70, 25.20, 0 ),
	cd( Caliber::_500_SW_MAGNUM,     6, 13.51, 0.00,  0,  14.22, 0.25, 0,  14.22, 1.50, 0,  12.19, 1.50, 0,  12.19,  2.64, 0,  13.46, 3.91, 0,  13.46, 41.28, 0 ),
//	cd( Caliber::???,                6,  0.00, 0.00,  0,   0.00, 0.00, 0,   0.00, 0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0 ),

	// bottle necked cases           8, bottom            rim (bottom/top)                  groove (bottom/top)                base (bottom/top)                     neck(bottom/top)
	cd( Caliber::HK_4_6X30,          8,  7.50, 0.00,  0,   8.00, 0.25, 0,   8.00, 1.10, 0,   6.80, 1.10, 0,   6.80,  2.00, 0,   8.02, 2.87, 0,   7.75, 23.02, 2.5,   5.31, 26.03, 2.5,  5.31, 30.50, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_7_62X25MM_TOKAREV, 8,  8.65, 0.00,  0,   9.95, 0.60, 0,   9.95, 1.32, 0,   8.65, 1.32, 0,   8.65,  2.32, 0,   9.83, 3.34, 0,   9.48, 19.60, 2.5,   8.49, 21.04, 2.5,  8.49, 25.00, 0,   0.00,  0.00, 0 ), // http://www.loaddata.com/members/search_detail.cfm?MetallicID=4641
	cd( Caliber::_357_SIG,           8,  9.31, 0.00,  0,  10.77, 0.51, 0,  10.77, 1.40, 0,   8.81, 1.40, 0,   8.81,  2.54, 0,  10.77, 3.59, 0,  10.77, 16.49, 2.5,   9.68, 18.16, 2.5,  9.68, 21.97, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_5_7X28,            8,  7.24, 0.00,  0,   7.80, 0.28, 0,   7.80, 1.14, 0,   6.60, 1.14, 0,   6.60,  1.93, 0,   7.95, 3.38, 0,   7.95, 23.15, 2.5,   6.38, 24.27, 1.5,  6.38, 28.90, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_5_56X45,           8,  8.97, 0.00,  0,   9.60, 0.45, 0,   9.60, 1.14, 0,   8.43, 1.14, 0,   8.43,  1.90, 0,   9.58, 3.13, 0,   9.00, 36.52, 2.5,   6.43, 39.55, 2.5,  6.43, 44.70, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_6_5MM_CREEDMOOR,   8, 10.39, 0.00,  0,  12.01, 0.38, 0,  12.01, 1.37, 0,  10.38, 1.37, 0,  10.38,  2.77, 0,  11.95, 3.85, 0,  11.73, 37.85, 2.5,   7.49, 41.52, 2.5,  7.49, 48.77, 0,   0.00,  0.00, 0 ), // http://precisionrifle.files.wordpress.com/2013/07/6-5-creedmoor-excerpt-from-berger-bullets-reloading-manual.jpg
	cd( Caliber::_5_45X39,           8,  9.00, 0.00,  0,  10.00, 0.50, 0,  10.00, 1.50, 0,   8.60, 1.50, 0,   8.60,  2.50, 0,  10.00, 3.20, 0,   9.25, 30.00, 2.5,   6.29, 34.00, 2.5,  6.29, 39.82, 0,   0.00,  0.00, 0 ),
//	cd( Caliber::_6_8MM_REMINGTON_SPC,8  ?.??  0.00,  0,  10.69, ?.??, 0,  10.69, ?.??, 0,   ?,??, ?,??, 0,   ?,??,  ?,??, 0,  10.69, ?,??, 0,  10.21, 33.02, 2.5,   7.57, 36.20, 2.5,  7.57, 42.55, 0,   0,00,  0.00, 0 ), // http://www.loaddata.com/members/search_detail.cfm?metallicid=4399&MW=&PM=&PT=, http://www.ar15.com/archive/topic.html?b=3&f=121&t=282708
	cd( Caliber::_7_62X39,           8, 10.85, 0.00,  0,  11.35, 0.25, 0,  11.35, 1.50, 0,   9.56, 1.50, 0,   9.56,  2.50, 0,  11.35, 3.20, 0,  10.07, 30.50, 2.5,   8.60, 33.00, 2.5,  8.60, 38.70, 0,   0.00,  0.00, 0 ),
//	cd( Caliber::_308_WINCHESTER,    8, 11.48, 0.00,  0,  12.01, 0.38, 0,  12.01, 1.37, 0,  10.39, 1.37, 0,  10.39,  2.77, 0,  11.96, 3.85, 0,  11.53, 39.62, 2.5,   8.72, 43.48, 2.5,  8.72, 51.18, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_30_06_SPRINGFIELD, 8, 10.92, 0.00,  0,  12.01, 0.38, 0,  12.01, 1.24, 0,  10.39, 1.24, 0,  10.39,  2.08, 0,  11.96, 3.16, 0,  11.20, 49.49, 2.5,   8.63, 53.56, 2.5,  8.63, 63.35, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_338_LAPUA_MAGNUM,  8, 13.93, 0.00,  0,  14.93, 0.50, 0,  14.93, 1.52, 0,  13.24, 1.52, 0,  13.24,  2.42, 0,  14.91, 3.12, 0,  13.82, 54.90, 2.5,   9.46, 60.89, 2.5,  9.41, 69.20, 0,   0.00,  0.00, 0 ),
//	cd( Caliber::???,                8,  0.00, 0.00,  0,   0.00, 0.00, 0,   0.00, 0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 2.5,   0.00,  0.00, 2.5,  0.00,  0.00, 0,   0.00,  0.00, 0 ),
//	cd( Caliber::_50BMG,             8,  0.00, 0.00,  0,   0.804,0.00, 0,   0.804,0.083,0,   0.680,0.083,0,   0.680, 0.212,0,   0.00, 0.00, 0,   0.00,  0.00, 2.5,   0.00,  0.00, 2.5,  0.00,  0.00, 0,   0.00,  0.00, 0 ),

	// rimmed bottle necked cases  bottom                 rim (bottom/top)                  base (bottom/top)                  neck(bottom/top)
	cd( Caliber::_7_62X54R,          6, 11.18, 0.00,  0,  14.48, 0.60, 0,  14.48, 1.60, 0,  12.37, 1.60, 0,  11.61, 37.70, 0,   8.53, 44.30, 0,  8.53, 53.72, 2.5,   0.00,  0.00, 2.5 ),
};


bool GetFMJBulletDesc( Caliber::Name caliber, BulletDesc& dest )
{
	for( int i=0; i<numof(sg_FMJBulletDimensions); i++ )
	{
		if( sg_FMJBulletDimensions[i].first == caliber )
		{
			dest = sg_FMJBulletDimensions[i].second;
			return true;
		}
	}

	return false;
}


bool GetCaseDesc( Caliber::Name caliber, CaseDesc& dest )
{
	for( int i=0; i<numof(sg_CaseDimensions); i++ )
	{
		if( sg_CaseDimensions[i].first == caliber )
		{
			dest = sg_CaseDimensions[i].second;
			return true;
		}
	}

	return false;
}



} // firearm


} // amorphous
