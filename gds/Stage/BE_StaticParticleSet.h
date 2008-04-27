#ifndef __BE_STATICPARTICLESET_H__
#define __BE_STATICPARTICLESET_H__

#include "BE_ParticleSet.h"
#include "3DCommon/FVF_BillboardVertex.h"
#include "3DCommon/TextureHandle.h"
#include "Support/FixedVector.h"


#include <vector>
#include <string>


class CBE_StaticParticleSet : public CBE_ParticleSet
{
private:

	enum params
	{
		NUM_MAX_PARTICLES_PER_VB = 2048,
		NUM_MAX_VERTEXBUFFERS = 8,
	};

//	std::vector<LPDIRECT3DVERTEXBUFFER9> m_vecpVB;
	TCFixedVector<LPDIRECT3DVERTEXBUFFER9,NUM_MAX_VERTEXBUFFERS> m_vecpVB;

	LPDIRECT3DVERTEXBUFFER9 m_pVB; // Buffer to hold vertices
	LPDIRECT3DINDEXBUFFER9  m_pIB;

	CTextureHandle m_BillboardTex;

	std::vector<STATICBILLBOARDVERTEX> m_vecParticleVertex;

    LPDIRECT3DVERTEXDECLARATION9 m_pParticleVertexDeclaration;

	CShaderTechniqueHandle m_aShaderTechHandle[2];

public:

	CBE_StaticParticleSet();
	virtual ~CBE_StaticParticleSet();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	virtual void Draw(CCopyEntity* pCopyEnt); 
	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_STATICPARTICLESET; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

    HRESULT InitIndexBuffer();

	void CommitStaticParticles();
};


#endif  /*  __BE_STATICPARTICLESET_H__  */
