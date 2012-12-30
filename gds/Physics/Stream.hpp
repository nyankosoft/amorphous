#ifndef  __PhysStream_H__
#define  __PhysStream_H__


#include <string>
#include "3DMath/Matrix34.hpp"
#include "Support/SerializableStream.hpp"

#include "fwd.hpp"


namespace amorphous
{


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

} // namespace amorphous



#endif		/*  __PhysStream_H__  */
