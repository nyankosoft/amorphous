#include "PyModule_Light.hpp"
#include "PyModule_Stage.hpp"

#include "3DMath/3DGameMath.hpp"
#include "Graphics/FloatRGBColor.hpp"

#include "BE_DirectionalLight.hpp"
#include "BE_PointLight.hpp"
#include "Stage.hpp"
#include "EntitySet.hpp"

#include "CopyEntityDesc.hpp"

#include "Support/Macro.h"
#include "Support/Log/DefaultLog.hpp"

#include "../base.hpp"
#include "../Script/convert_python_to_x.hpp"
#include "../Script/PythonScriptManager.hpp"


namespace amorphous
{

using boost::shared_ptr;


class CStageLightAttributeHolder
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

	BaseEntityHandle m_aBaseEntityHandle[NUM_DEFAULT_BASE_ENTITY_HANDLES];

	/// target light entity
	EntityHandle<LightEntity> m_TargetLightEntity;
};


CStageLightAttributeHolder gs_StageLightAttribute;


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
array2d<char> m_LightToObject;

int init_light_groups()
{
	const int num_light_groups = 16;
	const int num_ligting_groups = 16;
	m_LightToObject.resize( num_light_groups, num_ligting_groups );

	return 0;
}


inline static shared_ptr<LightEntity> GetTargetLightEntity()
{
	return gs_StageLightAttribute.m_TargetLightEntity.Get();
}


inline static void GetInvalidDesc( LightEntityDesc& desc )
{
	// set all colors to invalid values
	desc.aColor[0] = CBE_Light::ms_InvalidColor;
	desc.aColor[1] = CBE_Light::ms_InvalidColor;
	desc.aColor[2] = CBE_Light::ms_InvalidColor;
	desc.LightGroup = CBE_Light::ms_InvalidLightGroup;

	// attenuation (for point lights)
	desc.afAttenuation[0] = desc.afAttenuation[0];
	desc.afAttenuation[1] = desc.afAttenuation[1];
	desc.afAttenuation[2] = desc.afAttenuation[2];
}



using namespace py::light;


static CCopyEntity *GetEntityByName( const char* entity_name )
{
	if( GetStageForScriptCallback() )
        return GetStageForScriptCallback()->GetEntitySet()->GetEntityByName(entity_name);
	else
        return NULL;
}


