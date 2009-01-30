#include "PyModule_GraphicsElement.hpp"

#include "GraphicsEffectManager.hpp"

#include "../base.hpp"
#include "Graphics/3DGameMath.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/3DGameMath.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Vec3_StringAux.hpp"

#include "Graphics/Rect.hpp"
using namespace Graphics;

#include <vector>
#include <string>

using namespace std;



//------------------------------------------------------------------------
// in-file static variables
//------------------------------------------------------------------------

static CGraphicsElementManagerBase gs_NullGraphicsElementManager;

static CGraphicsElementManagerBase *gs_pGraphicsElementManager = &gs_NullGraphicsElementManager;

static CGraphicsElementManagerBase& GraphicsElementManager()
{
	if( gs_pGraphicsElementManager == &gs_NullGraphicsElementManager )
		g_Log.Print( WL_WARNING, "using NullGraphicsElementManager" );

	return *gs_pGraphicsElementManager;
}


static CAnimatedGraphicsManagerBase gs_NullAnimGraphicsManager;

static CAnimatedGraphicsManagerBase *gs_pAnimGraphicsManager = &gs_NullAnimGraphicsManager;

static CAnimatedGraphicsManagerBase& AnimatedGraphicsManager() { return *gs_pAnimGraphicsManager; }


// holds pointers of graphics elements created from script
static vector<CGraphicsElement *> gs_vecpGraphicsElement;

static int RegisterGraphicsElement( CGraphicsElement *pElement )
{
	int id = (int)gs_vecpGraphicsElement.size();

	gs_vecpGraphicsElement.push_back( pElement );

	return id;
}

static CGraphicsElement *GetGraphicsElement( int id )
{
	if( id < 0 || (int)gs_vecpGraphicsElement.size() <= id )
		return NULL;

	return gs_vecpGraphicsElement[id];
}


//------------------------------------------------------------------------
// global functions
//------------------------------------------------------------------------

void SetGraphicsElementManagerForScript( CGraphicsElementManager *pManager )
{
	gs_pGraphicsElementManager = pManager;
}

void RemoveGraphicsElementManagerForScript()
{
	gs_pGraphicsElementManager = &gs_NullGraphicsElementManager;
}

/**
 * sets AnimatedGraphicsManager & GraphicsElementManager used by python script
 */
void SetAnimatedGraphicsManagerForScript( CAnimatedGraphicsManager *pManager )
{
	gs_pAnimGraphicsManager = pManager;
	SetGraphicsElementManagerForScript( pManager->GetGraphicsElementManager() );
}

void RemoveAnimatedGraphicsManagerForScript()
{
	gs_pAnimGraphicsManager = &gs_NullAnimGraphicsManager;
	RemoveGraphicsElementManagerForScript();
}


enum eCoordMode
{
	CM_LTRB,
	CM_LTWH,
	NUM_COORD_MODES
};

unsigned int gs_CoordMode = CM_LTRB;

static SRect GetRect( int l, int t, int x, int y )
{
	switch( gs_CoordMode )
	{
	case CM_LTRB:
	default:
		return RectLTRB( l, t, x, y );

	case CM_LTWH:
		return RectLTWH( l, t, x, y );
	}

	return RectLTRB( l, t, x, y );
}


PyObject* SetCoordMode( PyObject* self, PyObject* args )
{
	unsigned int coord_mode;
	int result = PyArg_ParseTuple( args, "k", &coord_mode );

	gs_CoordMode = coord_mode;

	PyObject *obj = Py_BuildValue( "i", 0 );
	return obj;
}


