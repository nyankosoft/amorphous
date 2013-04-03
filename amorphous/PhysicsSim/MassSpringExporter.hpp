#ifndef  __MASSSPRINGEXPORTER_H__
#define  __MASSSPRINGEXPORTER_H__


#include "amorphous/PhysicsSim/MassSpringSim.hpp"


namespace amorphous
{


class CMassSpringExporter
{
protected:

	CMS_MassSpringSim m_MassSpringSim;

//	virtual void LoadModel( const char *pcFilename ) = 0;

public:

	CMassSpringExporter();

	~CMassSpringExporter();

	void BuildMassSpringObject( const char *pcFilename );


};


} // amorphous



#endif		/*  __MASSSPRINGEXPORTER_H__  */
