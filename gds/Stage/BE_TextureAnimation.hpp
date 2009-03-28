#ifndef	__BASEENTITY_TEXTUREANIMATION_H__
#define __BASEENTITY_TEXTUREANIMATION_H__

#include "BaseEntity.hpp"

#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/TextureHandle.hpp"


class CBE_TextureAnimation : public CBaseEntity
{
public:

	enum eAnimTypeFlag
	{
		TA_BILLBOARD	= (1 << 0),	///< texture animation which always facing to the camera
		TA_GROW			= (1 << 1)	///< texture animation that expands as the time passes
	};


private:

	/// texture for billboard animation
	std::string m_AnimTextureFilepath;
	CTextureHandle m_AnimTexture;

	float m_fTotalAnimationTime;

	int m_iNumTextureSegments;

	int m_iTextureWidth;

	/// alpha blend option
	int m_iDestBlend;

	/// how much the texture expands over the course of the animation
	float m_fExpansionFactor;

	/// texture animation type
	unsigned int m_AnimTypeFlag;

	/// use 2 billboards to achieve smooth transition between frames
	TEXTUREVERTEX m_avRectangle1[4];
	TEXTUREVERTEX m_avRectangle2[4];

	CShaderTechniqueHandle m_aShaderTechHandle[2];

public:

	CBE_TextureAnimation();
	~CBE_TextureAnimation();
	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	void Act(CCopyEntity* pCopyEnt);
	void Draw(CCopyEntity* pCopyEnt);
	//void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	//void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);
	void SetTextureCoord( TEXTUREVERTEX *pavRactangle, int iCurrentFrame );

//	void DrawGlare(CCopyEntity* pCopyEnt) { Draw( pCopyEnt ); }

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_TEXTUREANIMATION; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

};


#endif /*  __BASEENTITY_TEXTUREANIMATION_H__  */
