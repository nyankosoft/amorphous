
#include "LWS_Items.h"


class CTextFileScanner;


class CLightWaveSceneLoader
{
	vector<CLWS_ObjectLayer> m_vecObjectLayer;

	vector<CLWS_Light> m_vecLight;

	//camera;

	//bone;

	vector<CLWS_Item *> m_vecpItem;

	float m_afAmbientColor[3];	//0:red / 1:green / 2:blue

	float m_fAmbientIntensity;

	CLWS_Fog m_Fog;


	bool LoadFogDataBlock( CTextFileScanner& scanner );

	/// create links from parent items to child items
	void  AddParentToChildLinks();

//	void TryLoadingFogDataBlock(char* pcLine, FILE* fp);

public:

	CLightWaveSceneLoader();

	bool LoadFromFile( const char* pcLWS_Filename );

	int GetNumObjectLayers() { return m_vecObjectLayer.size(); }

	CLWS_ObjectLayer* GetObjectLayer(int i);

	CLWS_Fog *GetFog();

	float* GetAmbientColor() { return m_afAmbientColor; }
	float GetAmbientIntensity() { return m_fAmbientIntensity; }
	int GetNumLights() { return m_vecLight.size(); }
	CLWS_Light* GetLight(int i);
};