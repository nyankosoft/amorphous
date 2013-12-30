#ifndef __Caliber_HPP__
#define __Caliber_HPP__


#include <string.h>


namespace amorphous
{


class Caliber
{
public:
	enum Name
	{
		// handgun cartridges
		_22_SHORT,          ///< .22 Short
		_22_LR,             ///< .22 Long Rifle
		_22_WMR,            ///< .22 Winchester Magnum Rimfire
		_25_ACP,            ///< .25 ACP
		_7_62X25MM_TOKAREV, ///< 7.62x25mm Tokarev
		_32_ACP,            ///< .32 ACP
		_380_ACP,           ///< .380 ACP
		_9X18MM_MAKAROV,    ///< 9x18mm Makarov
		_9MM,               ///< 9x19mm Luger Parabellum
		_357_SIG,           ///< .357 SIG
		_45_ACP,            ///< .45 ACP
		_40_SW,             ///< .40 S&W (10x22mm)
		_10MM_AUTO,         ///< 10mm Auto
		_38_SPECIAL,        ///< .38 Smith & Wesson Special
		_357_MAGNUM,        ///< .357 Magnum
		_45_COLT,           ///< .45 Colt
		_44_SPECIAL,        ///< .44 S&W Special
		_44_MAGNUM,         ///< .44 Magnum
		_500_SW_MAGNUM,     ///< .500 S&W Magnum
		_50_AE,             ///< .50 Action Express

		// shotgun cartridges
		_410_BORE,          ///< .410 bore
		_20_GAUGE,          ///< 20 gauge shotgun shell
		_12_GAUGE,          ///< 12 gauge shotgun shell

		// rifle cartridges
		_17_HMR,            ///< .17 Hornady Magnum Rimfire
		HK_4_6X30,          ///< HK 4.6x30mm
		_5_7X28,            ///< 5.7x28mm
		_5_56X45,           ///< 5.56x45mm
		_6_8MM_REMINGTON_SPC, /// < 6.8 mm Remington SPC
		_30_CARBINE,        ///< .30 Carbine (7.62×33mm)
		_300_AAC_BLACKOUT,  ///< 300 AAC Blackout (7.62×35mm)
		_7_62X51,           ///< 7.62x51mm
//		_380_WINCHESTER,    ///< 
		_30_06_SPRINGFIELD, ///< .30-06 Springfield
		_338_LAPUA_MAGNUM,  ///< .338 Lapua Magnum
		_50BMG,             ///< .50 BMG
//		_12_7X99,           ///<
		_5_45X39,           ///< 5.45x39mm
		_7_62X39,           ///< 7.62x39mm
		_7_62X54R,          ///< 7.62x54mmR
		_7_92X57_MAUSER,    ///< 7.92x57mm Mauser

		_25X137MM,
		_30X173MM,

