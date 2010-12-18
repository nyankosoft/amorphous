#ifndef __Caliber_HPP__
#define __Caliber_HPP__


#include <string.h>


class Caliber
{
public:
	enum Name
	{
		// handgun cartridges
		_22LR,              ///< .22 Long Rifle
		_9MM,               ///< 9x19mm Luger Parabellum
		_25_ACP,            ///< .25 ACP
		_380_ACP,           ///< .380 ACP
		_45_ACP,            ///< .45 ACP
		_40_SW,             ///< .40 S&W (10x22mm)
		_357_MAGNUM,        ///< .357 Magnum
		_45_COLT,           ///< .45 Colt
		_44_MAGNUM,         ///< .44 Magnum
		_50_AE,             ///< .50 Action Express

		// shotgun cartridges
		_410_BORE,          ///< .410 bore
		_20_GAUGE,          ///< 20 gauge shotgun shell
		_12_GAUGE,          ///< 12 gauge shotgun shell

		// rifle cartridges
		_5_7X28,            ///< 5.7x28mm
		_5_56X45,           ///< 5.56x45mm
		_7_62X51,           ///< 7.62x51mm
//		_380_WINCHESTER,    ///< 
		_30_60_SPRINGFIELD, ///< .30-06 Springfield
		_50BMG,             ///< .50 BMG
//		_12_7X99,           ///<
		_7_62X39,           ///< 7.62x39mm
		_7_62X54R,          ///< 7.62x54mmR

		_25MM,
		_30MM,

		OTHER,
		NUM_CALIBERS
	};
};


inline const char *GetCaliberName( Caliber::Name cal )
{
	switch( cal )
	{
	case Caliber::_22LR:              return ".22 Long Rifle";
	case Caliber::_9MM:               return "9x19mm Parabellum";
	case Caliber::_25_ACP:            return ".25 ACP";
	case Caliber::_380_ACP:           return ".380 ACP";
	case Caliber::_45_ACP:            return ".45 ACP";
	case Caliber::_40_SW:             return ".40 S&W";
	case Caliber::_357_MAGNUM:        return ".357 S&W Magnum";
	case Caliber::_45_COLT:           return ".45 Colt";
	case Caliber::_44_MAGNUM:         return ".44 Remington Magnum";
	case Caliber::_50_AE:             return ".50 Action Express";

	// shotgun cartridge
	case Caliber::_410_BORE:          return "";
	case Caliber::_20_GAUGE:          return "";
	case Caliber::_12_GAUGE:          return "";

	// rifle cartridge
	case Caliber::_5_7X28:            return "";
	case Caliber::_5_56X45:           return "";
	case Caliber::_7_62X51:           return "";
//	case Caliber::_380_WINCHESTER:    return "";
	case Caliber::_30_60_SPRINGFIELD: return "";
	case Caliber::_50BMG:             return "";
//	case Caliber::_12_7X99:           return "";
	case Caliber::_7_62X39:           return "";
	case Caliber::_7_62X54R:          return "";

	case Caliber::_25MM:              return "";
	case Caliber::_30MM:              return "";

	case Caliber::OTHER:              return "";

	default:
		return "(unknown)";
	}
}


inline const char *GetAbbreviatedCaliberName( Caliber::Name cal )
{
	switch( cal )
	{
	case Caliber::_22LR:              return ".22 LR";
	case Caliber::_9MM:               return "9x19mm";
	case Caliber::_44_MAGNUM:         return ".44 Magnum";
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



#endif /* __Caliber_HPP__ */
