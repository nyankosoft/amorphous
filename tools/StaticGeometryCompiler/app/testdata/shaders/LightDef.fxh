#ifndef __LIGHTDEF_H__
#define __LIGHTDEF_H__


//-----------------------------------------------------------------------------
// light types
//-----------------------------------------------------------------------------

#define LIGHT_TYPE_NONE          0
#define LIGHT_TYPE_POINT         1
#define LIGHT_TYPE_SPOT          2
#define LIGHT_TYPE_DIRECTIONAL   3
#define LIGHT_NUM_TYPES          4


//-----------------------------------------------------------------------------
// variables
//-----------------------------------------------------------------------------

//initial and range of directional, point and spot lights within the light array
int iLightDirIni   = 0;
int iLightDirNum   = 0;
int iLightPointIni = 1;
int iLightPointNum = 0;
//int iLightSpotIni;
//int iLightSpotNum;


//-----------------------------------------------------------------------------
// light structure
//-----------------------------------------------------------------------------

struct CLight
{
   int iType;
   float3 vPos;		// position in world space
   float3 vPosVS;	// position in view space
   float3 vDir;
   float4 vAmbient;
   float4 vDiffuseUpper;
   float4 vDiffuseLower;
   float4 vSpecular;
   float  fRange;
   float3 vAttenuation; //1, D, D^2;
   float3 vSpot;        //cos(theta/2), cos(phi/2), falloff
};


#endif /* __LIGHTDEF_H__ */