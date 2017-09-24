#ifndef __LightWaveSceneLoader_HPP__
#define __LightWaveSceneLoader_HPP__


#include "LWS_Items.hpp"


namespace amorphous
{


class CTextFileScanner;


class LWSceneInfo
{
public:
	int m_Version;
	int m_RenderRangeType;
	int m_FirstFrame;
	int m_LastFrame;
	int m_FrameStep;
//	int m_RenderRangeArbitrary;
	int m_PreviewFirstFrame;
	int m_PreviewLastFrame;
	int m_PreviewFrameStep;
	int m_CurrentFrame;
	int m_FramesPerSecond;

	LWSceneInfo()
	{
		memset( this, 0, sizeof(LWSceneInfo) );
	}

	bool LoadSceneInfo( CTextFileScanner& scanner );
};


class LightWaveSceneLoader
{
	std::vector< std::shared_ptr<LWS_ObjectLayer> > m_vecObjectLayer;

	std::vector< std::shared_ptr<LWS_Light> > m_vecLight;

	std::vector< std::shared_ptr<LWS_Bone> > m_vecpBone;

//	std::vector< std::shared_ptr<LWS_Camera> > m_vecpCamera;

	// borrowed reference
	std::vector<LWS_Item *> m_vecpItem;

	float m_afAmbientColor[3];	//0:red / 1:green / 2:blue

	float m_fAmbientIntensity;

	LWS_Fog m_Fog;

	LWSceneInfo m_SceneInfo;

	bool LoadFogDataBlock( CTextFileScanner& scanner );

	/// create links from parent items to child items
	void  AddParentToChildLinks();

//	void TryLoadingFogDataBlock(char* pcLine, FILE* fp);

private:

	void UpdateItemTrees();

public:

	LightWaveSceneLoader();

	bool LoadFromFile( const std::string& filepath );

	// object layers

	int GetNumObjectLayers() { return (int)m_vecObjectLayer.size(); }

//	std::shared_ptr<LWS_ObjectLayer> GetObjectLayer(int i);
	LWS_ObjectLayer* GetObjectLayer(int i);

	// lights

	int GetNumLights() const { return (int)m_vecLight.size(); }

//	std::shared_ptr<LWS_Light> GetLight(int i);
	LWS_Light* GetLight(int i);

	// bones

	int GetNumBones() const { return (int)m_vecpBone.size(); }

	std::shared_ptr<LWS_Bone> GetBone(int i);

	const std::vector< std::shared_ptr<LWS_Bone> >& Bones() const{ return m_vecpBone; }

	std::vector< std::shared_ptr<LWS_Bone> > GetRootBones();

	inline LWS_Item *GetItemByID( int item_id );

	LWS_Fog *GetFog();

	const float* GetAmbientColor() const { return m_afAmbientColor; }
	float GetAmbientIntensity() const { return m_fAmbientIntensity; }

	const LWSceneInfo& GetSceneInfo() const { return m_SceneInfo; }

	void Clear();
};

//=============================== inline  implementations ===============================

inline LWS_Item *LightWaveSceneLoader::GetItemByID( int item_id )
{
	const int num_items = (int)m_vecpItem.size();
	for( int i=0; i<num_items; i++ )
	{
		if( m_vecpItem[i]->GetItemID() == item_id )
			return m_vecpItem[i];
	}

	return NULL;
}


} // amorphous



#endif /* __LightWaveSceneLoader_HPP__ */
