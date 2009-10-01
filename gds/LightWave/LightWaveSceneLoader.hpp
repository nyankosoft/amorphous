#include "LWS_Items.hpp"


class CTextFileScanner;


class CLightWaveSceneLoader
{
	std::vector<CLWS_ObjectLayer> m_vecObjectLayer;

	std::vector<CLWS_Light> m_vecLight;

	std::vector< boost::shared_ptr<CLWS_Bone> > m_vecpBone;

	//camera;

	//bone;

	std::vector<CLWS_Item *> m_vecpItem;

	float m_afAmbientColor[3];	//0:red / 1:green / 2:blue

	float m_fAmbientIntensity;

	CLWS_Fog m_Fog;


	bool LoadFogDataBlock( CTextFileScanner& scanner );

	/// create links from parent items to child items
	void  AddParentToChildLinks();

//	void TryLoadingFogDataBlock(char* pcLine, FILE* fp);

private:

	void UpdateItemTrees();

public:

	CLightWaveSceneLoader();

	bool LoadFromFile( const char* pcLWS_Filename );

	// object layers

	int GetNumObjectLayers() { return (int)m_vecObjectLayer.size(); }

	CLWS_ObjectLayer* GetObjectLayer(int i);

	// lights

	int GetNumLights() const { return (int)m_vecLight.size(); }

	CLWS_Light* GetLight(int i);

	// bones

	int GetNumBones() const { return (int)m_vecpBone.size(); }

	boost::shared_ptr<CLWS_Bone> GetBone(int i);

	const std::vector< boost::shared_ptr<CLWS_Bone> >& Bones() const{ return m_vecpBone; }

	CLWS_Fog *GetFog();

	const float* GetAmbientColor() const { return m_afAmbientColor; }
	float GetAmbientIntensity() const { return m_fAmbientIntensity; }
};
