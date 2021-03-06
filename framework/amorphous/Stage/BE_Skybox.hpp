#ifndef __BE_Skybox_H__
#define __BE_Skybox_H__


#include "BaseEntity.hpp"
#include "../Graphics/FloatRGBAColor.hpp"


namespace amorphous
{


class CBE_Skybox : public BaseEntity
{
private:

	/// load texture separately from the mesh
	std::string m_SkyboxTextureFilepath;
	TextureHandle m_SkyboxTexture;

	SFloatRGBAColor m_SkyboxTextureCenterPixelColor;

	// BaseEntity::m_MeshProperty is used for the mesh of the skybox

public:

	CBE_Skybox();
	virtual ~CBE_Skybox();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	void Draw(CCopyEntity* pCopyEnt); 
//	void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_SKYBOX; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	bool LoadSkyboxTexture( const std::string& texture_filename );

	TextureHandle GetSkyboxTexture() { return m_SkyboxTexture; }

	/// Returns a single color for simple fog effect
	/// Right now, it simply returns the texel at the center of the skybox texture, the texel which mapped near the horizon
	/// on the skybox mesh.
	bool GetFogColor( SFloatRGBAColor& dest );

	enum Type
	{
		BE_SKYBOX_NORMAL = 0,
		BE_SKYBOX_HIGH_ALTITUDE,
		NUM_SKYBOX_TYPES
	};
};
} // namespace amorphous



#endif  /*  __BE_Skybox_H__  */
