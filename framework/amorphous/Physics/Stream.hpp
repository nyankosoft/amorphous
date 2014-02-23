#ifndef  __PhysStream_H__
#define  __PhysStream_H__


#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/Support/SerializableStream.hpp"
#include "fwd.hpp"


namespace amorphous
{


namespace physics
{


class CStream : public SerializableStream
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
		SerializableStream::Serialize( ar, version );

		ar & m_PhysicsEngine;
	}

	/// preprocessors needs to set physics engine name to 'm_PhysicsEngine'
	friend class CPreprocessorImpl;
};


} // namespace physics

} // namespace amorphous



#endif		/*  __PhysStream_H__  */
