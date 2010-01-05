#ifndef __LightWaveSceneLoader_HPP__
#define __LightWaveSceneLoader_HPP__


#include "LWS_Items.hpp"


class CTextFileScanner;


class CLWSceneInfo
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

	CLWSceneInfo()
	{
		memset( this, 0, sizeof(CLWSceneInfo) );
	}

	bool LoadSceneInfo( CTextFileScanner& scanner );
};


class CLightWaveSceneLoader
{
	std::vector< boost::shared_ptr<CLWS_ObjectLayer> > m_vecObjectLayer;

	std::vector< boost::shared_ptr<CLWS_Light> > m_vecLight;

	std::vector< boost::shared_ptr<CLWS_Bone> > m_vecpBone;

//	std::vector< boost::shared_ptr<CLWS_Camera> > m_vecpCamera;

	// borrowed reference
	std::vector<CLWS_Item *> m_vecpItem;

	float m_afAmbientColor[3];	//0:red / 1:green / 2:blue

	float m_fAmbientIntensity;

	CLWS_Fog m_Fog;

	CLWSceneInfo m_SceneInfo;

	bool LoadFogDataBlock( CTextFileScanner& scanner );

	/// create links from parent items to child items
	void  AddParentToChildLinks();

//	void TryLoadingFogDataBlock(char* pcLine, FILE* fp);

private:

	void UpdateItemTrees();

public:

	CLightWaveSceneLoader();

	bool LoadFromFile( const std::string& filepath );

	// object layers

	int GetNumObjectLayers() { return (int)m_vecObjectLayer.size(); }

//	boost::shared_ptr<CLWS_ObjectLayer> GetObjectLayer(int i);
	CLWS_ObjectLayer* GetObjectLayer(int i);

	// lights

	int GetNumLights() const { return (int)m_vecLight.size(); }

//	boost::shared_ptr<CLWS_Light> GetLight(int i);
	CLWS_Light* GetLight(int i);

	// bones

	int GetNumBones() const { return (int)m_vecpBone.size(); }

	boost::shared_ptr<CLWS_Bone> GetBone(int i);

	const std::vector< boost::shared_ptr<CLWS_Bone> >& Bones() const{ return m_vecpBone; }

	std::vector< boost::shared_ptr<CLWS_Bone> > GetRootBones();

	inline CLWS_Item *GetItemByID( int item_id );

	CLWS_Fog *GetFog();

	const float* GetAmbientColor() const { return m_afAmbientColor; }
	float GetAmbientIntensity() const { return m_fAmbientIntensity; }

	const CLWSceneInfo& GetSceneInfo() const { return m_SceneInfo; }

	void Clear();
};

//=============================== inline  implementations ===============================

inline CLWS_Item *CLightWaveSceneLoader::GetItemByID( int item_id )
{
	const int num_items = (int)m_vecpItem.size();
	for( int i=0; i<num_items; i++ )
	{
		if( m_vecpItem[i]->GetItemID() == item_id )
			return m_vecpItem[i];
	}

	return NULL;
}



#endif /* __LightWaveSceneLoader_HPP__ */
