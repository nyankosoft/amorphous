//======================================================================================//
// filename: FixedFuncShader.fxh                                                        //
//                                                                                      //
// author:   Pedro V. Sander                                                            //
//           ATI Research, Inc.                                                         //
//           3D Application Research Group                                              //
//           email: psander@ati.com                                                     //
//                                                                                      //
// Description: FixedFuncShader defines for both .fx and .cpp                           //
//                                                                                      //
//======================================================================================//
//   (C) 2003 ATI Research, Inc.  All rights reserved.                                  //
//======================================================================================//

#define NUM_LIGHTS               5

#define LIGHT_TYPE_NONE          0
#define LIGHT_TYPE_POINT         1
#define LIGHT_TYPE_SPOT          2
#define LIGHT_TYPE_DIRECTIONAL   3
#define LIGHT_NUM_TYPES          4

#define TEX_TYPE_NONE            0
#define TEX_TYPE_CUBEMAP         1
#define TEX_NUM_TYPES            2