		OTHER,
		NUM_CALIBERS
	};
};


inline const char *GetCaliberName( Caliber::Name cal )
{
	switch( cal )
	{
	case Caliber::_22_SHORT:          return ".22 Short";
	case Caliber::_22_LR:             return ".22 Long Rifle";
	case Caliber::_22_WMR:            return ".22 Winchester Magnum Rimfire";
	case Caliber::_25_ACP:            return ".25 ACP";
	case Caliber::_7_62X25MM_TOKAREV: return "7.62x25mm Tokarev";
	case Caliber::_32_ACP:            return ".32 ACP";
	case Caliber::_380_ACP:           return ".380 ACP";
	case Caliber::_9X18MM_MAKAROV:    return "9x18mm Makarov";
	case Caliber::_9MM:               return "9x19mm Parabellum";
	case Caliber::_357_SIG:           return ".357 SIG";
	case Caliber::_45_ACP:            return ".45 ACP";
	case Caliber::_40_SW:             return ".40 S&W";
	case Caliber::_10MM_AUTO:         return "10mm Auto";
	case Caliber::_38_SPECIAL:        return ".38 Smith & Wesson Special";
	case Caliber::_357_MAGNUM:        return ".357 S&W Magnum";
	case Caliber::_45_COLT:           return ".45 Colt";
	case Caliber::_44_SPECIAL:        return ".44 S&W Special";
	case Caliber::_44_MAGNUM:         return ".44 Remington Magnum";
	case Caliber::_500_SW_MAGNUM:     return ".500 S&W Magnum";
	case Caliber::_50_AE:             return ".50 Action Express";

	// shotgun cartridge
	case Caliber::_410_BORE:          return ".410 bore";
	case Caliber::_20_GAUGE:          return "20 gauge shell";
	case Caliber::_12_GAUGE:          return "12 gauge shell";

	// rifle cartridge
	case Caliber::_17_HMR:            return ".17 Hornady Magnum Rimfire";
	case Caliber::HK_4_6X30:          return "HK 4.6x30mm";
	case Caliber::_5_7X28:            return "5.7x28mm";
	case Caliber::_5_56X45:           return "5.56x45mm NATO";
	case Caliber::_6_8MM_REMINGTON_SPC:return "6.8 mm Remington SPC";
	case Caliber::_30_CARBINE:        return ".30 Carbine";
	case Caliber::_300_AAC_BLACKOUT:  return "300 AAC Blackout";
	case Caliber::_7_62X51:           return "7.62x51mm NATO";
//	case Caliber::_380_WINCHESTER:    return "";
	case Caliber::_30_06_SPRINGFIELD: return ".30-06 Springfield";
	case Caliber::_338_LAPUA_MAGNUM:  return ".338 Lapua Magnum";
	case Caliber::_50BMG:             return ".50 BMG";
//	case Caliber::_12_7X99:           return "";
	case Caliber::_5_45X39:           return "5.45x39mm";
	case Caliber::_7_62X39:           return "7.62x39mm";
	case Caliber::_7_62X54R:          return "7.62x54mmR";
	case Caliber::_7_92X57_MAUSER:    return "7.92x57mm Mauser";

	case Caliber::_25X137MM:          return "25x137mm";
	case Caliber::_30X173MM:          return "30x173mm";

	case Caliber::OTHER:              return "other";

	default:
		return "(unknown)";
	}
}


inline const char *GetAbbreviatedCaliberName( Caliber::Name cal )
{
	switch( cal )
	{
	case Caliber::_22_LR:             return ".22 LR";
	case Caliber::_22_WMR:            return ".22 WMR";
	case Caliber::_9MM:               return "9x19mm";
	case Caliber::_38_SPECIAL:        return ".38 Special";
	case Caliber::_357_MAGNUM:        return ".357 Magnum";
	case Caliber::_44_SPECIAL:        return ".44 Special";
	case Caliber::_44_MAGNUM:         return ".44 Magnum";
	case Caliber::_17_HMR:            return ".17 HMR";
	case Caliber::_5_56X45:           return "5.56x45mm";
	case Caliber::_6_8MM_REMINGTON_SPC:return "6.8 SPC";
	case Caliber::_300_AAC_BLACKOUT:  return "300 BLK";
	case Caliber::_7_62X51:           return "7.62x51mm";
	default:
		return GetCaliberName( cal );
	}
}


inline Caliber::Name GetCaliberFromName( const char *caliber_name )
{
	if( !caliber_name
	 || strlen(caliber_name) == 0 )
	{
		return Caliber::OTHER;
	}

	for( int i=0; i<Caliber::NUM_CALIBERS; i++ )
	{
		Caliber::Name cal = (Caliber::Name)i;
		if( strcmp( GetCaliberName( cal ), caliber_name ) == 0 )
			return cal;
	}

	for( int i=0; i<Caliber::NUM_CALIBERS; i++ )
	{
		Caliber::Name cal = (Caliber::Name)i;
		if( strcmp( GetAbbreviatedCaliberName( cal ), caliber_name ) == 0 )
			return cal;
	}

	return Caliber::OTHER;
}


inline bool IsCenterfireCartridge( Caliber::Name cal )
{
	if( Caliber::_22_LR
	 || Caliber::_22_WMR
	 || Caliber::_7_62X54R )
	{
		return false;
	}
	else
	{
		return true;
	}
}


} // namespace amorphous



#endif /* __Caliber_HPP__ */