PyObject* py::light::CreateDirectionalLight( PyObject* self, PyObject* args, PyObject *keywords )
{
	LightEntityDesc desc( Light::DIRECTIONAL );
	char *base_name = "";
	char *light_name = "";
	int shadow_for_light = 1; // true(1) by default
	Vector3 dir = Vector3(0,-1,0); // default direction = vertically down
	SFloatRGBAColor color = SFloatRGBAColor(1,1,1,1);

	int result = 0;

	static char *kw_list[] = { "model", "name", "direction", "diffuse_color", "intensity", "light_group", "shadow_for_light", NULL };

	BaseEntityHandle basehandle( "__DirectionalLight__" );
	GetInvalidDesc( desc );
	dir = CBE_Light::ms_vInvalidDirection;
	desc.WorldPose.matOrient.SetColumn( 0, dir );
	desc.WorldPose.matOrient.SetColumn( 1, dir );
	desc.WorldPose.matOrient.SetColumn( 2, dir );

	result = PyArg_ParseTupleAndKeywords( args, keywords, "|ssO&O&fii", kw_list,
		&base_name,  // model
		&light_name,
		convert_python_to_cpp_Vector3, &dir,
		convert_python_to_cpp_FloatRGBA, &color,
		&desc.fIntensity,
		&desc.LightGroup,
		&shadow_for_light );

	// Leave it if it is CBE_Light::ms_vInvalidDirection
	if( dir != CBE_Light::ms_vInvalidDirection )
	{
		// User wants to overwrite the db value - check if it is a valid direction
		Vec3Normalize( dir, dir );
		if( Vec3Length( dir ) < 0.001f )
			dir = Vector3(0,-1,0);
	}

	if( 0 < strlen(base_name) )
		basehandle.SetBaseEntityName( base_name );

	color.alpha = 1.0f;

	desc.pBaseEntityHandle = &basehandle;
	desc.strName = light_name;
	desc.WorldPose.matOrient.SetColumn( 2, dir );
	desc.aColor[0] = color;

	CreateEntityFromDesc( desc );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::CreatePointLight( PyObject* self, PyObject* args, PyObject *keywords )
{
	LightEntityDesc desc( Light::POINT );
	char *base_name = "";
	char *light_name = "";
	int shadow_for_light = 1;     // true(1) by default
	Vector3 pos = Vector3(0,2,0); // default position = 2m above the world origin
	SFloatRGBAColor color = SFloatRGBAColor(1,1,1,1);
	float *pAttenu = desc.afAttenuation;

	int result = 0;

	static char *kw_list[] = { "model", "name", "position", "diffuse_color", "intensity", "att0", "att1", "att2", "light_group", "shadow_for_light", NULL };

	BaseEntityHandle basehandle( "__PointLight__" );

	GetInvalidDesc( desc );

	result = PyArg_ParseTupleAndKeywords( args, keywords, "|ssO&O&ffffii", kw_list,
		&base_name,
		&light_name,
		convert_python_to_cpp_Vector3, &pos,
		convert_python_to_cpp_FloatRGBA, &color,
		&desc.fIntensity,
		&pAttenu[0],
		&pAttenu[1],
		&pAttenu[2],
		&desc.LightGroup,
		&shadow_for_light );

	basehandle.SetBaseEntityName( base_name );

	color.alpha = 1.0f;

	desc.pBaseEntityHandle = &basehandle;
	desc.strName = light_name;
	desc.WorldPose.vPosition = pos;
	desc.aColor[0] = color;

	CreateEntityFromDesc( desc );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::CreateSpotlight( PyObject* self, PyObject* args, PyObject *keywords )
{
	LOG_PRINT_ERROR( "Not implemented yet." );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::CreateHSDirectionalLight( PyObject* self, PyObject* args, PyObject *keywords )
{
	LightEntityDesc desc( Light::HEMISPHERIC_DIRECTIONAL );

	// alias
	SFloatRGBAColor& uc = desc.aColor[0];
	SFloatRGBAColor& lc = desc.aColor[1];

	char *base_name = "";
	char *light_name = "";
	int shadow_for_light = 1; // true(1) by default
	Vector3 dir = Vector3(0,-1,0); // default direction = vertically down

	int result = 0;


	BaseEntityHandle basehandle;

	static char *kw_list[] = { "model", "name", "direction", "upper_diffuse_color", "lower_diffuse_color", "intensity", "light_group", "shadow_for_light", NULL };

	// 'load' mode
	// set invalid values to see which default values the caller
	// intends to override
	GetInvalidDesc( desc );
	dir = CBE_Light::ms_vInvalidDirection;
	desc.WorldPose.matOrient.SetColumn( 0, dir );
	desc.WorldPose.matOrient.SetColumn( 1, dir );
	desc.WorldPose.matOrient.SetColumn( 2, dir );

	result = PyArg_ParseTupleAndKeywords( args, keywords, "|ssO&O&O&fii", kw_list,
		&base_name, // model
		&light_name,
		convert_python_to_cpp_Vector3, &dir,
		convert_python_to_cpp_FloatRGBA, &uc,
		convert_python_to_cpp_FloatRGBA, &lc,
		&desc.fIntensity,
		&desc.LightGroup,
		&shadow_for_light );

	// Leave it if it is CBE_Light::ms_vInvalidDirection
	if( dir != CBE_Light::ms_vInvalidDirection )
	{
		// User wants to overwrite the db value - check if it is a valid direction
		Vec3Normalize( dir, dir );
		if( Vec3Length( dir ) < 0.001f )
			dir = Vector3(0,-1,0);
	}

	if( 0 < strlen(base_name) )
		basehandle.SetBaseEntityName( base_name );
	else
		basehandle.SetBaseEntityName( "__HemisphericDirectionalLight__" );

	desc.WorldPose.matOrient = CreateOrientFromFwdDir( dir );
	desc.pBaseEntityHandle = &basehandle;
	desc.strName = light_name;

	CreateEntityFromDesc( desc );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::CreateHSPointLight( PyObject* self, PyObject* args, PyObject *keywords )
{
	LightEntityDesc desc( Light::HEMISPHERIC_POINT );

	// alias
	Vector3& pos = desc.WorldPose.vPosition;
	SFloatRGBAColor& uc = desc.aColor[0];
	SFloatRGBAColor& lc = desc.aColor[1];

	// default value settings
	char *base_name = "";
	char *light_name = "";
	int shadow_for_light = 1;      // true(1) by default
	pos = Vector3(0,2,0);  // default position = 2m above the world origin
	float *pAttenu = desc.afAttenuation;

	int result = 0;
		
	static char *kw_list[] = { "model", "name", "position", "upper_diffuse_color", "lower_diffuse_color", "att0", "att1", "att2", "intensity", "light_group", "shadow_for_light", NULL };

	BaseEntityHandle basehandle;
	// pos = ???
	// position must always be specified.
	// This is true for 'load' mode as well as 'create' mode
	GetInvalidDesc( desc );

	result = PyArg_ParseTupleAndKeywords( args, keywords, "|ssO&O&O&ffffii", kw_list,
		&base_name,
		&light_name,
		convert_python_to_cpp_Vector3, &pos,
		convert_python_to_cpp_FloatRGBA, &uc,
		convert_python_to_cpp_FloatRGBA, &lc,
		&desc.fIntensity,
		&pAttenu[0],
		&pAttenu[1],
		&pAttenu[2],
		&desc.LightGroup,
		&shadow_for_light );

	if( 0 < strlen(base_name) )
		basehandle.SetBaseEntityName( base_name );
	else
		basehandle.SetBaseEntityName( "__HemisphericPointLight__" );

	desc.pBaseEntityHandle = &basehandle;
	desc.strName = light_name;

	CreateEntityFromDesc( desc );

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::CreateHSSpotlight( PyObject* self, PyObject* args, PyObject *keywords )
{
	LOG_PRINT_ERROR( "Not implemented yet." );

	Py_INCREF( Py_None );
	return Py_None;
}


/*
PyObject* py::light::LoadHSSpotlight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}
*/
/*
PyObject* py::light::CreateTriDirectionalLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::CreateTriPointLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::CreateTriSpotlight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}
*/

PyObject* SetAttenuationFactors( PyObject* self, PyObject* args )
{
	LOG_FUNCTION_SCOPE();

	float a[3];

	int result = PyArg_ParseTuple( args, "fff", &a[0], &a[1], &a[2] );

	shared_ptr<LightEntity> pLightEntity = GetTargetLightEntity();
	if( pLightEntity )
	{
		pLightEntity->SetAttenuationFactors( a[0], a[1], a[2] );
	}

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

	shared_ptr<LightEntity> pLightEntity = GetTargetLightEntity();
	if( pLightEntity )
	{
		pLightEntity->SetColor( index, color );
	}

/*	const Vector3 vColor = Vector3( color.red, color.green, color.blue );

	switch( index )
	{
	case 0: GetEntityForLight()->v1 = vColor; break;
	case 1: GetEntityForLight()->v2 = vColor; break;
	case 2: GetEntityForLight()->v3 = vColor; break;
	default:
		g_Log.Print( WL_ERROR, "SetLightColor() - invalid light color index: %d. Must be [0,2]" );
	}*/
}


PyObject* py::light::SetColorU32( PyObject* self, PyObject* args )
{
	int index;
	U32 color;	// 32-bit ARGB color (8 bits for each channel)
	int result = PyArg_ParseTuple( args, "ik", &index, &color );

	SFloatRGBColor dest_color;
	dest_color.SetARGB32( color );

	SetLightColor( index, dest_color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::SetColor( PyObject* self, PyObject* args )
{
	int index;
	SFloatRGBColor color;
	int result = PyArg_ParseTuple( args, "ifff", &index, &color.red, &color.green, &color.blue );

	SetLightColor( index, color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::SetPosition( PyObject* self, PyObject* args )
{
	Vector3 pos;
	int result = PyArg_ParseTuple( args, "fff", &pos.x, &pos.y, &pos.z );

	shared_ptr<LightEntity> pLightEntity = GetTargetLightEntity();
	if( pLightEntity )
	{
		pLightEntity->SetPosition( pos );
	}

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* py::light::SetTargetLight( PyObject* self, PyObject* args )
{
	LOG_FUNCTION_SCOPE();

	char *entity_name;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CCopyEntity* pEntityForLight = GetEntityByName( entity_name );

	LightEntity *pLightEntity = dynamic_cast<LightEntity *>(pEntityForLight);

	// TODO: what if the focus target entity is created later
	if( IsValidEntity(pLightEntity) )
	{
		gs_StageLightAttribute.m_TargetLightEntity
			= EntityHandle<LightEntity>( pLightEntity->LightEntitySelf() );
	}

	Py_INCREF( Py_None );
	return Py_None;
}


static PyMethodDef sg_PyModuleLightMethod[] =
{
	{ "CreateDirectionalLight",    (PyCFunction)py::light::CreateDirectionalLight,       METH_VARARGS | METH_KEYWORDS, "" },
	{ "CreatePointLight",          (PyCFunction)py::light::CreatePointLight,             METH_VARARGS | METH_KEYWORDS, "" },
	{ "CreateSpotlight",           (PyCFunction)py::light::CreateSpotlight,              METH_VARARGS | METH_KEYWORDS, "" },
	{ "CreateHSDirectionalLight",  (PyCFunction)py::light::CreateHSDirectionalLight,     METH_VARARGS | METH_KEYWORDS, "" },
	{ "CreateHSPointLight",        (PyCFunction)py::light::CreateHSPointLight,           METH_VARARGS | METH_KEYWORDS, "" },
	{ "CreateHSSpotlight",         (PyCFunction)py::light::CreateHSSpotlight,            METH_VARARGS | METH_KEYWORDS, "" },
//	{ "CreateTriPointLight",       (PyCFunction)py::light::CreateTriPointLight,          METH_VARARGS | METH_KEYWORDS, "" },
//	{ "CreateTriDirectionalLight", (PyCFunction)py::light::CreateTriDirectionalLight,    METH_VARARGS | METH_KEYWORDS, "" },
//	{ "CreateTriSpotlight",        (PyCFunction)py::light::CreateTriSpotlight,           METH_VARARGS | METH_KEYWORDS, "" },

//	{ "RemoveLight",               py::light::RemoveLight,                  METH_VARARGS, "" },
//	{ "RemoveNamedLight",          py::light::RemoveNamedLight,             METH_VARARGS, "" },

	// used to set/change light properies after creating the light
	{ "SetTargetLight",         py::light::SetTargetLight,       METH_VARARGS, "" },
	{ "SetColor",               py::light::SetColor,             METH_VARARGS, "" },
//	{ "SetUpperDiffuseColor",   py::light::SetUpperDiffuseColor, METH_VARARGS, "" },
//	{ "SetLowerDiffuseColor",   py::light::SetLowerDiffuseColor, METH_VARARGS, "" },
	{ "SetColorU32",            py::light::SetColorU32,          METH_VARARGS, "" },
	{ "SetPosition",            py::light::SetPosition,	      METH_VARARGS, "" },
//	{ "SetDirection",           py::light::SetDirection,	      METH_VARARGS, "" },
	{ "SetAttenuationFactors",  SetAttenuationFactors,	              METH_VARARGS, "set attenuation factors for point lights" },
//	{ "SetDirection",      py::light::SetDirection,	METH_VARARGS, "" },
//	{ "",       SetTarget,				METH_VARARGS, "" },
	{NULL, NULL}
};


void py::light::RegisterPythonModule_Light( PythonScriptManager& mgr )
{
	mgr.AddModule( "Light", sg_PyModuleLightMethod );
}


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


} // namespace amorphous
