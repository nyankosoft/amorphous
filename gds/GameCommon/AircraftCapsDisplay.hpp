#ifndef  __AircraftCapsDisplay_H__
#define  __AircraftCapsDisplay_H__


#include <vector>
#include <string>
#include "UI/GM_StdControlRendererManager.hpp"
#include "CriticalDamping.hpp"

#include "Graphics/Rect.hpp"
using namespace Graphics;


class CGI_Aircraft;


class CAircraftCaps
{
public:
	CGM_Static* pName;
	CGM_Static* pDesc;

	enum control_id
	{
		CID_CAPS_NAME,
		CID_CAPS_DESC,

        CID_PERF_OFFSET,
		CID_PERF_SPEED = CID_PERF_OFFSET,
		CID_PERF_MANEUVERABILITY,
		CID_PERF_DURABILITY,
		CID_PERF_AIR_TO_AIR,
		CID_PERF_AIR_TO_G,
		CID_PERF_STEALTHINESS,
		NUM_CIDS
	};

//	static int GetNumPerfs() const { return CID_PERF_STEALTHINESS - CID_PERF_OFFSET; }
	static int GetNumPerfs() { return CID_PERF_STEALTHINESS - CID_PERF_OFFSET; }
};


/**
 * displays performance info of an aircraft
 * things to display
 * - name
 * - brief description
 * - performance
 *  - speed
 *  - maneuvarability
 *  - durability
 *  - air-to-air capability
 *  - air-to-ground capability
 *  - maneuvarability
 *  - stealthiness
 *
 * what do we do about fonts ?
 */
class CAircraftCapsDisplay : public CGM_StdControlRendererManager
{
public:

	enum num_caps
	{
//		NAME = 0,
//		DESC,

		SPEED,
		MANEUVERABILITY,
		DURABILITY,
		AIR_TO_AIR,
		AIR_TO_GROUND,
		STEALTHINESS,
		NUM_CAPS,

		NUM_PERFS = STEALTHINESS - SPEED + 1
	};

	/// currently focused ammo
	CGM_Static* m_pAmmoName;
	CGM_Static* m_pAmmoDesc;

private:

	CGI_Aircraft *m_pAircraft;

	cdv<float> m_vecCapsVal[NUM_CAPS];

	void DrawDesc();

	void DrawCaps();

	void UpdateCaps();

public:

	SPoint m_vPos[NUM_CAPS];

	CAircraftCapsDisplay();
	virtual ~CAircraftCapsDisplay();

//	virtual void Update( float dt );
//	virtual void Render();

	virtual CGM_ControlRenderer *CreateStaticRenderer( CGM_Static *pStatic );

	inline CGI_Aircraft * GetAircraft() const { return m_pAircraft; }

	inline void SetAircraft( CGI_Aircraft *pAircraft )
	{
		m_pAircraft = pAircraft;
		UpdateCaps();	// re-calculate aircraft performance
	}

};


static const char g_AircraftCapsText[CAircraftCapsDisplay::NUM_PERFS][32] =
{
	"speed",
	"maneuverability",
	"defense",
	"air-to-air",
	"air-to-ground",
	"stealth"
};


#endif		/*  __AircraftCapsDisplay_H__  */
