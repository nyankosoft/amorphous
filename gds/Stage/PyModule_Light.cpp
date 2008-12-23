#include "PyModule_Light.h"
#include "PyModule_Stage.h"

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"
#include "3DCommon/FloatRGBColor.h"

#include "BE_DirectionalLight.h"
#include "BE_PointLight.h"
#include "Stage.h"
#include "EntitySet.h"

#include "CopyEntityDesc.h"

#include "Support/Macro.h"
#include "Support/Vec3_StringAux.h"
#include "Support/Log/DefaultLog.h"

#include "../base.h"

using namespace std;


class  CScriptGenMode
{
public:
	enum Name
	{
		CREATE,
		LOAD,
		NUM_MODES
	};
};


class CStageAttributeHolder
{
	enum DefaultBaseEntityHandleIndex
	{
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOTLIGHT,
		HS_DIRECTIONAL_LIGHT,
		HS_POINT_LIGHT,
		NUM_DEFAULT_BASE_ENTITY_HANDLES
	};

public:

	boost::weak_ptr<CStage> m_pStage;

	CBaseEntityHandle m_aBaseEntityHandle[NUM_DEFAULT_BASE_ENTITY_HANDLES];
};


CCopyEntity *CreateEntityFromDesc( CCopyEntityDesc& desc )
{
	CStage *pStage = GetStageForScriptCallback();

	if( pStage )
		return pStage->CreateEntity( desc );
	else
		return NULL;
};



/// row:    holds light groups
/// column: holds object groups (lighting groups)
C2DArray<char> m_LightToObject;

int init_light_groups()
{
	const int num_light_groups = 16;
	const int num_ligting_groups = 16;
	m_LightToObject.resize( num_light_groups, num_ligting_groups );

	return 0;
}


#define RETURN_PYNONE_IF_NO_STAGE()	 if( !GetStageForScriptCallback() )        { Py_INCREF( Py_None );	return Py_None;	}
#define RETURN_PYNONE_IF_NO_TARGET() if( !IsValidEntity(GetEntityForLight()) ) { Py_INCREF( Py_None );	return Py_None;	}


CCopyEntity *gs_pEntityForLight = NULL;

inline CCopyEntity *GetEntityForLight() { return gs_pEntityForLight; }


void gsf::py::light::SetEntityForLight( CCopyEntity* pEntity )
{
	gs_pEntityForLight = pEntity;
}


using namespace gsf::py::light;

/*
struct NamedEntityCache
{
	string name;
	CCopyEntity *pEntity;
};

#define NAMED_ENTITY_CACHE_SIZE	4
static NamedEntityCache g_NamedEntityCache[NAMED_ENTITY_CACHE_SIZE];
*/


static CCopyEntity *GetEntityByName( const char* entity_name )
{
	if( GetStageForScriptCallback() )
        return GetStageForScriptCallback()->GetEntitySet()->GetEntityByName(entity_name);
	else
        return NULL;
}


