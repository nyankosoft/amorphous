#ifndef  __PhysStream_H__
#define  __PhysStream_H__


#include <string>
#include "3DMath/Matrix34.h"
#include "Support/SerializableStream.hpp"

#include "fwd.h"


namespace physics
{


class CStream : public CSerializableStream
{
	/// name of physics engine
	std::string m_PhysicsEngine;

public:

	CStream()
	{
	}

	virtual ~CStream() {}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CSerializableStream::Serialize( ar, version );

		ar & m_PhysicsEngine;
	}

	/// preprocessors needs to set physics engine name to 'm_PhysicsEngine'
	friend class CPreprocessorImpl;
};


} // namespace physics


#endif		/*  __PhysStream_H__  */
