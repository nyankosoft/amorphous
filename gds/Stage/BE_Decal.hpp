#ifndef	__BE_Decal_H__
#define __BE_Decal_H__

#include "BaseEntity.hpp"
#include "../Graphics/2DPrimitive/2DRect.hpp"
//#include "../Graphics/2DPrimitive/2DRectSet.hpp"
#include "../Graphics/Mesh/RectSetMesh.hpp"


namespace amorphous
{


class CBE_Decal : public BaseEntity
{
	enum param { NUM_MAX_DECALS = 128 };

//	static int ms_iNumTotalUsedDecals;
	int m_NumTotalUsedDecals;

	std::string m_DecalTextureFilepath;
	TextureHandle m_DecalTexture;

	C2DRect m_DecalRect;

//	C2DRectSet m_DecalRectSet;

	RectSetMesh m_DecalRectsMesh;

	float m_fDecalRadius;

	/// number of segments along each axis on a texture
	/// used for generating random patterns
	int m_iNumSegments;

private:

	void UpdateRects();

public:

	CBE_Decal();
	~CBE_Decal();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	void Act(CCopyEntity* pCopyEnt);
	void Draw(CCopyEntity* pCopyEnt);
	//void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	//void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	void SweepRender();

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_DECAL; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};
} // namespace amorphous



#endif /*  __BE_Decal_H__  */
