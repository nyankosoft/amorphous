#ifndef __BE_LIGHT_H__
#define __BE_LIGHT_H__

#include "BaseEntity.h"
#include "CopyEntity.h"

#include "3DCommon/HemisphericLight.h"
#include "Support/Serialization/Serialization_Light.h"
#include "Support/StringAux.h"

#include <string>


class CBE_Light : public CBaseEntity
{
protected:

	unsigned int m_TypeFlag;

	float m_afBaseColor[3];

protected:

	float& FadeoutTime( CCopyEntity* pCopyEnt ) { return pCopyEnt->f4; }

	float& TimeLeft( CCopyEntity* pCopyEnt ) { return pCopyEnt->f5; }

public:

	enum eLightTypeFlag
	{
		TYPE_STATIC		= (1 << 0),
		TYPE_DYNAMIC	= (1 << 1),
		TYPE_TIMER		= (1 << 2),
		TYPE_FADEOUT	= (1 << 3),
		TYPE_GLARE		= (1 << 4),
	};

	/* examples for type flags

	// static lights on static geometry - loaded with the stage
	m_TypeFlag = TYPE_GLARE|TYPE_STATIC;

	// dynamic lights with no glare textures
	// - used with texture animations for explosions, muzzle flashes, etc.
	m_TypeFlag = TYPE_DYNAMIC;

	// glare effect only and don't illuminate anything. mainly used with items
	m_TypeFlag = TYPE_GLARE;

*/

	inline CBE_Light();
	virtual ~CBE_Light() {}

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
	{
		std::string light_type;

		if( scanner.TryScanLine( "LIGHT_TYPE", light_type ) )
		{
			// separate the string
			std::vector<std::string> vecFlag;
			SeparateStrings( vecFlag, light_type.c_str(), " |\n");

			size_t i, num_strings = vecFlag.size();
			for( i=0; i<num_strings; i++ )
			{
				if( vecFlag[i] == "STATIC" )		m_TypeFlag |= TYPE_STATIC;
				else if( vecFlag[i] == "DYNAMIC" )	m_TypeFlag |= TYPE_DYNAMIC;
				else if( vecFlag[i] == "FADEOUT" )	m_TypeFlag |= TYPE_FADEOUT;
				else if( vecFlag[i] == "TIMER" )	m_TypeFlag |= TYPE_TIMER;
				else if( vecFlag[i] == "GLARE" )	m_TypeFlag |= TYPE_GLARE;
			};
			
			return true;
		}

		if( scanner.TryScanLine( "BASE_COLOR", m_afBaseColor[0], m_afBaseColor[1], m_afBaseColor[2] ) ) return true;

		return false;
	}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CBaseEntity::Serialize( ar, version );

		ar & m_TypeFlag;

//		if( ar.GetMode() == IArchive::MODE_INPUT )
//			MsgBoxFmt( "loaded light type for %s: %d", m_strName.c_str(), m_TypeFlag );

		for( int i=0; i<3; i++ )
            ar & m_afBaseColor[i];
	}

	static inline void SetAttenuationFactors( CCopyEntity *pEntity, float a0, float a1, float a2 )
	{
		pEntity->f1 = a0;
		pEntity->f2 = a1;
		pEntity->f3 = a2;
	}
};


inline CBE_Light::CBE_Light()
{
	m_TypeFlag = 0;

	m_afBaseColor[0] = m_afBaseColor[1] = m_afBaseColor[2] = 1.0f;
}


#endif  /*  __BE_LIGHT_H__  */
