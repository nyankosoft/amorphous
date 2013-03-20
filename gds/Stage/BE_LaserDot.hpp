#ifndef __BE_LaserDot_H__
#define __BE_LaserDot_H__


#include "BaseEntity.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "Graphics/3DRect.hpp"


namespace amorphous
{


//#define BE_LASERDOT_OFF	0
#define BE_LASERDOT_ON	1
#define BE_LASERDOT_OFF_SURFACE	2


class CBE_LaserDot : public BaseEntity
{
private:

	C3DRect<SFloatRGBAColor,1> m_LaserDotRect;

	std::string m_LaserDotTexFilepath;
	TextureHandle m_LaserDotTex;

public:

	CBE_LaserDot();

//	~CBE_LaserDot();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	// behavior in one frame
	void Draw(CCopyEntity* pCopyEnt);
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	// void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) {}

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_LASERDOT; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};

} // namespace amorphous



#endif  /*  __BE_LaserDot_H__  */
