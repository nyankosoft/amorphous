#ifndef  __ShaderLightManager_H__
#define  __ShaderLightManager_H__


#include <d3dx9.h>

#include "../fwd.hpp"
#include "../HemisphericLight.hpp"


#include "Support/FixedVector.hpp"


//typedef TCFixedVector<class A, int B> fixed_vector<A,B>;
#define fixed_vector TCFixedVector


class CLightCache
{
public:
	fixed_vector<CDirectionalLight,8> vecDirecitonalLight;
	fixed_vector<CPointLight,8> vecPointLight;
	fixed_vector<CHemisphericDirectionalLight,8> vecHSDirecitonalLight;
	fixed_vector<CHemisphericPointLight,8> vecHSPointLight;

public:

	void Clear()
	{
		vecDirecitonalLight.resize(0);
		vecPointLight.resize(0);
		vecHSDirecitonalLight.resize(0);
		vecHSPointLight.resize(0);
	}
};


class CShaderLightManager
{
public:

	CShaderLightManager() {}
	virtual ~CShaderLightManager() {}

	virtual void SetAmbientLight( const CAmbientLight& light ) {}
	virtual void SetDirectionalLight( const CDirectionalLight& light ) {}
	virtual void SetPointLight( const CPointLight& light ) {}
	virtual void SetHemisphericDirectionalLight( const CHemisphericDirectionalLight& light ) {}
	virtual void SetHemisphericPointLight( const CHemisphericPointLight& light ) {}
//	virtual void SetTriDirectionalLight( const CTriDirectionalLight& light ) {}
//	virtual void SetTriPointLight( const CTriPointLight& light ) {}

	virtual void ClearLights() {}

	virtual void CommitChanges() {}

};


class CShaderLightParamsWriter : public CLightVisitor
{
	CShaderLightManager *m_pShaderLightManager;

public:

	CShaderLightParamsWriter( CShaderLightManager *pMgr )
	:
	m_pShaderLightManager(pMgr)
	{}

//	void VisitLight( CLight& light ) {}
	void VisitAmbientLight( CAmbientLight& ambient_light ) { m_pShaderLightManager->SetAmbientLight( ambient_light ); }
	void VisitPointLight( CPointLight& point_light )       { m_pShaderLightManager->SetPointLight( point_light ); }
	void VisitDirectionalLight( CDirectionalLight& directional_light )        { m_pShaderLightManager->SetDirectionalLight( directional_light ); }
	void VisitHemisphericPointLight( CHemisphericPointLight& hs_point_light ) { m_pShaderLightManager->SetHemisphericPointLight( hs_point_light ); }
	void VisitHemisphericDirectionalLight( CHemisphericDirectionalLight& hs_directional_light ) { m_pShaderLightManager->SetHemisphericDirectionalLight(  hs_directional_light ); }
//	void VisitTriPointLight( CTriPointLight& tri_point_light ) { m_pShaderLightManager->Set?Light(  ); }
//	void VisitTriDirectionalLight( CTriDirectionalLight& tri_directional_light ) { m_pShaderLightManager->Set?Light(  ); }
};


#endif		/*  __ShaderLightManager_H__  */
