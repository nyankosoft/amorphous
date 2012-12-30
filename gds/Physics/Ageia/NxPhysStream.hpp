#ifndef  __NxPhysStream_H__
#define  __NxPhysStream_H__


#include <NxStream.h>
#include "Support/stream_buffer.hpp"


namespace amorphous
{


namespace physics
{


class CNxPhysStream : public NxStream
{
	/// borrowed reference
	stream_buffer *m_pBuffer;

	bool m_bLoading;

public:

	inline                      CNxPhysStream( stream_buffer *pStreamBuffer, bool load );

	inline virtual              ~CNxPhysStream();

	inline virtual     NxU8            readByte()                              const;
	inline virtual     NxU16           readWord()                              const;
	inline virtual     NxU32           readDword()                             const;
	inline virtual     float           readFloat()                             const;
	inline virtual     double          readDouble()                            const;
	inline virtual     void            readBuffer(void* buffer, NxU32 size)    const;

	inline virtual     NxStream&       storeByte( NxU8 b);
	inline virtual     NxStream&       storeWord( NxU16 w);
	inline virtual     NxStream&       storeDword( NxU32 d);
	inline virtual     NxStream&       storeFloat( NxReal f);
	inline virtual     NxStream&       storeDouble( NxF64 f);
	inline virtual     NxStream&       storeBuffer(const void* buffer, NxU32 size);
};


inline CNxPhysStream::CNxPhysStream( stream_buffer *pStreamBuffer, bool load )
:
m_pBuffer(pStreamBuffer),
m_bLoading(load)
{
//	fp = fopen(filename, load ? "rb" : "wb");
}

inline CNxPhysStream::~CNxPhysStream()
{
}

// Loading API
inline NxU8 CNxPhysStream::readByte() const
{
	NxU8 b;
	size_t r = m_pBuffer->read( &b, sizeof(NxU8) );
//	NX_ASSERT(r);
	return b;
}

inline NxU16 CNxPhysStream::readWord() const
{
	NxU16 w;
	size_t r = m_pBuffer->read( &w, sizeof(NxU16) );
//	NX_ASSERT(r);
	return w;
}

inline NxU32 CNxPhysStream::readDword() const
{
	NxU32 dw;
	size_t r = m_pBuffer->read( &dw, sizeof(NxU32) );
//	NX_ASSERT(r);
	return dw;
}

inline float CNxPhysStream::readFloat() const
{
	float f;
	size_t r = m_pBuffer->read( &f, sizeof(float) );
//	NX_ASSERT(r);
	return f;
}

inline double CNxPhysStream::readDouble() const
{
	double f;
	size_t r = m_pBuffer->read( &f, sizeof(double) );
//	NX_ASSERT(r);
	return f;
}

inline void CNxPhysStream::readBuffer( void* buffer, NxU32 size )   const
{
	size_t w = m_pBuffer->read( buffer, size );
//	NX_ASSERT(w);
}


// Saving API
inline NxStream& CNxPhysStream::storeByte( NxU8 b )
{
	size_t w = m_pBuffer->write( &b, sizeof(NxU8) );
//	NX_ASSERT(w);
	return *this;
}

inline NxStream& CNxPhysStream::storeWord( NxU16 w )
{
	size_t ww = m_pBuffer->write( &w, sizeof(NxU16) );
//	NX_ASSERT(ww);
	return *this;
}

inline NxStream& CNxPhysStream::storeDword( NxU32 d )
{
	size_t ww = m_pBuffer->write( &d, sizeof(NxU32) );
//	NX_ASSERT(ww);
	return *this;
}

inline NxStream& CNxPhysStream::storeFloat( NxReal f )
{
	size_t ww = m_pBuffer->write( &f, sizeof(NxReal) );
//	NX_ASSERT(ww);
	return *this;
}

inline NxStream& CNxPhysStream::storeDouble( NxF64 f )
{
	size_t ww = m_pBuffer->write( &f, sizeof(NxF64) );
//	NX_ASSERT(ww);
	return *this;
}

//...

inline NxStream& CNxPhysStream::storeBuffer( const void* buffer, NxU32 size )
{
	size_t w = m_pBuffer->write( buffer, size );
//	NX_ASSERT(w);
	return *this;
}


} // namespace physics

} // namespace amorphous



#endif		/*  __NxPhysStream_H__  */
