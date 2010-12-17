#ifndef __BUMPWEIGHTVERTEX_H__
#define __BUMPWEIGHTVERTEX_H__

// BUMPWEIGHTVERTEX
//  for bump map and vertex blending


#include <d3dx9.h>
#include "../TextureCoord.hpp"


const D3DVERTEXELEMENT9 BUMPWEIGHTVERTEX_DECLARATION[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0 }, // 12
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,        0 }, //  4
    { 0, 16, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0 }, // 12
    { 0, 28, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,      0 }, // 12
    { 0, 40, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,     0 }, // 12
	{ 0, 52, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0 }, //  8
	{ 0, 60, D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 }, //  4
	{ 0, 64, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,  0 }, // 16

//	{ 0, 52, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0 }, // 12
//	{ 0, 64, D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 }, //  4
//	{ 0, 68, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,  0 }, // 16

    D3DDECL_END()
};


struct BUMPWEIGHTVERTEX
{
    D3DXVECTOR3 vPosition;
    DWORD       color;		// diffuse color
    D3DXVECTOR3 vNormal;			// vertex normal
    D3DXVECTOR3 vTangent;		// tangent
    D3DXVECTOR3 vBinormal;		// binormal
//    D3DXVECTOR3 texcoord; // ATI original
	TEXCOORD2	tex;
	DWORD       matrixIndices;  // DWORD is best
    D3DXVECTOR4 matrixWeights;
};

//#define D3DFVF_BUMPWEIGHTVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX0)


#endif  /*  __BUMPWEIGHTVERTEX_H__  */

/*
//-----------------------------------------------------------------------------
// LoadSKLShaders()
// Load the shaders for skeleton object.
//-----------------------------------------------------------------------------
void RenderObject::LoadSKLShaders()
{

    SAFE_RELEASE( m_pVertexDeclaration);

    D3DVERTEXELEMENT9 decl[] =
    {
        { 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0 },//size =12
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,        0 },//size=4
        { 0, 16, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0 },//size =12
        { 0, 28, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,      0 },//size =12
        { 0, 40, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,     0 },//size =12
        { 0, 52, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0 },//size =12
        { 0, 64, D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },//size=4
        { 0, 68, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,  0 },//size=16
        D3DDECL_END()
    };
    // load the start and ending locations of where we want the blended weights to live
    int maxVConstReg = 256;
    m_ShaderInfo.m_pVConstantStoreRange[0] = maxVConstReg - (m_pSkeleton->numBones*4);
    m_ShaderInfo.m_pVConstantStoreRange[1] = maxVConstReg;

    m_ShaderInfo.m_pPConstantStoreRange[0] = -1;
    m_ShaderInfo.m_pPConstantStoreRange[1] = -1;

    LPD3DXEFFECT tempEffect=NULL;
    if( S_OK != LoadShaderFile(m_pd3dDevice, m_pcFxFileName, &tempEffect, &(m_pVertexDeclaration) ,decl))
    {
        //-----------------------------------------------//
        // Problem loading this shader , report to user. //
        //-----------------------------------------------//
        Msg(TEXT("Could not load default effects shader file."),0);
        
    }
    else
    {
        SAFE_RELEASE( m_pEffect);
        m_pEffect = tempEffect;

        LoadTechniqeDescriptions( m_pEffect , &m_ShaderInfo.m_FXInfo );
        ComputeMaxTechniqueNameLength();
    }
}
*/