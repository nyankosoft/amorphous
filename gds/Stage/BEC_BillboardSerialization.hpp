#ifndef  __BEC_BILLBOARD_SERIALIZATION_H__
#define  __BEC_BILLBOARD_SERIALIZATION_H__


#include "Support/Serialization/Serialization.hpp"
using namespace GameLib1::Serialization;


#include "BEC_Billboard.hpp"


inline IArchive& operator & ( IArchive& ar, CBEC_Billboard& billboard )
{
	ar & billboard.m_strTextureFilename;
	ar & billboard.m_DestAlphaBlendMode;

	return ar;
}


#endif		/*  __BEC_BILLBOARD_SERIALIZATION_H__  */