PyObject* CreateRect( PyObject* self, PyObject* args )
{
	float l, t, r, b;
	SFloatRGBAColor color = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	float z = 0;
	int result = PyArg_ParseTuple( args, "ffff|fffff", &l, &t, &r, &b, &color.fRed, &color.fGreen, &color.fBlue, &color.fAlpha, &z );

	int layer_index = (int)z;

	CGraphicsElement *pElement = GraphicsElementManager().CreateRect( SRect( l,t,r,b ), color, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}

/**
 * call this when you pass ARGB color as 32-bit integer
 */
PyObject* CreateRectC32( PyObject* self, PyObject* args )
{
	float l, t, r, b;
	U32 col = 0xFFFFFFFF;
	float z = 0;
	int result = PyArg_ParseTuple( args, "ffff|kf", &l, &t, &r, &b, &col, &z );

	SFloatRGBAColor color;
	color.SetARGB32( col );

	// TODO: change from float z-depth to integer layer index
	// - fix this C++ source file
	// - then, fix scripts
	int layer_index = (int)z;

	CGraphicsElement *pElement = GraphicsElementManager().CreateRect( SRect( l,t,r,b ), color, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


/**
 * \param [in] left, top, right, bottom, r, g, b, a, border_width, z
 */
PyObject* CreateFrameRect( PyObject* self, PyObject* args )
{
	float l, t, r, b;
	SFloatRGBAColor color = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	float border_width = 2;
	float z = 0;
	int result = PyArg_ParseTuple( args, "ffff|ffffff",
		&l, &t, &r, &b, &color.fRed, &color.fGreen, &color.fBlue, &color.fAlpha, &border_width, &z );

	int layer_index = (int)z;

	CGraphicsElement *pElement = GraphicsElementManager().CreateFrameRect( SRect( l,t,r,b ), color, border_width, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreateFrameRectC32( PyObject* self, PyObject* args )
{
	float l, t, r, b;
	U32 col = 0xFFFFFFFF;
	float border_width = 2;
	float z = 0;
	int result = PyArg_ParseTuple( args, "ffff|kff", &l, &t, &r, &b, &col, &border_width, &z );

	SFloatRGBAColor color;
	color.SetARGB32( col );

	int layer_index = (int)z;

	CGraphicsElement *pElement = GraphicsElementManager().CreateFrameRect( SRect( l,t,r,b ), color, border_width, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreateRoundRect( PyObject* self, PyObject* args )
{
	float l, t, r, b;
	SFloatRGBAColor color = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	float corner_radius = 2;
	int layer_index = 0;
	int result = PyArg_ParseTuple( args, "ffff|ffffi",
		&l, &t, &r, &b,
		&color.fRed, &color.fGreen, &color.fBlue, &color.fAlpha,
		&corner_radius, &layer_index );

	CGraphicsElement *pElement = GraphicsElementManager().CreateRoundRect( SRect( l,t,r,b ), color, corner_radius, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreateRoundRectC32( PyObject* self, PyObject* args )
{
	float l, t, r, b;
	U32 col = 0xFFFFFFFF;
	float corner_radius = 2;
	int layer_index = 0;
	int result = PyArg_ParseTuple( args, "ffff|kfi", &l, &t, &r, &b, &col, &corner_radius, &layer_index );

	SFloatRGBAColor color;
	color.SetARGB32( col );

	CGraphicsElement *pElement = GraphicsElementManager().CreateRoundRect( SRect( l,t,r,b ),
		color, corner_radius, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreateRoundFrameRect( PyObject* self, PyObject* args )
{
	float l, t, r, b;
	SFloatRGBAColor color = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	float corner_radius = 2;
	float border_width = 2;
	int layer_index = 0;
	int result = PyArg_ParseTuple( args, "ffff|ffffffi",
		&l, &t, &r, &b,
		&color.fRed, &color.fGreen, &color.fBlue, &color.fAlpha,
		&corner_radius, &border_width, &layer_index );

	CGraphicsElement *pElement = GraphicsElementManager().CreateRoundFrameRect( SRect( l,t,r,b ),
		color, corner_radius, border_width, layer_index );

	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreateRoundFrameRectC32( PyObject* self, PyObject* args )
{
	float l, t, r, b;
	U32 col = 0xFFFFFFFF;
	float corner_radius = 2;
	float border_width = 2;
	int layer_index = 0;
	int result = PyArg_ParseTuple( args, "ffff|kffi",
		&l, &t, &r, &b,
		&col, &border_width, &corner_radius, &layer_index );

	SFloatRGBAColor color;
	color.SetARGB32( col );

	CGraphicsElement *pElement = GraphicsElementManager().CreateRoundFrameRect( SRect( l,t,r,b ), color, corner_radius, border_width, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreateText( PyObject* self, PyObject* args )
{
	int font_id;
	char *text_buffer;
	float x=0,y=0;
	SFloatRGBAColor color = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );
	float font_w=0, font_h=0;
	int result = PyArg_ParseTuple( args, "isff|fffff",
		&font_id, &text_buffer, &x, &y,
		&color.fRed, &color.fGreen, &color.fBlue, &color.fAlpha, &font_w, &font_h );

	int layer_index = 0;

	CGraphicsElement *pElement = GraphicsElementManager().CreateText( font_id, text_buffer, x, y, color, (int)font_w, (int)font_h, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreateTextC32( PyObject* self, PyObject* args )
{
	int font_id;
	char *text_buffer;
	float x=0,y=0;
	U32 col = 0xFFFFFFFF;
	float font_w=0, font_h=0;
	int result = PyArg_ParseTuple( args, "isff|kff", &font_id, &text_buffer, &x, &y, &col, &font_w, &font_h );

	SFloatRGBAColor color;
	color.SetARGB32( col );

	int layer_index = 0;

	CGraphicsElement *pElement = GraphicsElementManager().CreateText( font_id, text_buffer, x, y, color, (int)font_w, (int)font_h, layer_index );
	int resource_id = RegisterGraphicsElement( pElement );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreateTriangle( PyObject* self, PyObject* args )
{
	SFloatRGBAColor col = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );

	int resource_id = -1;//GraphicsElementManager().CreateTriangle();

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}


PyObject* CreatePolygon( PyObject* self, PyObject* args )
{
	SFloatRGBAColor col = SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f );

	int resource_id = -1;//GraphicsElementManager().CreatePolygon();

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}

/**
 * \param graphics element id
 * \param texture id
 */
PyObject* SetTexture( PyObject* self, PyObject* args )
{
	int element_id = -1, tex_id = -1;

	int result = PyArg_ParseTuple( args, "ii", &tex_id, &element_id );

	CGraphicsElement *pElement = GetGraphicsElement( element_id );

	bool res;
	if( pElement && 0 <= tex_id )
	{
		pElement->SetTexture( tex_id );
		res = true;
	}
	else
		res = false;

	PyObject *obj = Py_BuildValue( "i", res ? -1 : 0 );
	return obj;
}


PyObject* SetTextureCoord( PyObject* self, PyObject* args )
{
	int element_id = -1;
	TEXCOORD2 tex_min, tex_max;

	int result = PyArg_ParseTuple( args, "iffff", &element_id, &tex_min.u, &tex_min.v, &tex_max.u, &tex_max.v );

	CGraphicsElement *pElement = GetGraphicsElement( element_id );

	bool res;
	if( pElement )
	{
		pElement->SetTextureCoord( tex_min, tex_max );
		res = true;
	}
	else
		res = false;

	PyObject *obj = Py_BuildValue( "i", res ? -1 : 0 );
	return obj;
}


PyObject* LoadTexture( PyObject* self, PyObject* args )
{
	char *tex_filename;
	int result = PyArg_ParseTuple( args, "s", &tex_filename );

	int tex_id = GraphicsElementManager().LoadTexture(tex_filename);

	PyObject *obj = Py_BuildValue( "i", tex_id );
	return obj;
}

/*
PyObject* LoadFont( PyObject* self, PyObject* args )
{
	char *name;
	int width, height;
	int result = PyArg_ParseTuple( args, "sii", &name, &width, &height );

	int resource_id = GraphicsElementManager().LoadFont( filename, width, height );

	PyObject *obj = Py_BuildValue( "i", resource_id );
	return obj;
}
*/

PyObject* LoadTextureFont( PyObject* self, PyObject* args )
{
	char *tex_filename;
	int width=0, height=0;
	float bold = 0.0f, italic = 0.0f;

	int result = PyArg_ParseTuple( args, "s|ffff", &tex_filename, &width, &height, &bold, &italic );

	int font_id = GraphicsElementManager().LoadTextureFont( tex_filename, width, height, bold, italic );

	PyObject *obj = Py_BuildValue( "i", font_id );
	return obj;
}


PyObject* RemoveElement( PyObject* self, PyObject* args )
{
	int id = -1;
	int result = PyArg_ParseTuple( args, "i", &id );

	bool removed = false;
	if( 0 <= id )
		removed = GraphicsElementManager().RemoveElement( gs_vecpGraphicsElement[id] );

	PyObject *obj = Py_BuildValue( "i", removed ? -1 : 0 );
	return obj;
}

/*
PyObject* RemoveAllElements( PyObject* self, PyObject* args )
{

    Py_INCREF( Py_None );
	return Py_None;
}
*/


PyObject* SetTimeOffset( PyObject* self, PyObject* args )
{
	AnimatedGraphicsManager().SetTimeOffset();

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ChangeColorC32( PyObject* self, PyObject* args )
{
	int id = -1;
	float start_time = 0, end_time = 0;
	int color_index = 0;
	U32 color = 0xFFFFFFFF;
	int trans_mode = 0;
	int result = PyArg_ParseTuple( args, "iffk|i", &id, &start_time, &end_time, &color, &trans_mode );

	if( 0 <= id )
		AnimatedGraphicsManager().ChangeColorTo( gs_vecpGraphicsElement[id], start_time, end_time, color_index, color, trans_mode );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetColorC32( PyObject* self, PyObject* args )
{
	int id = -1;
	float time = 0;
	int color_index = 0;
	U32 color = 0xFFFFFFFF;
	int result = PyArg_ParseTuple( args, "ifk", &id, &time, &color );

	if( 0 <= id )
		AnimatedGraphicsManager().SetColor( gs_vecpGraphicsElement[id], time, color_index, color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* Translate( PyObject* self, PyObject* args )
{
	int id = -1;
	float start_time = 0, end_time = 0;
	float dest_x, dest_y;
	int coord_type = 0, trans_mode = 0;
	int result = PyArg_ParseTuple( args, "iffff|ii", &id, &start_time, &end_time, &dest_x, &dest_y, &coord_type, &trans_mode );

	if( 0 <= id )
		AnimatedGraphicsManager().TranslateTo( gs_vecpGraphicsElement[id], start_time, end_time, Vector2( dest_x, dest_y ), coord_type, trans_mode );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ChangeSize( PyObject* self, PyObject* args )
{
	int id = -1;
	float start_time = 0, end_time = 0;
	AABB2 dest = AABB2( Vector2(0,0), Vector2(0,0) );
	int trans_mode = 0;
	int result = PyArg_ParseTuple( args, "iffffff|i",
		&id, &start_time, &end_time, &dest.vMin.x, &dest.vMin.y, &dest.vMax.x, &dest.vMax.y, &trans_mode );

	if( 0 <= id )
		AnimatedGraphicsManager().ChangeSize( gs_vecpGraphicsElement[id], start_time, end_time, dest.vMin, dest.vMax );

    Py_INCREF( Py_None );
	return Py_None;
}


/*
 * \param start time
 * \param dest position (x,y)
 * \param initial velocity (x,y)
 * \param coord type
 * NOTE: This function does not take animation end time
 * end time is determined by the dist to cover, initial velocity, and smooth time
 */
PyObject* TranslateNonLinear( PyObject* self, PyObject* args )
{
	int id = -1;
	float start_time = 0;
	Vector2 vDest, vInitVel = Vector2(0,0);
	float smooth_time = 0.5f;
	int coord_type = 0;

	int result = PyArg_ParseTuple( args, "ifff|fffi",
		&id, &start_time, &vDest.x, &vDest.y, &vInitVel.x, &vInitVel.y, &smooth_time, &coord_type );

	if( 0 <= id )
		AnimatedGraphicsManager().TranslateNonLinear( gs_vecpGraphicsElement[id], start_time, vDest, vInitVel, smooth_time, coord_type );

    Py_INCREF( Py_None );
	return Py_None;
}


PyMethodDef g_PyModuleGraphicsElementMethod[] =
{
//	{ "SetRectColor",			SetRectColor,			METH_VARARGS, "" },
//	{ "SetRectPosition",		SetRectPosition,		METH_VARARGS, "" },
	{ "SetCoordMode",			SetCoordMode,			METH_VARARGS, "" },
	{ "CreateRect",				CreateRect,				METH_VARARGS, "" },
	{ "CreateRectC32",			CreateRectC32,			METH_VARARGS, "" },
//	{ "CreateRectLTWHC32",		CreateRectLTWHC32,		METH_VARARGS, "" },
	{ "CreateFrameRect",		CreateFrameRect,		METH_VARARGS, "" },
	{ "CreateFrameRectC32",		CreateFrameRectC32,		METH_VARARGS, "" },
	{ "CreateRoundRect",		CreateRoundRect,		METH_VARARGS, "" },
	{ "CreateRoundRectC32",		CreateRoundRectC32,		METH_VARARGS, "" },
	{ "CreateRoundFrameRect",	CreateRoundFrameRect,	METH_VARARGS, "" },
	{ "CreateRoundFrameRectC32",CreateRoundFrameRectC32,METH_VARARGS, "" },
	{ "CreateTriangle",			CreateTriangle,			METH_VARARGS, "" },
	{ "CreatePolygon",			CreatePolygon,			METH_VARARGS, "" },
	{ "CreateText",				CreateText,				METH_VARARGS, "" },
	{ "CreateTextC32",			CreateTextC32,			METH_VARARGS, "" },
	{ "SetTexture",				SetTexture,				METH_VARARGS, "" },
	{ "SetTextureCoord",		SetTextureCoord,		METH_VARARGS, "" },

	{ "LoadTexture",			LoadTexture,			METH_VARARGS, "loads a texture resource and returns a texture id" },
//	{ "LoadFont",				LoadFont,				METH_VARARGS, "" },
	{ "LoadTextureFont",		LoadTextureFont,		METH_VARARGS, "" },
	{ NULL, NULL }
};


PyMethodDef g_PyModuleAnimatedGraphicsMethod[] =
{
//	{ "Translate"				Translate,				METH_VARARGS, "" },
	{ "SetTimeOffset",			SetTimeOffset,			METH_VARARGS, "" },
	{ "Translate",				Translate,				METH_VARARGS, "" },
	{ "TranslateNonLinear",		TranslateNonLinear,		METH_VARARGS, "" },
	{ "ChangeColorC32",			ChangeColorC32,			METH_VARARGS, "" },
	{ "SetColorC32",			SetColorC32,			METH_VARARGS, "" },
	{ "ChangeSize",				ChangeSize,				METH_VARARGS, "" },
//	{ "ChangeAlpha",			ChangeAlpha,			METH_VARARGS, "" },
//	{ "Scale",					Scale,					METH_VARARGS, "" },
	{ NULL, NULL }
};


/*
PyMethodDef g_PyModuleGraphicsElementMethod[] =
{
	{ "LoadTexture",			LoadTexture,			METH_VARARGS, "" },
	{ "DrawRect",				DrawRect,				METH_VARARGS, "" },
	{ "DrawFrameRect",			DrawFrameRect,			METH_VARARGS, "" },
	{ "DrawTexRect",			DrawTexRect,			METH_VARARGS, "" },
	{ "LoadFont",				LoadFont,				METH_VARARGS, "" },
	{ "DrawText",				DrawText,				METH_VARARGS, "" },
	{ "LoadTexture",			LoadTexture,			METH_VARARGS, "" },
	{ "LoadTexture",			LoadTexture,			METH_VARARGS, "" },
	{ "LoadTexture",			LoadTexture,			METH_VARARGS, "" },
	{NULL, NULL}
};
*/
