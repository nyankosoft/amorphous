
#include "FloatLookUpTable.hpp"
/*
#include <math.h>


namespace amorphous
{


template <int iArraySize>
bool TCFloatLookUpTable<iArraySize>::SetQuadraticFadeout(float fStartVal, float fEndVal,
											float fFadeFactor, float fFadeDuration, float fSilence )
{
	if( fStartVal < fEndVal )
		return false;

	m_fTotalDuration = fFadeDuration + fSilence;

	m_fInterval = m_fTotalDuration / (float)(iArraySize - 1);

	float fHeight = fStartVal - fEndVal;
	float fOrigSpan = sqrtf( fHeight / fFadeFactor );

	float fOrigInterval = m_fInterval * fOrigSpan / fFadeDuration;

	float f;
	int i, iFadeEndPos = (int)( (float)iArraySize * (fFadeDuration / m_fTotalDuration) );
	for( i=0; i<iFadeEndPos; i++ )
	{
		f = (float)(iFadeEndPos - i) * fOrigInterval;
		m_afArray[i] = f * f * fFadeFactor;
	}

	for( i=iFadeEndPos; i<iArraySize; i++ )
	{
		m_afArray[i] = fEndVal;
	}

	return true;
}
*/

} // amorphous
