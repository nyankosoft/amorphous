#ifndef	__BASEENTITY_TEXTUREANIMATION_H__
#define __BASEENTITY_TEXTUREANIMATION_H__


#include "BaseEntity.hpp"
#include "Graphics/Mesh/RectSetMesh.hpp"


/// A note about alpha blending: the class does not have source/dest alpha blend settings
/// because it always uses pre-multiplied alpha, which is the default blend setting
/// of the mesh class.
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

	/// how much the texture expands over the course of the animation
	float m_fExpansionFactor;

	/// texture animation type
	unsigned int m_AnimTypeFlag;

	/// use 2 billboards to achieve smooth transition between frames
	CRectSetMesh m_FrontRectMesh;
	CRectSetMesh m_RearRectMesh;

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
	void SetTextureCoord( CRectSetMesh& rect_mesh, int iCurrentFrame );

//	void DrawGlare(CCopyEntity* pCopyEnt) { Draw( pCopyEnt ); }

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_TEXTUREANIMATION; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};


#endif /*  __BASEENTITY_TEXTUREANIMATION_H__  */
