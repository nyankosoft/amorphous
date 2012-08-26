#include "BuiltinCartridges.hpp"
#include "../Support/Macro.h"

using std::pair;


namespace firearm
{


static std::pair<int,CaseDesc> cd(
	int cal,
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
	float d11 = 0, float l11 = 0, float r11 = 0
	)
{
	CaseDesc desc;
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

	return std::pair<int,CaseDesc>( cal, desc );
}


// unit: millimeters (mm)
// each 3 digits represent the following dimensions : diameter, height, and radius
static std::pair<int,CaseDesc> sg_CaseDimensions[] =
{
	// non-bottle necked cases       bottom            rim (bottom/top)                  groove (bottom/top)                base (bottom/top)
	cd( Caliber::_380_ACP,            0.00, 0.00,  0,   9.50, 0.00, 0,   9.50, 0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_9MM,                9.66, 0.00,  0,   9.96, 0.30, 0,   9.96, 1.27, 0,   8.79, 1.27, 0,   8.79,  2.17, 0,   9.93, 2.98, 0,   9.65, 19.15, 0 ),
	cd( Caliber::_45_ACP,            11.65, 0.00,  0,  12.19, 0.30, 0,  12.19, 1.24, 0,  10.16, 1.24, 0,  10.16,  2.13, 0,  12.09, 4.11, 0,  12.01, 22.81, 0 ),
	cd( Caliber::_40_SW,             10.04, 0.00,  0,  10.77, 0.51, 0,  10.77, 1.40, 0,   8.81, 1.40, 0,   8.81,  2.54, 0,  10.77, 3.52, 0,  10.74, 21.59, 0 ),
	cd( Caliber::_10MM_AUTO,         10.35, 0.00,  0,  10.85, 0.50, 0,  10.85, 1.40, 0,   8.85, 1.40, 0,   8.85,  2.65, 0,  10.80, 3.63, 0,  10.70, 25.20, 0 ),
//	cd( Caliber::???,                 0.00, 0.00,  0,   0.00, 0.00, 0,   0.00, 0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0 ),

	// bottle necked cases           bottom            rim (bottom/top)                  groove (bottom/top)                base (bottom/top)                  neck(bottom/top)
	cd( Caliber::_357_SIG,            9.31, 0.00,  0,  10.77, 0.51, 0,  10.77, 1.40, 0,   8.81, 1.40, 0,   8.81,  2.54, 0,  10.77, 3.59, 0,  10.77, 16.49, 0,   9.68, 18.16, 0,  9.68, 21.97, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_5_7X28,             7.24, 0.00,  0,   7.80, 0.28, 0,   7.80, 1.14, 0,   6.60, 1.14, 0,   6.60,  1.93, 0,   7.95, 3.38, 0,   7.95, 23.15, 0,   6.38, 24.27, 0,  6.38, 28.90, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_5_56X45,            8.97, 0.00,  0,   9.60, 0.45, 0,   9.60, 1.14, 0,   8.43, 1.14, 0,   8.43,  1.90, 0,   9.58, 3.13, 0,   9.00, 36.52, 0,   6.43, 39.55, 0,  6.43, 44.70, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_7_62X39,           10.85, 0.00,  0,  11.35, 0.25, 0,  11.35, 1.50, 0,   9.56, 1.50, 0,   9.56,  2.50, 0,  11.35, 3.20, 0,  10.07, 30.50, 0,   8.60, 33.00, 0,  8.60, 38.70, 0,   0.00,  0.00, 0 ),
//	cd( Caliber::_308_WINCHESTER,    11.48, 0.00,  0,  12.01, 0.38, 0,  12.01, 1.37, 0,  10.39, 1.37, 0,  10.39,  2.77, 0,  11.96, 3.85, 0,  11.53, 39.62, 0,   8.72, 43.48, 0,  8.72, 51.18, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_30_06_SPRINGFIELD, 10.92, 0.00,  0,  12.01, 0.38, 0,  12.01, 1.24, 0,  10.39, 1.24, 0,  10.39,  2.08, 0,  11.96, 3.16, 0,  11.20, 49.49, 0,   8.63, 53.56, 0,  8.63, 63.35, 0,   0.00,  0.00, 0 ),
	cd( Caliber::_338_LAPUA_MAGNUM,  13.93, 0.00,  0,  14.93, 0.50, 0,  14.93, 1.52, 0,  13.24, 1.52, 0,  13.24,  2.42, 0,  14.91, 3.12, 0,  13.82, 54.90, 0,   9.46, 60.89, 0,  9.41, 69.20, 0,   0.00,  0.00, 0 ),
//	cd( Caliber::_50BMG,              0.00, 0.00,  0,   0.804,0.00, 0,   0.804,0.083,0,   0.680,0.083,0,   0.680, 0.212,0,   0.00, 0.00, 0,   0.00,  0.00, 0,   0.00,  0.00, 0,  0.00,  0.00, 0,   0.00,  0.00, 0 ),
//	cd( Caliber::???,                 0.00, 0.00,  0,   0.00, 0.00, 0,   0.00, 0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0,   0.00, 0.00, 0,   0.00,  0.00, 0,   0.00,  0.00, 0,  0.00,  0.00, 0,   0.00,  0.00, 0 ),

	// rimmed bottle necked cases  bottom            rim (bottom/top)                  base (bottom/top)                  neck(bottom/top)
	cd( Caliber::_7_62X54R,          11.18, 0.00,  0,  14.48, 0.60, 0,  14.48, 1.60, 0,  12.37, 1.60, 0,  11.61, 37.70, 0,   8.53, 44.30, 0,  8.53,  53.72, 0,   0.00,  0.00, 0 ),
};


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
