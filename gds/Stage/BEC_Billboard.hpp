#ifndef  __BEC_BILLBOARD_H__
#define  __BEC_BILLBOARD_H__


#include "Graphics/AlphaBlend.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/FVF_BillboardVertex.h"
#include "Graphics/BillboardArrayMesh.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"

#include "Support/Serialization/Archive.hpp"
using namespace GameLib1::Serialization;


class CStage;
class CTextFileScanner;
class CBillboardArrayMesh;


class CBEC_Billboard
{
	inline void DrawPrimitives( int num_rects, int group, int num_rects_per_group );

protected:

	enum eParams
	{
//		NUM_MAX_BILLBOARDS_PER_ENTITY = 64,
//		NUM_MAX_BILLBOARDS_PER_ENTITY = 128,	// 2:05 2007/05/11
		NUM_MAX_BILLBOARDS_PER_ENTITY = 1024,
	};

	std::string m_BillboardTextureFilepath;
	CTextureHandle m_BillboardTexture;

	TEXTUREVERTEX m_avBillboardRect[NUM_MAX_BILLBOARDS_PER_ENTITY * 4];

	BILLBOARDVERTEX m_avBillboardRect_S[NUM_MAX_BILLBOARDS_PER_ENTITY * 4];

//	DWORD m_DestAlphaBlendMode;
	AlphaBlend::Mode m_DestAlphaBlendMode;

	CBillboardArrayMesh *m_pBillboardArrayMesh;

	C3DMeshModelArchive m_BillboardArrayMeshArchive;

	CShaderTechniqueHandle m_aShaderTechHandle[4];

	int m_Type;

	enum eType
	{
		TYPE_BILLBOARDARRAYMESH,        ///< billboard vertices are stored in vertex buffer. VB is not shared by billboard groups
		TYPE_BILLBOARDARRAYMESH_SHARED, ///< push billboard vertices at every draw calls. single VB is shared by billboard groups
		TYPE_RECT_ARRAY_AND_INDICES,    ///< push billboard vertices at every draw calls
		NUM_TYPES
	};

public:

//	void LoadBillboardArrayMesh( float billboard_radius, int num_tex_divisions );
	void LoadBillboardArrayMesh( float billboard_radius, int num_groups, int num_billboards_par_groups, int num_tex_divisions );

	void SetBillboardType( int type ) { m_Type = type; }

public:

	CBEC_Billboard();

	~CBEC_Billboard();

	virtual void Init();

	void Release();

	inline void SetTexCoord( int rect_index, int vert_index, const TEXCOORD2& tex );

	/// render billboard rects.
	/// called after all the necessary transformations for the billboard rects
	/// has been done
	void DrawBillboards( int num_rects, int group, int num_rects_per_group, CStage *pStage );

//	inline TEXTUREVERTEX*& GetBillboardRect() { return m_avBillboardRect; }

	inline AlphaBlend::Mode GetDestAlphaBlendMode() const { return m_DestAlphaBlendMode; }

	inline void SetDestAlphaBlendMode( AlphaBlend::Mode mode ) { m_DestAlphaBlendMode = mode; }

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

//	virtual void Serialize( IArchive& ar, const unsigned int version );

	void SerializeBillboardProperty( IArchive& ar, const unsigned int version );

	void LoadGraphicsComponentResources( const CGraphicsParameters& rParam );

	void ReleaseGraphicsComponentResources();

};


inline void CBEC_Billboard::SetTexCoord( int rect_index, int vert_index, const TEXCOORD2& tex )
{
//	m_avBillboardRect[ rect_index*4 + vert_index ].tex = tex;
	m_avBillboardRect_S[ rect_index*4 + vert_index ].tex = tex;
}


#endif		/*  __BEC_BILLBOARD_H__  */
