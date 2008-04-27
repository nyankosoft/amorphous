
#ifndef  __FLOATLOOKUPTABLE_H__
#define  __FLOATLOOKUPTABLE_H__


//#include <vector>
//using namespace std;


template <int iArraySize>
class TCFloatLookUpTable
{
	float m_afArray[iArraySize];

	float m_fInterval;
	float m_fTotalDuration;

public:

	inline TCFloatLookUpTable();

	inline float GetValue( float f );

	inline bool SetQuadraticFadeout(float fStartVal, float fEndVal,
		                     float fFadeFactor, float fFadeDuration, float fSilence = 0.0f );

};


template <int iArraySize>
inline TCFloatLookUpTable<iArraySize>::TCFloatLookUpTable()
{
	m_fInterval = 0;
	m_fTotalDuration = 0;

	int i;
	for( i=0; i<iArraySize; i++ )
	{
		m_afArray[i] = 0;
	}
}


template <int iArraySize>
inline float TCFloatLookUpTable<iArraySize>::GetValue( float f )
{
	int i = (int)(f / m_fInterval);

	float t = f - (float)i;

	return m_afArray[i] * (1.0f - t) + m_afArray[i+1] * t;
}


#include <math.h>


template <int iArraySize>
inline bool TCFloatLookUpTable<iArraySize>::SetQuadraticFadeout(float fStartVal, float fEndVal,
											                    float fFadeFactor, float fFadeDuration, float fSilence )
{
	if( fStartVal < fEndVal )
		return false;

	int _iArraySize = iArraySize - 1;	// treat as smaller size array

	m_fTotalDuration = fFadeDuration + fSilence;

	m_fInterval = m_fTotalDuration / (float)(_iArraySize - 1);

	float fHeight = fStartVal - fEndVal;
	float fOrigSpan = sqrtf( fHeight / fFadeFactor );

	float fOrigInterval = m_fInterval * fOrigSpan / fFadeDuration;

	float f;
	int i, iFadeEndPos = (int)( (float)_iArraySize * (fFadeDuration / m_fTotalDuration) );
	for( i=0; i<iFadeEndPos; i++ )
	{
		f = (float)(iFadeEndPos - i - 1) * fOrigInterval;
		m_afArray[i] = f * f * fFadeFactor + fEndVal;
	}

	for( i=iFadeEndPos; i<_iArraySize; i++ )
	{
		m_afArray[i] = fEndVal;
	}

	m_afArray[_iArraySize] = fEndVal;	// avoid boundary error in GetValue()

	return true;
}



#endif		/*  __FLOATLOOKUPTABLE_H__  */