PyObject* GenerateDirectionalLight( PyObject* self, PyObject* args, CScriptGenMode::Name mode )
{
	CLightEntityDesc desc( CLight::DIRECTIONAL );
	char *base_name = "";
	char *light_name = "";
	int shadow_for_light = 1; // true(1) by default
	Vector3 dir = Vector3(0,-1,0); // default direction = vertically down
	SFloatRGBAColor color = SFloatRGBAColor(1,1,1,1);

	int result = 0;
	
	CBaseEntityHandle basehandle( "DiretionalLight" );
	if( mode == CScriptGenMode::LOAD )
	{
		PyArg_ParseTuple( args, "s|sfffffffdd",
			&base_name,
			&light_name, &dir.x, &dir.y, &dir.z,
			&color.fRed, &color.fGreen, &color.fBlue,
			&desc.fIntensity,
			&desc.LightGroup,
			&shadow_for_light );
	}
	else
	{
		PyArg_ParseTuple( args, "|sfffffffdd",
			&light_name, &dir.x, &dir.y, &dir.z,
			&color.fRed, &color.fGreen, &color.fBlue,
			&desc.fIntensity,
			&desc.LightGroup,
			&shadow_for_light );
	}

	color.fAlpha = 1.0f;

	desc.pBaseEntityHandle = &basehandle;
	desc.strName = light_name;
	desc.WorldPose.matOrient.SetColumn( 2, dir );
	desc.aColor[0] = color;

	CreateEntityFromDesc( desc );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateDirectionalLight( PyObject* self, PyObject* args )
{
	return GenerateDirectionalLight( self, args, CScriptGenMode::CREATE );
}


PyObject* LoadDirectionalLight( PyObject* self, PyObject* args )
{
	return GenerateDirectionalLight( self, args, CScriptGenMode::LOAD );
}


PyObject* gsf::py::light::CreatePointLight( PyObject* self, PyObject* args )
{
	CLightEntityDesc desc( CLight::POINT );
	char *light_name = "";
	int shadow_for_light = 1; // true(1) by default
	Vector3 pos = Vector3(0,2,0); // default position = 2m above the world origin
	SFloatRGBAColor color = SFloatRGBAColor(1,1,1,1);
	float *pAttenu = desc.afAttenuation;

	int result = PyArg_ParseTuple( args, "|sfffffffdd",
		&light_name, &pos.x, &pos.y, &pos.z,
		&color.fRed, &color.fGreen, &color.fBlue,
		&desc.fIntensity,
		&pAttenu[0],
		&pAttenu[1],
		&pAttenu[2],
		&desc.LightGroup, &shadow_for_light );

	color.fAlpha = 1.0f;

	desc.strName = light_name;
	desc.WorldPose.vPosition = pos;
	desc.aColor[0] = color;

	CreateEntityFromDesc( desc );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateSpotlight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}
/*
PyObject* gsf::py::light::LoadHSDirectionalLight( PyObject* self, PyObject* args )
{
}
*/
PyObject* gsf::py::light::CreateHSDirectionalLight( PyObject* self, PyObject* args )
{
	char *light_name = "";
	int light_group = 0;
	int shadow_for_light = 1; // true(1) by default
	Vector3 dir = Vector3(0,-1,0); // default direction = vertically down
	SFloatRGBAColor uc = SFloatRGBAColor(1,1,1,1);
	SFloatRGBAColor lc = SFloatRGBAColor(0,0,0,1);
	float fIntensity = 1.0f;

	int result = PyArg_ParseTuple( args, "|sffffffffffdd",
		&light_name, &dir.x, &dir.y, &dir.z,
		&uc.fRed, uc.fGreen, &uc.fBlue,  &lc.fRed, lc.fGreen, &lc.fBlue,
		&light_group, &shadow_for_light );

	uc.fAlpha = lc.fAlpha = 1.0f;

	CLightEntityDesc desc;
	desc.LightType = CLight::HEMISPHERIC_DIRECTIONAL;
	desc.strName = light_name;
	desc.LightGroup = light_group;
	desc.fIntensity = fIntensity;
	desc.WorldPose.matOrient.SetColumn( 2, dir );
	desc.aColor[0] = uc;
	desc.aColor[1] = lc;

	CreateEntityFromDesc( desc );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateHSPointLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateHSSpotlight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateTriDirectionalLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateTriPointLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateTriSpotlight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetAttenuationFactors( PyObject* self, PyObject* args )
{
	g_Log.Print( "Light.SetAttenuFactors() - called" );

	float a[3];

	int result = PyArg_ParseTuple( args, "fff", &a[0], &a[1], &a[2] );

	RETURN_PYNONE_IF_NO_TARGET()
	RETURN_PYNONE_IF_NO_STAGE()

	CLightEntity *pLightEntity = NULL;
//		= GetStageForScriptCallback()->GetEntitySet()->GetLightEntity( GetEntityForLight()->iExtraDataIndex );

	if( pLightEntity )
		pLightEntity->SetAttenuationFactors( a[0], a[1], a[2] );

	CBE_Light::SetAttenuationFactors( GetEntityForLight(), a[0], a[1], a[2] );

    Py_INCREF( Py_None );
	return Py_None;
}


/**
 \param index 0:upper hemisphere color, 1:??? color, 2: lower hemisphere color


*/
static void SetLightColor( int index, const SFloatRGBColor& color )
{
	if( !GetStageForScriptCallback() )
		return;

	CLightEntity *pLightEntity = NULL;
//		= GetStageForScriptCallback()->GetEntitySet()->GetLightEntity( GetEntityForLight()->iExtraDataIndex );

	if( pLightEntity )
		pLightEntity->SetColor( index, color );

	const Vector3 vColor = Vector3( color.fRed, color.fGreen, color.fBlue );

	switch( index )
	{
	case 0: GetEntityForLight()->v1 = vColor; break;
	case 1: GetEntityForLight()->v2 = vColor; break;
	case 2: GetEntityForLight()->v3 = vColor; break;
	default:
		g_Log.Print( WL_ERROR, "SetColor() - invalid light color index: %d. Must be [0,2]" );
	}
}


PyObject* gsf::py::light::SetColorU32( PyObject* self, PyObject* args )
{
	int index;
	U32 color;	// 32-bit ARGB color (8 bits for each channel)
	int result = PyArg_ParseTuple( args, "ik", &index, &color );

	SFloatRGBColor dest_color;
	dest_color.SetARGB32( color );

	RETURN_PYNONE_IF_NO_TARGET()

	SetLightColor( index, dest_color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::SetColor( PyObject* self, PyObject* args )
{
	int index;
	SFloatRGBColor color;
	int result = PyArg_ParseTuple( args, "ifff", &index, &color.fRed, &color.fGreen, &color.fBlue );

	RETURN_PYNONE_IF_NO_TARGET()

	SetLightColor( index, color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::SetTargetEntity( PyObject* self, PyObject* args )
{
	g_Log.Print( "Light.SetTargetEntity() - called" );

	RETURN_PYNONE_IF_NO_STAGE()

	char *entity_name;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CCopyEntity* pEntityForLight = GetEntityByName( entity_name );

	// TODO: what if the focus target entity is created later
	if( IsValidEntity(pEntityForLight) )
	{
		SetEntityForLight( pEntityForLight );
	}
	else
	{
//		g_Log.Print( "" );
		Py_INCREF( Py_None );
		return Py_None;
	}


	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::SetPosition( PyObject* self, PyObject* args )
{
	Vector3 pos;
	int result = PyArg_ParseTuple( args, "fff", &pos.x, &pos.y, &pos.z );

	RETURN_PYNONE_IF_NO_TARGET()
	RETURN_PYNONE_IF_NO_STAGE()

	CLightEntity *pLightEntity = NULL;
//		= GetStageForScriptCallback()->GetEntitySet()->GetLightEntity( GetEntityForLight()->iExtraDataIndex );

	if( pLightEntity )
        pLightEntity->SetPosition( pos );
	GetEntityForLight()->SetPosition( pos );

    Py_INCREF( Py_None );
	return Py_None;
}


PyMethodDef gsf::py::light::g_PyModuleLightMethod[] =
{
	{ "CreatePointLight",          gsf::py::light::CreatePointLight,             METH_VARARGS, "" },
	{ "CreateDirectionalLight",    gsf::py::light::CreateDirectionalLight,       METH_VARARGS, "" },
	{ "CreateSpotlight",           gsf::py::light::CreateSpotlight,              METH_VARARGS, "" },
	{ "CreateHSPointLight",        gsf::py::light::CreateHSPointLight,           METH_VARARGS, "" },
	{ "CreateHSDirectionalLight",  gsf::py::light::CreateHSDirectionalLight,     METH_VARARGS, "" },
	{ "CreateHSSpotlight",         gsf::py::light::CreateHSSpotlight,            METH_VARARGS, "" },
	{ "CreateTriPointLight",       gsf::py::light::CreateTriPointLight,          METH_VARARGS, "" },
	{ "CreateTriDirectionalLight", gsf::py::light::CreateTriDirectionalLight,    METH_VARARGS, "" },
	{ "CreateTriSpotlight",        gsf::py::light::CreateTriSpotlight,           METH_VARARGS, "" },
//	{ "RemoveLight",               gsf::py::light::RemoveLight,                  METH_VARARGS, "" },
//	{ "RemoveNamedLight",          gsf::py::light::RemoveNamedLight,             METH_VARARGS, "" },

	// used to set/change light properies after creating the light
	{ "SetTargetEntity",        gsf::py::light::SetTargetEntity, METH_VARARGS, "" },
	{ "SetColor",               gsf::py::light::SetColor,        METH_VARARGS, "" },
//	{ "SetUpperColor",          gsf::py::light::SetUpperColor,   METH_VARARGS, "" },
//	{ "SetLowerColor",          gsf::py::light::SetLowerColor,   METH_VARARGS, "" },
	{ "SetColorU32",            gsf::py::light::SetColorU32,     METH_VARARGS, "" },
	{ "SetPosition",            gsf::py::light::SetPosition,	 METH_VARARGS, "" },
//	{ "SetDirection",           gsf::py::light::SetDirection,	 METH_VARARGS, "" },
	{ "SetAttenuationFactors",  SetAttenuationFactors,	         METH_VARARGS, "set attenuation factors for point lights" },
//	{ "SetDirection",      gsf::py::light::SetDirection,	METH_VARARGS, "" },
//	{ "",       SetTarget,				METH_VARARGS, "" },
	{NULL, NULL}
};


void CreateDirectionalLight( const std::string& name,
                             const Vector3& vDirection,
							 const SFloatRGBAColor& color,
							 float intensity )
{
	
}
/*
BOOST_PYTHON_MODULE(Light)
{
    using namespace boost::python;
    def( "CreateDirectionalLight", CreateDirectionalLight );
}
*/
